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

/****************************************************************************
** Main Game Window
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/
#ifndef PUZZLE_WINDOW_H
#define PUZZLE_WINDOW_H

#include <qmainwindow.h>
#include "playground.h"

class QTimer;
class MenuButton;
class MenuButtonList;
class QPixmap;
class QBitmap;
class GameDialog;
class AboutDialog;
class FileDialog;

class DemoMove 
{ 
public:

    DemoMove(int x, int y, int t);
	
	inline QString toString()
	{
		QString t;
		t.sprintf("%d %d %d",ix,iy,it);
		return t;
	}

	inline QPoint getPos()
	{
		return QPoint(ix,iy);
	}
	inline int getTick()
	{
		return it;
	}

private:
	int ix;
	int iy;
	int	it;

};

class PuzzleWindow : public QMainWindow
{ 
    Q_OBJECT

public:

   enum GAME_STATE 
   {
      GAME,
      ABOUT,
      HISTORY,
	  WELCOME,
	  BROWSE,
	  DEMO
   };

   enum BUTTONS 
   {
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


    PuzzleWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PuzzleWindow();



	inline GAME_STATE gameState()
	{
		return state;
	};

public slots:
	void switchState(GAME_STATE s);

	
protected:
    void keyPressEvent ( QKeyEvent * e );
    void mousePressEvent ( QMouseEvent * e );
    void paintEvent ( QPaintEvent *e ); 
    void resizeEvent ( QResizeEvent *e ); 
    void closeEvent ( QCloseEvent * e );
   
private slots:
	void synchTimeout();
   void buttonClicked(int id);
	void loadGame(const QString &fileName);
	void fileDialogDone();

private:
      
   void generateMovement(int x, int y, bool keyBoardMode=false);

   void drawTitleScreen(QPainter *p);
   void drawPlayground(QPainter *p=0,QPainter *lp=0,bool drawAll=false);
   void drawAboutScreen(QPainter *p);

   void updateInfoBar();

   void initGame();
   void startLevel(bool shuffle=true);   
   void restartLevel();
   void outOfTime();
   void abortGame();
   void levelCompleted();
   void gameCompleted();
   void demoCompleted();

   bool initDemo();
   void loadDemo(const QString &fileName);
   void loadDemo(const char * demo[]);
   void saveDemo();
   void saveDemoCPPVersion();

   void saveGameState(const QString &fileName);
   bool loadGameState(const QString &fileName);

   void	init();
   void initImages();

   void errorMsg(const QString &error);
   GAME_STATE  state;
   QTimer *timer;



   QColor  menuClr;
   QColor  hMenuClr;

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

// Title Screen Variables


	QPixmap *pix;
	Playground *play;
	QPixmap *info;
	QBitmap *infoMask;
	QList<DemoMove> demoList;
	DemoMove *curMove;
	int demoLevel;
	int currentLevel;
	unsigned int currPoints;
	unsigned int firstDemoLevel;
	int ox;
	int oy;
   bool isPaused;
	static int infoBarHeight;
	GameDialog *dlg;
	AboutDialog *aDlg;
	FileDialog  *fDlg;
};

#endif // PUZZLE_WINDOW_H

