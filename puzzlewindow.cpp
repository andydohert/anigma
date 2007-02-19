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

#include <qsettings.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qfontmetrics.h>
#include <qbitmap.h>
#include <qfile.h>
#include <qtextstream.h>
#include <QKeyEvent>
#include <qdir.h>
 #include <QGradient>
#include "levels.dat"
#include "soundrepository.h"
#include "puzzlewindow.h"
#include "imagerepository.h"
#include "aboutdialog.h"
#include "filedialog.h"
#include "puzzle.h"
#include "menubutton.h"
#include "gamedialog.h"
#include "optionsdialog.h"
#include <qdebug.h>
#include <stdlib.h>
#include <time.h>

PuzzleWindow::PuzzleWindow(QWidget * parent): QMainWindow(parent)
{
    infoBar = new InfoBar(this);
    infoBar->setGeometry(0, 0, 240, 24);
    infoBar->hide();
    
    srand(time(NULL));
    QColor black(0, 0, 0);
    QColor blue(0, 148, 255);
    setFont(QFont("Helvetica", 10, QFont::Bold));
    QPalette pal = palette();
    QColorGroup cg = pal.active();
    cg.setColor(QColorGroup::Base, black);
    cg.setColor(QColorGroup::Background, black);
    cg.setColor(QColorGroup::Button, black);
    cg.setColor(QColorGroup::ButtonText, blue);
    cg.setColor(QColorGroup::Midlight, blue);
    cg.setColor(QColorGroup::Shadow, blue);
    cg.setColor(QColorGroup::Dark, blue);
    cg.setColor(QColorGroup::Light, black);
    cg.setColor(QColorGroup::Mid, black);
    pal.setActive(cg);
    pal.setInactive(cg);
    pal.setDisabled(cg);
    setPalette(pal);

    setWindowTitle("Puzz-le");
    timer = 0;
    ox = 0;
    oy = 0;
    isPaused = false;
    dlg = new GameDialog(this);
    aboutDialog = new AboutDialog(this);
    fileDialog = new FileDialog(QString(getenv("HOME")) + QString("/puzz-le"), this);
    play = 0;
    connect(fileDialog, SIGNAL(loadSavedGame(const QString &)), this, SLOT(loadGame(const QString &)));
    connect(fileDialog, SIGNAL(done(void)), this, SLOT(fileDialogDone(void)));
    init();
    switchState(InfoBar::WELCOME);
    resize(240, 320);
}

PuzzleWindow::~PuzzleWindow()
{
    timer->stop();
    delete pix;
    delete Puzzle::sounds;
    delete Puzzle::images;
    delete play;
}

// Game main state machine
void PuzzleWindow::switchState(InfoBar::GAME_STATE s)
{
    infoBar->state = s;
    switch (infoBar->state) {
    case InfoBar::WELCOME:
        timer->stop();
        mainButtonList->setVisible(true);
        aboutButtonList->setVisible(false);
        gameButtonList->setVisible(false);
        aboutDialog->hide();
        fileDialog->hide();
        playGame->setFocus();
        update();
        break;
    case InfoBar::ABOUT:
        timer->stop();
        mainButtonList->setVisible(false);
        aboutButtonList->setVisible(true);
        playDemo->show();
        gameButtonList->setVisible(false);
        aboutDialog->setMode(AboutDialog::ABOUT);
        aboutDialog->show();
        fileDialog->hide();
        update();
        break;
    case InfoBar::HISTORY:
        timer->stop();
        mainButtonList->setVisible(false);
        aboutButtonList->setVisible(true);
        playDemo->hide();
        gameButtonList->setVisible(false);
        aboutDialog->setMode(AboutDialog::HISTORY);
        aboutDialog->show();
        fileDialog->hide();
        update();
        break;
    case InfoBar::BROWSE:
        timer->stop();
        mainButtonList->setVisible(false);
        aboutButtonList->setVisible(false);
        gameButtonList->setVisible(false);
        aboutDialog->hide();
        fileDialog->refresh();
        fileDialog->show();
        fileDialog->setFocus();
        break;
    case InfoBar::GAME:
        mainButtonList->setVisible(false);
        aboutButtonList->setVisible(false);
        gameButtonList->setVisible(true);
        aboutDialog->hide();
        fileDialog->hide();
        setFocus();
        initGame();
        startLevel();
        break;
    case InfoBar::DEMO:
        play->currentLevel = -1;
        if (!demo.initDemo()) {
            errorMsg("Unable to play demo !");
            switchState(InfoBar::ABOUT);
        } else {
            infoBar->totalPoints = 0;
            nextLevelLoaded();
        }
        break;
    default:
        break;
    }
}

