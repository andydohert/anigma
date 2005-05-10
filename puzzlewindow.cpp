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
**                                                        c
**********************************************************************/

#ifdef KDE_RELEASE
#include <kconfig.h>
#include <kapplication.h>
#define Config KConfig
#else
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#endif

#include <qpainter.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qfontmetrics.h>
#include <qbitmap.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include "levels.dat"
#include "level_demo.h"
#include <stdlib.h>
#include "SoundRepository.h"
#include "PuzzleWindow.h"
#include "ImageRepository.h"
#include "AboutDialog.h"
#include "FileDialog.h"
#include "Puzzle.h"
#include "MenuButton.h"
#include "GameDialog.h"
#include "OptionsDialog.h"

DemoMove::DemoMove(int x, int y, int t):ix(x),iy(y),it(t)
{
    
}

int PuzzleWindow::infoBarHeight=24;

PuzzleWindow::PuzzleWindow( QWidget* parent , const char* name , WFlags fl): QMainWindow( parent, name,fl )
{
    setBackgroundMode(NoBackground);


    QColor black(0,0,0);
    QColor blue(0,148,255);

    setFont(QFont("Helvetica",10,QFont::Bold));
    QPalette pal=palette();
    QColorGroup cg=pal.active();
    cg.setColor(QColorGroup::Base,black);
    cg.setColor(QColorGroup::Background,black);
    cg.setColor(QColorGroup::Button,black);
    cg.setColor(QColorGroup::ButtonText,blue);
    cg.setColor(QColorGroup::Midlight,blue);
    cg.setColor(QColorGroup::Shadow,blue);
    cg.setColor(QColorGroup::Dark,blue);
    cg.setColor(QColorGroup::Light,black);
    cg.setColor(QColorGroup::Mid,black);
    pal.setActive(cg);
    pal.setInactive(cg);
    pal.setDisabled(cg);
    setPalette(pal);    

    setCaption("Puzz-le");      
    info=0;
    timer=0;   
    ox=0;
    oy=0;
    isPaused=false;
    dlg=new GameDialog(this);
    aDlg=new AboutDialog(this);
    fDlg=new FileDialog(getenv("HOME")+ QString("/puzz-le"),this);
    play= 0;
    connect( fDlg, SIGNAL(loadSavedGame(const QString &)),this, SLOT(loadGame(const QString &)));
    connect( fDlg, SIGNAL(done(void)),this, SLOT(fileDialogDone(void)));
   

    init();      
    switchState(WELCOME);

}
PuzzleWindow::~PuzzleWindow() 
{
    timer->stop();
    delete pix;
    delete info;
    delete Puzzle::sounds;
    delete Puzzle::images; 
    delete play;
}

// Game main state machine

void PuzzleWindow::switchState(GAME_STATE s)
{
    state=s;
    switch ( state )
    {
        case    WELCOME:
            timer->stop();
            mainButtonList->configureButtons(true,false);
            aboutButtonList->configureButtons(false,false);
            gameButtonList->configureButtons(false,false);
            playGame->setFocus();            
            aDlg->hide();
            fDlg->hide();
            repaint();
            break;
        case    ABOUT:
            timer->stop();
            mainButtonList->configureButtons(false,false);
            aboutButtonList->configureButtons(true,false);
            playDemo->show();
            gameButtonList->configureButtons(false,false);
            aDlg->setMode(AboutDialog::ABOUT);
            aDlg->show();
            fDlg->hide();
            repaint();
            break;
        case    HISTORY:
            timer->stop();
            mainButtonList->configureButtons(false,false);
            aboutButtonList->configureButtons(true,false);
            playDemo->hide();
            gameButtonList->configureButtons(false,false);
            aDlg->setMode(AboutDialog::HISTORY);
            aDlg->show();
            fDlg->hide();
            repaint();
            break;
        case    BROWSE:
            timer->stop();
            mainButtonList->configureButtons(false,false);
            aboutButtonList->configureButtons(false,false);
            gameButtonList->configureButtons(false,false);
            aDlg->hide();
            fDlg->refresh(play);
            fDlg->show();
            fDlg->setFocus();
            break;      
        case    GAME:
            mainButtonList->configureButtons(false,false);
            aboutButtonList->configureButtons(false,false);
            gameButtonList->configureButtons(true,false);
            aDlg->hide();
            fDlg->hide();
            initGame();
            startLevel();
            break;
        case    DEMO:
            currentLevel=-1;
            if ( !initDemo() )
            {
                errorMsg("Unable to play demo !");
                switchState(ABOUT);
                currentLevel=-1;
            }
            break;

        default:
            break;
    }
}

