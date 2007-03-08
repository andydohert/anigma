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

#include "infobar.h"
#include "playground.h"
#include <qpainter.h>

InfoBar::InfoBar(QWidget *parent) : QWidget(parent)
, play(0)
, m_totalPoints(0)
, m_state(WELCOME) {
    setFocusPolicy(Qt::NoFocus);
}

void InfoBar::paintEvent(QPaintEvent *) {
    if (!play)
        return;
    QString msg;
    int rx = 46;
    if (m_state == InfoBar::DEMO) {
        rx += 6;
        msg.sprintf("DEMO  TIME: %03d  SCORE: %05d", play->timeLimit(),
                     m_totalPoints + play->currentPoints());
    } else {
        if (Puzzle::timeLimit) {
            if (play->gameType() == Playground::TIME_BASED)
                msg.sprintf("L: %02d   TIME: %03d   SCORE: %05d",
                            play->currentLevel, play->timeLimit(),
                            m_totalPoints + play->currentPoints());
            else
                msg.sprintf("L: %02d   MOV: %03d   SCORE: %05d",
                            play->currentLevel, play->timeLimit(),
                            m_totalPoints + play->currentPoints());
        } else
            msg.sprintf("LEVEL: %02d   SCORE: %05d", play->currentLevel,
                        m_totalPoints + play->currentPoints());
    }

    QPainter p(this);
    QLinearGradient gradient(width()/2, 0, width()/2, height());
    QColor start(0,179,255);
    QColor end(start.dark(200));
    gradient.setColorAt(0, start);
    gradient.setColorAt(1, end);
    p.setPen(QPen(gradient, 5));
    QFont font("Helvetica", 16, QFont::Bold);
    int length = width() * 2;
    while (length > width()) {
        font.setPointSize(font.pointSize() - 1);
        QFontMetrics fm(font);
        length = fm.width(msg);
    }

    p.setFont(font);
    p.drawText(2, 2, width() - 4, height() - 4, Qt::AlignCenter, msg);
    if (Puzzle::timeLimit && play->gameType() == Playground::TIME_BASED) {
        if (play->timeLimit() < 10 && (play->timeLimit() % 2)) {
            QFontMetrics fm(font);
            p.drawRect(fm.width("DEMO  ")-2, 2, fm.width("TIME :000 "), fm.height() - 7);
        }
    }
}