// Standard accelerators are not good enough here. We want plain key presses without any of the keyboard modifiers (CTRL, ALT etc )
// This would probably be better handled by creating filter.
// Just forward it to coresponding handler for button press ...
void PuzzleWindow::keyPressEvent(QKeyEvent * e)
{
    if (!isPaused) {
        switch (e->key()) {
        case Qt::Key_Escape:
            if (gameState() == InfoBar::ABOUT || gameState() == InfoBar::HISTORY)
                buttonClicked(BACK_TO_WELCOME_FROM_ABOUT);
            else if (gameState() == InfoBar::GAME)
                buttonClicked(BACK_TO_WELCOME_FROM_GAME);
            break;
        case Qt::Key_F33:
            if (gameState() == InfoBar::GAME) {
                buttonClicked(RESTART_LEVEL);
            } else if (gameState() == InfoBar::ABOUT) {
                buttonClicked(PLAY_DEMO);
            }
            break;

        case Qt::Key_Space:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
            if (gameState() == InfoBar::GAME) {
                QPoint p = play->selectedPositon(e->key());
                generateMovement(p.x() + ox, p.y() + oy, true);
            }
            break;
        default:
            break;
        }
    }
}

// If we have game in progress, call Playground mouse press handler.
// Record the move if required.

void PuzzleWindow::mousePressEvent(QMouseEvent * e)
{
    if (gameState() == InfoBar::GAME && !isPaused) {
        generateMovement(e->x(), e->y());
    }
}

void PuzzleWindow::generateMovement(int x, int y, bool keyBoardMode)
{
    if (Puzzle::recordGames)
        demo.recordDemo(DemoMove(x - ox, y - oy, play->tickValue()));
    if (play->setSelected(x - ox, y - oy, keyBoardMode)) {
        update();
    }
}

void PuzzleWindow::closeEvent(QCloseEvent * e)
{
    QSettings cfg("Puzz_le");
    cfg.beginGroup("Puzz_le");
    cfg.setValue("timeoutValue", Puzzle::timeoutValue);
    cfg.setValue("progressSaving", (int) Puzzle::progressSaving);
    cfg.setValue("playSounds", (int) Puzzle::playSound);
    cfg.setValue("currentLevelName", Puzzle::currLevelsName);
    cfg.setValue("timeLimit", (int) Puzzle::timeLimit);
    cfg.setValue("theme", Puzzle::theme);
    e->accept();
}

void PuzzleWindow::resizeEvent(QResizeEvent *)
{
    if (width() == 0)
        return;
    if (!Puzzle::images) {
        initImages();
        Puzzle::images->shuffleTheme();
        play = new Playground();
        infoBar->play = play;
        demo.play = play;
    }

    aboutDialog->setGeometry(4, 80, width() - 8, height() - 110);
    fileDialog->setGeometry(4, 80, width() - 8, height() - 80);

    playGame->setGeometry(110, 118, 145, 25);
    optionsGame->setGeometry(playGame->x(), playGame->y() + 23, 145, playGame->height());
    aboutGame->setGeometry(playGame->x(), optionsGame->y() + 23, 145, playGame->height());
    historyGame->setGeometry(playGame->x(), aboutGame->y() + 23, 145, playGame->height());
    loadSavedGame->setGeometry(playGame->x(), historyGame->y() + 23, 145 , playGame->height());
    quitGame->setGeometry(playGame->x(), loadSavedGame->y() + 23, 145, playGame->height());

    backToWelcomeFromAbout->setGeometry(4,
                                        height() - playGame->height() - 4,
                                        110, playGame->height());
    playDemo->setGeometry(width() - 110 - 4,
                          height() - playGame->height() - 4, 110,
                          playGame->height());
    backToWelcomeFromGame->setGeometry(4,
                                       height() - playGame->height() - 4,
                                       75, playGame->height());
    pauseGame->setGeometry((width() - 75) / 2,
                           height() - playGame->height() - 4, 75,
                           playGame->height());
    restartCurrent->setGeometry(width() - 75 - 4,
                                height() - playGame->height() - 4, 75,
                                playGame->height());
    pix->fill();
}

void PuzzleWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPainter lp(pix);
    switch (gameState()) {
    case InfoBar::WELCOME:
    case InfoBar::BROWSE:
        infoBar->hide();
        pix->fill(QColor(0, 0, 0));
        drawTitleScreen(&lp);
        break;
    case InfoBar::GAME:
    case InfoBar::DEMO:
        infoBar->show();
        drawPlayground(&p, &lp, true);
        break;
    case InfoBar::ABOUT:
    case InfoBar::HISTORY:
        infoBar->hide();
        pix->fill(QColor(0, 0, 0));
        drawAboutScreen(&lp);
        break;
    default:
        break;
    }

    if (gameState() != InfoBar::GAME && gameState() != InfoBar::DEMO)
        p.drawPixmap(0, 0, *pix);
    QMainWindow::paintEvent(event);
}

void PuzzleWindow::drawTitleScreen(QPainter * p)
{
    if (Puzzle::images) {
        QPixmap tmp = Puzzle::images->findPixmap("front_title");
        p->drawPixmap(10, 10, tmp);
            if (gameState() != InfoBar::BROWSE) {
            tmp = Puzzle::images->findPixmap("front_1");
            p->drawPixmap(10, loadSavedGame->y() + loadSavedGame->height() - 40, tmp);
            p->setPen(QColor(0, 135, 234));
            MenuButton::drawColorizedText("Copyright (C) 2001 Walter Rawdanik",
                                          10, 80, p, menuClr, 150);
            MenuButton::drawColorizedText("Copyright (C) 2005 Benjamin Meyer", 10,
                                          95, p, menuClr, 150);
            MenuButton::drawColorizedText("ver: " + Puzzle::gameVersion, 174,
                                          278, p, menuClr, 150);
        }
    }
}

// Update screen using background pixmap (which is in turn updated by Playground::painPlayground() handler )
// repainting only stripes which are marked dirty (1) in QBitArray returned by Playground::painPlayground() handler.
// If neccesary update score line ( level #, time and score)
void PuzzleWindow::drawPlayground(QPainter * p, QPainter * lp, bool drawAll)
{
    if (Puzzle::images) {
        if (play->gameState() != Playground::INVALID) {
            bool deleteP = false;
            bool deleteLP = false;
            if (!p) {
                p = new QPainter(this);
                deleteP = true;
            }
            if (!lp) {
                lp = new QPainter(pix);
                deleteLP = true;
            }
            if (isPaused) {
                p->fillRect(0, 0, pix->width(), pix->height(),
                            QColor(0, 0, 0));
                QPixmap tmp = Puzzle::images->findPixmap("front_title");
                p->drawPixmap(18, 36, tmp);
                // hardcoded position
                MenuButton::drawColorizedText("Game Paused", 80, 140, p,
                                              QColor(0, 148, 255), 150);
                MenuButton::
                drawColorizedText("Click 'Resume' to unpause the Game",
                                  30, 170, p, QColor(0, 148, 255),
                                  150);

            } else {
                QBitArray *stripes = play->paintPlayground(lp, ox, oy, drawAll);
                unsigned int i;
                if (drawAll) {
                    p->drawPixmap(0, 0, *pix);
                } else {
                    for (i = 0; i < (uint) stripes->size(); ++i) {
                        if (stripes->at(i)) {
                            p->drawPixmap(i * Puzzle::blockPixelSize + ox, oy, *pix,
                                          i * Puzzle::blockPixelSize + ox, oy,
                                          Puzzle::blockPixelSize,
                                          play->height() * Puzzle::blockPixelSize);
                        }
                    }
                }
                stripes->fill(false);
                if (play->isInfoChange() || drawAll) {
                    updateInfoBar();
                }
            }
            if (deleteP)
                delete p;
            if (deleteLP)
                delete lp;
        }
    }
}

