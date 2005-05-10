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

#include <qstring.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpaintdevicemetrics.h>
#include <stdlib.h>
#include "playground.h"
#include "imagerepository.h"
#include "soundrepository.h"

Playground::Playground()
{
	dBlocks.setAutoDelete(true);
	sBlocks.setAutoDelete(true);
	empty=new Playblock(0,0);
	stone=new Playblock(0,0);
	stone->setType(Playblock::STONE);
	deletionCounter=0;
	numLevels=0;
	sb=0;
	beat=50;
	totalNumBlocks=0;
	gState=INVALID;
	levelPoints=0;
}

Playground::~Playground()
{
	delete empty;
	delete stone;
}

bool Playground::loadLevel(const char * level[], unsigned int number)
{
	gState=INVALID;
	gType=TIME_BASED;

	if ( level )
	{
		numLevels=0;
		if ( sscanf( level[0], "%d", &numLevels) ==1 )
		{
			int i;
			unsigned int j;
			int counter=1;

			for ( i=0;i<numLevels;i++ )
			{
				if ( !parseLevelHeader(level[counter]) )
				{
					break;
				}
				if ( num==number )
				{
					counter++;
					initGrid(w,h);
					for ( j=0;j<h;j++ )
					{
						if ( !parseLevelLine(level[counter+j],j) )
						{
							return (bool)gState;
						}
					}
					gState=READY;
					break;
				}
				else
				{
					counter+=h+2;
				}
			}
		}
		parseAttachedBlocks();
	}
	return (bool)gState;
}

bool Playground::loadLevel(const QString & fileName,unsigned int number)
{
	gState=INVALID;

	QFile f(fileName);


	if ( f.exists() && f.open(IO_ReadOnly) )
	{

		QTextStream t( &f );
		QString line;
		unsigned int j;
		int counter=1;

		line=t.readLine();
		gType=TIME_BASED;
		if ( line.at(0)=='.' )
		{
			if ( line.at(1)=='1' )
				gType=MOVES_BASED;
			line=t.readLine();
		}
		sscanf( line, "%d", &numLevels);
		while ( !t.eof() )
		{
			line=t.readLine();
			counter--;
			if ( !counter )
			{
				if ( !parseLevelHeader(line.latin1()) )
				{
					break;
				}
				if ( num==number )
				{
					initGrid(w,h);
					for ( j=0;j<h;j++ )
					{
						if ( !parseLevelLine(t.readLine().latin1(),j) )
						{
							return (bool)gState;
						}
					}
					gState=READY;
					break;
				}
				else
				{
					counter=h+2;
				}
			}
		}
		f.close();
		parseAttachedBlocks();

	}
	return (bool)gState;
}

void Playground::initGrid(int w, int h)
{
	dBlocks.clear();
	sBlocks.clear();
	grid.fill(Playblock::EMPTY,w*h);
	dBlocks.resize(w);
	vBlocks.resize(w*h);
	deletionCounter=0;
	needsClearCheck=true;
	updateInfoBar=true;
	gameWon=false;
	totalNumBlocks=0;
	sb=0;
	levelPoints=0;
	int i;
	QSortedList<Playblock> *plist;

	for ( i=0;i<w;i++ )
	{
		plist=new QSortedList<Playblock>;
		plist->setAutoDelete(true);
		dBlocks.insert(i,plist);
	}
	stripes.fill(false,w);
}

void Playground::parseAttachedBlocks()
{
	if ( gState!=INVALID )
	{
		bool wasAttached;
		int lastY=0;
		Playblock *pb;
		QSortedList<Playblock> *plist;
		unsigned int i;


		for ( i=0;i<dBlocks.count();i++ )
		{
			plist=dBlocks.at(i);
			plist->sort();
			wasAttached=false;
			for ( pb=plist->first(); pb != 0; pb=plist->next() )
			{
				if ( pb->type()==Playblock::MOVING_STONE )
				{
					pb->setAttached(true);
					lastY=pb->y();
				}
				else
				{
					if ( wasAttached )
					{
						if ( lastY==pb->y()+Puzzle::bSize )
						{
							pb->setAttached(true);
							lastY=pb->y();
						}
					}
				}
				wasAttached=pb->attached();
			}

		}
	}
}

bool Playground::parseLevelHeader(const char *line)
{
	if ( !line || sscanf( line, "%d-%d-%d-%d", &num, &w, &h, &t ) < 4 )
		return false;
	else
		return true;
}

