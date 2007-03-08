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

#include "puzzlewindow.h"

#include <qsettings.h>
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
#include <qdebug.h>
#include <time.h>

#include "levels.dat"
#include "soundrepository.h"
#include "imagerepository.h"
#include "aboutdialog.h"
#include "filedialog.h"
#include "puzzle.h"
#include "menubutton.h"
#include "gamedialog.h"
#include "optionsdialog.h"
#include <qapplication.h>

class Board : public QWidget {
public:
    Board(QWidget * parent) : QWidget(parent), play(0), isPaused(false) {}
    Playground *play;
    InfoBar *infoBar;
    bool isPaused;
protected:
    void paintEvent(QPaintEvent * e);
};

PuzzleWindow::PuzzleWindow(QWidget * parent): QMainWindow(parent)
{
    infoBar = new InfoBar(this);
    infoBar->setGeometry(0, 0, 240, 24);
    infoBar->hide();

    srand(time(NULL));
    setFont(QFont("Helvetica", 10, QFont::Bold));

    QPalette pal = palette();
    blue = QColor(0, 148, 255);
    pal.setColor(QPalette::Base, Qt::black);
    pal.setColor(QPalette::Background, Qt::black);
    pal.setColor(QPalette::Button, Qt::black);
    pal.setColor(QPalette::ButtonText, blue);
    pal.setColor(QPalette::WindowText, blue);
    QApplication::setPalette(pal);

    setWindowTitle("Puzz-le");
    timer = 0;
    dlg = new GameDialog(this);
    fileDialog = new FileDialog(QString(QDir::homePath()) + QString("/.puzz-le"), this);
    play = 0;
    connect(fileDialog, SIGNAL(loadSavedGame(const QString &)), this, SLOT(loadGame(const QString &)));
    connect(fileDialog, SIGNAL(done(void)), this, SLOT(fileDialogDone(void)));
    init();
    aboutDialog = new AboutDialog(this);
    connect(aboutDialog, SIGNAL(showWelcomeScreen()), this, SLOT(showWelcomeScreen()));
    connect(aboutDialog, SIGNAL(showDemo()), this, SLOT(showDemo()));

    board = new Board(this);
    board->infoBar = infoBar;

    switchState(InfoBar::WELCOME);
    resize(240, 300);
    setMinimumWidth(240);
    setMaximumWidth(240);
    setMinimumHeight(300);
    setMaximumHeight(300);
}

PuzzleWindow::~PuzzleWindow()
{
    timer->stop();
    delete Puzzle::sounds;
    delete Puzzle::images;
    delete play;
}