void PuzzleWindow::drawAboutScreen(QPainter * p)
{
    if (Puzzle::images) {
        p->drawPixmap(10, 10, Puzzle::images->findPixmap("front_title"));
    }
}

// Slot method called by QTimer handler.
// This method calls Playground.update() which updates game state and returns
// true if game screen needs to be repainted.
// After each update Playground class is checked for any changes in
// the game state ( like finished level or out_of_time condition.)
void PuzzleWindow::synchTimeout()
{
    if (gameState() == InfoBar::DEMO) {
        if (demo.timeout(play->tickValue()))
            update();
        if (play->update())
            update();
        if (play->gameState() == Playground::OVER) {
            demoCompleted();
        }
    } else {
        if (gameState() == InfoBar::GAME && play->update()) {
            update();
            if (play->gameState() == Playground::OVER) {
                if (Puzzle::recordGames) {
                    demo.saveDemo();
                }
                if (play->isGameWon()) {
#ifdef DEMO_VERSION
                    if (play->currentLevel < play->totalLevels()
                            && play->currentLevel < 8) {
                        levelCompleted();
                        play->currentLevel++;
                        startLevel();
                    } else {
                        gameCompleted();
                    }
#else
                    if (play->currentLevel < play->totalLevels()) {
                        levelCompleted();
                        play->currentLevel++;
                        startLevel();
                    } else {
                        gameCompleted();
                    }
#endif
                } else {
                    outOfTime();
                }
            }
        }
    }
}

// Initalizes game images.
// Should be called AFTER main window is initialized ( this method needs to know final size of our main game window.)
void PuzzleWindow::initImages()
{
    Puzzle::initImages();
    pix = new QPixmap(width(), height());
    pix->fill();
}

void PuzzleWindow::init()
{
    if (!timer) {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(synchTimeout()));
    }
    Puzzle::initSounds(ROOTHOME + "sounds/puzz-le");

    menuClr.setRgb(0, 148, 255);
    hMenuClr.setRgb(255, 255, 255);

    mainButtonList = new MenuButtonList(this);
    playGame = new MenuButton(PLAY_GAME, "Play the game", this);
    playGame->setColors(menuClr, QColor(0, 0, 0));
    optionsGame = new MenuButton(OPTIONS_GAME, "Options", this);
    optionsGame->setColors(menuClr, QColor(0, 0, 0));
    aboutGame = new MenuButton(ABOUT_GAME, "Instructions", this);
    aboutGame->setColors(menuClr, QColor(0, 0, 0));
    historyGame = new MenuButton(HISTORY_GAME, "Changes", this);
    historyGame->setColors(menuClr, QColor(0, 0, 0));
    loadSavedGame = new MenuButton(LOAD_GAME, "Load saved game", this);
    loadSavedGame->setColors(menuClr, QColor(0, 0, 0));
    quitGame = new MenuButton(QUIT, "Quit", this);
    quitGame->setColors(menuClr, QColor(0, 0, 0));
    mainButtonList->appendMenuButton(playGame);
    mainButtonList->appendMenuButton(optionsGame);
    mainButtonList->appendMenuButton(aboutGame);
    mainButtonList->appendMenuButton(historyGame);
    mainButtonList->appendMenuButton(loadSavedGame);
    mainButtonList->appendMenuButton(quitGame);

    aboutButtonList = new MenuButtonList(this);
    backToWelcomeFromAbout = new MenuButton(BACK_TO_WELCOME_FROM_ABOUT, "Go Back", this, true);
    backToWelcomeFromAbout->setColors(menuClr, QColor(0, 0, 0));
    backToWelcomeFromAbout->setCentered(true);
    playDemo = new MenuButton(PLAY_DEMO, "Demo Game", this, true);
    playDemo->setColors(menuClr, QColor(0, 0, 0));
    playDemo->setCentered(true);
    aboutButtonList->appendMenuButton(backToWelcomeFromAbout);
    aboutButtonList->appendMenuButton(playDemo);

    gameButtonList = new MenuButtonList(this);
    backToWelcomeFromGame = new MenuButton(BACK_TO_WELCOME_FROM_GAME, "Abort", this, true);
    backToWelcomeFromGame->setFocusPolicy(Qt::NoFocus);
    backToWelcomeFromGame->setColors(menuClr, QColor(0, 0, 0));
    backToWelcomeFromGame->setCentered(true);

    pauseGame = new MenuButton(PAUSE_GAME, "Pause", this, true);
    pauseGame->setFocusPolicy(Qt::NoFocus);
    pauseGame->setColors(menuClr, QColor(0, 0, 0));
    pauseGame->setCentered(true);

    restartCurrent = new MenuButton(RESTART_LEVEL, "Restart", this, true);
    restartCurrent->setFocusPolicy(Qt::NoFocus);
    restartCurrent->setColors(menuClr, QColor(0, 0, 0));
    restartCurrent->setCentered(true);
    gameButtonList->appendMenuButton(backToWelcomeFromGame);
    gameButtonList->appendMenuButton(pauseGame);
    gameButtonList->appendMenuButton(restartCurrent);

    connect(mainButtonList, SIGNAL(clicked(int)), this,
            SLOT(buttonClicked(int)));
    connect(aboutButtonList, SIGNAL(clicked(int)), this,
            SLOT(buttonClicked(int)));
    connect(gameButtonList, SIGNAL(clicked(int)), this,
            SLOT(buttonClicked(int)));
}