bool Playground::parseLevelLine(const char *line, int cl)
{
	unsigned int count=0;
	unsigned int offset=0;
	Playblock *pb=0;

	QString tmp(line);
	QString tc;
	int ex;
	int ey;

	if ( tmp.isEmpty() )
		return false;

	Playblock::GAME_BLOCK type=Playblock::EMPTY;

	while ( offset<tmp.length() )
	{
		pb=0;
		switch ( tmp.at(offset).latin1() )
		{
			case 'H':
			case 'h':
				type=Playblock::WHITE;
				break;
			case 'A':
			case 'a':
				type=Playblock::CYAN;
				break;
			case 'E':
			case 'e':
				type=Playblock::GREY;
				break;
			case 'R':
			case 'r':
				type=Playblock::RED;
				break;
			case 'Y':
			case 'y':
				type=Playblock::YELLOW;
				break;
			case 'B':
			case 'b':
				type=Playblock::BLUE;
				break;
			case 'P':
			case 'p':
				type=Playblock::PURPLE;
				break;
			case 'G':
			case 'g':
				type=Playblock::GREEN;
				break;
			case 'W':
			case 'w':
				type=Playblock::STONE;
				break;
			case 'C':
			case 'c':
				type=Playblock::BROKEN_STONE;
				break;
			case 'T':
			case 't':
				type=Playblock::TRAP_STONE;
				break;
			case 'F':
			case 'f':
				type=Playblock::FIRE_STONE;
				break;
			case 'M':
			case 'm':
				offset++;
				tc=tmp.mid(offset,2);
				if ( sscanf( tc.latin1(), "%d",&ex)!=1 )
				{
					return false;
				}
				offset+=2;
				tc=tmp.mid(offset,2);
				if ( sscanf( tc.latin1(), "%d",&ey)!=1 )
				{
					return false;
				}
				pb=new Playblock(count,cl,count,cl,ex,ey,0,Playblock::MOVING_STONE);
				if ( pb->y1()>pb->y2() )
				{
					int tmp=pb->y1();
					pb->setStartPos(pb->x1(),pb->y2());
					pb->setEndPos(pb->x1(),tmp);
				}
				type=Playblock::MOVING_STONE;
				offset++;
				break;
			default:
				type=Playblock::EMPTY;
				break;
		}

		switch ( type )
		{
			case	Playblock::STONE:
				grid.at(count+(cl*w))=Playblock::STONE;
				break;
			case	Playblock::MOVING_STONE:
				dBlocks.at(count)->append(pb); // already allocated so we just assign
				break;
			case	Playblock::RED:
			case	Playblock::YELLOW:
			case	Playblock::PURPLE:
			case	Playblock::GREEN:
			case	Playblock::BLUE:
			case	Playblock::WHITE:
			case	Playblock::CYAN:
			case	Playblock::GREY:
			case	Playblock::BROKEN_STONE:
				pb=new Playblock(count,cl,count,cl,0,0,0,type);
				dBlocks.at(count)->append(pb);
				if ( type<Playblock::STONE || type>Playblock::TRANSITIONAL)
					totalNumBlocks++;
				if ( type==Playblock::BROKEN_STONE )
					grid.at(count+(cl*w))=Playblock::BROKEN_STONE;
				break;
			case	Playblock::FIRE_STONE:
			case	Playblock::TRAP_STONE:
				pb=new Playblock(count,cl,count,cl,0,0,0,type);
				sBlocks.append(pb);
				break;
			default:
				grid.at(count+(cl*w))=Playblock::EMPTY;
				break;
		}
		offset++;
		count++;
	}
	if ( count<w )
		return false;
	return true;
}

// Update playground repainting only stripes which are marked dirty (1)

QBitArray * Playground::paintPlayground(QPainter *p, int ox, int oy, bool drawAll)
{
	if ( p && p->isActive() && Puzzle::images )
	{
		// First update background image for all dirty stripes


		unsigned int i,j;
		QPixmap *tmp=0;
		int selX=0;
		int selY=0;
		if ( sb && sb->type()==Playblock::STONE )
		{
			selX=sb->bx();
			selY=sb->by();
		}
		if ( drawAll )
		{
			stripes.fill(true);
			tmp=Puzzle::images->findPixmap(ImageRepository::BACKGROUND);
			if ( tmp )
				p->drawPixmap(0,0,*tmp);
			else
			{
				QPaintDevice *pd=p->device();
				if ( p )
				{
					QPaintDeviceMetrics pdm(pd);
					p->fillRect(0,0,pdm.width(),pdm.height(),QColor(0,0,0));

				}
			}

			for ( i=0;i<w;i++ )
			{
				for ( j=0;j<h;j++ )
				{
					if ( isStone(i,j) )
					{
						stone->recalculatePos(i,j);
						stone->draw(p,ox,oy);
					}
				}
			}
		}
		else
		{
			for ( i=0;i<stripes.size();i++ )
			{
				if ( stripes.at(i) )
				{
					tmp=Puzzle::images->findPixmap(ImageRepository::BACKGROUND);
					if ( tmp )
						p->drawPixmap(i*Puzzle::bSize+ox,oy,*tmp,i*Puzzle::bSize+ox,oy,
									Puzzle::bSize,h*Puzzle::bSize);
					else
						p->fillRect(i*Puzzle::bSize+ox,oy,Puzzle::bSize,h*Puzzle::bSize,QColor(0,0,0));

					for ( j=0;j<h;j++ )
					{
						if ( isStone(i,j) )
						{
							stone->recalculatePos(i,j);
							stone->draw(p,ox,oy);
						}
					}
				}
			}
		}
		if ( sb && sb->type()==Playblock::STONE )
		{
			sb->recalculatePos(selX,selY);

		}


		// Now loop thru all dynamic objects and repaint ones who belong to dirty stripes

		Playblock *pb;
		QSortedList<Playblock> *plist;


		for ( i=0;i<dBlocks.count();i++ )
		{
			if ( stripes.at(i) )
			{
				plist=dBlocks.at(i);
				for ( pb=plist->first(); pb != 0; pb=plist->next() )
				{
					pb->draw(p, ox,oy);
				}
			}

		}

		//Finally paint selection rectangle

		if ( sb && stripes.at(sb->bx()) )
		{
			if ( sb->grabbed() )
				p->setPen(QColor(255,255,255));
			else
				p->setPen(QColor(255,0,0));

			int ix=sb->x();
			int iy=sb->y();

			p->drawLine(ix+ox,iy+oy,ix+ox+4,iy+oy);
			p->drawLine(ix+ox,iy+oy+4,ix+ox,iy+oy);
			p->drawLine(ix+ox+Puzzle::bSize-4-1,iy+oy,ix+ox+Puzzle::bSize-1,iy+oy);
			p->drawLine(ix+ox+Puzzle::bSize-1,iy+oy,ix+ox+Puzzle::bSize-1,iy+oy+4);
			p->drawLine(ix+ox,iy+oy+Puzzle::bSize-4-1,ix+ox,iy+oy+Puzzle::bSize-1);
			p->drawLine(ix+ox,iy+oy+Puzzle::bSize-1,ix+ox+4,iy+oy+Puzzle::bSize-1);
			p->drawLine(ix+ox+Puzzle::bSize-4-1,iy+oy+Puzzle::bSize-1,ix+ox+Puzzle::bSize-1,iy+oy+Puzzle::bSize-1);
			p->drawLine(ix+ox+Puzzle::bSize-1,iy+oy+Puzzle::bSize-4-1,ix+ox+Puzzle::bSize-1,iy+oy+Puzzle::bSize-1);
			p->drawPoint(ix+ox+1,iy+oy+1);
			p->drawPoint(ix+ox+Puzzle::bSize-1-1,iy+oy+1);
			p->drawPoint(ix+ox+1,iy+oy+Puzzle::bSize-1-1);
			p->drawPoint(ix+ox+Puzzle::bSize-1-1,iy+oy+Puzzle::bSize-1-1);


		}
		for ( pb=sBlocks.first();pb!=0;pb=sBlocks.next() )
		{
			if ( stripes.at(pb->bx()) )
			{
				pb->draw(p, ox,oy);
			}
		}

	}

	return &stripes;
}