// Standard accelerators are not good enough here. We want plain key presses without any of the keyboard modifiers (CTRL, ALT etc )
// This would probably be better handled by creating filter.
// Just forward it to coresponding handler for button press ...

void PuzzleWindow::keyPressEvent ( QKeyEvent * e )
{
    qDebug("key pressed: %i", e->key());
		if ( !isPaused )
    {
        switch ( e->key() )
        {
            case  Key_Escape:
                if ( state==ABOUT || state==HISTORY)
                    buttonClicked(BACK_TO_WELCOME_FROM_ABOUT);
                else if ( state==GAME )
                    buttonClicked(BACK_TO_WELCOME_FROM_GAME);
                break;
            case  Key_F33:
                if ( state==GAME )
                {
                    buttonClicked(RESTART_LEVEL);
                }
                else if ( state==ABOUT )
                {
                    buttonClicked(PLAY_DEMO);
                }

                break;


            case Key_Space:
            case    Key_Up:
            case    Key_Down:
            case    Key_Left:
            case    Key_Right:      
                if ( state==GAME )
                {
                    QPoint p=play->selectedPositon(e->key());
                    generateMovement(p.x()+ox,p.y()+oy,true);
                }
                break;
            default:
                break;
        }
    }
}

// If we have game in progress, call Playground mouse press handler.
// Record the move if required.

void PuzzleWindow::mousePressEvent ( QMouseEvent * e )
{
    if ( state==GAME && !isPaused)
    {
        generateMovement(e->x(),e->y());
    }
}


void PuzzleWindow::generateMovement(int x, int y, bool keyBoardMode)
{
    if ( Puzzle::recordGames )
        demoList.append(new DemoMove(x-ox,y-oy,play->tickValue()));
    if ( play->setSelected(x-ox,y-oy,keyBoardMode) )
    {
        drawPlayground();
    }

}

void PuzzleWindow::closeEvent ( QCloseEvent * e )
{
    Config cfg("Puzz_le");
    cfg.setGroup("Puzz_le");
    cfg.writeEntry("timeoutValue",Puzzle::timeoutValue);
    cfg.writeEntry("progressSaving",(int)Puzzle::progressSaving);
    cfg.writeEntry("playSounds",(int)Puzzle::playSound);
    cfg.writeEntry("currentLevelName",Puzzle::currLevelsName);
    cfg.writeEntry("timeLimit",(int)Puzzle::timeLimit);
    cfg.writeEntry("theme",Puzzle::theme);

    e->accept();
}


void PuzzleWindow::resizeEvent ( QResizeEvent *)
{
		if(width() == 0)
			return;
    if ( !Puzzle::images )
    {
        initImages();
        Puzzle::images->shuffleTheme();        
        play=new Playground();
    }

    aDlg->setGeometry(4,80,width()-8,height()-110);
    fDlg->setGeometry(4,80,width()-8,height()-80);

    playGame->setGeometry(115,118,150,20);
    optionsGame->setGeometry(playGame->x(),playGame->y()+23,playGame->width(),playGame->height());    
    aboutGame->setGeometry(playGame->x(),optionsGame->y()+23,playGame->width(),playGame->height());
    historyGame->setGeometry(playGame->x(),aboutGame->y()+23,playGame->width(),playGame->height());
    loadSavedGame->setGeometry(playGame->x(),historyGame->y()+23,playGame->width(),playGame->height());
    quitGame->setGeometry(playGame->x(),loadSavedGame->y()+23,playGame->width(),playGame->height());    
    backToWelcomeFromAbout->setGeometry(4,height()-playGame->height()-4,100,playGame->height());
    playDemo->setGeometry(width()-100-4,height()-playGame->height()-4,100,playGame->height());
    backToWelcomeFromGame->setGeometry(4,height()-playGame->height()-4,70,playGame->height());
    pauseGame->setGeometry((width()-70)/2,height()-playGame->height()-4,70,playGame->height());    
    restartCurrent->setGeometry(width()-70-4,height()-playGame->height()-4,70,playGame->height());
    pix->resize(width(),height());
}


