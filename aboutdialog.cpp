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

<<<<<<< .mine
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

#include <qpainter.h>
#include "AboutDialog.h"
#include "MenuButton.h"
#include "Playground.h"    
#include "ImageRepository.h"
#include "Puzzle.h"

static const char *historyData[]= {
    "",
    "Changes:",
    "",
    "*** VERSION 2.0.0 **************",
    "- added ability to create additional sets of",
    "   colored blocks ( themes.)",
    "- new themes ( Flags and Letters)",
    "- 400 new levels courtesy of Vexed",
    "   project ( ported to Puzz-le level" ,
    "   format by Alexander Paersch )",
    "- minor bug fixes ..",
    "",
    "*** VERSION 1.0.3 **************",
    "- added option to enable/disable time limit",
    "- added ability to pause game",
    "- minor bug fixes ..",
    "",
    "END"
};

static const char *infoData[]= {
    "",
    "Puzz-le is an arcade puzzler.",
    "",
    "Your task is to eliminate all of",
    "the colored blocks by putting them",
    "together.",
    "It is relatively easy with two blocks,",
    "but can be quite tricky with three",
    "or more.",
    "Depending on the levels pack, there is ",
    "either a time limit (time:000) or a limited",
    "number of moves (mov:00) you are allowed ",
    "to make.",
    "",
    "Some of the objects you will encounter",
    "are:",
    "|B   -  colored blocks",
    "|E   -  elevators",
    "|F   -  fire pits",
    "|D   -  damaged platforms",
    "|T   -  trap doors",
    "",
    "To pick up object, click on it.",
    "The cursor around the block turns white",
    "while you are carrying an object.",
    "",
    "Once you have picked up a block,",
    "you can move it horizontally.",
    "When you touch two or more similar",
    "blocks together, they will disappear.",
    "",
    "Tap on the Demo Game button to watch ",
    "computer solving each of the 50 available",
    "levels.",
    #ifdef DEMO_VERSION
    "",
    "",
    "This is a demo version that contains",
    "a sample of 8 levels. The full version",
    "of the game contains all 50 levels",
    #endif
    "",
    "Have fun and good luck !",
    "",
    "END"
};

AboutDialog::AboutDialog(QWidget  *parent,const char *name):QWidget(parent,name)
{

    QFont f=font();
    f.setBold(true);
    setFont(f); 

    lBox=new QListBox(this);
    lBox->setFrameStyle ( QFrame::NoFrame ) ;
    lBox->setFocusPolicy(QWidget::NoFocus);
    lBox->setHScrollBarMode(QScrollView::AlwaysOff);

    setBackgroundColor(QColor(0,0,0));
    setMode(ABOUT);

}

void AboutDialog::setMode(MODE m)
{
    lBox->clear();
    QString tmp;
    int i=0;
    while ( true )
    {
        if(m==ABOUT)
            tmp=infoData[i];
        else
            tmp=historyData[i];
        if ( tmp=="END" )
            break;
        lBox->insertItem(new AboutDialogItem(tmp,lBox));
        i++;
    }
    
}

void AboutDialog::resizeEvent ( QResizeEvent * )
{
    lBox->setGeometry(0,1,width(),height()-2);
}

void AboutDialog::paintEvent ( QPaintEvent * )
{

    QPainter p(this);

    p.setPen(QColor(0,148,255));
    p.drawLine(0,0,width(),0);
    p.drawLine(0,height()-1,width(),height()-1);
}

AboutDialogItem::AboutDialogItem(const QString &textLine, QListBox *lb)
{
    fHeight=0;

    setText(textLine);
    setCustomHighlighting ( true ); 

    if ( lb )
    {
        if ( textLine.at(0)=='|' )
            fHeight=Puzzle::bSize+4;
        else
        {
            QFontMetrics fm(lb->font());
            fHeight=fm.height()+6;
        }
    }

}

int AboutDialogItem::height ( const QListBox * ) const
{
      return fHeight;
}
int AboutDialogItem::width ( const QListBox *l ) const
{
    return l->width();
}

void AboutDialogItem::paint ( QPainter *p) 
{
    QColor q(0,148,255);
    QString txt=text();

    if ( txt.at(0)=='|' )
    {
        QPixmap *pix=0;	
		ImageRepository::I_NAMES iname=ImageRepository::YELLOW;
        switch ( txt.at(1).latin1() )
        {
            case    'B':
				iname=ImageRepository::YELLOW;       
                break;
            case    'E':
				iname=ImageRepository::WALL;   
                break;
            case    'F':
				iname=ImageRepository::FIRE;   
                break;  
            case    'D':
				iname=ImageRepository::BROKEN_WALL;    
                break;
            case    'T':
				iname=ImageRepository::TRAP_LEFT;  
                break;  
        }
		pix=Puzzle::images->findPixmap(iname); 
        if ( pix )
        {
			p->drawPixmap(0,2,*pix,0,0,Puzzle::bSize,Puzzle::bSize);
            if ( txt.at(1)=='T' )
            {
                pix=Puzzle::images->findPixmap(ImageRepository::TRAP_RIGHT);
                if ( p )
                    p->drawPixmap(0,2,*pix,0,0,Puzzle::bSize,Puzzle::bSize);
            }
        }
        MenuButton::drawColorizedText(txt.remove(0,2),Puzzle::bSize+2,3,p,q,150);
     }
    else
    {
        MenuButton::drawColorizedText(text(),0,3,p,q,150);  
    }

}