QPoint Playground::selectedPositon(int pd)
{
	qDebug(" Playground::selectedPositon");
		QPoint p(w*Puzzle::bSize/2,h*Puzzle::bSize/2);

	int offx=1;
	int offy=1;

	switch ( pd )
	{
		case	Qt::Key_Up:
			offy-=Puzzle::bSize;
			break;
		case  Qt::Key_Down:
			offy+=Puzzle::bSize;
			break;
		case  Qt::Key_Left:
			offx-=Puzzle::bSize;
			break;
		case	Qt::Key_Right:
			offx+=Puzzle::bSize;
			break;
		default:
			break;
	}


	if ( sb )
	{
		p.setX(sb->x()+offx);
		p.setY(sb->y()+offy);
	}



	return p;
}


bool Playground::setSelected(int px, int py,bool keyBoardMode)
{
	bool result=false;

	if ( deletionCounter || gState!=INPROGRESS )
		return false;

	stripes.fill(false);

	QPoint bc=blockCoordinatesAt(px,py);

	if ( bc.x()< (int)w && bc.y()< (int)h && px>=0 && py>=0 )
	{
		QSortedList<Playblock> *plist;

		Playblock *pbn;

		plist=dBlocks.at(bc.x());
		for ( pbn=plist->first(); pbn != 0; pbn=plist->next() )
		{
			if ( pbn->isPointInside(py) )
				break;
		}
		if ( !pbn )
		{
			if ( isEmpty(bc.x(),bc.y()) )
				pbn=empty;
			else
				pbn=stone;
		}
		// handle attempt to select already selected block
		if ( sb && (bc.x()==sb->bx() && bc.y()==sb->by()) )
		{
			if ( pbn && (pbn->type()<Playblock::STONE || pbn->type()>Playblock::TRANSITIONAL))
			{
				pbn->setGrabbed(!pbn->grabbed());
				stripes.setBit(sb->bx());
				sb=pbn;
				return true;
			}
			return false;
		}

		// we have already selected and grabbed block so we try to move it
		if ( sb && sb->grabbed() && (sb->type()<Playblock::STONE || sb->type()>Playblock::TRANSITIONAL) && (pbn->type()>=Playblock::STONE && pbn->type()<Playblock::WHITE))
		{
			if ( sb->attached() || sb->accelY()==0 )
			{
				int destStripe;

				if ( bc.x()<sb->bx() )
					destStripe=sb->bx()-1;
				else if ( bc.x()>sb->bx() )
					destStripe=sb->bx()+1;
				else
				{
					return false;
				}

				result=moveBlock(sb->bx(), destStripe,sb);
				if ( result )
				{
					Puzzle::sounds->playSound("move");
					if ( Puzzle::timeLimit && gType==MOVES_BASED )
					{
						t--;
						updateInfoBar=true;
					}
				}
				if ( clearBlocks() )
					deletionCounter=Puzzle::deletionDelay;
				return result;
			}
			return false;

		}

		// otherwise just switch selection to a new block if selectable or switch selection to empty field

		if ( pbn )
		{
			clearSelected();
			pbn->setSelected(true);
			if ( (pbn->type()<Playblock::STONE || pbn->type()>Playblock::TRANSITIONAL) && !keyBoardMode )
				pbn->setGrabbed(true);
			else
			{
				if ( pbn->type()==Playblock::STONE || pbn->type()==Playblock::EMPTY )
					pbn->recalculatePos(bc.x(),bc.y());

			}
			sb=pbn;
			stripes.setBit(sb->bx());
			result=true;
		}
	}


	return result;
}

void Playground::handleAnimation()
{
	QSortedList<Playblock> *plist;
	Playblock *pb;
	stripes.fill(true);

	unsigned int i;
	for ( i=0;i<dBlocks.count();i++ )
	{
		plist=dBlocks.at(i);
		for ( pb=plist->first(); pb != 0; pb=plist->next() )
		{
			if ( initTicks )
			{
				pb->updatePos();
			}
			else
			{
				pb->recalculatePos(pb->bx(), pb->by());
				pb->setAccelX(0);
				pb->setAccelY(0);
				gState=INPROGRESS;
			}
		}
	}
	if ( initTicks )
		initTicks--;
}

