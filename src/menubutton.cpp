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

#include "menubutton.h"
#include <qpainter.h>
#include <qpixmap.h>
#include <qdebug.h>
#include "puzzle.h"
#include "imagerepository.h"
#include "soundrepository.h"

MenuButton::MenuButton(int id, const QString& label, QWidget* parent, bool df): QPushButton(parent)
{
    centered = false;
    drawFrame = df;
    iId = id;
    setText(label);

    QFont f = font();
    f.setBold(true);
    setFont(f);
}

MenuButton::~MenuButton()
{
}

void MenuButton::setColors(const QColor& foreground, const QColor& background)
{
    QPalette pl = palette();
    pl.setColor(QColorGroup::Background, background);
    pl.setColor(QColorGroup::Text, foreground);
    setPalette(pl);
}

void MenuButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setOpacity(0.75);
    painter.fillRect(QRect(0,0,width(), height()), colorGroup().background());
    painter.setOpacity(1.0);
    QFontMetrics fm = painter.fontMetrics();
    int y = (height() - fm.height()) / 2 ;
    int x = 2;
    if ( centered ) {
        x = (width() - fm.width(text())) / 2;
    }

    if ( isDown() || hasFocus())
        MenuButton::drawColorizedText(text(), x, y, &painter, QColor(255, 255, 255), 150);
    else
        MenuButton::drawColorizedText(text(), x, y, &painter, colorGroup().text(), 150);

    if ( drawFrame ) {
        if ( !isDown() )
            painter.setPen(QColor(0, 96, 165));
        else
            painter.setPen(QColor(255, 255, 255));
        painter.drawRect(0, 0, width() - 1, height() - 1);
    }
}

void MenuButton::drawColorizedText(const QString& text, int x, int y, QPainter *p, const QColor& clr, int min)
{
    if ( p && p->isActive() && text.length() ) {
        p->save();
        QFontMetrics fm = p->fontMetrics();
        y = y + fm.ascent();
        int cs, ch, cv;
        QColor tmp = clr;
        tmp.getHsv(&cs, &ch, &cv);
        int step = (cv - min) / qMax(((int)text.length() / 2), 1);
        if ( step < 0 ) {
            step = 0;
            min = 0;
        }

        for (int i = 0;i < text.length();i++ ) {
            clr.getHsv(&cs, &ch, &cv);
            if ( i < text.length() / 2 )
                tmp.setHsv(cs, ch, min + (step*i));
            else
                tmp.setHsv(cs, ch, cv - (step*(i - (text.length() / 2))));
            p->setPen(tmp);

            p->drawText(x, y, text.at(i));
            x += fm.width(text.at(i));
        }
        p->restore();
    }
}

MenuButtonList::MenuButtonList(QObject * parent): QObject(parent)
{
}

void MenuButtonList::appendMenuButton(MenuButton *btn)
{
    Q_ASSERT(btn->parent() != 0);
    list.append(btn);
    connect( btn, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void MenuButtonList::removeMenuButton(MenuButton *btn)
{
    if ( btn ) {
        list.removeAll(btn);
        btn->hide();
        disconnect(btn,SIGNAL(clicked()),this, SLOT(buttonClicked()));
    }
}

void MenuButtonList::buttonClicked()
{
    MenuButton *btn = (MenuButton *)sender();
    Puzzle::sounds->playSound("button");
    emit clicked(btn->id());
}

void MenuButtonList::setVisible(bool show)
{
    for (int i = 0; i < list.count(); ++i)
        list.at(i)->setVisible(show);
}