=======
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

#include <qpainter.h>
#include "aboutdialog.h"
#include "menubutton.h"
#include "playground.h"    
#include "imagerepository.h"
#include "puzzle.h"

static const char *historyData[]= {
    "",
    "Changes:",
    "",
    "*** VERSION 2.0.0 **************",
    "- added ability to create additional sets of",
    "   colored blocks ( themes.)",
    "- new themes ( Flags and Letters)",
    "- 400 new levels courtesy of Vexed",
    "   project ( ported to Puzz-le level" ,
    "   format by Alexander Paersch )",
    "- minor bug fixes ..",
    "",
    "*** VERSION 1.0.3 **************",
    "- added option to enable/disable time limit",
    "- added ability to pause game",
    "- minor bug fixes ..",
    "",
    "END"
};

static const char *infoData[]= {
    "",
    "Puzz-le is an arcade puzzler.",
    "",
    "Your task is to eliminate all of",
    "the colored blocks by putting them",
    "together.",
    "It is relatively easy with two blocks,",
    "but can be quite tricky with three",
    "or more.",
    "Depending on the levels pack, there is ",
    "either a time limit (time:000) or a limited",
    "number of moves (mov:00) you are allowed ",
    "to make.",
    "",
    "Some of the objects you will encounter",
    "are:",
    "|B   -  colored blocks",
    "|E   -  elevators",
    "|F   -  fire pits",
    "|D   -  damaged platforms",
    "|T   -  trap doors",
    "",
    "To pick up object, click on it.",
    "The cursor around the block turns white",
    "while you are carrying an object.",
    "",
    "Once you have picked up a block,",
    "you can move it horizontally.",
    "When you touch two or more similar",
    "blocks together, they will disappear.",
    "",
    "Tap on the Demo Game button to watch ",
    "computer solving each of the 50 available",
    "levels.",
    #ifdef DEMO_VERSION
    "",
    "",
    "This is a demo version that contains",
    "a sample of 8 levels. The full version",
    "of the game contains all 50 levels",
    #endif
    "",
    "Have fun and good luck !",
    "",
    "END"
};

AboutDialog::AboutDialog(QWidget  *parent,const char *name):QWidget(parent,name)
{

    QFont f=font();
    f.setBold(true);
    setFont(f); 

    lBox=new QListBox(this);
    lBox->setFrameStyle ( QFrame::NoFrame ) ;
    lBox->setFocusPolicy(QWidget::NoFocus);
    lBox->setHScrollBarMode(QScrollView::AlwaysOff);

    setBackgroundColor(QColor(0,0,0));
    setMode(ABOUT);

}

void AboutDialog::setMode(MODE m)
{
    lBox->clear();
    QString tmp;
    int i=0;
    while ( true )
    {
        if(m==ABOUT)
            tmp=infoData[i];
        else
            tmp=historyData[i];
        if ( tmp=="END" )
            break;
        lBox->insertItem(new AboutDialogItem(tmp,lBox));
        i++;
    }
    
}

void AboutDialog::resizeEvent ( QResizeEvent * )
{
    lBox->setGeometry(0,1,width(),height()-2);
}

void AboutDialog::paintEvent ( QPaintEvent * )
{

    QPainter p(this);

    p.setPen(QColor(0,148,255));
    p.drawLine(0,0,width(),0);
    p.drawLine(0,height()-1,width(),height()-1);
}

AboutDialogItem::AboutDialogItem(const QString &textLine, QListBox *lb)
{
    fHeight=0;

    setText(textLine);
    setCustomHighlighting ( true ); 

    if ( lb )
    {
        if ( textLine.at(0)=='|' )
            fHeight=Puzzle::bSize+4;
        else
        {
            QFontMetrics fm(lb->font());
            fHeight=fm.height()+6;
        }
    }

}

int AboutDialogItem::height ( const QListBox * ) const
{
      return fHeight;
}
int AboutDialogItem::width ( const QListBox *l ) const
{
    return l->width();
}

void AboutDialogItem::paint ( QPainter *p) 
{
    QColor q(0,148,255);
    QString txt=text();

    if ( txt.at(0)=='|' )
    {
        QPixmap *pix=0;	
		ImageRepository::I_NAMES iname=ImageRepository::YELLOW;
        switch ( txt.at(1).latin1() )
        {
            case    'B':
				iname=ImageRepository::YELLOW;       
                break;
            case    'E':
				iname=ImageRepository::WALL;   
                break;
            case    'F':
				iname=ImageRepository::FIRE;   
                break;  
            case    'D':
				iname=ImageRepository::BROKEN_WALL;    
                break;
            case    'T':
				iname=ImageRepository::TRAP_LEFT;  
                break;  
        }
		pix=Puzzle::images->findPixmap(iname); 
        if ( pix )
        {
			p->drawPixmap(0,2,*pix,0,0,Puzzle::bSize,Puzzle::bSize);
            if ( txt.at(1)=='T' )
            {
                pix=Puzzle::images->findPixmap(ImageRepository::TRAP_RIGHT);
                if ( p )
                    p->drawPixmap(0,2,*pix,0,0,Puzzle::bSize,Puzzle::bSize);
            }
        }
        MenuButton::drawColorizedText(txt.remove(0,2),Puzzle::bSize+2,3,p,q,150);
     }
    else
    {
        MenuButton::drawColorizedText(text(),0,3,p,q,150);  
    }

}



>>>>>>> .r164