bool Playground::update()
{

	unsigned int i;
	bool result=false;


	if ( gState!=INPROGRESS )
	{
		if ( gState==ANIMATE )
		{
			handleAnimation();
			return true;
		}
		return false;
	}

	tick++;
	Playblock *pb;
	if ( deletionCounter )
	{
		handleDeletion();
		for ( pb=sBlocks.first();pb!=0;pb=sBlocks.next() )
		{
			switch ( pb->type() )
			{
				case	Playblock::FIRE_STONE:
					handleFireBlock(pb->bx(), pb);
					break;
				default:
					break;
			}
		}
		for ( i=0;i<stripes.size();i++ )
		{
			if ( stripes.testBit(i) )
			{
				result=true;
				break;
			}
		}
	}
	else
	{
		if ( Puzzle::timeLimit )
		{
			if ( gType==TIME_BASED )
			{
				if ( beat )
					beat--;
				else
				{
					beat=33;
					t--;
					updateInfoBar=true;
				}
				if ( !t )
				{
					gState=OVER;
				}

			}

		}
		for ( i=0;i<dBlocks.count();i++ )
		{
			handleStripe(i);
		}
		for ( pb=sBlocks.first();pb!=0;pb=sBlocks.next() )
		{
			switch ( pb->type() )
			{
				case	Playblock::FIRE_STONE:

					if ( handleFireBlock(pb->bx(), pb) )
						deletionCounter=Puzzle::deletionDelay;
					break;
				case	Playblock::TRAP_STONE:
					handleTrapBlock(pb->bx(), pb);
					break;
				default:
					break;
			}
		}
		for ( i=0;i<stripes.size();i++ )
		{
			if ( stripes.testBit(i) || needsClearCheck )
			{
				result=true;
				if ( clearBlocks() )
					deletionCounter=Puzzle::deletionDelay;
				break;
			}
		}
	}

	if ( Puzzle::timeLimit && gType==MOVES_BASED && !t && !result )
	{
		gState=OVER;
		updateInfoBar=true;
	}
	

	needsClearCheck=false;


	if ( updateInfoBar )
		result=true;
	return result;
}

void Playground::handleDeletion()
{
	Playblock *pb;
	QSortedList<Playblock> *plist;

	unsigned int i;
	bool wasDeleted;
	deletionCounter--;
	QList<Playblock> tmpList;

	for ( i=0;i<dBlocks.count();i++ )
	{
		plist=dBlocks.at(i);
		wasDeleted=false;

		for ( pb=plist->first(); pb != 0; pb=plist->next() )
		{
			if ( (pb->type()<Playblock::STONE || pb->type()>Playblock::TRANSITIONAL) && pb->counter() )
			{
				pb->setCounter(deletionCounter);
				levelPoints+=Puzzle::blockBonus/Puzzle::deletionDelay; // won't work if deletionDelay > blockBonus (make it float)
				if ( !pb->counter() )
				{
					wasDeleted=true;
					if ( sb==pb )
					{
						sb=0;
						sb=empty;
						empty->setSelected(true);
						sb->recalculatePos(pb->bx(),pb->by());
					}
					tmpList.append(pb);
				}
				updateInfoBar=true;
				stripes.setBit(i);
			}
		}
		if ( tmpList.first() )
			detachBlocks(i,tmpList.first());

		int remainder=Puzzle::blockBonus%Puzzle::deletionDelay;

		for ( pb=tmpList.first(); pb != 0; pb=tmpList.next() )
		{
			levelPoints+=remainder;
			plist->removeRef(pb);
			totalNumBlocks--;
		}
		tmpList.clear();
		if ( wasDeleted )
		{
			plist->sort();
		}
	}
	if ( !totalNumBlocks )
	{
		gState=OVER;
		gameWon=true;
		if ( Puzzle::timeLimit )
			levelPoints+=Puzzle::timeBonus*t;

	}
}
// Update single stripe

void Playground::handleStripe(int stripeNum)
{
	Playblock *pb;
	QSortedList<Playblock> *plist;

	plist=dBlocks.at(stripeNum);

	// clear update flag for all blocks
	for ( pb=plist->first(); pb != 0; pb=plist->next() )
	{
		pb->setUpdated(false);
	}

	// Update all stones moving upwards

	QListIterator<Playblock> it(*plist);

	for ( ; it.current(); ++it )
	{
		pb=it.current();
		if ( !pb->updated() && pb->type()==Playblock::MOVING_STONE )
		{
			if ( !pb->accelY() )
			{
				if ( pb->counter() )
				{
					pb->setCounter(pb->counter()-1);
					pb->setUpdated(true);
				}
				else
				{
					if ( pb->lastDirection() )
						handleMovingBlockDown(stripeNum,pb);
					else
						handleMovingBlockUp(stripeNum,pb);
				}
			}
			else if ( pb->accelY()<0 )
			{
				handleMovingBlockUp(stripeNum,pb);
			}
		}
	}

	// now update all stones moving downwards as well as free faling blocks
	it.toLast();
	for ( ; it.current(); --it )
	{
		pb=it.current();
		if ( !pb->updated() )
		{
			switch ( pb->type() )
			{
				case	Playblock::MOVING_STONE:
					handleMovingBlockDown(stripeNum,pb);
					break;
				case	Playblock::BROKEN_STONE:
					if ( handleBrokenBlock(stripeNum,pb) )
						stripes.setBit(stripeNum);
					break;
				default:
					if ( !pb->attached() && handleBlock(stripeNum,pb) )
						stripes.setBit(stripeNum);
					break;
			}
		}
	}


}

bool Playground::handleBrokenBlock(int stripeNum, Playblock *pb)
{

	if ( pb->type()==Playblock::BROKEN_STONE )
	{
		QSortedList<Playblock> *plist=dBlocks.at(stripeNum);
		plist->findRef(pb);
		Playblock *prev=plist->next();

		if ( prev && (prev->type()<Playblock::STONE || prev->type()>Playblock::TRANSITIONAL) && prev->y()+Puzzle::bSize==pb->y() )
		{
			if ( pb->counter() )
			{
				pb->setCounter(pb->counter()-1);
			}
			else
			{
				pb->setFrame(pb->frame()+1);
				if ( pb->frame()==5 )
				{
					if ( sb==pb )
						sb=0;
					grid.at(stripeNum+(pb->by()*w))=Playblock::EMPTY;
					plist->removeRef(pb);

					return true;
				}
				else
					pb->setCounter(Puzzle::brokenDelay);
				return true;
			}
		}
		else
		{
			pb->setCounter(0);
		}
	}

	return false;
}

