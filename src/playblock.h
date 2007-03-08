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

#ifndef PLAYBLOCK_H
#define PLAYBLOCK_H

#include "puzzle.h"

class QPixmap;
class QPainter;

class Playblock
{

public:
    enum GAME_BLOCK
    {
        RED = 0,
        YELLOW,
        BLUE,
        GREEN,
        PURPLE,
        STONE,
        BROKEN_STONE,
        MOVING_STONE,
        FIRE_STONE,
        TRAP_STONE,
        EMPTY,
        TRANSITIONAL,
        WHITE,
        CYAN,
        GREY,
        INVALID
    };

    Playblock(int bx = -1, int by = -1,
              int x1 = -1, int y1 = -1,
              int x2 = -9999, int y2 = -9999,
              GAME_BLOCK type = EMPTY);

    void recalculatePos(int nx, int ny);
    void recalculatePos();
    QPixmap createBlendedImage(const QPixmap &tile) const;
    
    inline bool attached() const
    {
        return iat;
    }

    inline void setLastDirection(int d)
    {
        iDir = d;
    }

    inline int lastDirection()
    {
        return iDir;
    }

    inline void setAttached(bool f = true)
    {
        iat = f;
    }

    inline void updatePos()
    {
        iy += iay;
        ix += iax;
    }

    inline void setX(int x)
    {
        ix = x;
    }

    inline void setY(int y)
    {
        iy = y;
    }

    inline void setAccelY(float a)
    {
        iay = a;
    }

    inline float accelY() const
    {
        return iay;
    }

    inline void setAccelX(float a)
    {
        iax = a;
    }

    inline float accelX() const
    {
        return iax;
    }

    inline int x() const
    {
        return (int) (ix + 0.5);
    }

    inline int y() const
    {
        return (int)(iy + 0.5);
    }

    inline int frame() const
    {
        return iaf;
    }

    inline void setFrame(int f)
    {
        iaf = f;
    }

    inline void setStartPos(int x1, int y1)
    {
        ix1 = x1;
        iy1 = y1;
    }

    inline void setEndPos(int x2, int y2)
    {
        ix2 = x2;
        iy2 = y2;
    }

    inline void setCounter(int c)
    {
        cnt = c;
    }

    inline int counter() const
    {
        return cnt;
    }

    inline bool isPointInside(int y) const
    {
        if(y >= iy && y < iy + Puzzle::blockPixelSize)
            return true;
        else
            return false;
    }

    inline int bx() const
    {
        return cx;
    }

    inline int by() const
    {
        return cy;
    }

    inline void setCurrentBlockPos(int bx, int by)
    {
        cx = bx;
        cy = by;
    }

    inline int x1() const
    {
        return ix1;
    }

    inline int y1() const
    {
        return iy1;
    }

    inline int x2() const
    {
        return ix2;
    }

    inline int y2() const
    {
        return iy2;
    }

    inline GAME_BLOCK type() const
    {
        return itype;
    }

    inline void setType(GAME_BLOCK t)
    {
        itype = t;
    }

    inline void setSelected(bool f)
    {
        iSel = f;
    }

    inline bool selected() const
    {
        return iSel;
    }

    inline void setGrabbed(bool f)
    {
        iGrab = f;
    }

    inline bool grabbed() const
    {
        return iGrab;
    }

    inline bool updated() const
    {
        return iUpd;
    }

    inline void setUpdated(bool f)
    {
        iUpd = f;
    }

    void paint(QPainter *p, int ox = 0, int oy = 0) const;

    static bool lessThan(Playblock* left, Playblock *right)
    {
        return (left->iy > right->iy);
    }

    QString toString() const;
    bool fromString(const QString &s);

private:
    int cx;   // current pos x ( in blocks)
    int cy;   // current pos y ( in blocks)
    int ix1;  // starting pos x ( in blocks , used by movable blocks)
    int iy1;  // starting pos y ( in blocks , used by movable blocks)
    int ix2;  // ending pos x ( in blocks , used by movable blocks)
    int iy2;  // ending pos y ( in blocks , used for moving blocks)
    int iaf;  // current frame of animation
    GAME_BLOCK itype; // type of block
    int iSel;  // is selected ( used to paint selection box)
    int iGrab; // is grabbed ( used to paint selection box)
    float ix;  // pos x ( in pixels)
    float iy;  // pos y ( in pixels)
    float iay; // vertical acceleration
    float iax; // horizontal acceleration ( used only for initial animation)
    int cnt;   // this is a generic counter used for controling speed of animation etc .. ( 33/sec )
    int iat;   // whetber this box is attached to moving column
    int iUpd;  // if true, block has been already processes by the current update()
    int iDir;  // last direction (1=up 0 =down)
};

#endif // PLAYBLOCK_H
