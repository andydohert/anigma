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

#include "optionsdialog.h"

#include "menubutton.h"
#include "imagerepository.h"
#include "puzzle.h"
#include <qcombobox.h>
#include <qpainter.h>
#include <qdir.h>
#include <qfile.h>

int OptionsDialog::sOptions[5] = {60, 50, 40, 30, 20};

OptionsDialog::OptionsDialog(QString *currLevel, QString *th, bool *se, bool *ar, bool *tl, int *gs, QWidget *parent): QDialog(parent,  Qt::FramelessWindowHint )
{
    setModal(true);
    the = th;
    ile = currLevel;
    ise = se;
    iar = ar;
    igs = gs;
    itl = tl;

    setFont(QFont("Helvetica", 10, QFont::Bold));
    setFixedSize(220, 260);

    if (parentWidget()) {
        QRect pr = parentWidget()->frameGeometry ();
        move(pr.x() + (pr.width() - width()) / 2, (pr.y() + (pr.height() - height()) / 2) + 20);
    }

    OKButton = new MenuButton("Save", this);
    OKButton->showFrame(true);
    OKButton->setCentered(true);
    connect(OKButton, SIGNAL(clicked()), this, SLOT(accept()));
    cancelButton = new MenuButton("Cancel", this);
    cancelButton->showFrame(true);
    cancelButton->setCentered(true);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    soundEffects = new QComboBox(this);
    soundEffects->addItem("Yes");
    soundEffects->addItem("No");
    soundEffects->setCurrentIndex(!(*ise));

    timeLimit = new QComboBox(this);
    timeLimit->addItem("Enabled");
    timeLimit->addItem("Disabled");
    timeLimit->setCurrentIndex(!(*itl));


    gameSpeed = new QComboBox(this);
    gameSpeed->addItem("Very Slow");
    gameSpeed->addItem("Slow");
    gameSpeed->addItem("Normal");
    gameSpeed->addItem("Fast");
    gameSpeed->addItem("Very Fast");

    int i;
    for ( i = 0;i < 5;i++) {
        if (*igs == OptionsDialog::sOptions[i] )
            break;
    }
    gameSpeed->setCurrentIndex(i);
    abortConfirmation = new QComboBox(this);
    abortConfirmation->addItem("Yes");
    abortConfirmation->addItem("No");
    abortConfirmation->setCurrentIndex(!(*iar));

    themes = new QComboBox(this);
    QDir dir(ROOTHOME + "/pics/themes");
    if (dir.exists() ) {
        dir.setFilter( QDir::Dirs);
        const QFileInfoList list = dir.entryInfoList();
        i = 0;
        for (int j = 0; j < list.count(); ++j) {
            QFileInfo info = list.at(j);
            QString fName = info.fileName();
            if (fName != "." && fName != "..") {
                themes->addItem(fName);
                if (fName == *the ) {
                    themes->setCurrentIndex(i);
                }
                i++;
            }
        }
    }
    customLevel = new QComboBox(this);
    customLevel->addItem("Default");
#ifdef DEMO_VERSION
    customLevel->setEnabled(false);
#endif

    QDir cl(ROOTHOME + "/pics/levels");
    if (cl.exists()) {
        if (ile->isEmpty())
            customLevel->setCurrentIndex(0);
        cl.setFilter( QDir::Dirs);
        const QFileInfoList list = cl.entryInfoList();
        i = 1;
        for (int i = 0; i < list.count(); ++i) {
            QFileInfo info = list.at(i);
            QString fName = info.fileName();
            if (fName != "." && fName != ".." && QFile::exists(info.absoluteFilePath() + "/levels") ) {
                customLevel->addItem(fName);
                if (fName == *ile ) {
                    customLevel->setCurrentIndex(i);
                }
                i++;
            }
        }
    }
}

void OptionsDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), QColor(0, 0, 0));

    MenuButton::drawColorizedText("- Game Options -", 65, 20, &p, QColor(0, 148, 255), 150);

    MenuButton::drawColorizedText("Sound Effects:", 10, 55, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Game Speed:", 10, 80, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Time/Moves Limit:", 10, 105, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Prompt on Abort:", 10, 130, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Graphic Set:", 10, 155, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Levels Set:", 10, 180, &p, QColor(0, 148, 255), 150);

    p.setPen(QColor(0, 148, 255));
    p.drawRect(0, 0, width() - 1, height() - 1);
}

void OptionsDialog::resizeEvent(QResizeEvent *)
{
    // just hardcode everything ...
    OKButton->setGeometry(10, height() - 35, 60, 20);
    cancelButton->setGeometry(width() - 70, height() - 35, 60, 20);
    soundEffects->setGeometry(120, 52, 90, 18);
    gameSpeed->setGeometry(120, 77, 90, 18);
    timeLimit->setGeometry(120, 102, 90, 18);
    abortConfirmation->setGeometry(120, 128, 90, 18);
    themes->setGeometry(120, 154, 90, 18);
    customLevel->setGeometry(75, 179, 135, 18);
}

void OptionsDialog::accept()
{
    *ise = !soundEffects->currentIndex();
    *iar = !abortConfirmation->currentIndex();
    *itl = !timeLimit->currentIndex();
    *igs = sOptions[gameSpeed->currentIndex()];
    QString cit = customLevel->currentText();
    if (cit == "Default" )
        *ile = "";
    else
        *ile = cit;
    *the = themes->currentText();
    QDialog::accept();
}