bool Playground::handleBlock(int stripeNum, Playblock *pb)
{
	bool justSet=false;

	pb->setUpdated(true);
	if ( !pb->accelY() )
	{
		if ( isEmpty(pb->bx(),pb->by()+1) )
		{
			pb->setAccelY(Puzzle::fallAccel);
			justSet=true;
		}
	}
	if ( pb->accelY() )
	{
		//int oldY=pb->y();
		pb->updatePos();
		if ( isEmptyByPixel(pb->x(),pb->y()+Puzzle::bSize-1) )
		{
			QSortedList<Playblock> *plist=dBlocks.at(stripeNum);
			plist->findRef(pb);
			Playblock *next=plist->prev();
			if ( next && pb->y()+Puzzle::bSize > next->y() )
			{
				pb->setY(next->y()-Puzzle::bSize);
				if ( next->attached() )
				{
					pb->setAttached(true);
					pb->setAccelY(next->accelY());
				}
				else
				{
					pb->setAccelY(0);
					if ( !justSet )
						needsClearCheck=true;
				}
				if ( !justSet )
				{
					if ( next->type()>=Playblock::STONE && next->type()<Playblock::WHITE)
						Puzzle::sounds->playSound("wall_hit");
					else
						Puzzle::sounds->playSound("block_hit");
				}
			}
			pb->recalculatePos();
		}
		else
		{
			pb->setAccelY(0);
			QPoint p=blockCoordinatesAt(pb->x(),pb->y()+Puzzle::bSize-1);
			pb->recalculatePos(pb->bx(),p.y()-1);
			needsClearCheck=true;
			Puzzle::sounds->playSound("wall_hit");
		}
		if ( justSet && !pb->accelY() )
			return false;
		else
		{
			return true;
		}

	}

	return false;
}

void Playground::handleTrapBlock(int stripeNum, Playblock *pb)
{
	QSortedList<Playblock> *plist;

	plist=dBlocks.at(stripeNum);

	Playblock *cb;

	if ( isEmpty(pb->bx(),pb->by()) )
	{

		for ( cb=plist->first(); cb != 0; cb=plist->next() )
		{
			if ( cb->by()==pb->by() )
			{
				grid.at(pb->by()*w+pb->bx())=Playblock::TRANSITIONAL;
				pb->setCounter(3);
				break;
			}
		}
	}
	else
	{
		if ( !pb->frame() )
		{
			grid.at(pb->by()*w+pb->bx())=Playblock::STONE;
			sBlocks.removeRef(pb);
			stripes.setBit(stripeNum);
			return;
		}
		if ( pb->counter() )
		{
			pb->setCounter(pb->counter()-1);
		}
		else
		{
			pb->setFrame(pb->frame()-1);
			pb->setCounter(0);
			stripes.setBit(stripeNum);
		}
	}
}

bool Playground::handleFireBlock(int stripeNum, Playblock *pb)
{
	bool cleared=false;

	QSortedList<Playblock> *plist;

	plist=dBlocks.at(stripeNum);

	Playblock *cb;

	if ( isEmpty(pb->bx(),pb->by()) )
	{

		for ( cb=plist->first(); cb != 0; cb=plist->next() )
		{
			if ( cb->by()==pb->by() )
			{
				cb->setCounter(Puzzle::deletionDelay);
				grid.at(pb->by()*w+pb->bx())=Playblock::TRANSITIONAL;
				cb->setGrabbed(false);
				pb->setAccelY(6);
				cleared=true;
				Puzzle::sounds->playSound("delete");
				break;
			}
		}
	}
	else
	{
		if ( pb->accelY()==1 )
		{
			grid.at(pb->by()*w+pb->bx())=Playblock::STONE;
			sBlocks.removeRef(pb);
			stripes.setBit(stripeNum);
			return false;
		}
	}
	if ( pb->accelY() && !deletionCounter )
	{
		pb->setAccelY(pb->accelY()-1);
		pb->setCounter(0);
	}
	if ( pb->counter() )
	{
		pb->setCounter(pb->counter()-1);
	}
	else
	{
		pb->setFrame(pb->frame()+1);
		if ( pb->frame()==10 )
		{
			pb->setFrame(0);
		}

		pb->setCounter(2);
		stripes.setBit(stripeNum);
	}

	return cleared;
}

void Playground::handleMovingBlockDown( int stripeNum, Playblock *pb)
{

	QSortedList<Playblock> *plist=dBlocks.at(stripeNum);

	Playblock *last;
	QListIterator<Playblock> it(*plist);

	pb->setLastDirection(0);


	if ( pb->by()<=pb->y2() )
	{
		if ( !pb->accelY() )
		{
			applyAcceleration(stripeNum, pb,Puzzle::movingAccel);
		}
		it+=plist->findRef(pb);
		for ( ; it.current(); ++it )
		{
			last=it.current();
			if ( !last->attached() || (last->type()==Playblock::MOVING_STONE && last!=pb) )
			{
				break;
			}

			last->setUpdated(true);
			int oldY=pb->y();
			last->updatePos();
			if ( isEmptyByPixel(last->x(),last->y()+Puzzle::bSize-1) )
			{
				plist->findRef(last);
				Playblock *next=plist->prev();
				if ( next && last->y()+Puzzle::bSize > next->y() )
				{
					last->setY(next->y()-Puzzle::bSize);
					if ( oldY==last->y() )
						last->setAccelY(0);

				}

				last->recalculatePos();
			}
			else
			{
				pb->recalculatePos();
				applyAcceleration(stripeNum,pb,0,true);
			}
		}
		if ( pb->type()==Playblock::MOVING_STONE && isMovingLimit(pb) )
		{
			applyAcceleration(stripeNum,pb,0);
		}
	}
	if ( !pb->accelY() )
	{
		pb->setCounter(Puzzle::movingDelay);
	}

	needsClearCheck=true;
	stripes.setBit(stripeNum);
}