void PuzzleWindow::paintEvent ( QPaintEvent * )
{
    QPainter p(this);


    QPainter lp(pix);   

    switch ( state )
    {
        case  WELCOME:
            pix->fill(QColor(0,0,0));
            drawTitleScreen(&lp);
            break;
        case  GAME:
        case    DEMO:
            drawPlayground(&p,&lp,true);
            break;
        case  ABOUT:
        case HISTORY:
            pix->fill(QColor(0,0,0));
            drawAboutScreen(&lp);
            break;
	default:
	    break;    
    }

    if ( state!=GAME && state!=DEMO )
        p.drawPixmap(0,0,*pix);

}

void PuzzleWindow::drawTitleScreen(QPainter *p)
{
    if ( Puzzle::images )
    {
        QPixmap *tmp=Puzzle::images->findPixmap("front_title");
        if ( tmp )
            p->drawPixmap(10,10,*tmp);
        tmp=Puzzle::images->findPixmap("front_1");
        if ( tmp )
            p->drawPixmap(10,loadSavedGame->y()+loadSavedGame->height()-40,*(tmp));
        p->setPen(QColor(0,135,234));
        MenuButton::drawColorizedText("Copyright (c) 2002 Walter Rawdanik", 20,80,p,menuClr,150);
        MenuButton::drawColorizedText("http://www.warmi.net/zaurus", 20,95,p,menuClr,150);
        MenuButton::drawColorizedText("ver: "+Puzzle::gameVersion, 174,278,p,menuClr,150);

    }
}

// Update screen using background pixmap (which is in turn updated by Playground::painPlayground() handler )
// repainting only stripes which are marked dirty (1) in QBitArray returned by Playground::painPlayground() handler.
// If neccesary update score line ( level #, time and score)

void PuzzleWindow::drawPlayground(QPainter *p,QPainter *lp, bool drawAll)
{
    if ( Puzzle::images )
    {
        if ( play->gameState()!=Playground::INVALID )
        {
            bool deleteP=false;
            bool deleteLP=false;

            if ( !p )
            {
                p=new QPainter(this);
                deleteP=true;

            }
            if ( !lp )
            {
                lp=new QPainter(pix);
                deleteLP=true;
            }

            if ( isPaused )
            {
                p->fillRect(0,0,pix->width(),pix->height(),QColor(0,0,0));
                QPixmap *tmp=Puzzle::images->findPixmap("front_title");
                if ( tmp )
                    p->drawPixmap(18,36,*tmp);                
                // hardcoded position
                MenuButton::drawColorizedText("Game Paused",80,140,p,QColor(0,148,255),150);
                MenuButton::drawColorizedText("Click 'Resume' to unpause the Game",30,170,p,QColor(0,148,255),150);
                
            }
            else
            {

                QBitArray *stripes=play->paintPlayground(lp,ox,oy,drawAll);

                unsigned int i;             

                if ( drawAll )
                {
                    p->drawPixmap(0,0,*pix);
                }
                else
                {

                    for ( i=0;i<stripes->size();i++ )
                    {
                        if ( stripes->at(i) )
                        {
                            p->drawPixmap(i*Puzzle::bSize+ox,oy,*pix,i*Puzzle::bSize+ox,oy,
                                          Puzzle::bSize,play->height()*Puzzle::bSize);
                        }
                    }
                }
                stripes->fill(false);            
                QPixmap *tmp=0;

                if ( play->isInfoChange() || drawAll )
                {
                    updateInfoBar();
                    tmp=Puzzle::images->findPixmap("background");
                    if ( tmp )
                        lp->drawPixmap(0,0,*tmp,0,0,info->width(),info->height());
                    else
                        lp->fillRect(0,0,info->width(),info->height(),QColor(0,0,0));
                    lp->drawPixmap(0,0,*info);
                    p->drawPixmap(0,0,*pix,0,0,info->width(),info->height());
                }
            }
            if ( deleteP )
                delete p;
            if ( deleteLP )
                delete lp;

        }

    }

}

