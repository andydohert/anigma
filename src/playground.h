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

#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <qvector.h>
#include <qlist.h>
#include <qbitarray.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qvector.h>
#include <math.h>

#include "puzzle.h"
#include "playblock.h"

class QPainter;

/**
 *  Game engine
 */
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

    bool loadLevel(const char * level[], unsigned int number = 1);
    bool loadLevel(const QString & fileName, unsigned int number = 1);

    QBitArray* paintPlayground(QPainter *p, int ox = 0, int oy = 0, bool drawAll = false);
    void paintAnimatedPlayground(QPainter *p, int ox = 0, int oy = 0);
    bool update();

    bool setSelected(int px, int py, bool keyBoardMode = false);

    // return current positon of selected block ( in pixels, no offset, suitable to be used for setSelected - this is to emulate tapping with keyboard
    QPoint selectedPositon(int pd);

    inline GAME_TYPE gameType() const
    {
        return gType;
    }

    inline bool isDeletionInProgresss() const
    {
        return (bool)deletionCounter;
    }

    inline int width() const
    {
        return w;
    }
    inline int height() const
    {
        return h;
    }
    inline int number() const
    {
        return levelNumber;
    }
    inline int timeLimit() const
    {
        return timeLeft;
    }
    inline int totalLevels() const
    {
        return numLevels;
    }

    inline unsigned int currentPoints() const
    {
        return levelPoints;
    }

    inline void clearPoints()
    {
        levelPoints = 0;
    }

    void setGameState(GAME_STATE s);

    inline GAME_STATE gameState() const
    {
        return gState;
    }

    inline bool isGameWon() const
    {
        return gameWon;
    }
    inline bool isInfoChange()
    {
        bool result = updateInfoBar;
        updateInfoBar = false;
        return result;
    }

    inline int tickValue() const
    {
        return tick;
    }

    int currentLevel;

    bool savePlayground(const QString &fileName, int currPoints, const QString &levelFileName ) const;
    bool loadPlayground(const QString &fileName, int *currPoints, const char * level[]);
    static bool savedPlaygroundInfo(const QString &fileName, unsigned int *points, unsigned int *level);

private:
    void cleanup();

    // tries to move block into another stripe
    bool moveBlock(int srcStripe, int destStripe, Playblock *pb);
    Playblock* blockAt(int bx, int by); //returns block at block position x,y

    Playblock* getVBlock(int bx, int by);

    bool isMovingLimit(Playblock *pb);

    Playblock* applyAcceleration(int stripeNum, Playblock *pb, float ac, bool normalize = false);
    void  detachBlocks(int stripeNum, Playblock *pb);

    bool isEmpty(int bx, int by) const;

    // returns if this position is empty
    inline bool isStone(int bx, int by) const
    {
        Playblock::GAME_BLOCK b = grid.at(by * w + bx);
        return (b == Playblock::STONE);
    }

    bool isEmptyByPixel(int px, int py) const;

    // given block position , returns its pixel coordinates
    inline QRect pixelCoordinatesAt(unsigned int bx, unsigned int by) const
    {
        return QRect(bx*Puzzle::blockPixelSize, by*Puzzle::blockPixelSize,
                     Puzzle::blockPixelSize, Puzzle::blockPixelSize);
    }

    inline QPoint blockCoordinatesAt(unsigned int px, unsigned int py) const
    {
        return QPoint(px / Puzzle::blockPixelSize, py / Puzzle::blockPixelSize);
    }

    inline bool isBlockStatic(Playblock *pb) const
    {
        QRect r = pixelCoordinatesAt(pb->bx(), pb->by());
        if(r.y() == pb->y())
            return true;
        else
            return false;
    }

    void initGrid(int w, int h);

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


    QVector<Playblock::GAME_BLOCK> grid; // general outlay of the game grid ( 1- blocked, 0 - passable)
    QBitArray stripes;    // list of update stripes ( 1- needs to be update, 0 - no need to update)
    QVector<QList<Playblock*> *> dBlocks; // holds lists of movable objects for each stripe
    QVector<Playblock*> vBlocks; // used to create 2d grid of clearable blocks for proximity checking and also for initial animation
    QList<Playblock*> sBlocks;     // list of special blocks ( fire, trap etc ..)
    int w;     // width of the level ( in blocks)
    int h;     // height of the level ( in blocks)

    unsigned int levelNumber;
    unsigned int timeLeft;     // time alloted for level completion (in seconds)
    Playblock *empty;
    Playblock *stone;
    Playblock *selectedBlock;

    int deletionCounter;   // if > 0 then we are stuck in deletion loop
    bool needsClearCheck;   // if true, check for blocks proximity needs to be performed
    int beat;      // clock beat( 1 per second )
    unsigned int levelPoints;    // total # of points
    bool updateInfoBar;    // if true, points or time needs to be updated
    bool gameWon;     // after level is over, returns true if it was sucessfully completed
    GAME_STATE gState;
    int totalNumBlocks;    // if ever gets to zero, the level has been solved
    int numLevels;
    int tick;      // internal ever-increasing clock
    int initTicks;
    GAME_TYPE gType;                 // game type
};

#endif // PLAYGROUND_H