void Playground::handleMovingBlockUp( int stripeNum, Playblock *pb)
{
	QSortedList<Playblock> *plist=dBlocks.at(stripeNum);

	Playblock *last;

	pb->setLastDirection(1);

	if ( pb->by()>=pb->y1() )
	{
		last=applyAcceleration(stripeNum,pb,Puzzle::movingAccel*-1);

		QListIterator<Playblock> it(*plist);

		it.toFirst();
		it+=plist->findRef(last);
		while ( true )
		{
			int oldY=last->y();
			last->updatePos();
			last->setUpdated(true);
			if ( isEmptyByPixel(last->x(),last->y()) )
			{
				plist->findRef(last);
				Playblock *next=plist->next();
				if ( next )
				{
					if ( next->y()+Puzzle::bSize>last->y() )
					{
						last->setY(next->y()+Puzzle::bSize);
						if ( oldY==last->y() && ((next->attached() && next->accelY()==0) || next->type()==Playblock::BROKEN_STONE) )
							last->setAccelY(0);
					}
				}
				last->recalculatePos();
			}
			else
			{
				last->recalculatePos(last->bx(),last->by());
				last->setAccelY(0);
			}

			if ( last==pb )
				break;
			else
			{
				--it;
				last=it.current();

			}
		}
		if ( pb->type()==Playblock::MOVING_STONE && isMovingLimit(pb) )
		{
			applyAcceleration(stripeNum,pb,0,true);
		}
	}
	if ( !pb->accelY() )
	{
		pb->setCounter(Puzzle::movingDelay);
	}

	needsClearCheck=true;
	stripes.setBit(stripeNum);
}

bool Playground::isEmptyByPixel( unsigned int px, unsigned int py)
{
	int bx=px/Puzzle::bSize;
	int by=py/Puzzle::bSize;

	return isEmpty(bx,by);
}

// Verify if given MOVING_STONE block is at its limit ( top or bottom)

bool Playground::isMovingLimit(Playblock *pb)
{
	bool result=false;

	QRect r;
	if ( pb->accelY()>0 )
	{
		r=pixelCoordinatesAt(pb->bx(),pb->y2());
		if ( pb->y()>=r.y() )
		{
			pb->setY(r.y());
			result=true;
		}
	}
	else
	{
		r=pixelCoordinatesAt(pb->bx(),pb->y1());
		if ( pb->y()<=r.y() )
		{
			pb->setY(r.y());
			pb->recalculatePos();
			result=true;
		}
	}
	return result;
}
// Detach *pb and all blocks above it with attached() atribute being true

void Playground::detachBlocks(int stripeNum,Playblock *pb)
{
	QSortedList<Playblock> *plist=dBlocks.at(stripeNum);
	QListIterator<Playblock> it(*plist);
	it+=plist->findRef(pb);

	for ( ; it.current(); ++it )
	{

		if ( !it.current()->attached() || it.current()->type()==Playblock::MOVING_STONE )
		{
			break;
		}
		it.current()->setAttached(false);
		it.current()->setAccelY(0);
	}

}


// Apply acceleration to *pb and all blocks above it with attached() atribute being true

Playblock* Playground::applyAcceleration(int stripeNum, Playblock *pb, float ac,bool normalize)
{
	Playblock *last=0;
	if ( pb->type()==Playblock::MOVING_STONE )
	{
		int cy=pb->by();
		QSortedList<Playblock> *plist=dBlocks.at(stripeNum);
		QListIterator<Playblock> it(*plist);
		it+=plist->findRef(pb);

		for ( ; it.current(); ++it )
		{

			if ( !it.current()->attached() || (it.current()->type()==Playblock::MOVING_STONE && it.current()!=pb) )
			{
				break;
			}
			last=it.current();
			last->setAccelY(ac);
			if ( normalize )
			{
				last->recalculatePos(last->bx(),cy);
				cy--;
			}
		}
	}

	return last;

}
// Clear current selection

bool Playground::clearSelected()
{
	bool result=false;
	if ( gState!=INVALID && sb )
	{
		sb->setSelected(false);
		sb->setGrabbed(false);
		stripes.setBit(sb->bx());
		result=true;
	}

	return result;
}
// Move block horizontally

// Basically, do some checks and if the new position is valid, remove block from
// one stripe and insert it into another stripe.
// If user is trying to squeeze block between two wall blocks , given him some slack ( if at least 50% of the block is
// facing empty space, let the user move the block by snapping it into empty place.)
// Return true if move was successful.

bool Playground::moveBlock(int srcStripe, int destStripe, Playblock *pb)
{
	if ( destStripe< (int)w && destStripe>=0 )
	{
		QSortedList<Playblock> *dlist=dBlocks.at(destStripe);
		QSortedList<Playblock> *slist=dBlocks.at(srcStripe);

		int nx=pb->x()+((destStripe-srcStripe)*Puzzle::bSize);
		int ny=pb->y();

		int addOffset=0;

		bool needToSnap=false;
		if ( !isEmptyByPixel(nx,ny) )
			needToSnap=true;

		if ( !isEmptyByPixel(nx,ny+(Puzzle::bSize/2)) )
			return false;
		if ( needToSnap )
			addOffset=1;

		if ( !isEmptyByPixel(nx,ny+Puzzle::bSize-(Puzzle::bSize/2)) )
		{
			return false;
		}
		if ( !isEmptyByPixel(nx,ny+Puzzle::bSize) )
		{
			needToSnap=true;
		}
		QListIterator<Playblock> it(*dlist);
		for ( ; it.current(); ++it )
		{
			if ( it.current()->isPointInside(ny) || it.current()->isPointInside(ny+Puzzle::bSize-1) )
			{
				if ( !(it.current()->type()==Playblock::BROKEN_STONE && needToSnap) )
					return false;
			}
		}
		if ( pb->attached() )
		{
			detachBlocks(srcStripe,pb);
		}
		slist->find (pb);
		slist->take();
		slist->sort();
		dlist->append(pb);
		stripes.setBit(srcStripe);
		stripes.setBit(destStripe);
		pb->setX(nx);
		if ( needToSnap )
			pb->recalculatePos(destStripe, pb->by()+addOffset);
		else
			pb->setCurrentBlockPos(destStripe,pb->by());
		dlist->sort();
		if ( isEmpty(pb->bx(),pb->by()+1) )
		{
			pb->setAccelY(Puzzle::fallAccel);
		}
		return true;

	}

	return false;
}

