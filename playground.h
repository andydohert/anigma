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
** Copyright (C) 2004 Benjamin Meyer.   All rights reserved.
**
** This file is part of Puzz-le game.
**
** This file may be distributed and/or modified under the terms of the
** BSD license appearing in the file LICENSEL included in the
** packaging of this file.
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <qvector.h>
#include <qlist.h>
#include <qsortedlist.h>
#include <qbitarray.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qarray.h> 
#include <math.h>
#include "Puzzle.h"
#include "block.h"

class QPixmap;
class QCString;
class QPainter;

class Playground
{ 

public:	

	enum GAME_STATE
	{
		INVALID,			
		READY,
		ANIMATE,
		INPROGRESS,
		OVER,
		DEMO
	};

	 enum GAME_TYPE
	 {
			TIME_BASED,
			MOVES_BASED
	 };


	Playground();
	~Playground();

	bool loadLevel(const char * level[],unsigned int number=1);
	bool loadLevel(const QString & fileName,unsigned int number=1);

	QBitArray* paintPlayground(QPainter *p, int ox=0, int oy=0, bool drawAll=false);
	void paintAnimatedPlayground(QPainter *p, int ox=0, int oy=0);
	bool update();	

	bool setSelected(int px, int py, bool keyBoardMode=false);

	// return current positon of selected block ( in pixels, no offset, suitable to be used for setSelected - this is to emulate tapping with keyboard
	QPoint selectedPositon(int pd);

	inline GAME_TYPE gameType() const
	{
		 return gType;
	}

	inline bool isDeletionInProgresss()
	{
		 return (bool)deletionCounter;
	}

	inline int width()
	{
		return w;
	}
	inline int height()
	{
		return h;
	}
	inline int number()
	{
		return num;
	}
	inline int timeLimit()
	{
		return t;
	}
	inline int totalLevels()
	{
		return numLevels;
	}

	inline unsigned int currentPoints()
	{
		return levelPoints;
	}
	inline void clearPoints()
	{
		levelPoints=0;
	}

	void setGameState(GAME_STATE s);

	inline GAME_STATE gameState()
	{
		return gState;
	}

	inline bool isGameWon()
	{
		return gameWon;
	}
	inline bool isInfoChange()
	{
		bool result=updateInfoBar;
		updateInfoBar=false;
		return result;
	}

	inline int tickValue()
	{
		return tick;
	}
	
	bool savePlayground(const QString &fileName,unsigned int currPoints, const QString &levelFileName );
	bool loadPlayground(const QString &fileName, unsigned int *currPoints,const char * level[]);

	bool savedPlaygroundInfo(const QString &fileName,unsigned int *points, unsigned int *level);


private:

	bool moveBlock(int srcStripe, int destStripe, Playblock *pb);		// tries to move block into another stripe

	bool isEmpty(unsigned int bx, unsigned int by);		// returns if this position is empty
	inline bool isStone(unsigned int bx, unsigned int by)
	{
		Playblock::GAME_BLOCK b=grid.at(by*w+bx);
		return (b==Playblock::STONE);			
	}
	bool isEmptyByPixel( unsigned int px, unsigned int py);

	inline QRect pixelCoordinatesAt(unsigned int bx, unsigned int by) // given block position , returns its pixel coordinates
	{
		return QRect(bx*Puzzle::bSize,by*Puzzle::bSize,
			Puzzle::bSize,Puzzle::bSize);		
	}

	inline QPoint blockCoordinatesAt(unsigned int px,unsigned int py)
	{
		return QPoint(px/Puzzle::bSize,py/Puzzle::bSize);
	}

	inline bool isBlockStatic(Playblock *pb)
	{
		QRect r=pixelCoordinatesAt(pb->bx(),pb->by());
		if(r.y()==pb->y())
			return true;
		else
			return false;

	}

	Playblock* getVBlock(unsigned int bx,unsigned int by);

	bool isMovingLimit(Playblock *pb);

	Playblock* applyAcceleration(int stripeNum,Playblock *pb, float ac, bool normalize=false);
	void detachBlocks(int stripeNum,Playblock *pb);

	Playblock* blockAt(int bx, int by); //returns block at block position x,y 


	void initGrid(int w,int h);

	bool parseLevelHeader(const char *line);
	bool parseLevelLine(const char *line, int cl);
	void parseAttachedBlocks();

	void handleStripe(int stripeNum);
	void handleMovingBlockUp( int stripeNum, Playblock *pb);
	void handleMovingBlockDown( int stripeNum, Playblock *pb);
	bool handleBlock(int stripeNum, Playblock *pb);
	bool handleBrokenBlock(int stripeNum, Playblock *pb);
	bool handleFireBlock(int stripeNum, Playblock *pb);
	void handleTrapBlock(int stripeNum, Playblock *pb);
	void handleDeletion();
	void handleAnimation();

	bool clearBlocks();

	bool clearSelected();


	//bool updateBlock(Playblock *pb);

	QArray<Playblock::GAME_BLOCK> grid; // general outlay of the game grid ( 1- blocked, 0 - passable)
	QBitArray stripes; // list of update stripes ( 1- needs to be update, 0 - no need to update)
	QVector< QSortedList<Playblock> > dBlocks; // holds lists of movable objects for each stripe
	QVector< Playblock > vBlocks; // used to create 2d grid of clearable blocks for proximity checking and also for initial animation
	QList< Playblock > sBlocks; // list of special blocks ( fire, trap etc ..)
	unsigned int w; // width of the level ( in blocks)
	unsigned int h; // height of the level ( in blocks)
	unsigned int num; // level number
	unsigned int t; // time alloted for level completion (in seconds)
	Playblock *empty;
	Playblock *stone;
	Playblock *sb; // currently selected block

	int deletionCounter; // if > 0 then we are stuck in deletion loop
	bool needsClearCheck; // if true, check for blocks proximity needs to be performed
	int beat; // clock beat( 1 per second )
	unsigned int levelPoints; // total # of points 
	bool updateInfoBar; // if true, points or time needs to be updated
	bool gameWon; // after level is over, returns true if it was sucessfully completed
	GAME_STATE gState;
	int totalNumBlocks; // if ever gets to zero, the level has been solved
	int numLevels;
	int tick; // internal ever-increasing clock
	int initTicks;
	GAME_TYPE gType; // game type 
};

#endif // PLAYGROUND_H