void PuzzleWindow::drawAboutScreen(QPainter *p)
{
    if ( Puzzle::images )
    {
        p->drawPixmap(10,10,*(Puzzle::images->findPixmap("front_title")));    
    }
}

// Slot method called by QTimer handler.
// This method calls Playground.update() which updates game state and returns true if game screen needs to be repainted.
// After each update Playground class is checked for any changes in the game state ( like finished level or out_of_time condition.)


void PuzzleWindow::synchTimeout()
{
    if ( state==DEMO )
    {

        if ( curMove )
        {
            if ( curMove->getTick()==play->tickValue() )
            {
                QPoint p=curMove->getPos();
                if ( play->setSelected(p.x(),p.y()) )
                {
                    drawPlayground();
                }
                curMove=demoList.next();
            }

        }
        if ( play->update() )
            drawPlayground();
        if ( play->gameState()==Playground::OVER )
        {
            demoCompleted();
        }
    }
    else
    {
        if ( state==GAME && play->update() )
        {
            drawPlayground();
            if ( play->gameState()==Playground::OVER )
            {
                if ( Puzzle::recordGames )
                {
                    saveDemoCPPVersion();
                    saveDemo();
                    demoList.clear();
                }
                if ( play->isGameWon() )
                {

                    #ifdef DEMO_VERSION
                    if ( currentLevel<play->totalLevels() && currentLevel<8 )
                    {
                        levelCompleted();
                        currentLevel++;
                        startLevel();               
                    }
                    else
                    {
                        gameCompleted();
                    }
                    #else
                    if ( currentLevel<play->totalLevels() )
                    {
                        levelCompleted();
                        currentLevel++;
                        startLevel();               
                    }
                    else
                    {
                        gameCompleted();
                    }
                    #endif
                }
                else
                {
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
    Puzzle::initImages(width(),height());
    pix=new QPixmap(width(),height());
    info=new QPixmap(width(),PuzzleWindow::infoBarHeight);
    QPainter p(info);
    QColor start(0,179,255);
    int i;      
    for ( i=0;i<info->height();i++ )
    {
        p.setPen(start.dark(100+(i*(100/PuzzleWindow::infoBarHeight))));
        p.drawLine(0,i,info->width()-1,i);      
    }
    infoMask=new QBitmap(width(),PuzzleWindow::infoBarHeight);
    info->setMask(*infoMask);       
}

void PuzzleWindow::init()
{
    if ( !timer )
    {
        timer=new QTimer(this,"Main Synch Timer");
        connect( timer, SIGNAL(timeout()),this, SLOT(synchTimeout()));
    }
    Puzzle::initSounds(QString(getenv("QPEDIR")) + "/sounds/puzz-le");

    menuClr.setRgb(0,148,255);
    hMenuClr.setRgb(255,255,255);

    mainButtonList=new MenuButtonList(this);
    playGame=new MenuButton(PLAY_GAME,"Play the game",this);
    playGame->setColors(menuClr,QColor(0,0,0));
    optionsGame=new MenuButton(OPTIONS_GAME,"Options",this);
    optionsGame->setColors(menuClr,QColor(0,0,0));    
    aboutGame=new MenuButton(ABOUT_GAME,"Instructions",this);
    aboutGame->setColors(menuClr,QColor(0,0,0));
    historyGame=new MenuButton(HISTORY_GAME,"Changes",this);
    historyGame->setColors(menuClr,QColor(0,0,0));
    loadSavedGame=new MenuButton(LOAD_GAME,"Load saved game",this);
    loadSavedGame->setColors(menuClr,QColor(0,0,0));
    quitGame=new MenuButton(QUIT,"Quit",this);
    quitGame->setColors(menuClr,QColor(0,0,0));
    mainButtonList->appendMenuButton(playGame);
    mainButtonList->appendMenuButton(optionsGame);    
    mainButtonList->appendMenuButton(aboutGame);
    mainButtonList->appendMenuButton(historyGame);
    mainButtonList->appendMenuButton(loadSavedGame);
    mainButtonList->appendMenuButton(quitGame);

    aboutButtonList=new MenuButtonList(this);
    backToWelcomeFromAbout=new MenuButton(BACK_TO_WELCOME_FROM_ABOUT,"Go Back",this,true);
    backToWelcomeFromAbout->setColors(menuClr,QColor(0,0,0));
    backToWelcomeFromAbout->setCentered(true);
    playDemo=new MenuButton(PLAY_DEMO,"Demo Game",this,true);
    playDemo->setColors(menuClr,QColor(0,0,0));
    playDemo->setCentered(true);
    aboutButtonList->appendMenuButton(backToWelcomeFromAbout);
    aboutButtonList->appendMenuButton(playDemo);

    gameButtonList=new MenuButtonList(this);
    backToWelcomeFromGame=new MenuButton(BACK_TO_WELCOME_FROM_GAME,"Abort",this,true);
    backToWelcomeFromGame->setFocusPolicy(NoFocus);
    backToWelcomeFromGame->setColors(menuClr,QColor(0,0,0));
    backToWelcomeFromGame->setCentered(true);

    pauseGame=new MenuButton(PAUSE_GAME,"Pause",this,true);
    pauseGame->setFocusPolicy(NoFocus);
    pauseGame->setColors(menuClr,QColor(0,0,0));
    pauseGame->setCentered(true);

    restartCurrent=new MenuButton(RESTART_LEVEL,"Restart",this,true);
    restartCurrent->setFocusPolicy(NoFocus);
    restartCurrent->setColors(menuClr,QColor(0,0,0));
    restartCurrent->setCentered(true);
    gameButtonList->appendMenuButton(backToWelcomeFromGame);
    gameButtonList->appendMenuButton(pauseGame);
    gameButtonList->appendMenuButton(restartCurrent);

    connect( mainButtonList, SIGNAL(clicked(int)),this, SLOT(buttonClicked(int)));
    connect( aboutButtonList, SIGNAL(clicked(int)),this, SLOT(buttonClicked(int)));
    connect( gameButtonList, SIGNAL(clicked(int)),this, SLOT(buttonClicked(int)));
}

void PuzzleWindow::buttonClicked(int id)
{
    if ( id==PAUSE_GAME )
    {
        if ( isPaused )
        {
            isPaused=false;
            pauseGame->setText("Pause");
            restartCurrent->show();
            backToWelcomeFromGame->show();            
            timer->start(Puzzle::timeoutValue);
            repaint();
        }
        else
        {
            pauseGame->setText("Resume");
            restartCurrent->hide();
            backToWelcomeFromGame->hide();
            timer->stop();
            isPaused=true;
            repaint();
        }
    }

    if ( !isPaused )
    {
        switch ( id )
        {
            case    PLAY_GAME:
                switchState(GAME);
                break;
            case    OPTIONS_GAME:
                {
                    int gs=Puzzle::timeoutValue;
                    bool se=Puzzle::playSound;
                    bool ar=Puzzle::progressSaving;
                    bool tl=Puzzle::timeLimit;
                   
                    QString cTheme=Puzzle::theme;
                    OptionsDialog *dlg=new OptionsDialog(&Puzzle::currLevelsName,&cTheme,&se,&ar,&tl,&gs,this);
                    if ( dlg->exec()==QDialog::Accepted )
                    {
                        Puzzle::timeoutValue=gs;
                        Puzzle::playSound=se;
                        Puzzle::progressSaving=ar;
                        Puzzle::timeLimit=tl;
                        Puzzle::theme=cTheme;
                        if(!Puzzle::images->initTheme(Puzzle::theme))
                        {
                            Puzzle::theme="Default";
                            Puzzle::images->initTheme(Puzzle::theme);
                        }
                    }
                }
                break;
            case    HISTORY_GAME:
                switchState(HISTORY);
                break;
            case    ABOUT_GAME:                            
                switchState(ABOUT);
                break;
            case    BACK_TO_WELCOME_FROM_ABOUT:
                switchState(WELCOME);
                break;
            case    BACK_TO_WELCOME_FROM_GAME:
                abortGame();
                break;
            case    PLAY_DEMO:
                switchState(DEMO);
                break;
            case    RESTART_LEVEL:
                restartLevel();
                break;
            case    QUIT:
                close();
                break;  
            case    LOAD_GAME:
                switchState(BROWSE);
                break;
	    default:
	        break;			
        }
    }
}

void PuzzleWindow::updateInfoBar()
{
    const QBitmap *bt=info->mask();
    static bool flash=true;
    QPainter p(bt);
    QString msg;
    p.setPen(Qt::color1);
    int rx=46;
    if ( state==DEMO )
    {
        rx+=6;
        msg.sprintf("DEMO  TIME: %03d  SCORE: %05d",play->timeLimit(),currPoints+play->currentPoints());
    }
    else
    {
        if ( Puzzle::timeLimit )
        {
            if(play->gameType()==Playground::TIME_BASED)
                msg.sprintf("L: %02d   TIME: %03d   SCORE: %05d",currentLevel,play->timeLimit(),currPoints+play->currentPoints());
            else
                msg.sprintf("L: %02d   MOV: %03d   SCORE: %05d",currentLevel,play->timeLimit(),currPoints+play->currentPoints());
        }
        else
            msg.sprintf("LEVEL: %02d   SCORE: %05d",currentLevel,currPoints+play->currentPoints());
    }

    p.setFont(QFont("Helvetica",16,QFont::Bold));
    p.fillRect(0,0,bt->width(),bt->height(),Qt::color0);
    p.drawText(2,2,bt->width()-4,bt->height()-4,AlignCenter,msg);
    if ( Puzzle::timeLimit && play->gameType()==Playground::TIME_BASED)
    {
        if ( play->timeLimit()<10 )
        {
            if ( flash )
                flash=false;
            else
                flash=true;
        }

        if ( flash )
            p.drawRect(rx,2,78,21);
    }
}

bool PuzzleWindow::initDemo()
{
    if ( Puzzle::demoFileName.isEmpty() )
        loadDemo(demo_level);
    else
    {
        if ( Puzzle::demoLoop )
        {
            QString nextName=Puzzle::demoFileName;
            if ( currentLevel==-1 )
            {
                QString tmp=(QString)Puzzle::demoFileName.at(Puzzle::demoFileName.length()-7);
                tmp.append(Puzzle::demoFileName.at(Puzzle::demoFileName.length()-6));   
                sscanf( tmp.latin1(), "%d",&currentLevel);              
            }
            else
            {
                QString tmp;
                tmp.sprintf("%02d",currentLevel);
                nextName.replace(nextName.length()-7,2,tmp);
            }
            loadDemo(nextName);
        }
        else
            loadDemo(Puzzle::demoFileName);
    }
    if ( demoList.count() )
    {
        currPoints=0;
        demoList.first();
        curMove=demoList.current();
        bool result;

        Puzzle::images->shuffleTheme();        
        if ( Puzzle::levelFileName.isEmpty() )
            result=play->loadLevel(levels,demoLevel);
        else
            result=play->loadLevel(Puzzle::levelFileName,demoLevel);    
        if ( result )
        {

            ox=(width()-play->width()*Puzzle::bSize)/2;
            oy=((height()-play->height()*Puzzle::bSize)/2);
            oy=QMAX(oy,PuzzleWindow::infoBarHeight);
            play->setGameState(Playground::INPROGRESS);     
            timer->start(Puzzle::timeoutValue);
            aDlg->hide();
            repaint();  
            return true;
        }
    }

    return false;
}

void PuzzleWindow::loadDemo(const QString &fileName)
{
    QFile file(fileName);


    demoList.clear();

    if ( file.exists() && file.open(IO_ReadOnly) )
    {
        QTextStream ts(&file);
        QString line;
        int dx,dy,dt;
        line=ts.readLine(); // skip count;
        line= ts.readLine();
        sscanf( line, "%d", &demoLevel);
        while ( !ts.eof() )
        {
            line= ts.readLine();
            sscanf( line, "%d %d %d ", &dx,&dy,&dt);
            demoList.append(new DemoMove(dx,dy,dt));
        }       
    }
}

void PuzzleWindow::loadDemo(const char * demo[])
{

    demoList.clear();
    int numItems;

    if ( sscanf( demo[0], "%d", &numItems) ==1 )
    {
        sscanf( demo[1], "%d", &demoLevel);
        int i;
        int dx,dy,dt;

        for ( i=0;i<numItems;i++ )
        {
            sscanf( demo[i+2], "%d %d %d ", &dx,&dy,&dt);
            demoList.append(new DemoMove(dx,dy,dt));                            
        }       
    }
}

// Save demo into plain ASCII text file.
// Demo file is simply recording of timestamped player moves ( mouse click coordinates )

void PuzzleWindow::saveDemo()
{
    DemoMove *dm;
    QString fName;
    fName.sprintf("Level_%d.demo",currentLevel);    
    QFile file(fName);

    if ( file.open(IO_WriteOnly|IO_Truncate) )
    {
        QTextStream ts(&file);
        QString header;
        header.sprintf("%d\n",demoList.count());
        ts << header;
        header.sprintf("%d\n",currentLevel);
        ts << header;
        for ( dm=demoList.first(); dm != 0; dm=demoList.next() )
        {
            ts<< dm->toString()+"\n";   
        }
        file.close();
    }
}

// Save demo into C++ style include.

void PuzzleWindow::saveDemoCPPVersion()
{
    DemoMove *dm;
    QString fName;
    fName.sprintf("level_demo.h");

    QFile file(fName);

    if ( file.open(IO_WriteOnly|IO_Truncate) )
    {
        QTextStream ts(&file);
        QString header;
        ts << "static const char *demo_level[]= {\n";
        header.sprintf("\"%d\",\n",demoList.count());
        ts << header;
        header.sprintf("\"%d\",\n",currentLevel);
        ts << header;
        for ( dm=demoList.first(); dm != 0; dm=demoList.next() )
        {
            ts<< "\""+dm->toString()+"\",\n";   
        }
        ts << "};";
        file.close();
    }
}

void PuzzleWindow::initGame()
{
    currentLevel=Puzzle::startLevel; 
    demoList.clear();
    play->clearPoints();
    currPoints=0;
    timer->start(Puzzle::timeoutValue);     
}

// Called when loading next level 

void PuzzleWindow::startLevel(bool shuffle)
{
    if(shuffle)
        Puzzle::images->shuffleTheme();
    currPoints+=play->currentPoints();
    bool result;

    if ( Puzzle::levelFileName.isEmpty() && Puzzle::currLevelsName.isEmpty() )
        result=play->loadLevel(levels,currentLevel);
    else
    {
        if ( !Puzzle::levelFileName.isEmpty() )
            result=play->loadLevel(Puzzle::levelFileName,currentLevel);
        else
        {
				   result=play->loadLevel(ROOTHOME+"pics/puzz-le/levels/"+Puzzle::currLevelsName+"/levels",currentLevel);
        }
    }
    if ( result )
    {
        ox=(width()-play->width()*Puzzle::bSize)/2;
        oy=((height()-play->height()*Puzzle::bSize)/2);
        oy=QMAX(oy,PuzzleWindow::infoBarHeight);
        play->setGameState(Playground::INPROGRESS); 
        repaint();        

    }
    else
    {
        switchState(WELCOME);
        errorMsg("Unable to load game levels.");        
    }
}

void PuzzleWindow::gameCompleted()
{
    timer->stop();
    dlg->configure(pix,"* Game Completed *",true,false,0," OK");
    dlg->exec();        
    switchState(WELCOME);       
}
void PuzzleWindow::demoCompleted()
{
    timer->stop();
    if ( Puzzle::demoLoop )
    {
        QString tmp;
        tmp.sprintf("* Demo Level %d completed *",currentLevel);
        dlg->configure(pix,tmp,true,false,1800);
        dlg->exec();                
        currentLevel++;
        if ( !initDemo() )
        {
            currentLevel=-1;
            if ( !initDemo() )
            {
                switchState(ABOUT); // if somebody deleted starting file we better get out to avoid infinite L
            }
        }
    }
    else
    {
        dlg->configure(pix,"* Demo Completed *",true,false,0," OK");
        dlg->exec();        
        switchState(ABOUT);     
    }
}

void PuzzleWindow::restartLevel()
{
    timer->stop();
    dlg->configure(pix,"Restart current level ?",false,false,0," Yes",QString::null,"No");

    if ( dlg->exec()==0 )
    {
        play->clearPoints();
        if ( currPoints>=(unsigned int)Puzzle::blockBonus )
            currPoints-=Puzzle::blockBonus;
        else
            currPoints=0;
        startLevel(false);   
    }
    timer->start(Puzzle::timeoutValue);
}

void PuzzleWindow::outOfTime()
{
    timer->stop();
    QString text;
    if(play->gameType()==Playground::TIME_BASED)
        text="Sorry - Out of Time !";
    else
        text="Sorry - Out of Moves !";
        
    dlg->configure(pix,text,false,false,0,"Again",QString::null,"Quit");
    if ( dlg->exec()==0 )
    {
        play->clearPoints();
        if ( currPoints>=(unsigned int)Puzzle::blockBonus )
            currPoints-=Puzzle::blockBonus;
        else
            currPoints=0;
        startLevel(false);
        timer->start(Puzzle::timeoutValue);
    }
    else
    {
        switchState(WELCOME);
    }
}

void PuzzleWindow::abortGame()
{
    timer->stop();
    if ( Puzzle::progressSaving )
    {
        dlg->configure(pix,"Save the current game as?",false,true,0,"Yes","No","Cancel");
        switch ( dlg->exec() )
        {
            case    0:
                saveGameState(dlg->requesterText());
                switchState(WELCOME);
                break;
            case    1:
                switchState(WELCOME);
                break;
            case    2:
                timer->start(Puzzle::timeoutValue);
                break;
            default:
                break;
        }       
    }
    else
        switchState(WELCOME);
}

void PuzzleWindow::levelCompleted()
{
    QString tmp;
    tmp.sprintf("* Level %d completed *",currentLevel);
    dlg->configure(pix,tmp,true,false,1800);
    dlg->exec();
}

void PuzzleWindow::errorMsg(const QString &error)
{
    dlg->configure(0,error,false,false,0," OK");
    dlg->exec();    
}

void PuzzleWindow::saveGameState(const QString &fileName)
{
    QDir dir(getenv("HOME")+ QString("/puzz-le"));
    if ( !dir.exists() )
    {
        dir.mkdir(dir.absPath());
    }
    QString tmp=dir.absPath()+"/"+fileName;
    if ( !Puzzle::levelFileName.isEmpty() )
        play->savePlayground(tmp.append(".puzzle"),currPoints,Puzzle::levelFileName);
    else
        play->savePlayground(tmp.append(".puzzle"),currPoints,Puzzle::currLevelsName);        
}

void PuzzleWindow::loadGame(const QString &fileName)
{
    QString tmp(getenv("HOME")+QString("/puzz-le/")+fileName);

    if ( !loadGameState(tmp) )
    {
        errorMsg("Unable to load saved game."); 
    }
}
void PuzzleWindow::fileDialogDone()
{
    switchState(WELCOME);
}

bool PuzzleWindow::loadGameState(const QString &fileName)
{
    QString oldCurrLevelsName=Puzzle::currLevelsName;
    if ( play->loadPlayground(fileName,&currPoints,levels) )
    {

        aDlg->hide();
        fDlg->hide(); 
        state=GAME;
        currentLevel=play->number();
        mainButtonList->configureButtons(false,false);
        aboutButtonList->configureButtons(false,false);
        gameButtonList->configureButtons(true,false);      
        demoList.clear();               
        ox=(width()-play->width()*Puzzle::bSize)/2;
        oy=((height()-play->height()*Puzzle::bSize)/2);
        oy=QMAX(oy,PuzzleWindow::infoBarHeight);     
        repaint();
        if ( Puzzle::currLevelsName!=oldCurrLevelsName )
        {
            QString tmp;            
            if ( !Puzzle::currLevelsName.isEmpty() )
                tmp.sprintf("Switching to %s levels set.",Puzzle::currLevelsName.latin1());
            else
                tmp.sprintf("Switching to default levels set.");                            
            dlg->configure(pix,tmp,true,false,1800);
            dlg->exec();                
        }
        timer->start(Puzzle::timeoutValue);                
        return true;
    }
    else
    {
    	Puzzle::currLevelsName="";	// restore default levels set    
    }
    return false;
}