void PuzzleWindow::buttonClicked(int id)
{
    if (id == PAUSE_GAME) {
        if (isPaused) {
            isPaused = false;
            pauseGame->setText("Pause");
            restartCurrent->show();
            backToWelcomeFromGame->show();
            timer->start(Puzzle::timeoutValue);
            update();
        } else {
            pauseGame->setText("Resume");
            restartCurrent->hide();
            backToWelcomeFromGame->hide();
            timer->stop();
            isPaused = true;
            update();
        }
    }

    if (!isPaused) {
        switch (id) {
        case PLAY_GAME:
            switchState(InfoBar::GAME);
            break;
        case OPTIONS_GAME: {
            int gs = Puzzle::timeoutValue;
            bool se = Puzzle::playSound;
            bool ar = Puzzle::progressSaving;
            bool tl = Puzzle::timeLimit;

            QString cTheme = Puzzle::theme;
            OptionsDialog *optionsDialog =
                new OptionsDialog(&Puzzle::currLevelsName, &cTheme,
                                  &se, &ar, &tl, &gs, this);
            if (optionsDialog->exec() == QDialog::Accepted) {
                Puzzle::timeoutValue = gs;
                Puzzle::playSound = se;
                Puzzle::progressSaving = ar;
                Puzzle::timeLimit = tl;
                Puzzle::theme = cTheme;
                if (!Puzzle::images->initTheme(Puzzle::theme)) {
                    Puzzle::theme = "Default";
                    Puzzle::images->initTheme(Puzzle::theme);
                }
            }
        }
        break;
        case HISTORY_GAME:
            switchState(InfoBar::HISTORY);
            break;
        case ABOUT_GAME:
            switchState(InfoBar::ABOUT);
            break;
        case BACK_TO_WELCOME_FROM_ABOUT:
            switchState(InfoBar::WELCOME);
            break;
        case BACK_TO_WELCOME_FROM_GAME:
            abortGame();
            break;
        case PLAY_DEMO:
            switchState(InfoBar::DEMO);
            break;
        case RESTART_LEVEL:
            restartLevel();
            break;
        case QUIT:
            close();
            break;
        case LOAD_GAME:
            switchState(InfoBar::BROWSE);
            break;
        default:
            break;
        }
    }
}

void PuzzleWindow::updateInfoBar()
{
    infoBar->update();
}

