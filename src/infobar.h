/*
* Copyright (C) 2005-2007 Benjamin C Meyer
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

#ifndef INFOBAR_H
#define INFOBAR_H

#include <qwidget.h>
#include <qevent.h>

class Playground;

class InfoBar : public QWidget
{
    Q_OBJECT

public:
    enum GAME_STATE {
        GAME,
        ABOUT,
        HISTORY,
        WELCOME,
        BROWSE,
        DEMO
    };

    InfoBar(QWidget *parent);
    void paintEvent(QPaintEvent *);

    void setState(GAME_STATE newState)
    {
        m_state = newState; update();
    }
    void setTotalPoints(int points)
    {
        m_totalPoints = points; update();
    }
    inline GAME_STATE state()
    {
        return m_state;
    }
    inline int totalPoints()
    {
        return m_totalPoints;
    }

    Playground *play;

private:
    int m_totalPoints;
    GAME_STATE m_state;

};

#endif

