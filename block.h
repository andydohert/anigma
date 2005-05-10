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
		RED,
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
	
	Playblock(int bx, int by,int x1=-1,int y1=-1, int x2=-9999,int y2=-9999, int aframe=0,GAME_BLOCK type=EMPTY,bool sel=false, bool grab=false);

	void recalculatePos(int nx, int ny);
	void recalculatePos();
	void createBlendedImage(QPixmap *tile, QPixmap *back, int ox, int oy, QPixmap *f);
	static QImage& blend(QImage& src, QImage& dst, float opacity);

	inline bool attached()
	{
		return iat;
	}

	inline void setLastDirection(int d)
	{
		iDir=d;
	}

	inline int lastDirection()
	{
		return iDir;
	}

	inline void setAttached(bool f=true)
	{
		iat=f;
	}

	inline void updatePos()
	{
		iy+=iay;
		ix+=iax;
	}

	inline void setX(int x)
	{
		ix=x;
	}

	inline void setY(int y)
	{
		iy=y;
	}

	inline void setAccelY(float a)
	{
		iay=a;
	}

	inline float accelY()
	{
		return iay;
	}

	inline void setAccelX(float a)
	{
		iax=a;
	}

	inline float accelX()
	{
		return iax;
	}

	inline int x()
	{
		return (int) (ix+0.5);
	}

	inline int y()
	{
		return (int)(iy+0.5);
	}

	inline int frame()
	{
		return iaf;
	}

	inline void setFrame(int f)
	{
		iaf=f;
	}

	inline void setStartPos(int x1, int y1)
	{
		ix1=x1;
		iy1=y1;
	}

	inline void setEndPos(int x2, int y2)
	{
		ix2=x2;
		iy2=y2;
	}

	inline void setCounter(int c)
	{
		cnt=c;
	}

	inline int counter()
	{
		return cnt;
	}

	inline bool isPointInside(int y)
	{
		if(y>=iy && y<iy+Puzzle::bSize)
			return true;
		else
			return false;
	}


	inline int bx()
	{
		return cx;
	}

	inline int by()
	{
		return cy;
	}

	inline void setCurrentBlockPos(int bx, int by)
	{
		cx=bx;
		cy=by;
	}

	inline int x1()
	{
		return ix1;
	}
	inline int y1()
	{
		return iy1;
	}
	inline int x2()
	{
		return ix2;
	}
	inline int y2()
	{
		return iy2;
	}

	inline GAME_BLOCK type()
	{
		return itype;
	}

	inline void setType(GAME_BLOCK t)
	{
		itype=t;
	}
	
	
	inline void setSelected(bool f)
	{
		iSel=f;
	}

	inline bool selected()
	{
		return iSel;
	}

	inline void setGrabbed( bool f)
	{
		iGrab=f;
	}

	inline bool grabbed()
	{
		return iGrab;
	}

	inline bool updated()
	{
		return iUpd;
	}

	inline void setUpdated(bool f)
	{
		iUpd=f;
	}

	void draw(QPainter *p,int ox=0,int oy=0);

// We need these operators for QSortedList.sort()

	friend inline int operator < (Playblock& left, Playblock& right)
	{
		if(left.iy>right.iy)
			return true;
		else
			return false;
	}
	friend inline int operator == (Playblock& left, Playblock& right)
	{
		if(left.iy==right.iy)
			return true;
		else
			return false;
	}

	QString toString();
	bool fromString(const QString &s);

private:

	GAME_BLOCK itype; // type of block

	int iSel;		// is selected ( used to paint selection box)
	int iGrab;	// is grabbed ( used to paint selection box)
	int iaf;		// current frame of animation
	float ix;		// pos x ( in pixels)
	float iy;		// pos y ( in pixels)
	int cx;			// current pos x ( in blocks)
	int cy;			// current pos y ( in blocks)
	int ix1;		// starting pos x ( in blocks , used by movable blocks) 
	int iy1;		// starting pos y ( in blocks , used by movable blocks) 
	int ix2;		// ending pos x ( in blocks , used by movable blocks) 
	int iy2;		// ending pos y ( in blocks , used for moving blocks)
	float iay;	// vertical acceleration
	float iax;	// horizontal acceleration ( used only for initial animation)
	int iat;		// whetber this box is attached to moving column
	int cnt;		// this is a generic counter used for controling speed of animation etc .. ( 33/sec )
	int iUpd;		// if true, block has been already processes by the current update()
	int iDir;		// last direction (1=up 0 =down)

};

#endif // PLAYBLOCK_H