// Figure out which blocks need to be deleted.

// Create grid of all static ( not in between grid positions) blocks and then
// find all horizontally or vertically adjusted blocks of the same color.
// Set their internal counter to deletionDelay.
// Return true if there are blocks to be deleted.

bool Playground::clearBlocks()
{

	Playblock *pb;
	Playblock *next;
	QSortedList<Playblock> *plist;

	vBlocks.fill(empty);

	bool cleared=false;
	unsigned int i,j;
	int blocksCounted=0;

	for ( i=0;i<dBlocks.count();i++ )
	{
		plist=dBlocks.at(i);
		for ( pb=plist->first(); pb != 0; pb=plist->next() )
		{
			if ( (pb->type()<Playblock::STONE || pb->type()>Playblock::TRANSITIONAL) && (pb->attached() || pb->accelY()==0) && isBlockStatic(pb) )
			{
				vBlocks.insert(pb->bx()+pb->by()*w,pb);
				blocksCounted++;
			}
		}
	}

	for ( i=0;i<h;i++ )
	{
		for ( j=0;j<w;j++ )
		{
			pb=getVBlock(j,i);
			if ( pb && pb->type()!=Playblock::EMPTY )
			{
				next=getVBlock(j+1,i);
				if ( next && pb->type()==next->type() )
				{
					pb->setCounter(Puzzle::deletionDelay);
					next->setCounter(Puzzle::deletionDelay);
					pb->setGrabbed(false);
					next->setGrabbed(false);
					cleared=true;
				}
				next=getVBlock(j,i+1);
				if ( next && pb->type()==next->type() )
				{
					pb->setCounter(Puzzle::deletionDelay);
					next->setCounter(Puzzle::deletionDelay);
					pb->setGrabbed(false);
					next->setGrabbed(false);
					cleared=true;
				}
				blocksCounted--;
				if ( !blocksCounted )
					break;
			}
		}
	}

	if ( cleared )
		Puzzle::sounds->playSound("delete");

	return cleared;
}

void Playground::setGameState(GAME_STATE s)
{
	gState=s;
	if ( s==INPROGRESS )
	{
		tick=0;
		gState=ANIMATE;
		needsClearCheck=true;

		QSortedList<Playblock> *plist;
		Playblock *pb;
		unsigned int i;
		float sx,sy;
		float ex,ey;
		initTicks=Puzzle::initNumSteps;

		int aStart=rand() / (RAND_MAX / 12 + 1); // middle should happen three times as often as any other position

		for ( i=0;i<dBlocks.count();i++ )
		{
			plist=dBlocks.at(i);
			for ( pb=plist->first(); pb != 0; pb=plist->next() )
			{

				switch ( aStart )
				{
					case	0: // bottom right corner
						sx=(w-1)*Puzzle::bSize;
						sy=(h-1)*Puzzle::bSize;
						break;
					case	1: // bottom left corner
						sx=0;
						sy=(h-1)*Puzzle::bSize;
						break;
					case	2: // top left corner
						sx=0;
						sy=0;
						break;
					case	3: // top right corner
						sx=(w-1)*Puzzle::bSize;;
						sy=0;
						break;
					default:	// middle
						sx=(w*Puzzle::bSize-(Puzzle::bSize/2))/2;
						sy=(h*Puzzle::bSize-(Puzzle::bSize/2))/2;
						break;
				}

				ex=(float)pb->x();
				ey=(float)pb->y();
				pb->setX((int)sx);
				pb->setY((int)sy);
				pb->setAccelY((int)((ey-sy)/(float)Puzzle::initNumSteps));
				pb->setAccelX((int)((ex-sx)/(float)Puzzle::initNumSteps));
			}
		}
	}
}

bool Playground::savePlayground(const QString &fileName, unsigned int currPoints, const QString &levelFileName)
{
	QFile file(fileName);
	bool result=false;
	if ( file.open(IO_WriteOnly|IO_Truncate) )
	{
		QTextStream ts(&file);
		if ( gType==MOVES_BASED )
		{
			ts << ".1\n";
		}
		ts << num << "-" << w << "-" << h << "-" << t << "\n"; // level number, width, height, time left
		ts << levelFileName << "\n"; // empty line or level file name
		ts << currPoints << "\n";
		ts << levelPoints << "\n";
		ts << "\n";
		unsigned int x,y;
		for ( y=0;y<h;y++ )
		{
			for ( x=0;x<w;x++ )
			{
				switch ( grid.at(y*w+x) )
				{
					case	Playblock::TRANSITIONAL:
						ts << "t";
						break;
					case	Playblock::EMPTY:
						ts << ".";
						break;
					case	Playblock::STONE:
						ts << "w";
						break;
					case	Playblock::BROKEN_STONE:
						ts << "b";
						break;
					default:
						break;
				}
			}
			ts << "\n";
		}
		ts << "\n";

		QSortedList<Playblock> *plist;
		Playblock *pb;

		for ( x=0;x<dBlocks.count();x++ )
		{
			plist=dBlocks.at(x);
			ts << x << "-" << plist->count() << "\n";
			for ( pb=plist->first(); pb != 0; pb=plist->next() )
			{
				ts << pb->toString();
				ts << "\n";
			}
		}
		ts << "\n";

		ts << sBlocks.count() << "\n";
		for ( pb=sBlocks.first(); pb != 0; pb=sBlocks.next() )
		{
			ts << pb->toString();
			ts << "\n";
		}
		ts << "\n";
		ts << deletionCounter << "-" << beat << "-" << totalNumBlocks << "\n";

		file.close();
	}
	return result;
}