// Game main state machine
void PuzzleWindow::switchState(InfoBar::GAME_STATE s)
{
    infoBar->setState(s);
    switch (infoBar->state()) {
    case InfoBar::WELCOME:
        timer->stop();
        mainButtonList->setVisible(true);
        gameButtonList->setVisible(false);
        aboutDialog->hide();
        fileDialog->hide();
        board->hide();
        infoBar->hide();
        playGameButton->setFocus();
        update();
        break;
    case InfoBar::ABOUT:
        timer->stop();
        infoBar->hide();
        mainButtonList->setVisible(false);
        gameButtonList->setVisible(false);
        aboutDialog->setMode(AboutDialog::ABOUT);
        aboutDialog->show();
        fileDialog->hide();
        infoBar->hide();
        board->hide();
        update();
        break;
    case InfoBar::HISTORY:
        timer->stop();
        infoBar->hide();
        mainButtonList->setVisible(false);
        gameButtonList->setVisible(false);
        aboutDialog->setMode(AboutDialog::HISTORY);
        aboutDialog->show();
        fileDialog->hide();
        board->hide();
        infoBar->hide();
        update();
        break;
    case InfoBar::BROWSE:
        timer->stop();
        infoBar->hide();
        mainButtonList->setVisible(false);
        gameButtonList->setVisible(false);
        aboutDialog->hide();
        fileDialog->refresh();
        infoBar->hide();
        fileDialog->show();
        board->hide();
        break;
    case InfoBar::GAME:
        mainButtonList->setVisible(false);
        gameButtonList->setVisible(true);
        aboutDialog->hide();
        fileDialog->hide();
        infoBar->show();
        board->show();
        initGame();
        infoBar->show();
        startLevel();
        setFocus();
        break;
    case InfoBar::DEMO:
        infoBar->show();
        board->show();
        play->currentLevel = -1;
        if (!demo.initDemo()) {
            errorMsg("Unable to play demo !");
            switchState(InfoBar::ABOUT);
        } else {
            infoBar->setTotalPoints(0);
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
    if (!board->isPaused) {
        switch (e->key()) {
        case Qt::Key_Escape:
            if (gameState() == InfoBar::ABOUT || gameState() == InfoBar::HISTORY)
                showWelcomeScreen();
            else if (gameState() == InfoBar::GAME)
                abortGame();
            break;
        case Qt::Key_F33:
            if (gameState() == InfoBar::GAME) {
                restartLevel();
            } else if (gameState() == InfoBar::ABOUT) {
                showDemo();
            }
            break;

        case Qt::Key_Space:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
            if (gameState() == InfoBar::GAME) {
                QPoint p = play->selectedPositon(e->key());
                generateMovement(p.x() + board->x(), p.y() + board->y(), true);
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
    if (gameState() == InfoBar::GAME && !board->isPaused) {
        generateMovement(e->x(), e->y());
    }
}

void PuzzleWindow::generateMovement(int x, int y, bool keyBoardMode)
{
    if (Puzzle::recordGames)
        demo.recordDemo(DemoMove(x - board->x(), y - board->y(), play->tickValue()));
    if (play->setSelected(x - board->x(), y - board->y(), keyBoardMode)) {
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
        board->play = play;
        infoBar->play = play;
        demo.play = play;
    }

    aboutDialog->setGeometry(4, 80, width() - 8, height() - 80);
    fileDialog->setGeometry(4, 80, width() - 8, height() - 80);

    playGameButton->setGeometry(110, 118, 145, 25);
    optionsGameButton->setGeometry(playGameButton->x(), playGameButton->y() + 23, 145, playGameButton->height());
    aboutGameButton->setGeometry(playGameButton->x(), optionsGameButton->y() + 23, 145, playGameButton->height());
    historyGameButton->setGeometry(playGameButton->x(), aboutGameButton->y() + 23, 145, playGameButton->height());
    loadSavedGameButton->setGeometry(playGameButton->x(), historyGameButton->y() + 23, 145 , playGameButton->height());
    quitGameButton->setGeometry(playGameButton->x(), loadSavedGameButton->y() + 23, 145, playGameButton->height());

    backToWelcomeFromGame->setGeometry(4,
                                       height() - playGameButton->height() - 4,
                                       75, playGameButton->height());
    pauseGame->setGeometry((width() - 75) / 2,
                           height() - playGameButton->height() - 4, 75,
                           playGameButton->height());
    restartCurrent->setGeometry(width() - 75 - 4,
                                height() - playGameButton->height() - 4, 75,
                                playGameButton->height());
    endDemo->setGeometry(8,height() - endDemo->height() - 4,
                                        110, endDemo->height());
}

// Update screen using background pixmap (which is in turn updated by Playground::painPlayground() handler )
// repainting only stripes which are marked dirty (1) in QBitArray returned by Playground::painPlayground() handler.
// If neccesary update score line ( level #, time and score)
void Board::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!Puzzle::images || play->gameState() == Playground::INVALID)
        return;

    QPainter p(this);
    if (isPaused) {
        p.fillRect(0, 0, width(), height(), QColor(0, 0, 0));
        QPixmap tmp = Puzzle::images->findPixmap("front_title");
        p.drawPixmap(18, 36, tmp);
        // hardcoded position
        MenuButton::drawColorizedText("Game Paused", 80, 140, &p, QColor(0, 148, 255), 150);
        MenuButton::drawColorizedText("Click 'Resume' to unpause the Game",
                          30, 170, &p, QColor(0, 148, 255), 150);

    } else {
        play->paintPlayground(&p, 0, 0, true);
        if (play->isInfoChange())
            infoBar->update();
    }
}

void PuzzleWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    switch (gameState()) {
    case InfoBar::GAME:
    case InfoBar::DEMO: {
        QPixmap tmp = Puzzle::images->findPixmap(ImageRepository::BACKGROUND);
        p.drawPixmap(0, 0, tmp);
        break;
    }
    case InfoBar::WELCOME:
    case InfoBar::BROWSE:
        p.fillRect(QRect(0, 0,width(), height()), Qt::black);
        drawTitleScreen(&p);
        break;
    case InfoBar::ABOUT:
    case InfoBar::HISTORY:
        drawAboutScreen(&p);
        break;
    default:
        break;
    }
    QMainWindow::paintEvent(event);
}

void PuzzleWindow::drawTitleScreen(QPainter * p)
{
    if (Puzzle::images) {
        QPixmap tmp = Puzzle::images->findPixmap("front_title");
        p->drawPixmap(10, 10, tmp);
            if (gameState() != InfoBar::BROWSE) {
            tmp = Puzzle::images->findPixmap("front_1");
            p->drawPixmap(10, loadSavedGameButton->y() + loadSavedGameButton->height() - 40, tmp);
            p->setPen(QColor(0, 135, 234));
            MenuButton::drawColorizedText("Copyright (C) 2001 Walter Rawdanik",
                                          10, 80, p, blue, 150);
            MenuButton::drawColorizedText("Copyright (C) 2005 Benjamin Meyer", 10,
                                          95, p, blue, 150);
            MenuButton::drawColorizedText("ver: " + Puzzle::gameVersion, 174,
                                          278, p, blue, 150);
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
            board->update();
        if (play->update())
            board->update();
        if (play->gameState() == Playground::OVER) {
            demoCompleted();
        }
    } else {
        if (gameState() == InfoBar::GAME && play->update()) {
            board->update();
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
}

void PuzzleWindow::init()
{
    if (!timer) {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(synchTimeout()));
    }
#ifdef Q_OS_MAC
    Puzzle::initSounds("sounds");
#else
    QString location = "/usr/share/puzzle/sounds";
    if (!QFile::exists(location))
        location = "sounds";
    Puzzle::initSounds(location);
#endif
    mainButtonList = new MenuButtonList(this);
    playGameButton = new MenuButton("Play the game", this);
    connect(playGameButton, SIGNAL(clicked()), this, SLOT(playGame()));
    optionsGameButton = new MenuButton("Options", this);
    connect(optionsGameButton, SIGNAL(clicked()), this, SLOT(showOptions()));
    aboutGameButton = new MenuButton("Instructions", this);
    connect(aboutGameButton, SIGNAL(clicked()), this, SLOT(showAbout()));
    historyGameButton = new MenuButton("Changes", this);
    connect(historyGameButton, SIGNAL(clicked()), this, SLOT(showHistory()));
    loadSavedGameButton = new MenuButton("Load saved game", this);
    connect(loadSavedGameButton, SIGNAL(clicked()), this, SLOT(loadSavedGame()));
    quitGameButton = new MenuButton("Quit", this);
    connect(quitGameButton, SIGNAL(clicked()), this, SLOT(close()));
    mainButtonList->addButton(playGameButton);
    mainButtonList->addButton(optionsGameButton);
    mainButtonList->addButton(aboutGameButton);
    mainButtonList->addButton(historyGameButton);
    mainButtonList->addButton(loadSavedGameButton);
    mainButtonList->addButton(quitGameButton);
    gameButtonList = new MenuButtonList(this);
    endDemo = new MenuButton("Go Back", this);
    endDemo->showFrame(true);
    endDemo->hide();
    endDemo->setFocusPolicy(Qt::NoFocus);
    endDemo->setCentered(true);
    connect(endDemo, SIGNAL(clicked()), this, SLOT(showWelcomeScreen()));
    backToWelcomeFromGame = new MenuButton("Abort", this);
    backToWelcomeFromGame->showFrame(true);
    backToWelcomeFromGame->setFocusPolicy(Qt::NoFocus);
    backToWelcomeFromGame->setCentered(true);
    connect(backToWelcomeFromGame, SIGNAL(clicked()), this, SLOT(abortGame()));
    pauseGame = new MenuButton("Pause", this);
    pauseGame->showFrame(true);
    pauseGame->setFocusPolicy(Qt::NoFocus);
    pauseGame->setCentered(true);
    connect(pauseGame, SIGNAL(clicked()), this, SLOT(pauseLevel()));
    restartCurrent = new MenuButton("Restart", this);
    restartCurrent->showFrame(true);
    restartCurrent->setFocusPolicy(Qt::NoFocus);
    restartCurrent->setCentered(true);
    connect(restartCurrent, SIGNAL(clicked()), this, SLOT(restartLevel()));
    gameButtonList->addButton(backToWelcomeFromGame);
    gameButtonList->addButton(pauseGame);
    gameButtonList->addButton(restartCurrent);
}

void PuzzleWindow::playGame()
{
    if (board->isPaused)
        return;
    switchState(InfoBar::GAME);
}
void PuzzleWindow::showOptions()
{
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

void PuzzleWindow::showAbout()
{
    if (board->isPaused)
        return;
    switchState(InfoBar::ABOUT);
}

void PuzzleWindow::showHistory()
{
    if (board->isPaused)
        return;
    switchState(InfoBar::HISTORY);
}

void PuzzleWindow::loadSavedGame()
{
    if (board->isPaused)
        return;
    switchState(InfoBar::BROWSE);
}

void PuzzleWindow::pauseLevel()
{
    if (board->isPaused) {
        board->isPaused = false;
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
        board->isPaused = true;
        update();
    }
}

void PuzzleWindow::showWelcomeScreen()
{
    if (board->isPaused)
        return;
    endDemo->hide();
    switchState(InfoBar::WELCOME);
}

void PuzzleWindow::showDemo()
{
    if (board->isPaused)
        return;
    endDemo->show();
    switchState(InfoBar::DEMO);
}

void PuzzleWindow::initGame()
{
    play->currentLevel = Puzzle::startLevel;
    demo.clearRecording();
    play->clearPoints();
    infoBar->setTotalPoints(0);
    timer->start(Puzzle::timeoutValue);
}

// Called when loading next level
void PuzzleWindow::startLevel(bool shuffle)
{
    if (shuffle)
        Puzzle::images->shuffleTheme();
    infoBar->setTotalPoints(infoBar->totalPoints() + play->currentPoints());
    bool result;
    if (Puzzle::levelFileName.isEmpty() && Puzzle::currLevelsName.isEmpty())
        result = play->loadLevel(levels, play->currentLevel);
    else {
        if (!Puzzle::levelFileName.isEmpty())
            result = play->loadLevel(Puzzle::levelFileName, play->currentLevel);
        else {
            result =
                play->loadLevel(ROOTHOME + "pics/levels/" +
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
    int ox = (width() - play->width() * Puzzle::blockPixelSize) / 2;
    int oy = ((height() - play->height() * Puzzle::blockPixelSize) / 2);
    oy = qMax(oy, infoBar->height());
    board->setGeometry(ox, oy, play->width() * Puzzle::blockPixelSize,
                               play->height() * Puzzle::blockPixelSize);
    play->setGameState(Playground::INPROGRESS);
    timer->start(Puzzle::timeoutValue);
    aboutDialog->hide();
    update();
    setFocus();
}

void PuzzleWindow::gameCompleted()
{
    timer->stop();
    dlg->configure("* Game Completed *", true, false, 0, " OK");
    dlg->show();
    dlg->exec();
    switchState(InfoBar::WELCOME);
}

void PuzzleWindow::levelCompleted()
{
    QString tmp;
    tmp.sprintf("* Level %d completed *", play->currentLevel);
    dlg->configure(tmp, true, false, 1800);
    dlg->show();
    dlg->exec();
}

void PuzzleWindow::demoCompleted()
{
    timer->stop();
    if (Puzzle::demoLoop) {
        QString tmp;
        tmp.sprintf("* Demo Level %d completed *", play->currentLevel);
        dlg->configure(tmp, true, false, 1800);
        dlg->show();
        dlg->exec();

        play->currentLevel++;
        infoBar->setTotalPoints(infoBar->totalPoints() + play->currentPoints());
        if (!demo.initDemo()) {
            play->currentLevel = -1;
            if (!demo.initDemo()) {
                switchState(InfoBar::ABOUT);     // if somebody deleted starting files
                                        // we better get out to avoid infinite loop
            } else {
                infoBar->setTotalPoints(0);
                nextLevelLoaded();
            }
        } else {
            nextLevelLoaded();
        }
    } else {
        dlg->configure("* Demo Completed *", true, false, 0, " OK");
        dlg->show();
        dlg->exec();
        switchState(InfoBar::ABOUT);
    }
}

void PuzzleWindow::restartLevel()
{
    if (board->isPaused)
        return;
    timer->stop();
    dlg->configure("Restart current level ?", false, false, 0, " Yes",
                   QString::null, "No");
    if (dlg->exec() == 0) {
        play->clearPoints();
        if (infoBar->totalPoints() >= Puzzle::blockBonus)
            infoBar->setTotalPoints(infoBar->totalPoints() - Puzzle::blockBonus);
        else
            infoBar->setTotalPoints(0);
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

    dlg->configure(text, false, false, 0, "Again", QString::null,
                   "Quit");
    if (dlg->exec() == 0) {
        play->clearPoints();
        if (infoBar->totalPoints() >= Puzzle::blockBonus)
            infoBar->setTotalPoints(infoBar->totalPoints() - Puzzle::blockBonus);
        else
            infoBar->setTotalPoints(0);
        startLevel(false);
        timer->start(Puzzle::timeoutValue);
    } else {
        switchState(InfoBar::WELCOME);
    }
}

void PuzzleWindow::abortGame()
{
    if (board->isPaused)
        return;

    timer->stop();
    if (Puzzle::progressSaving) {
        dlg->configure("Save the current game?", false, true, 0,
                       "Yes", "No", "Cancel");
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
    dlg->configure(error, false, false, 0, " OK");
    dlg->exec();
}

void PuzzleWindow::saveGameState(const QString & fileName)
{
    QDir dir(QString(QDir::homePath()) + QString("/.puzz-le"));
    if (!dir.exists()) {
        dir.mkdir(dir.absolutePath());
    }
    QString tmp = dir.absolutePath() + "/" + fileName;
    if (!Puzzle::levelFileName.isEmpty())
        play->savePlayground(tmp.append(".puzzle"), infoBar->totalPoints(),
                             Puzzle::levelFileName);
    else
        play->savePlayground(tmp.append(".puzzle"), infoBar->totalPoints(),
                             Puzzle::currLevelsName);
}

void PuzzleWindow::loadGame(const QString &fileName)
{
    if (!loadGameState(fileName)) {
        errorMsg("Unable to load saved game.");
    }
}

void PuzzleWindow::fileDialogDone()
{
    switchState(InfoBar::WELCOME);
}

bool PuzzleWindow::loadGameState(const QString &fileName)
{
    QString oldCurrLevelsName = Puzzle::currLevelsName;
    int currPoints = 0;
    if (play->loadPlayground(fileName, &currPoints, levels)) {
        infoBar->setTotalPoints(currPoints);
        aboutDialog->hide();
        fileDialog->hide();
        infoBar->setState(InfoBar::GAME);
        play->currentLevel = play->number();
        mainButtonList->setVisible(false);
        gameButtonList->setVisible(true);
        demo.clearRecording();
        nextLevelLoaded();
        update();
        if (Puzzle::currLevelsName != oldCurrLevelsName) {
            QString tmp;
            if (!Puzzle::currLevelsName.isEmpty())
                tmp.sprintf("Switching to %s levels set.",
                            Puzzle::currLevelsName.toLatin1().constData());
            else
                tmp.sprintf("Switching to default levels set.");
            dlg->configure(tmp, true, false, 1800);
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
