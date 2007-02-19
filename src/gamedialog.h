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

#ifndef GAMEDIALOG_H
#define GAMEDIALOG_H

#include <qdialog.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qevent.h>

class QTimer;
class QLineEdit;
class MenuButton;
class MenuButtonList;

/*
 * Quck and dirty adaptation of standard modal QDialog for the game.
 */
class GameDialog : public QDialog
{
    Q_OBJECT

public:
    GameDialog( QWidget *parent = 0);
    void show();
    void hide ();

    void configure(QPixmap *background, const QString &text, bool fancy, bool requester = false, int timeout = 0, const QString &ls = QString::null,
                   const QString &ms = QString::null, const QString &rs = QString::null);

    inline QString requesterText()
    {
        return reqText;
    }

protected:
    virtual void paintEvent ( QPaintEvent * ) ;
    virtual void keyPressEvent ( QKeyEvent * e );
    virtual void resizeEvent ( QResizeEvent * );

private slots:
    void timeSynch();
    void buttonClicked(int);

private:
    enum DEFINES
    {
        NUM_FW = 3,
        NUM_FP = 32,
    };

    void updateFireworks();
    void createTintedBackground(QPixmap *back);

    QString dText;
    bool needsButtons;
    MenuButton *leftButton;
    MenuButton *middleButton;
    MenuButton *rightButton;
    MenuButtonList *dButtonList;
    QLineEdit *lEdit;
    int tCounter;
    int margin;
    int textWidth;
    int fireWorks[NUM_FW];
    int firePoints[NUM_FP*NUM_FW];
    int fcount[NUM_FW];
    qreal xc[NUM_FP*NUM_FW];
    qreal yc[NUM_FP*NUM_FW];
    qreal xa[NUM_FP*NUM_FW];
    qreal ya[NUM_FP*NUM_FW];
    QTimer *timer;
    static int numFW;
    static int numFP;
    bool doFancy;
    QPixmap tinted;
    bool doTinted;
    QString reqText;

    QImage textLightCache;
    QImage textDarkCache;
};

#endif // GAMEDIALOG_H
