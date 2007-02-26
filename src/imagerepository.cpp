/*
* Copyright (C) 2005-2007 Benjamin C Meyer
* Copyright (C) 2001-2002 Walter Rawdanik
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of the contributors may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY <copyright holder> ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "imagerepository.h"

#include <qdir.h>
#include <qdebug.h>
#include <qpixmap.h>
#include <qbitmap.h>

#include "puzzle.h"

static const char *imageNames[] =
{
    "red",
    "green",
    "blue",
    "purple",
    "yellow",
    "cyan",
    "white",
    "grey",

    "wall",
    "fire",
    "trap_right",
    "trap_left",
    "broken_wall",
    "front_1",
    "background",
    "front_title",
    "freezing_animation"
};

ImageRepository::ImageRepository()
{
    addImage("wall");
    addImage("fire");
    addImage("trap_right");
    addImage("trap_left");
    addImage("broken_wall");
    addImage("front_1");
    addImage("background");
    addImage("front_title");
    addImage("freezing_animation");
}

void ImageRepository::addImage(const QString &name)
{
    QImage img(ROOTHOME + "/pics/graphics/" + name + ".png");
    if (img.isNull())
        qWarning() << "missing image file:" << name;
    if (img.depth() < 32)
        img = img.convertToFormat(QImage::Format_RGB32); // we MUST have 32 bit image for createHighlight to work.
    pixmaps.insert(name, QPixmap::fromImage(img));
}

QPixmap ImageRepository::findPixmap(const QString & name) const
{
    if (!pixmaps.contains(name))
        qWarning() << "unknown pixmap" << name;
    return pixmaps.value(name);
}

QPixmap ImageRepository::findPixmap(IMAGE_NAMES id) const
{
    return findPixmap(imageNames[id]);
}

bool ImageRepository::initTheme(const QString &theme)
{
    QDir directory(ROOTHOME + "/pics/themes/" + theme);
    if (theme.isEmpty() || !directory.exists()) {
        qWarning() << "error loading theme" << theme;
        return false;
    }

    currentTheme.clear();
    currentHighlightTheme.clear();

    directory.setFilter(QDir::Files);
    directory.setNameFilters(QStringList("*.png_"));

    const QFileInfoList list = directory.entryInfoList();
    for (int i = 0; i < list.count(); ++i) {
        QImage img(list.at(i).filePath());
        if (!img.isNull()) {
            if (img.width() != Puzzle::blockPixelSize
                || img.height() != Puzzle::blockPixelSize) {
                img = img.scaled(Puzzle::blockPixelSize, Puzzle::blockPixelSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }
            if (img.depth() < 32)
                img = img.convertToFormat(QImage::Format_ARGB32);
            QPixmap pix;
            pix = QPixmap::fromImage(img);
            pix.setMask(pix.createHeuristicMask());
            currentTheme.append(pix);
            createHighlighted(img);
            QPixmap hPix = QPixmap::fromImage(img);
            hPix.setMask(pix.mask());
            currentHighlightTheme.append(hPix);
        } else {
            qDebug() << "Unable to load image" << list.at(i).filePath();
        }
    }
    if (currentHighlightTheme.count() >= 8) {
        shuffleTheme();
        return true;
    }
    return false;
}

void ImageRepository::shuffleTheme()
{
    QVector<int> ns;
    ns.resize(8);
    ns.fill(-1);

    int n;
    int cnt = 0;
    Q_ASSERT(currentTheme.count() >= 8);
    while (cnt < 8) {
        n = (rand() / (RAND_MAX / currentTheme.count()));
        if (ns.indexOf(n) == -1) {
            ns[cnt] = n;
            QString name = imageNames[cnt];
            QString hName = name + "h";
            pixmaps.remove(name);
            pixmaps.remove(hName);
            pixmaps.insert(name, currentTheme[n]);
            pixmaps.insert(hName, currentHighlightTheme[n]);
            cnt++;
        }
    }
}

void ImageRepository::createHighlighted(QImage &img) const
{
    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            // Create 60% lighter image
            // Seems like QColor does not retain alpha info which means we have to restore it later ...
            QRgb s = img.pixel(x, y);
            QColor clr(s);
            QRgb d = clr.light(160).rgb();
            img.setPixel(x, y, qRgba(qRed(d), qGreen(d), qBlue(d), qAlpha(s)));
        }
    }
}
