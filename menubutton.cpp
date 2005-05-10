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

#include "MenuButton.h"
#include <qpainter.h>
#include <qtimer.h>
#include <qpixmap.h>
#include "Puzzle.h"



MenuButton::MenuButton( int id,const QString& label,QWidget* parent, bool df,const char* name, WFlags fl):
QButton(parent,name,fl)
{
    centered=false;
    drawFrame=df;
    iId=id;
    setText(label);
    iHighlighted=false;
    QFont f=font();
    f.setBold(true);
    setFont(f);
    setBackgroundMode(NoBackground);
}

MenuButton::~MenuButton()
{
    
}

void MenuButton::setColors(const QColor& foreground, const QColor& background)
{
    QPalette pl=palette();
    pl.setColor(QColorGroup::Background,background);
    pl.setColor(QColorGroup::Text,foreground);
    setPalette(pl);
}

void MenuButton::setHighlighted(bool flag)
{
    iHighlighted=flag;
    update();
}

void MenuButton::focusInEvent ( QFocusEvent * )
{
    setHighlighted(true);
}

void MenuButton::focusOutEvent ( QFocusEvent * )
{
    setHighlighted(false);
}

void MenuButton::drawButton ( QPainter *p ) 
{
    if ( p )
    {
        QPixmap pix(width(),height());
        pix.fill(colorGroup().background());    
        QPainter lp(&pix);

        lp.setFont(font());
        
        QFontMetrics fm=p->fontMetrics();   
        int y=(height()-fm.height())/2 ;    
        int x=2;
        if ( centered )
        {
            x=(width()-fm.width(text()))/2;
        }

        if ( isDown() || isHighlighted())
            MenuButton::drawColorizedText(text(),x,y,&lp,QColor(255,255,255),150);
        else
            MenuButton::drawColorizedText(text(),x,y,&lp,colorGroup().text(),150);


        if ( drawFrame )
        {                 
            if ( !isDown() )
                lp.setPen(QColor(0,96,165));
            else
                lp.setPen(QColor(255,255,255));             
            lp.drawRect(0,0,width()-1,height()-1);
        }

        p->drawPixmap(0,0,pix);
    }
}

void MenuButton::drawColorizedText(const QString& text, int x, int y, QPainter *p, const QColor& clr, int min)
{
    if ( p && p->isActive() && text.length() )
    {
        p->save();
        QFontMetrics fm=p->fontMetrics();
        y=y+fm.ascent();
        unsigned int    i;              
        int cs,ch,cv;               
        QColor tmp=clr;
        tmp.hsv(&cs,&ch,&cv);
        int step=(cv-min)/QMAX(((int)text.length()/2),1);
        if ( step<0 )
        {
            step=0;
            min=0;          
        }

        for ( i=0;i<text.length();i++ )
        {
            clr.hsv(&cs,&ch,&cv);           
            if ( i<text.length()/2 )
                tmp.setHsv(cs,ch,min+(step*i));
            else
                tmp.setHsv(cs,ch,cv-(step*(i-(text.length()/2))));
            p->setPen(tmp);     

            p->drawText(x,y,text.at(i));
            x+=fm.width(text.at(i));
        } 

        p->restore();
    }
}

MenuButtonList::MenuButtonList(QObject * parent, const char * name):QObject(parent,name)
{
    list.setAutoDelete(false);
    doHighlighting=false;
    curHighlighted=0;
    timer=new QTimer(this);
    connect( timer, SIGNAL(timeout()),this, SLOT(timeout()));
}

void MenuButtonList::appendMenuButton(MenuButton *btn)
{
    list.append(btn);
    if ( !timer->isActive() && doHighlighting )
    {
        timer->start(500);
    }
    connect( btn,SIGNAL(clicked()),this, SLOT(buttonClicked()));
}

void MenuButtonList::removeMenuButton(MenuButton *btn)
{
    if ( btn )
    {
        list.removeRef(btn);
        btn->hide();
        if ( btn->isHighlighted() )
        {
            curHighlighted=0;
        }
        if ( !list.count() )
        {
            timer->stop();
        }
        disconnect(btn,SIGNAL(clicked()),this, SLOT(buttonClicked()));
    }
}

void MenuButtonList::buttonClicked()
{
    MenuButton *btn = (MenuButton *)sender();
    Puzzle::sounds->playSound("button");
    emit clicked(btn->id());
}

void MenuButtonList::configureButtons(bool show, bool highlight)
{
    doHighlighting=highlight;

    if ( doHighlighting && list.count()>0 )
    {
        timer->start(500);
    }
    else
        timer->stop();
    MenuButton *btn;
    for ( btn=list.first(); btn != 0; btn=list.next() )
    {
        btn->setHighlighted(false);
        if ( show )
            btn->show();
        else
            btn->hide();
    }

    curHighlighted=list.count()-1;

}
void MenuButtonList::setHighlighted(int index)
{
    MenuButton *btn=list.at(curHighlighted);
    if ( btn )
        btn->setHighlighted(false);
    curHighlighted=index;
    btn=list.at(curHighlighted);
    if ( btn )
        btn->setHighlighted(false);
}


void MenuButtonList::timeout()
{
    MenuButton *btn;

    btn=list.at(curHighlighted);
    if ( btn )
        btn->setHighlighted(false);
    curHighlighted++;

    if ( curHighlighted==(int)list.count() && list.count()!=1 )    // accomodate list with single MenuButton ( need to flash on/off)
    {
        curHighlighted=0;
    }
    if ( curHighlighted>(int)list.count() )
        curHighlighted=0;
    btn=list.at(curHighlighted);
    if ( btn )
        btn->setHighlighted(true);
}


