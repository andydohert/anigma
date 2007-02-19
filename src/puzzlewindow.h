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

#ifndef PUZZLEWINDOW_H
#define PUZZLEWINDOW_H

#include <qmainwindow.h>
#include "playground.h"
#include "demo.h"
#include "infobar.h"

class QTimer;
class MenuButton;
class MenuButtonList;
class QPixmap;
class QBitmap;
class GameDialog;
class AboutDialog;
class FileDialog;

class PuzzleWindow: public QMainWindow
{

Q_OBJECT

public:
    enum BUTTONS {
        PLAY_GAME,
        OPTIONS_GAME,
        ABOUT_GAME,
        HISTORY_GAME,
        LOAD_GAME,
        BACK_TO_WELCOME_FROM_ABOUT,
        BACK_TO_WELCOME_FROM_GAME,
        PLAY_DEMO,
        RESTART_LEVEL,
        PAUSE_GAME,
        QUIT
    };

    PuzzleWindow(QWidget * parent = 0);
    ~PuzzleWindow();

    inline InfoBar::GAME_STATE gameState()
    {
        return infoBar->state();
    };

public slots:
    void switchState(InfoBar::GAME_STATE s);

protected:
    void keyPressEvent(QKeyEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void paintEvent(QPaintEvent * e);
    void resizeEvent(QResizeEvent * e);
    void closeEvent(QCloseEvent * e);

private slots:
    void synchTimeout();
    void buttonClicked(int id);
    void loadGame(const QString & fileName);
    void fileDialogDone();

private:
    void generateMovement(int x, int y, bool keyBoardMode = false);
    void drawTitleScreen(QPainter *p);
    void drawPlayground(QPainter *p = 0, QPainter *lp = 0, bool drawAll = false);
    void drawAboutScreen(QPainter *p);
    void initGame();
    void startLevel(bool shuffle = true);
    void restartLevel();
    void outOfTime();
    void abortGame();
    void saveGameState(const QString & fileName);
    bool loadGameState(const QString & fileName);
    void init();
    void initImages();
    void errorMsg(const QString & error);

    void levelCompleted();
    void gameCompleted();
    void demoCompleted();
    void nextLevelLoaded();

    QTimer *timer;
    QColor menuClr;
    QColor hMenuClr;

    MenuButton *playGame;
    MenuButton *optionsGame;
    MenuButton *aboutGame;
    MenuButton *historyGame;
    MenuButton *loadSavedGame;
    MenuButton *quitGame;
    MenuButtonList *mainButtonList;

    MenuButton *backToWelcomeFromAbout;
    MenuButton *playDemo;
    MenuButtonList *aboutButtonList;

    MenuButton *backToWelcomeFromGame;
    MenuButton *restartCurrent;
    MenuButton *pauseGame;
    MenuButtonList *gameButtonList;

    InfoBar *infoBar;

    // Title Screen Variables
    QPixmap *pix;
    Playground *play;
    int ox;
    int oy;
    bool isPaused;
    GameDialog *dlg;
    AboutDialog *aboutDialog;
    FileDialog *fileDialog;
    Demo demo;
};

#endif // PUZZLE_WINDOW_H
