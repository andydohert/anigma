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
**
**********************************************************************/

#include <qpainter.h>
#include <qbitmap.h>
#include <qpaintdevicemetrics.h>
#include "block.h"
#include "Playground.h"

Playblock::Playblock(int bx, int by,int x1, int y1,int x2,int y2, int aframe,GAME_BLOCK type,bool sel, bool grab)
:cx(bx),cy(by),ix1(x1),iy1(y1),ix2(x2),iy2(y2),iaf(aframe),itype(type),iSel(sel),iGrab(grab),ix(0),iy(0),iay(0),cnt(0),iat(0),iDir(0)
{
	ix=cx*Puzzle::bSize;
	iy=cy*Puzzle::bSize;
	if ( type==MOVING_STONE )
	{
		if ( by==y2 )
			iDir=1;
		else
			iDir=0;
	}
	else if ( type==TRAP_STONE )
	{
		iaf=4;
	}
}

QString Playblock::toString()
{
	QString tmp;

	tmp.sprintf("%d-%d-%d-%d-%f-%f-%d-%d-%d-%d-%d-%d-%f-%f-%d-%d-%d-%d",
				itype,iSel,iGrab,iaf,ix,iy,cx,cy,ix1,iy1,ix2,iy2,iay,iax,iat,cnt,iUpd,iDir);

	return tmp;
}

bool Playblock::fromString(const QString &s)
{
	if ( sscanf( s, "%d-%d-%d-%d-%f-%f-%d-%d-%d-%d-%d-%d-%f-%f-%d-%d-%d-%d",
				 &itype,&iSel,&iGrab,&iaf,&ix,&iy,&cx,&cy,&ix1,&iy1,&ix2,&iy2,&iay,&iax,&iat,&cnt,&iUpd,&iDir)==18 )
		return true;
	else
		return false;
}

void Playblock::recalculatePos(int nx, int ny)
{
	cx=nx;
	cy=ny;
	ix=nx*Puzzle::bSize;
	iy=ny*Puzzle::bSize;
}


void Playblock::recalculatePos()
{
	cx=(int)ix/Puzzle::bSize;
	cy=(int)iy/Puzzle::bSize;

}

QImage& Playblock::blend(QImage& src, QImage& dst, float opacity)
{
	unsigned int mult=(unsigned int)(opacity*(1<<8));

	if ( src.depth() != 32 ) src = src.convertDepth(32);
	if ( dst.depth() != 32 ) dst = dst.convertDepth(32);

	int pixels = src.width() * src.height();

	register unsigned char *data1;
	register unsigned char *data2;


// should be compile time switch

	if ( Puzzle::isBigEndian )
	{
		data1 = (unsigned char *)dst.bits() + 1;
		data2 = (unsigned char *)src.bits() + 1;
	}
	else
	{
		data1 = (unsigned char *)dst.bits();
		data2 = (unsigned char *)src.bits();
	}


	for ( register int i=0; i<pixels; i++ )
	{
		*(data1++)+=(unsigned char)((((unsigned int)(*(data2++) - *data1)<<8)*mult)>>16);
		*(data1++)+=(unsigned char)((((unsigned int)(*(data2++) - *data1)<<8)*mult)>>16);
		*(data1++)+=(unsigned char)((((unsigned int)(*(data2++) - *data1)<<8)*mult)>>16);
		data1++;
		data2++;
	}

	return dst;
}

void Playblock::createBlendedImage(QPixmap *tile, QPixmap *back, int ox, int oy, QPixmap *f)
{
	QPixmap tmpTile(tile->width(),tile->height());

	bitBlt(&tmpTile,0,0,back,(int)(ix+ox),(int)(iy+oy));
	QImage backImage=tmpTile.convertToImage();
	QImage final(tile->width(),tile->height(),tile->depth());
	QImage tileImage=tile->convertToImage();

	QBitmap mask(*tile->mask());

	float t=(float)cnt/16;

	blend(tileImage,backImage ,t);
	f->convertFromImage(backImage);
	f->setMask(mask);

}

void Playblock::draw(QPainter *p,int ox,int oy)
{
	if ( p && Puzzle::images && p->isActive() )
	{
		QPixmap *pix=0;
		QString tmp;
		int sx=0;

		if ( itype!=TRAP_STONE )
		{
			switch ( itype )
			{
				case	WHITE:
					tmp="white";
					break;
				case	CYAN:
					tmp="cyan";
					break;
				case	GREY:
					tmp="grey";
					break;
				case	YELLOW:
					tmp="yellow";
					break;
				case	GREEN:
					tmp="green";
					break;
				case	BLUE:
					tmp="blue";
					break;
				case	PURPLE:
					tmp="purple";
					break;
				case	RED:
					tmp="red";
					break;
				case	FIRE_STONE:
					tmp="fire";
					sx=iaf*Puzzle::bSize;
					break;
				case	TRAP_STONE:
					tmp="trap_left";
					sx=iaf;
					break;
				case	STONE:
				case	MOVING_STONE:
					tmp="wall";
					break;  
				case	BROKEN_STONE:
					tmp=("broken_wall"); 
					sx=iaf*Puzzle::bSize;
					break;  
				default:
					break;
			}


			if ( !tmp.isEmpty() )
			{
				//if ( iGrab || (itype<STONE && cnt>0 && (cnt&1)) )
				if ( iGrab )
				{
					tmp.append("h");
				}
				pix=Puzzle::images->findPixmap(tmp);
			}
			if ( pix )
			{
				if ( cnt && (itype<STONE || itype>TRANSITIONAL))
				{
					QPixmap apix(*pix);
					createBlendedImage(pix,Puzzle::images->findPixmap("background"),ox,oy,&apix);					
					p->drawPixmap((int)ix+ox,(int)iy+oy,apix,sx,0,Puzzle::bSize,Puzzle::bSize);					
				}
				else
				{
					p->drawPixmap((int)ix+ox,(int)iy+oy,*pix,sx,0,Puzzle::bSize,Puzzle::bSize);
				}
				//p->drawPixmap((int)ix+ox,(int)iy+oy,*pix,sx,0,Puzzle::bSize,Puzzle::bSize);
			}
			if ( itype==FIRE_STONE && iay )
			{
				pix=Puzzle::images->findPixmap(ImageRepository::FREEZING_ANIMATION);
				if ( pix )
				{
					p->drawPixmap((int)ix+ox,(int)iy+oy,*pix,(int)(80-(iay*Puzzle::bSize)),0,Puzzle::bSize,Puzzle::bSize);
				}
			}
		}
		else
		{

			pix=Puzzle::images->findPixmap(ImageRepository::TRAP_LEFT);
			if ( pix )
			{
				p->drawPixmap((int)ix+ox,(int)iy+oy,*pix,iaf,0,(int)(Puzzle::bSize-iaf),Puzzle::bSize);
			}
			pix=Puzzle::images->findPixmap(ImageRepository::TRAP_RIGHT);
			if ( pix )
			{
				p->drawPixmap((int)ix+ox+iaf,(int)iy+oy,*pix,0,0,(int)(Puzzle::bSize-iaf),Puzzle::bSize);
			}

		}
	}
}

