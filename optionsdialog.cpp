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
#include <qevent.h>

int OptionsDialog::sOptions[5] = {60, 50, 40, 30, 20};

OptionsDialog::OptionsDialog(QString *currLevel, QString *th, bool *se, bool *ar, bool *tl, int *gs, QWidget *parent, const char *name): QDialog(parent, name, true, Qt::WStyle_Customize | Qt::WStyle_NoBorder )
{
    the = th;
    ile = currLevel;
    ise = se;
    iar = ar;
    igs = gs;
    itl = tl;

    QFont f = font();
    f.setBold(true);
    setFont(f);

    QColor black(0, 0, 0);
    QColor blue(0, 148, 255);

    setFont(QFont("Helvetica", 10, QFont::Bold));
    
    QPalette pal = palette();
    QColorGroup cg = pal.active();
    cg.setColor(QColorGroup::Base, Qt::black);
    cg.setColor(QColorGroup::Text, Qt::blue);
    cg.setColor(QColorGroup::Background, Qt::black);
    cg.setColor(QColorGroup::Button, Qt::black);
    cg.setColor(QColorGroup::ButtonText, Qt::blue);
    cg.setColor(QColorGroup::Midlight, Qt::blue);
    cg.setColor(QColorGroup::Shadow, Qt::blue);
    cg.setColor(QColorGroup::Dark, Qt::black);
    cg.setColor(QColorGroup::Light, Qt::blue);
    cg.setColor(QColorGroup::Mid, Qt::blue);
    cg.setColor(QColorGroup::Highlight, Qt::blue);
    cg.setColor(QColorGroup::HighlightedText, Qt::black);

    pal.setActive(cg);
    pal.setInactive(cg);
    pal.setDisabled(cg);
    setPalette(pal);
    setBackgroundMode(Qt::NoBackground);
    
    setFixedSize(220, 260);

    if ( parentWidget() ) {
        QRect pr = parentWidget()->frameGeometry ();
        move(pr.x() + (pr.width() - width()) / 2, (pr.y() + (pr.height() - height()) / 2) + 20);
    }

    OKButton = new MenuButton(0, "Save", this, true);
    OKButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
    OKButton->setCentered(true);
    connect(OKButton, SIGNAL(clicked()), this, SLOT(accept()));
    cancelButton = new MenuButton(1, "Cancel", this, true);
    cancelButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
    cancelButton->setCentered(true);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    dButtonList = new MenuButtonList(this);;
    dButtonList->appendMenuButton(OKButton);
    dButtonList->appendMenuButton(cancelButton);

    soundEffects = new QComboBox(this);
    soundEffects->insertItem("Yes");
    soundEffects->insertItem("No");
    soundEffects->setCurrentItem(!(*ise));

    timeLimit = new QComboBox(this);
    timeLimit->insertItem("Enabled");
    timeLimit->insertItem("Disabled");
    timeLimit->setCurrentItem(!(*itl));


    gameSpeed = new QComboBox(this);
    gameSpeed->insertItem("Very Slow");
    gameSpeed->insertItem("Slow");
    gameSpeed->insertItem("Normal");
    gameSpeed->insertItem("Fast");
    gameSpeed->insertItem("Very Fast");

    int i;
    for ( i = 0;i < 5;i++ ) {
        if ( *igs == OptionsDialog::sOptions[i] )
            break;
    }
    gameSpeed->setCurrentItem(i);
    abortConfirmation = new QComboBox(this);
    abortConfirmation->insertItem("Yes");
    abortConfirmation->insertItem("No");
    abortConfirmation->setCurrentItem(!(*iar));

    themes = new QComboBox(this);
    QDir    dir(ROOTHOME + "/pics/puzz-le/themes");
    if ( dir.exists() )
    {
        dir.setFilter( QDir::Dirs);
        const QFileInfoList list = dir.entryInfoList();
        i = 0;
        for (int j = 0; j < list.count(); ++j) {
            QFileInfo info = list.at(j);
            QString fName = info.fileName();
            if ( fName!="." && fName!="..")
            {
                themes->insertItem(fName);
                if ( fName==*the )
                {
                    themes->setCurrentItem(i);
                }
                i++;
            }
        }
    }
    customLevel = new QComboBox(this);
    customLevel->insertItem("Default");
#ifdef DEMO_VERSION
    customLevel->setEnabled(false);
#endif

        QDir    cl(ROOTHOME+"/pics/puzz-le/levels");
        if ( cl.exists() )
        {
            if ( ile->isEmpty() )
                customLevel->setCurrentItem(0);
            cl.setFilter( QDir::Dirs);
            const QFileInfoList list = cl.entryInfoList();
            i=1;
            for (int i = 0; i < list.count(); ++i)
            {
                QFileInfo info = list.at(i);
                QString fName = info.fileName();
                if ( fName!="." && fName!=".." && QFile::exists(info.absFilePath()+"/levels") )
                {
                    customLevel->insertItem(fName);
                    if ( fName==*ile )
                    {
                        customLevel->setCurrentItem(i);
                    }
                    i++;
                }
            }
        }
        
        //Q3ListBox *lb = soundEffects->listBox();
        //pal=lb->palette();
        //cg=pal.active();
        //cg.setColor(QColorGroup::Highlight,QColor(0,80,170));
        //pal.setActive(cg);
        //pal.setInactive(cg);
        //pal.setDisabled(cg);
        /*
        lb->setPalette(pal);
        lb=gameSpeed->listBox();
        lb->setPalette(pal);
        lb=abortConfirmation->listBox();
        lb->setPalette(pal);
        lb=customLevel->listBox();
        lb->setPalette(pal);
        lb=timeLimit->listBox();
        lb->setPalette(pal);
        lb=themes->listBox();
        lb->setPalette(pal);
        */
}

void OptionsDialog::paintEvent ( QPaintEvent * )
{
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), QColor(0, 0, 0));

    MenuButton::drawColorizedText("- Game Options -", 65, 20, &p, QColor(0, 148, 255), 150);

    MenuButton::drawColorizedText("Play Sound Effects:", 10, 55, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Game Speed:", 10, 80, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Time/Moves Limit:", 10, 105, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Prompt on Abort:", 10, 130, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Graphic Set:", 10, 155, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Levels Set:", 10, 180, &p, QColor(0, 148, 255), 150);

    p.setPen(QColor(0, 148, 255));
    p.drawRect(0, 0, width() - 1, height() - 1);

}

void OptionsDialog::resizeEvent ( QResizeEvent * )
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
    *ise = !soundEffects->currentItem();
    *iar = !abortConfirmation->currentItem();
    *itl = !timeLimit->currentItem();
    *igs = sOptions[gameSpeed->currentItem()];
    QString cit = customLevel->text(customLevel->currentItem());
    if ( cit == "Default" )
        *ile = "";
    else
        *ile = cit;
    *the = themes->text(themes->currentItem());
    QDialog::accept();
}