void InfoBar::paintEvent(QPaintEvent *) {
    if (!play)
        return;
    QString msg;
    int rx = 46;
    if (state == InfoBar::DEMO) {
        rx += 6;
        msg.sprintf("DEMO  TIME: %03d  SCORE: %05d", play->timeLimit(),
                     totalPoints + play->currentPoints());
    } else {
        if (Puzzle::timeLimit) {
            if (play->gameType() == Playground::TIME_BASED)
                msg.sprintf("L: %02d   TIME: %03d   SCORE: %05d",
                            play->currentLevel, play->timeLimit(),
                            totalPoints + play->currentPoints());
            else
                msg.sprintf("L: %02d   MOV: %03d   SCORE: %05d",
                            play->currentLevel, play->timeLimit(),
                            totalPoints + play->currentPoints());
        } else
            msg.sprintf("LEVEL: %02d   SCORE: %05d", play->currentLevel,
                        totalPoints + play->currentPoints());
    }

    QPainter p(this);
    QLinearGradient gradient(width()/2, 0, width()/2, height());
    QColor start(0,179,255);
    QColor end(start);
    end = end.dark(100 + (height()*100 / height()));
    gradient.setColorAt(0, start);
    gradient.setColorAt(1, end);
    p.setPen(QPen(gradient, 0));
    QFont font("Helvetica", 16, QFont::Bold);
    int length = width() * 2;
    while(length > width()) {
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


void PuzzleWindow::initGame()
{
    play->currentLevel = Puzzle::startLevel;
    demo.clearRecording();
    play->clearPoints();
    infoBar->totalPoints = 0;
    timer->start(Puzzle::timeoutValue);
}

// Called when loading next level
void PuzzleWindow::startLevel(bool shuffle)
{
    if (shuffle)
        Puzzle::images->shuffleTheme();
    infoBar->totalPoints += play->currentPoints();
    
    bool result;
    if (Puzzle::levelFileName.isEmpty() && Puzzle::currLevelsName.isEmpty())
        result = play->loadLevel(levels, play->currentLevel);
    else {
        if (!Puzzle::levelFileName.isEmpty())
            result = play->loadLevel(Puzzle::levelFileName, play->currentLevel);
        else {
            result =
                play->loadLevel(ROOTHOME + "pics/puzz-le/levels/" +
                                Puzzle::currLevelsName + "/levels",
                                play->currentLevel);
        }
    }
    if (result) {
        nextLevelLoaded();
    } else {
        switchState(InfoBar::WELCOME);
        errorMsg("Unable to load game levels.");
    }
}

void PuzzleWindow::nextLevelLoaded()
{
    ox = (width() - play->width() * Puzzle::blockPixelSize) / 2;
    oy = ((height() - play->height() * Puzzle::blockPixelSize) / 2);
    oy = qMax(oy, infoBar->height());
    play->setGameState(Playground::INPROGRESS);
    timer->start(Puzzle::timeoutValue);
    aboutDialog->hide();
    update();
}

void PuzzleWindow::gameCompleted()
{
    timer->stop();
    dlg->configure(pix, "* Game Completed *", true, false, 0, " OK");
    dlg->show();
    dlg->exec();
    switchState(InfoBar::WELCOME);
}

void PuzzleWindow::levelCompleted()
{
    QString tmp;
    tmp.sprintf("* Level %d completed *", play->currentLevel);
    dlg->configure(pix, tmp, true, false, 1800);
    dlg->show();
    dlg->exec();
}

void PuzzleWindow::demoCompleted()
{
    timer->stop();
    if (Puzzle::demoLoop) {
        QString tmp;
        tmp.sprintf("* Demo Level %d completed *", play->currentLevel);
        dlg->configure(pix, tmp, true, false, 1800);
        dlg->show();
        dlg->exec();

        play->currentLevel++;
        infoBar->totalPoints += play->currentPoints();
        if (!demo.initDemo()) {
            play->currentLevel = -1;
            if (!demo.initDemo()) {
                switchState(InfoBar::ABOUT);     // if somebody deleted starting files
                                        // we better get out to avoid infinite loop
            } else {
                infoBar->totalPoints = 0;
                nextLevelLoaded();
            }
        } else {
            nextLevelLoaded();
        }
    } else {
        dlg->configure(pix, "* Demo Completed *", true, false, 0, " OK");
        dlg->show();
        dlg->exec();
        switchState(InfoBar::ABOUT);
    }
}

void PuzzleWindow::restartLevel()
{
    timer->stop();
    dlg->configure(pix, "Restart current level ?", false, false, 0, " Yes",
                   QString::null, "No");
    if (dlg->exec() == 0) {
        play->clearPoints();
        if (infoBar->totalPoints >= Puzzle::blockBonus)
            infoBar->totalPoints -= Puzzle::blockBonus;
        else
            infoBar->totalPoints = 0;
        startLevel(false);
    }
    timer->start(Puzzle::timeoutValue);
}

void PuzzleWindow::outOfTime()
{
    timer->stop();
    QString text;
    if (play->gameType() == Playground::TIME_BASED)
        text = "Sorry - Out of Time !";
    else
        text = "Sorry - Out of Moves !";

    dlg->configure(pix, text, false, false, 0, "Again", QString::null,
                   "Quit");
    if (dlg->exec() == 0) {
        play->clearPoints();
        if (infoBar->totalPoints >= Puzzle::blockBonus)
            infoBar->totalPoints -= Puzzle::blockBonus;
        else
            infoBar->totalPoints = 0;
        startLevel(false);
        timer->start(Puzzle::timeoutValue);
    } else {
        switchState(InfoBar::WELCOME);
    }
}

void PuzzleWindow::abortGame()
{
    timer->stop();
    if (Puzzle::progressSaving) {
        dlg->configure(pix, "Save the current game?", false, true, 0,
                       "Yes", "No", "Cancel");
        dlg->show();
        switch (dlg->exec()) {
        case 0:
            saveGameState(dlg->requesterText());
            switchState(InfoBar::WELCOME);
            break;
        case 1:
            switchState(InfoBar::WELCOME);
            break;
        case 2:
            timer->start(Puzzle::timeoutValue);
            break;
        default:
            break;
        }
    } else
        switchState(InfoBar::WELCOME);
}

void PuzzleWindow::errorMsg(const QString & error)
{
    dlg->configure(0, error, false, false, 0, " OK");
    dlg->exec();
}

void PuzzleWindow::saveGameState(const QString & fileName)
{
    QDir dir(QString(getenv("HOME")) + QString("/puzz-le"));
    if (!dir.exists()) {
        dir.mkdir(dir.absolutePath());
    }
    QString tmp = dir.absolutePath() + "/" + fileName;
    if (!Puzzle::levelFileName.isEmpty())
        play->savePlayground(tmp.append(".puzzle"), infoBar->totalPoints,
                             Puzzle::levelFileName);
    else
        play->savePlayground(tmp.append(".puzzle"), infoBar->totalPoints,
                             Puzzle::currLevelsName);
}

void PuzzleWindow::loadGame(const QString & fileName)
{
    QString tmp(QString(getenv("HOME")) + QString("/puzz-le/") + fileName);

    if (!loadGameState(tmp)) {
        errorMsg("Unable to load saved game.");
    }
}
void PuzzleWindow::fileDialogDone()
{
    switchState(InfoBar::WELCOME);
}

bool PuzzleWindow::loadGameState(const QString & fileName)
{
    QString oldCurrLevelsName = Puzzle::currLevelsName;
    if (play->loadPlayground(fileName, &infoBar->totalPoints, levels)) {
        aboutDialog->hide();
        fileDialog->hide();
        infoBar->state = InfoBar::GAME;
        play->currentLevel = play->number();
        mainButtonList->setVisible(false);
        aboutButtonList->setVisible(false);
        gameButtonList->setVisible(true);
        demo.clearRecording();
        ox = (width() - play->width() * Puzzle::blockPixelSize) / 2;
        update();
        if (Puzzle::currLevelsName != oldCurrLevelsName) {
            QString tmp;
            if (!Puzzle::currLevelsName.isEmpty())
                tmp.sprintf("Switching to %s levels set.",
                            Puzzle::currLevelsName.toLatin1().constData());
            else
                tmp.sprintf("Switching to default levels set.");
            dlg->configure(pix, tmp, true, false, 1800);
            dlg->show();
            dlg->exec();
        }
        timer->start(Puzzle::timeoutValue);
        return true;
    } else {
        Puzzle::currLevelsName = "";    // restore default levels set
    }
    return false;
}
