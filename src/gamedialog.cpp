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
#include <qpainter.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qdebug.h>
#include <stdlib.h>

#include "menubutton.h"
#include "puzzle.h"

GameDialog::GameDialog( QWidget *parent): QDialog(parent, Qt::FramelessWindowHint )
{
    setModal(true);
    margin = 16;
    leftButton = 0;
    rightButton = 0;
    middleButton = 0;
    dButtonList = 0;
    lEdit = 0;

    QFont f = font();
    f.setBold(true);
    setFont(f);

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(timeSynch()));
}

void GameDialog::paintEvent ( QPaintEvent * )
{
    updateFireworks();
}

void GameDialog::resizeEvent(QResizeEvent *)
{
    tinted = QPixmap(width(), height());
}

void GameDialog::keyPressEvent ( QKeyEvent * e )
{
    switch ( e->key() ) {
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
    if ( tCounter != -1 ) {
        if ( tCounter == 0 ) {
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
    int i, j;
    int index;
    p.drawPixmap(0, 0, tinted);
    if ( doFancy ) {
        for ( i = 0;i < NUM_FW;i++ ) {
            if ( fireWorks[i] == 0 ) {
                fireWorks[i] = 1;

                int x = (rand() / (RAND_MAX / (width() - 40) + 1)) + 20;
                int y = (rand() / (RAND_MAX / (height() - 40) + 1)) + 20;

                for ( j = 0; j < NUM_FP; j++ ) {
                    index = i * NUM_FP + j;
                    xc[index] = x;
                    yc[index] = y;

                    xa[index] = ((((float)rand()) / RAND_MAX) * 6) - 3;
                    ya[index] = ((((float)rand()) / RAND_MAX) * 7) - 3.5;
                }

            }

            qreal fround = 0.5;
            qreal adv = 0.1;

            for ( j = 0;j < NUM_FP;j++ ) {
                index = i * NUM_FP + j;
                switch ( i ) {
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

                p.drawLine((xc[index] + fround), (yc[index] + fround),
                            ((xc[index] - xa[index]) + fround), ((yc[index] - ya[index]) - fround));
                xc[index] += xa[index];
                yc[index] += ya[index];
                ya[index] = ya[index] + adv;
            }
            fcount[i]--;
            if ( fcount[i] <= 0 ) {
                fcount[i] = (rand() / (RAND_MAX / 54)) + 32;
                fireWorks[i] = 0;
            }

        }
    }
    p.setFont(font());

    int topPos;
    if ( leftButton || rightButton )
        topPos = margin * 2 ;
    else
        topPos = margin * 3;

    if ( lEdit ) {
        topPos -= 10;
    }

    MenuButton::drawColorizedText(dText, (width() - textWidth) / 2 + 1, topPos + 1, &p, QColor(0, 0, 0), 150);
    MenuButton::drawColorizedText(dText, (width() - textWidth) / 2, topPos, &p, QColor(255, 255, 255), 180);

    p.setPen(QColor(0, 148, 255));
    p.drawRect(0, 0, width(), height());
}

void GameDialog::show()
{
    if (doFancy)
        timer->start(33);
    QDialog::show();
}

void GameDialog::hide()
{
    delete leftButton;
    delete rightButton;
    delete middleButton;
    delete dButtonList;
    if ( lEdit )
        reqText = lEdit->text();
    delete lEdit;
    leftButton = 0;
    rightButton = 0;
    middleButton = 0;
    dButtonList = 0;
    lEdit = 0;
    if ( doFancy )
        timer->stop();
    QDialog::hide();
}

void GameDialog::configure(QPixmap *background , const QString &text, bool fancy,
        bool requester, int timeout, const QString &ls,
                           const QString &ms, const QString &rs )
{
    reqText.truncate(0);
    dText = text;
    doFancy = fancy;
    tCounter = timeout / 33;
    if ( !tCounter )
        tCounter = -1;

    QFontMetrics fm(this->font());
    QRect r = fm.boundingRect(text);
    textWidth = r.width();

    // don't bother with parent widget, just hardcode the size ( otherwise if on screen keyboard is present our dialog gets too small)

    //if(parentWidget())
    // setqrealSize(parentWidget()->width()-10,parentWidget()->height()/3);
    //else
    setFixedSize(220, 110);

    if ( parentWidget() ) {
        QRect pr = parentWidget()->frameGeometry ();
        move(pr.x() + (pr.width() - width()) / 2, (pr.y() + (pr.height() - height()) / 2) - 10);
    }

    if ( fancy ) {
        int i;
        for ( i = 0;i < NUM_FW;i++ ) {
            fireWorks[i] = 0;
            fcount[i] = (rand() % 53) + 32;
        }
        for ( i = 0;i < NUM_FP;i++ ) {
            xc[i] = -1;
            yc[i] = 0;
            xa[i] = 0;
            ya[i] = 0;
        }
    }

    if ( requester ) {
        QString fileName;

        if ( !Puzzle::levelFileName.isEmpty() )
            fileName.append(Puzzle::levelFileName);
        else {
            if(!Puzzle::currLevelsName.isEmpty())
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

    if ( !ls.isNull() || !rs.isNull() ) {
        dButtonList = new MenuButtonList(this);;
        if ( !ls.isNull() ) {
            leftButton = new MenuButton(0, ls, this, true);
            leftButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
            leftButton->setGeometry(120, 280, 60, 20);
            dButtonList->appendMenuButton(leftButton);
            leftButton->setCentered(true);
        }
        if ( !ms.isNull() ) {
            middleButton = new MenuButton(1, ms, this, true);
            middleButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
            middleButton->setGeometry(120, 280, 60, 20);
            dButtonList->appendMenuButton(middleButton);
            middleButton->setCentered(true);
        }
        if ( !rs.isNull() ) {
            rightButton = new MenuButton(2, rs, this, true);
            rightButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
            rightButton->setGeometry(120, 280, 60, 20);
            dButtonList->appendMenuButton(rightButton);
            rightButton->setCentered(true);
        }
    }
    if ( leftButton ) {
        if ( rightButton || middleButton ) {
            leftButton->move(margin, height() - leftButton->height() - margin);
        } else {
            leftButton->move(((width() - leftButton->width()) / 2), height() - leftButton->height() - margin);
        }
    }
    if ( middleButton ) {

        middleButton->move(((width() - middleButton->width()) / 2), height() - middleButton->height() - margin);

    }
    if ( rightButton || middleButton ) {
        if ( leftButton ) {
            rightButton->move(width() - rightButton->width() - margin, height() - rightButton->height() - margin);
        } else {
            rightButton->move(((width() - rightButton->width()) / 2), height() - rightButton->height() - margin);
        }
    }

    if ( dButtonList ) {
        dButtonList->setVisible(true);
        connect(dButtonList, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
    }

    if ( background && parentWidget() ) {
        doTinted = true;
        createTintedBackground(background);
    } else {
        doTinted = false;
    }
}

void GameDialog::buttonClicked(int b)
{
    hide();
    done(b);
}

// Make the source image only half as bright
void GameDialog::createTintedBackground(QPixmap *back)
{
    QRect r = parentWidget()->geometry();
    tinted = QPixmap(width(), height());
    QPainter p(&tinted);
    p.drawPixmap(QPoint(0, 0), *back, QRect(x()-r.x(), y()-r.y(), width(), height()));
    p.end();
    QImage img = tinted.convertToImage();     // this one is slow like hell !!!!
    img = img.convertToFormat(QImage::Format_RGB32);
    QRgb *pixel = (QRgb*)img.bits();
    QRgb *pixel2 = pixel + (img.width() * img.height());
    while ( pixel != pixel2 ) {
        *pixel++ = qRgb(qRed(*pixel) >> 1, qGreen(*pixel) >> 1, qBlue(*pixel) >> 1);
    }
    tinted.convertFromImage(img);
}
