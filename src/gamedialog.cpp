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

#include "gamedialog.h"

#include <qtimer.h>
#include <qfontmetrics.h>
#include <qbuttongroup.h>
#include <qpainter.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qdebug.h>

#include "menubutton.h"
#include "puzzle.h"

GameDialog::GameDialog( QWidget *parent): QDialog(parent, Qt::FramelessWindowHint )
{
    setPalette(parent->palette());
    setModal(true);
    margin = 16;
    leftButton = 0;
    rightButton = 0;
    middleButton = 0;
    buttonGroup = 0;
    lEdit = 0;

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(timeSynch()));
}

void GameDialog::paintEvent(QPaintEvent *)
{
    updateFireworks();
}

void GameDialog::keyPressEvent(QKeyEvent *e)
{
    switch ( e->key()) {
    case    Qt::Key_Return:
        done(0);
        break;
    case    Qt::Key_Escape:
        done(2);
        break;
    }
}

void GameDialog::timeSynch()
{
    if (tCounter != -1) {
        if (tCounter == 0) {
            accept();
        } else {
            update();
            tCounter--;
        }
    }
}

void GameDialog::updateFireworks()
{
    QPainter p(this);
    p.drawPixmap(0, 0, tinted);
    if (doFancy) {
        for (int i = 0;i < NUM_FW;i++) {
            if (fireWorks[i] == 0) {
                fireWorks[i] = 1;

                int x = (rand() / (RAND_MAX / (width() - 40) + 1)) + 20;
                int y = (rand() / (RAND_MAX / (height() - 40) + 1)) + 20;

                for (int j = 0; j < NUM_FP; j++) {
                    int index = i * NUM_FP + j;
                    xc[index] = x;
                    yc[index] = y;

                    xa[index] = ((((float)rand()) / RAND_MAX) * 6) - 3;
                    ya[index] = ((((float)rand()) / RAND_MAX) * 7) - 3.5;
                }

            }

            qreal fround = 0.5;
            qreal adv = 0.1;

            for (int j = 0; j < NUM_FP; j++) {
                int index = i * NUM_FP + j;
                switch ( i) {
                case 0:
                    p.setPen(QColor(255, fcount[i]*3, fcount[i]*3));
                    break;
                case 1:
                    p.setPen(QColor(fcount[i]*3, 255, fcount[i]*3));
                    break;
                case 2:
                    p.setPen(QColor(fcount[i]*3, fcount[i]*3, 255));
                    break;
                }

                p.drawLine((int)(xc[index] + fround),
                           (int)(yc[index] + fround),
                           (int)((xc[index] - xa[index]) + fround),
                           (int)((yc[index] - ya[index]) - fround));
                xc[index] += xa[index];
                yc[index] += ya[index];
                ya[index] = ya[index] + adv;
            }
            fcount[i]--;
            if (fcount[i] <= 0) {
                fcount[i] = (rand() / (RAND_MAX / 54)) + 32;
                fireWorks[i] = 0;
            }

        }
    }
    p.setFont(font());

    int topPos;
    if (leftButton || rightButton )
        topPos = margin * 2 ;
    else
        topPos = margin * 3;

    if (lEdit) {
        topPos -= 10;
    }

    /*
    if (textLightCache.isNull()) {
        textLightCache = QImage(width(), height(), QImage::Format_ARGB32);
        textDarkCache = QImage(width(), height(), QImage::Format_ARGB32);
        QPainter t(&textLightCache);
        t.setFont(font());
        textLightCache.fill(0);
        MenuButton::drawColorizedText(dText, (width() - textWidth) / 2 + 1, topPos + 1, &t, QColor(0, 0, 0), 150);
        QPainter u(&textDarkCache);
        u.setFont(font());
        textDarkCache.fill(0);
        MenuButton::drawColorizedText(dText, (width() - textWidth) / 2, topPos, &u, QColor(255, 255, 255), 180);
    }
    p.drawImage(0, 0, textLightCache);
    p.drawImage(0, 0, textDarkCache);
    */

    MenuButton::drawColorizedText(dText, (width() - textWidth) / 2 + 1, topPos + 1, &p, QColor(0, 0, 0), 150);
    MenuButton::drawColorizedText(dText, (width() - textWidth) / 2, topPos, &p, QColor(255, 255, 255), 180);

    p.setPen(QColor(0, 148, 255));
    p.drawRect(0, 0, width() - 1, height() - 1);
}

void GameDialog::show()
{
    update();
    if (doFancy)
        timer->start(33);
    QDialog::show();
}

void GameDialog::hide()
{
    if (lEdit )
        reqText = lEdit->text();
    delete lEdit;
    lEdit = 0;
    if (doFancy )
        timer->stop();
    QDialog::hide();
}