// Load specified saved game
// It will return false in case :
//	1) Saved game file is corrupted
//	2) Saved game file is for level text file that is NOT present on this system

bool Playground::loadPlayground(const QString &fileName,unsigned int *currPoints,const char * level[])
{
	gState=INVALID;
	QFile f(fileName);


	if ( f.exists() && f.open(IO_ReadOnly) )
	{
		QString line;
		QTextStream t( &f );

		// load basic level information
		line=t.readLine();
		gType=TIME_BASED;
		if ( line.at(0)=='.' )
		{
			if ( line.at(1)=='1' )
				gType=MOVES_BASED;
			line=t.readLine();
		}
		
		if ( sscanf( line, "%d-%d-%d-%d", &num,&w,&h,&this->t)!=4 )
			goto error;


		int savedLimit=this->t;

		// load level file name
		line=t.readLine();
		if ( !line.isEmpty() )
		{
			#ifndef DEMO_VERSION
			if ( QFile::exists(line) ) // see if this is full file path or just level set name
			{
				if ( !loadLevel(line,num) )
					goto error;
			}
			else
			{
				Puzzle::currLevelsName=line;
				if ( !loadLevel(ROOTHOME+"pics/puzz-le/levels/"+Puzzle::currLevelsName+"/levels",num) )
					goto error;
			}
			#else
			goto error;
			#endif
		}
		else
		{
			Puzzle::currLevelsName="";
			loadLevel(level,num);
		}

		this->t=savedLimit;


		// load points information
		line=t.readLine();
		initGrid(w,h);
		if ( currPoints )
		{
			if ( sscanf( line, "%d", currPoints)!=1 )
				goto error;
		}
		line=t.readLine();
		if ( sscanf( line, "%d", &levelPoints)!=1 )
			goto error;

		line=t.readLine();

		// load grid entries
		unsigned int x,y;

		for ( y=0;y<h;y++ )
		{

			line=t.readLine();
			if ( line.isEmpty() )
				goto error;
			for ( x=0;x<w;x++ )
			{
				switch ( line.at(x).latin1() )
				{
					case	'.':
						grid.at(y*w+x)=Playblock::EMPTY;
						break;
					case	'w':
						grid.at(y*w+x)=Playblock::STONE;
						break;
					case	't':
						grid.at(y*w+x)=Playblock::EMPTY;
						break;
					case	'b':
						grid.at(y*w+x)=Playblock::BROKEN_STONE;
						break;
					default:
						goto error;
				}
			}
		}

		line=t.readLine();

		// get standard blocks
		unsigned int snum,bnum;
		QSortedList<Playblock> *plist;
		Playblock *pb;

		for ( x=0;x<w;x++ )
		{
			line=t.readLine();
			if ( sscanf( line, "%d-%d", &snum,&bnum)!=2 )
				goto error;

			plist=dBlocks.at(x);
			for ( y=0;y<bnum;y++ )
			{
				line=t.readLine();
				pb= new Playblock(0,0);
				plist->append(pb);
				if ( !pb->fromString(line) )
					goto error;
				if ( pb->selected() )
					sb=pb;
			}
		}

		line=t.readLine();

		// load special blocks
		line=t.readLine();
		if ( sscanf( line, "%d",&bnum)!=1 )
			goto error;

		for ( y=0;y<bnum;y++ )
		{
			line=t.readLine();
			pb= new Playblock(0,0);
			sBlocks.append(pb);
			if ( !pb->fromString(line) )
				goto error;
		}

		line=t.readLine();

		// load Playblock class variables
		line=t.readLine();
		if ( sscanf( line, "%d-%d-%d", &deletionCounter,&beat,&totalNumBlocks)!=3 )
			goto error;

		gState=INPROGRESS;
	}
	error:

	if ( f.isOpen() )
		f.close();

	return (bool) gState;
}

// Scan saved game for levelNumber, and points

bool Playground::savedPlaygroundInfo(const QString &fileName,unsigned int *points, unsigned int *level)
{
	bool result=false;
	QFile f(fileName);

	if ( f.exists() && f.open(IO_ReadOnly) )
	{
		int wd,hg,tm,pt;
		QString line;
		QTextStream t( &f );

		// load basic level information
		line=t.readLine();
		if ( line.at(0)=='.' )
		{
			line=t.readLine();
		}

		if ( sscanf( line, "%d-%d-%d-%d", level,&wd,&hg,&tm)!=4 )
			goto error;
		// skip level file name
		line=t.readLine();

		// get points information
		line=t.readLine();

		if ( sscanf( line, "%d", &pt)!=1 )
			goto error;

		*points=pt;
		line=t.readLine();
		if ( sscanf( line, "%d", &pt)!=1 )
			goto error;
		(*points)+=pt;
		result=true;
	}
	error:

	if ( f.isOpen() )
		f.close();

	return result;
}

bool Playground::isEmpty(unsigned int bx, unsigned int by)
{
	Playblock::GAME_BLOCK b=grid.at(by*w+bx);
	return !(b==Playblock::TRANSITIONAL || b==Playblock::STONE || b==Playblock::BROKEN_STONE);
}

Playblock* Playground::getVBlock(unsigned int bx,unsigned int by)
{
	if ( bx<w && by <h )
		return vBlocks.at(bx+by*w);
	else
		return 0;
}

