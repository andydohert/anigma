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

#ifndef MENU_BUTTON_H
#define MENU_BUTTON_H

#include <qpushbutton.h>

class QTimer;

/**
 * Quck and dirty class for custom painted buttons.
 */
class MenuButton : public QPushButton
{
    Q_OBJECT

public:
    MenuButton(const QString &text, QWidget *parent = 0);
    ~MenuButton();

    void setColors(const QColor& foreground, const QColor& background);
    static void drawColorizedText(const QString& text, int x, int y, QPainter *p, const QColor& clr, int min = 0);

    void showFrame(bool draw)
    {
        drawFrame = draw;
    }
    inline void setCentered(bool f)
    {
        centered = f;
        update();
    }
    inline bool isCentered() const
    {
        return centered;
    }

    void paintEvent(QPaintEvent *);

private slots:
    void buttonClicked();

private:
    QImage textCache;
    QImage textFocusCache;

    bool centered;
    bool drawFrame;
};

class MenuButtonList: public QObject
{
    Q_OBJECT
public:
    MenuButtonList(QObject * parent = 0) : QObject(parent){} ;
    void addButton(QAbstractButton *button);
    void setVisible(bool show);

private:
    QList<QAbstractButton *> list;
};

#endif // MENU_BUTTON_H