void GameDialog::configure(const QString &text, bool fancy,
                           bool requester, int timeout, const QString &ls,
                           const QString &ms, const QString &rs )
{
    QWidget *par = parentWidget();

    if (leftButton) leftButton->deleteLater();
    if (rightButton) rightButton->deleteLater();
    if (middleButton) middleButton->deleteLater();
    if (buttonGroup) buttonGroup->deleteLater();
    leftButton = 0;
    rightButton = 0;
    middleButton = 0;
    buttonGroup = 0;

    reqText.truncate(0);
    dText = text;
    doFancy = fancy;
    tCounter = timeout / 33;
    if (!tCounter )
        tCounter = -1;

    QFontMetrics fm(this->font());
    QRect r = fm.boundingRect(text);
    textWidth = r.width();

    // don't bother with parent widget, just hardcode the size ( otherwise if on screen keyboard is present our dialog gets too small)

    //if (parentWidget())
    // setqrealSize(parentWidget()->width()-10,parentWidget()->height()/3);
    //else
    setFixedSize(220, 110);

    if (parentWidget()) {
        QRect pr = parentWidget()->frameGeometry ();
        move(pr.x() + (pr.width() - width()) / 2, (pr.y() + (pr.height() - height()) / 2) - 10);
    }

    if (fancy) {
        int i;
        for ( i = 0;i < NUM_FW;i++) {
            fireWorks[i] = 0;
            fcount[i] = (rand() % 53) + 32;
        }
        for ( i = 0;i < NUM_FP;i++) {
            xc[i] = -1;
            yc[i] = 0;
            xa[i] = 0;
            ya[i] = 0;
        }
    }

    if (requester) {
        QString fileName;

        if (!Puzzle::levelFileName.isEmpty() )
            fileName.append(Puzzle::levelFileName);
        else {
            if (!Puzzle::currLevelsName.isEmpty())
                fileName.append(Puzzle::currLevelsName);
            else
                fileName.append("Default");
        }

        QDate cdate = QDate::currentDate();
        QTime ctime = QTime::currentTime();
        fileName.sprintf("%s-%d_%d_%d_%d_%d", fileName.toLatin1().constData(), cdate.day(), cdate.month(), ctime.hour(), ctime.minute(), ctime.second());
        lEdit = new QLineEdit(this);
        lEdit->setMaxLength (36);
        lEdit->setGeometry((width() - 160) / 2, (height() - 20) / 2, 160, 20);
        lEdit->setText(fileName);
        lEdit->setSelection (0, 20);
        lEdit->setFocus();
    }

    if (!ls.isNull() || !rs.isNull()) {
        buttonGroup = new QButtonGroup(this);
        if (!ls.isNull()) {
            leftButton = new MenuButton(ls, this);
            leftButton->showFrame(true);
            leftButton->setGeometry(120, 280, 60, 20);
            buttonGroup->addButton(leftButton, 0);
            leftButton->setCentered(true);
        }
        if (!ms.isNull()) {
            middleButton = new MenuButton(ms, this);
            middleButton->showFrame(true);
            middleButton->setGeometry(120, 280, 60, 20);
            buttonGroup->addButton(middleButton, 1);
            middleButton->setCentered(true);
        }
        if (!rs.isNull()) {
            rightButton = new MenuButton(rs, this);
            rightButton->showFrame(true);
            rightButton->setGeometry(120, 280, 60, 20);
            buttonGroup->addButton(rightButton, 2);
            rightButton->setCentered(true);
        }
    }
    if (leftButton) {
        if (rightButton || middleButton) {
            leftButton->move(margin, height() - leftButton->height() - margin);
        } else {
            leftButton->move(((width() - leftButton->width()) / 2), height() - leftButton->height() - margin);
        }
    }
    if (middleButton) {

        middleButton->move(((width() - middleButton->width()) / 2), height() - middleButton->height() - margin);

    }
    if (rightButton || middleButton) {
        if (leftButton) {
            rightButton->move(width() - rightButton->width() - margin, height() - rightButton->height() - margin);
        } else {
            rightButton->move(((width() - rightButton->width()) / 2), height() - rightButton->height() - margin);
        }
    }

    if (buttonGroup) {
        connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));
    }

    Q_ASSERT(par);
    if (parentWidget()) {
        QPixmap background = QPixmap::grabWidget(par, QRect(0, 0, par->width(), par->height()));
        createTintedBackground(background);
        repaint();
    }
}

void GameDialog::buttonClicked(int id)
{
    hide();
    done(id);
}

// Make the source image only half as bright
void GameDialog::createTintedBackground(const QPixmap &back)
{
    tinted = QPixmap(width(), height());
    tinted.fill(Qt::transparent);
    QPainter p(&tinted);
    QRect r = parentWidget()->geometry();
    QRect source(x() - r.x(), y() - r.y(), width(), height());
    QRectF target(0.0, 0.0, tinted.width(), tinted.height());
    p.setOpacity(0.60);
    p.drawPixmap(target, back, source);
}
