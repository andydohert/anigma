/* 
   Copyright (C) 2005 Benjamin C Meyer <ben+ksearch@meyerhome.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/**********************************************************************
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
** This file is part of Puzz-le game for Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** BSD license appearing in the file LICENSEL included in the
** packaging of this file.
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "optionsdialog.h"
#include "menubutton.h"
#include "imagerepository.h"
#include <qcombobox.h>
#include <qpainter.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qfile.h>

int OptionsDialog::sOptions[5]={60,50,40,30,20};

OptionsDialog::OptionsDialog(QString *currLevel,QString *th, bool *se,bool *ar,bool *tl, int *gs, QWidget *parent,const char *name):QDialog(parent,name,true,WStyle_Customize|WStyle_NoBorderEx )
{
    the=th;
    ile=currLevel;
    ise=se;
    iar=ar;
    igs=gs;
    itl=tl;

    QFont f=font();
    f.setBold(true);
    setFont(f);     

    QColor black(0,0,0);
    QColor blue(0,148,255);

    setFont(QFont("Helvetica",10,QFont::Bold));
    QPalette pal=palette();
    QColorGroup cg=pal.active();
    cg.setColor(QColorGroup::Base,black);
    cg.setColor(QColorGroup::Text,blue);    
    cg.setColor(QColorGroup::Background,black);
    cg.setColor(QColorGroup::Button,black);
    cg.setColor(QColorGroup::ButtonText,blue);
    cg.setColor(QColorGroup::Midlight,blue);
    cg.setColor(QColorGroup::Shadow,blue);
    cg.setColor(QColorGroup::Dark,black);
    cg.setColor(QColorGroup::Light,blue);
    cg.setColor(QColorGroup::Mid,blue);
    cg.setColor(QColorGroup::Highlight,blue);
    cg.setColor(QColorGroup::HighlightedText,black);

    pal.setActive(cg);
    pal.setInactive(cg);
    pal.setDisabled(cg);
    setPalette(pal);    


    setBackgroundMode(NoBackground);
    setFixedSize(220,260);

    if ( parentWidget() )
    {
        QRect pr=parentWidget()->frameGeometry ();
        move(pr.x()+(pr.width()-width())/2,(pr.y()+(pr.height()-height())/2)+20);
    }

    OKButton=new MenuButton(0,"Save",this,true);           
    OKButton->setColors(QColor(0,148,255),QColor(0,0,0));
    OKButton->setCentered(true);
    cancelButton=new MenuButton(1,"Cancel",this,true);           
    cancelButton->setColors(QColor(0,148,255),QColor(0,0,0));
    cancelButton->setCentered(true);

    dButtonList=new MenuButtonList(this);;    
    dButtonList->appendMenuButton(OKButton);
    dButtonList->appendMenuButton(cancelButton);
    connect( dButtonList, SIGNAL(clicked(int)),this, SLOT(buttonClicked(int))); 

    soundEffects=new QComboBox(this);
    soundEffects->insertItem("Yes");
    soundEffects->insertItem("No");
    soundEffects->setCurrentItem(!(*ise));

    timeLimit=new QComboBox(this);
    timeLimit->insertItem("Enabled");
    timeLimit->insertItem("Disabled");
    timeLimit->setCurrentItem(!(*itl));


    gameSpeed=new QComboBox(this);
    gameSpeed->insertItem("Very Slow");
    gameSpeed->insertItem("Slow");
    gameSpeed->insertItem("Normal");
    gameSpeed->insertItem("Fast");
    gameSpeed->insertItem("Very Fast");

    int i;
    for ( i=0;i<5;i++ )
    {
        if ( *igs==OptionsDialog::sOptions[i] )
            break;
    }
    gameSpeed->setCurrentItem(i);
    abortConfirmation=new QComboBox(this);
    abortConfirmation->insertItem("Yes");
    abortConfirmation->insertItem("No");
    abortConfirmation->setCurrentItem(!(*iar));

    themes=new QComboBox(this);        
    QDir    dir(ROOTHOME+"/pics/puzz-le/themes");
    if ( dir.exists() )
    {
        dir.setFilter( QDir::Dirs);

        const QFileInfoList *list = dir.entryInfoList();
        QFileInfoListIterator it( *list );      // create list iterator
        QFileInfo *fi; 
        QString fName;
        i=0;
        while ( (fi=it.current()) )
        {            
            fName=fi->fileName();
            if ( fName!="." && fName!="..")
            {
                themes->insertItem(fName);
                if ( fName==*the )
                {
                    themes->setCurrentItem(i);
                }
                i++;
            }
            ++it;            
        }


    }

    customLevel=new QComboBox(this);
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

        const QFileInfoList *list = cl.entryInfoList();
        QFileInfoListIterator it( *list );      // create list iterator
        QFileInfo *fi; 
        QString fName;
        i=1;
        while ( (fi=it.current()) )
        {
            fName=fi->fileName();
            if ( fName!="." && fName!=".." && QFile::exists(fi->absFilePath()+"/levels") )
            {
                customLevel->insertItem(fName);
                if ( fName==*ile )
                {
                    customLevel->setCurrentItem(i);
                }
                i++;
            }
            ++it;            
        }

    }



    QListBox *lb=soundEffects->listBox();
    pal=lb->palette();    
    cg=pal.active();
    cg.setColor(QColorGroup::Highlight,QColor(0,80,170)); 
    pal.setActive(cg);
    pal.setInactive(cg);
    pal.setDisabled(cg);    
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
}

void OptionsDialog::paintEvent ( QPaintEvent * ) 
{
    QPainter p(this);
    p.fillRect(0,0,width(),height(),QColor(0,0,0));

    MenuButton::drawColorizedText("- Game Options -",65,20,&p,QColor(0,148,255),150);

    MenuButton::drawColorizedText("Play Sound Effects:",10,55,&p,QColor(0,148,255),150);
    MenuButton::drawColorizedText("Game Speed:",10,80,&p,QColor(0,148,255),150);
    MenuButton::drawColorizedText("Time/Moves Limit:",10,105,&p,QColor(0,148,255),150); 
    MenuButton::drawColorizedText("Prompt on Abort:",10,130,&p,QColor(0,148,255),150);
    MenuButton::drawColorizedText("Graphic Set:",10,155,&p,QColor(0,148,255),150);    
    MenuButton::drawColorizedText("Levels Set:",10,180,&p,QColor(0,148,255),150);

    p.setPen(QColor(0,148,255));
    p.drawRect(0,0,width()-1,height()-1);

}
void OptionsDialog::resizeEvent ( QResizeEvent * )
{
    // just hardcode everything ...

    OKButton->setGeometry(10,height()-35,60,20);
    cancelButton->setGeometry(width()-70,height()-35,60,20);
    soundEffects->setGeometry(120,52,90,18);
    gameSpeed->setGeometry(120,77,90,18);
    timeLimit->setGeometry(120,102,90,18);
    abortConfirmation->setGeometry(120,128,90,18);
    themes->setGeometry(120,154,90,18);    
    customLevel->setGeometry(75,179,135,18);
}

void OptionsDialog::buttonClicked(int b)
{
    if ( b )
        reject();
    else
        accept();   
}

void OptionsDialog::accept()
{
    *ise=!soundEffects->currentItem();
    *iar=!abortConfirmation->currentItem();
    *itl=!timeLimit->currentItem();
    *igs=sOptions[gameSpeed->currentItem()];
    QString cit=customLevel->text(customLevel->currentItem());
    if ( cit=="Default" )
        *ile="";
    else
        *ile=cit;
    *the=themes->text(themes->currentItem());


    QDialog::accept();
}
