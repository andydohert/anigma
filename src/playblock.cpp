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

#include <qpainter.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qdebug.h>

#include "playblock.h"
#include "playground.h"
#include "imagerepository.h"

Playblock::Playblock(int bx, int by, int x1, int y1, int x2, int y2, GAME_BLOCK type)
        : cx(bx), cy(by), ix1(x1), iy1(y1), ix2(x2), iy2(y2), iaf(0), itype(type), iSel(false), iGrab(false), ix(0), iy(0), iay(0), cnt(0), iat(0), iDir(0)
{
    ix = cx * Puzzle::blockPixelSize;
    iy = cy * Puzzle::blockPixelSize;
    if ( type == MOVING_STONE ) {
        if ( by == y2 )
            iDir = 1;
        else
            iDir = 0;
    } else if ( type == TRAP_STONE ) {
        iaf = 4;
    }
}

QString Playblock::toString() const
{
    QString tmp;
    tmp.sprintf("%d-%d-%d-%d-%f-%f-%d-%d-%d-%d-%d-%d-%f-%f-%d-%d-%d-%d",
                itype, iSel, iGrab, iaf, ix, iy, cx, cy, ix1, iy1, ix2, iy2, iay, iax, iat, cnt, iUpd, iDir);
    return tmp;
}

bool Playblock::fromString(const QString &s)
{
    if ( sscanf( s.toLatin1(), "%d-%d-%d-%d-%f-%f-%d-%d-%d-%d-%d-%d-%f-%f-%d-%d-%d-%d",
                 (int*)&itype, &iSel, &iGrab, &iaf, &ix, &iy, &cx, &cy, &ix1, &iy1, &ix2, &iy2, &iay, &iax, &iat, &cnt, &iUpd, &iDir) == 18 )
        return true;
    else
        return false;
}

void Playblock::recalculatePos(int nx, int ny)
{
    cx = nx;
    cy = ny;
    ix = nx * Puzzle::blockPixelSize;
    iy = ny * Puzzle::blockPixelSize;
}

void Playblock::recalculatePos()
{
    cx = (int)ix / Puzzle::blockPixelSize;
    cy = (int)iy / Puzzle::blockPixelSize;
}

QPixmap Playblock::createBlendedImage(const QPixmap &tile) const
{
    QPixmap backImage(tile.width(), tile.height());
    backImage.fill(Qt::transparent);
    QPainter p(&backImage);
    QRectF target(0.0, 0.0, backImage.width(), backImage.height());
    p.setOpacity((float)1/16 * cnt);
    p.drawPixmap(target, tile, target);
    return backImage;
}

void Playblock::paint(QPainter *p, int ox, int oy) const
{
    if (p && Puzzle::images && p->isActive() ) {
        QString tmp;
        int sx = 0;
        if ( itype != TRAP_STONE ) {
            switch ( itype ) {
            case WHITE:
                tmp = "white";
                break;
            case CYAN:
                tmp = "cyan";
                break;
            case GREY:
                tmp = "grey";
                break;
            case YELLOW:
                tmp = "yellow";
                break;
            case GREEN:
                tmp = "green";
                break;
            case BLUE:
                tmp = "blue";
                break;
            case PURPLE:
                tmp = "purple";
                break;
            case RED:
                tmp = "red";
                break;
            case FIRE_STONE:
                tmp = "fire";
                sx = iaf * Puzzle::blockPixelSize;
                break;
            case TRAP_STONE:
                tmp = "trap_left";
                sx = iaf;
                break;
            case STONE:
            case MOVING_STONE:
                tmp = "wall";
                break;
            case BROKEN_STONE:
                tmp = ("broken_wall");
                sx = iaf * Puzzle::blockPixelSize;
                break;
            default:
                break;
            }
            QPixmap pix;

            if ( !tmp.isEmpty() ) {
                //if ( iGrab || (itype<STONE && cnt>0 && (cnt&1)) )
                if ( iGrab ) {
                    tmp.append("h");
                }
                pix = Puzzle::images->findPixmap(tmp);
            }
            if ( cnt && (itype < STONE || itype > TRANSITIONAL)) {
                QPixmap apix = createBlendedImage(pix);
                p->drawPixmap(ix + ox, iy + oy, apix, sx, 0, Puzzle::blockPixelSize, Puzzle::blockPixelSize);
            } else {
                p->drawPixmap(ix + ox, iy + oy, pix, sx, 0, Puzzle::blockPixelSize, Puzzle::blockPixelSize);
            }
            //p->drawPixmap((int)ix+ox,(int)iy+oy,*pix,sx,0,Puzzle::blockPixelSize,Puzzle::blockPixelSize);
            if ( itype == FIRE_STONE && iay ) {
                pix = Puzzle::images->findPixmap(ImageRepository::FREEZING_ANIMATION);
                p->drawPixmap(ix + ox, iy + oy, pix, (80 - (iay*Puzzle::blockPixelSize)), 0, Puzzle::blockPixelSize, Puzzle::blockPixelSize);
            }
        } else {
            QPixmap pix = Puzzle::images->findPixmap(ImageRepository::TRAP_LEFT);
            p->drawPixmap(ix + ox, iy + oy, pix, iaf, 0, (Puzzle::blockPixelSize - iaf), Puzzle::blockPixelSize);
            pix = Puzzle::images->findPixmap(ImageRepository::TRAP_RIGHT);
            p->drawPixmap(ix + ox + iaf, iy + oy, pix, 0, 0, (Puzzle::blockPixelSize - iaf), Puzzle::blockPixelSize);
        }
    }
}

