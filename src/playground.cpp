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

#include "playground.h"

#include <qstring.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qfile.h>
#include <qtextstream.h>
#include <stdlib.h>
#include <qdebug.h>

#include "imagerepository.h"
#include "soundrepository.h"

Playground::Playground()
{
    empty = new Playblock(0, 0);
    stone = new Playblock(0, 0);
    stone->setType(Playblock::STONE);

    deletionCounter = 0;
    numLevels = 0;
    selectedBlock = 0;
    beat = 50;
    totalNumBlocks = 0;
    gState = INVALID;
    levelPoints = 0;
}

Playground::~Playground()
{
    cleanup();
    delete empty;
    delete stone;
}

void Playground::cleanup()
{
    for (int i = 0; i < dBlocks.count(); ++i) {
        while (!dBlocks[i]->isEmpty())
            delete dBlocks[i]->takeAt(0);
        delete dBlocks[i];
    }
    dBlocks.clear();
    while (!sBlocks.isEmpty())
        delete sBlocks.takeAt(0);
}

bool Playground::loadLevel(const char * level[], unsigned int levelNumberber)
{
    gState = INVALID;
    gType = TIME_BASED;
    if ( level ) {
        numLevels = 0;
        if ( sscanf( level[0], "%d", &numLevels) == 1 ) {
            int counter = 1;
            for (int i = 0; i < numLevels;i++ ) {
                if ( !parseLevelHeader(level[counter]) ) {
                    break;
                }
                if ( levelNumber == levelNumberber ) {
                    counter++;
                    initGrid(w, h);
                    for (int j = 0;j < h;j++ ) {
                        if ( !parseLevelLine(level[counter+j], j) ) {
                            return (bool)gState;
                        }
                    }
                    gState = READY;
                    break;
                } else {
                    counter += h + 2;
                }
            }
        }
        parseAttachedBlocks();
    }
    return (bool)gState;
}

bool Playground::loadLevel(const QString & fileName, unsigned int levelNumberber)
{
    gState = INVALID;
    QFile f(fileName);
    if ( f.exists() && f.open(QIODevice::ReadOnly) ) {
        QTextStream stream( &f );
        QString line;
        int counter = 1;

        line = stream.readLine();
        gType = TIME_BASED;
        if ( line.at(0) == '.' ) {
            if ( line.at(1) == '1' )
                gType = MOVES_BASED;
            line = stream.readLine();
        }
        sscanf( line.toLatin1(), "%d", &numLevels);
        while ( !stream.atEnd() ) {
            line = stream.readLine();
            counter--;
            if ( !counter ) {
                if ( !parseLevelHeader(line.toLatin1()) ) {
                    break;
                }
                if ( levelNumber == levelNumberber ) {
                    initGrid(w, h);
                    for (int j = 0;j < h;j++ ) {
                        if ( !parseLevelLine(stream.readLine().toLatin1(), j) ) {
                            return (bool)gState;
                        }
                    }
                    gState = READY;
                    break;
                } else {
                    counter = h + 2;
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
    cleanup();

    grid.fill(Playblock::EMPTY, w * h);
    dBlocks.resize(w);
    vBlocks.resize(w * h);
    deletionCounter = 0;
    needsClearCheck = true;
    updateInfoBar = true;
    gameWon = false;
    totalNumBlocks = 0;
    selectedBlock = 0;
    levelPoints = 0;
    for (int i = 0; i < w; ++i) {
        QList<Playblock*> *plist = new QList<Playblock*>();
        dBlocks[i] = plist;
    }
    stripes.fill(false, w);
}

void Playground::parseAttachedBlocks()
{
    if ( gState != INVALID ) {
        bool wasAttached;
        int lastY = 0;
        QList<Playblock*> *plist;
        for (int i = 0; i < dBlocks.count(); ++i) {
            plist = dBlocks[i];
            qSort(plist->begin(), plist->end(), Playblock::lessThan);
            wasAttached = false;
            for (int j = 0; j < plist->count(); ++j) {
                Playblock *block = plist->at(j);
                if ( block->type() == Playblock::MOVING_STONE ) {
                    block->setAttached(true);
                    lastY = block->y();
                } else {
                    if ( wasAttached ) {
                        if ( lastY == block->y() + Puzzle::blockPixelSize ) {
                            block->setAttached(true);
                            lastY = block->y();
                        }
                    }
                }
                wasAttached = block->attached();
            }

        }
    }
}

bool Playground::parseLevelHeader(const char *line)
{
    if ( !line || sscanf( line, "%d-%d-%d-%d", &levelNumber, &w, &h, &timeLeft ) < 4 )
        return false;
    else
        return true;
}

bool Playground::parseLevelLine(const char *line, int cl)
{
    int count = 0;
    int offset = 0;
    Playblock *block = 0;

    QString tmp(line);
    QString tc;
    int ex;
    int ey;

    if ( tmp.isEmpty() )
        return false;

    Playblock::GAME_BLOCK type = Playblock::EMPTY;
    while ( offset < tmp.length() ) {
        block = 0;
        switch ( tmp.at(offset).toLatin1() ) {
        case 'H':
        case 'h':
            type = Playblock::WHITE;
            break;
        case 'A':
        case 'a':
            type = Playblock::CYAN;
            break;
        case 'E':
        case 'e':
            type = Playblock::GREY;
            break;
        case 'R':
        case 'r':
            type = Playblock::RED;
            break;
        case 'Y':
        case 'y':
            type = Playblock::YELLOW;
            break;
        case 'B':
        case 'b':
            type = Playblock::BLUE;
            break;
        case 'P':
        case 'p':
            type = Playblock::PURPLE;
            break;
        case 'G':
        case 'g':
            type = Playblock::GREEN;
            break;
        case 'W':
        case 'w':
            type = Playblock::STONE;
            break;
        case 'C':
        case 'c':
            type = Playblock::BROKEN_STONE;
            break;
        case 'T':
        case 't':
            type = Playblock::TRAP_STONE;
            break;
        case 'F':
        case 'f':
            type = Playblock::FIRE_STONE;
            break;
        case 'M':
        case 'm':
            offset++;
            tc = tmp.mid(offset, 2);
            if ( sscanf( tc.toLatin1(), "%d", &ex) != 1 ) {
                return false;
            }
            offset += 2;
            tc = tmp.mid(offset, 2);
            if ( sscanf( tc.toLatin1(), "%d", &ey) != 1 ) {
                return false;
            }
            block = new Playblock(count, cl, count, cl, ex, ey, Playblock::MOVING_STONE);
            if ( block->y1() > block->y2() ) {
                int tmp = block->y1();
                block->setStartPos(block->x1(), block->y2());
                block->setEndPos(block->x1(), tmp);
            }
            type = Playblock::MOVING_STONE;
            offset++;
            break;
        default:
            type = Playblock::EMPTY;
            break;
        }

        switch ( type ) {
        case    Playblock::STONE:
            grid[count+(cl*w)] = Playblock::STONE;
            break;
        case    Playblock::MOVING_STONE:
            dBlocks[count]->append(block);  // already allocated so we just assign
            break;
        case    Playblock::RED:
        case    Playblock::YELLOW:
        case    Playblock::PURPLE:
        case    Playblock::GREEN:
        case    Playblock::BLUE:
        case    Playblock::WHITE:
        case    Playblock::CYAN:
        case    Playblock::GREY:
        case    Playblock::BROKEN_STONE:
            block = new Playblock(count, cl, count, cl, 0, 0, type);
            dBlocks[count]->append(block);
            if ( type < Playblock::STONE || type > Playblock::TRANSITIONAL)
                totalNumBlocks++;
            if ( type == Playblock::BROKEN_STONE )
                grid[count+(cl*w)] = Playblock::BROKEN_STONE;
            break;
        case    Playblock::FIRE_STONE:
        case    Playblock::TRAP_STONE:
            block = new Playblock(count, cl, count, cl, 0, 0, type);
            sBlocks.append(block);
            break;
        default:
            grid[count+(cl*w)] = Playblock::EMPTY;
            break;
        }
        offset++;
        count++;
    }
    if ( count < w )
        return false;
    return true;
}

// Update playground repainting only stripes which are marked dirty (1)
QBitArray * Playground::paintPlayground(QPainter *p, int ox, int oy, bool drawAll)
{
    if ( p && p->isActive() && Puzzle::images ) {
        // First update background image for all dirty stripes
        QPixmap tmp;
        int selX = 0;
        int selY = 0;
        if ( selectedBlock && selectedBlock->type() == Playblock::STONE ) {
            selX = selectedBlock->bx();
            selY = selectedBlock->by();
        }
        if ( drawAll ) {
            stripes.fill(true);
            tmp = Puzzle::images->findPixmap(ImageRepository::BACKGROUND);
            p->drawPixmap(0, 0, tmp);
            for (int i = 0;i < w;i++ ) {
                for (int j = 0;j < h;j++ ) {
                    if ( isStone(i, j) ) {
                        stone->recalculatePos(i, j);
                        stone->paint(p, ox, oy);
                    }
                }
            }
        } else {
            for (int i = 0;i < stripes.size();i++ ) {
                if ( stripes.at(i) ) {
                    tmp = Puzzle::images->findPixmap(ImageRepository::BACKGROUND);
                    p->drawPixmap(i*Puzzle::blockPixelSize + ox, oy, tmp, i*Puzzle::blockPixelSize + ox, oy,
                                      Puzzle::blockPixelSize, h*Puzzle::blockPixelSize);
                    
                    for (int j = 0;j < h;j++ ) {
                        if ( isStone(i, j) ) {
                            stone->recalculatePos(i, j);
                            stone->paint(p, ox, oy);
                        }
                    }
                }
            }
        }
        if ( selectedBlock && selectedBlock->type() == Playblock::STONE ) {
            selectedBlock->recalculatePos(selX, selY);
        }

        // Now loop thru all dynamic objects and repaint ones who belong to dirty stripes
        Playblock *block;
        QList<Playblock*> *plist;
        for (int i = 0; i < dBlocks.count(); ++i) {
            if ( stripes.at(i) ) {
                plist = dBlocks[i];
                for (int j = 0; j < plist->count(); ++j) {
                    plist->at(j)->paint(p, ox, oy);
                }
            }

        }

        //Finally paint selection rectangle
        if ( selectedBlock && stripes.at(selectedBlock->bx()) ) {
            if ( selectedBlock->grabbed() )
                p->setPen(QColor(255, 255, 255));
            else
                p->setPen(QColor(255, 0, 0));

            int ix = selectedBlock->x();
            int iy = selectedBlock->y();

            p->drawLine(ix + ox, iy + oy, ix + ox + 4, iy + oy);
            p->drawLine(ix + ox, iy + oy + 4, ix + ox, iy + oy);
            p->drawLine(ix + ox + Puzzle::blockPixelSize - 4 - 1, iy + oy, ix + ox + Puzzle::blockPixelSize - 1, iy + oy);
            p->drawLine(ix + ox + Puzzle::blockPixelSize - 1, iy + oy, ix + ox + Puzzle::blockPixelSize - 1, iy + oy + 4);
            p->drawLine(ix + ox, iy + oy + Puzzle::blockPixelSize - 4 - 1, ix + ox, iy + oy + Puzzle::blockPixelSize - 1);
            p->drawLine(ix + ox, iy + oy + Puzzle::blockPixelSize - 1, ix + ox + 4, iy + oy + Puzzle::blockPixelSize - 1);
            p->drawLine(ix + ox + Puzzle::blockPixelSize - 4 - 1, iy + oy + Puzzle::blockPixelSize - 1, ix + ox + Puzzle::blockPixelSize - 1, iy + oy + Puzzle::blockPixelSize - 1);
            p->drawLine(ix + ox + Puzzle::blockPixelSize - 1, iy + oy + Puzzle::blockPixelSize - 4 - 1, ix + ox + Puzzle::blockPixelSize - 1, iy + oy + Puzzle::blockPixelSize - 1);
            p->drawPoint(ix + ox + 1, iy + oy + 1);
            p->drawPoint(ix + ox + Puzzle::blockPixelSize - 1 - 1, iy + oy + 1);
            p->drawPoint(ix + ox + 1, iy + oy + Puzzle::blockPixelSize - 1 - 1);
            p->drawPoint(ix + ox + Puzzle::blockPixelSize - 1 - 1, iy + oy + Puzzle::blockPixelSize - 1 - 1);
        }
        for (int j = 0; j < sBlocks.count(); ++j) {
            block = sBlocks[j];
            if ( stripes.at(block->bx()) ) {
                block->paint(p, ox, oy);
            }
        }
    }

    return &stripes;
}

QPoint Playground::selectedPositon(int pd)
{
    QPoint p(w*Puzzle::blockPixelSize / 2, h*Puzzle::blockPixelSize / 2);

    int offx = 1;
    int offy = 1;

    switch ( pd ) {
    case    Qt::Key_Up:
        offy -= Puzzle::blockPixelSize;
        break;
    case   Qt::Key_Down:
        offy += Puzzle::blockPixelSize;
        break;
    case  Qt::Key_Left:
        offx -= Puzzle::blockPixelSize;
        break;
    case    Qt::Key_Right:
        offx += Puzzle::blockPixelSize;
        break;
    default:
        break;
    }

    if ( selectedBlock ) {
        p.setX(selectedBlock->x() + offx);
        p.setY(selectedBlock->y() + offy);
    }
    return  p;
}

bool Playground::setSelected(int px, int py, bool keyBoardMode)
{
    if ( deletionCounter || gState != INPROGRESS )
        return false;

    stripes.fill(false);

    if (px < 0 || py < 0)
        return false;

    QPoint bc = blockCoordinatesAt(px, py);
    bool result = false;
    if (bc.x() < w && bc.y() < h) {
        Playblock *block = 0;
        QList<Playblock*> *plist = dBlocks.at(bc.x());
        for (int i = 0; i < plist->count(); ++i) {
            if (plist->at(i)->isPointInside(py)) {
                block = plist->at(i);
                break;
            }
        }
        if ( !block ) {
            if (isEmpty(bc.x(), bc.y()) )
                block = empty;
            else
                block = stone;
        }

        // handle attempt to select already selected block
        if (selectedBlock && (bc.x() == selectedBlock->bx() && bc.y() == selectedBlock->by()) ) {
            if ( block && (block->type() < Playblock::STONE  || block->type() > Playblock::TRANSITIONAL)) {
                block->setGrabbed(!block->grabbed());
                stripes.setBit(selectedBlock->bx());
                selectedBlock = block;
                return true;
            }
            return false;
        }

        // we have already selected and grabbed block so we try to move it
        if ( selectedBlock && selectedBlock->grabbed()
                && (selectedBlock->type() < Playblock::STONE || selectedBlock->type() > Playblock::TRANSITIONAL)
                && (block->type() >= Playblock::STONE && block->type() < Playblock::WHITE)) {
            if ( selectedBlock->attached() || selectedBlock->accelY() == 0 ) {
                int destStripe;
                if ( bc.x() < selectedBlock->bx() )
                    destStripe = selectedBlock->bx() - 1;
                else if ( bc.x() > selectedBlock->bx() )
                    destStripe = selectedBlock->bx() + 1;
                else {
                    return false;
                }

                result = moveBlock(selectedBlock->bx(), destStripe, selectedBlock);
                if ( result ) {
                    //Puzzle::sounds->playSound("move");
                    if ( Puzzle::timeLimit && gType == MOVES_BASED ) {
                        timeLeft--;
                        updateInfoBar = true;
                    }
                }
                if ( clearBlocks() )
                    deletionCounter = Puzzle::deletionDelay;
                return result;
            }
            return false;
        }

        // otherwise just switch selection to a new block if selectable
        // or switch selection to empty field
        if (block) {
            clearSelected();
            block->setSelected(true);
            if ( (block->type() < Playblock::STONE
                    || block->type() > Playblock::TRANSITIONAL) && !keyBoardMode )
                block->setGrabbed(true);
            else {
                if (block->type() == Playblock::STONE || block->type() == Playblock::EMPTY )
                    block->recalculatePos(bc.x(), bc.y());
            }
            selectedBlock = block;
            stripes.setBit(selectedBlock->bx());
            result = true;
        }
    }
    return result;
}

void Playground::handleAnimation()
{
    stripes.fill(true);
    for (int i = 0;i < dBlocks.count();i++ ) {
        QList<Playblock*> *plist = dBlocks[i];
        for (int j = 0; j < plist->count(); ++j) {
            Playblock *block = (*plist)[j];
            if ( initTicks ) {
                block->updatePos();
            } else {
                block->recalculatePos(block->bx(), block->by());
                block->setAccelX(0);
                block->setAccelY(0);
                gState = INPROGRESS;
            }
        }
    }
    if ( initTicks )
        initTicks--;
}

bool Playground::update()
{
    if ( gState != INPROGRESS ) {
        if ( gState == ANIMATE ) {
            handleAnimation();
            return true;
        }
        return false;
    }

    tick++;
    bool result = false;
    if (deletionCounter) {
        handleDeletion();
        for (int i = 0; i < sBlocks.count(); ++i) {
            Playblock *block = sBlocks.at(i);
            switch ( block->type() ) {
            case    Playblock::FIRE_STONE:
                handleFireBlock(block->bx(), block);
                break;
            default:
                break;
            }
        }
        for (int i = 0;i < stripes.size();i++ ) {
            if ( stripes.testBit(i) ) {
                result = true;
                break;
            }
        }
    } else {
        if ( Puzzle::timeLimit ) {
            if ( gType == TIME_BASED ) {
                if ( beat )
                    beat--;
                else {
                    beat = 33;
                    timeLeft--;
                    updateInfoBar = true;
                }
                if ( !timeLeft ) {
                    gState = OVER;
                }

            }

        }
        for (int i = 0;i < dBlocks.count();i++ ) {
            handleStripe(i);
        }
        for (int i = 0; i < sBlocks.count(); ++i) {
            Playblock *block = sBlocks.at(i);
            switch ( block->type() ) {
            case    Playblock::FIRE_STONE:
                if ( handleFireBlock(block->bx(), block) )
                    deletionCounter = Puzzle::deletionDelay;
                break;
            case    Playblock::TRAP_STONE:
                handleTrapBlock(block->bx(), block);
                break;
            default:
                break;
            }
        }
        for (int i = 0;i < stripes.size();i++ ) {
            if ( stripes.testBit(i) || needsClearCheck ) {
                result = true;
                if ( clearBlocks() )
                    deletionCounter = Puzzle::deletionDelay;
                break;
            }
        }
    }

    if (Puzzle::timeLimit && gType == MOVES_BASED && !timeLeft && !result ) {
        gState = OVER;
        updateInfoBar = true;
    }

    needsClearCheck = false;

    if ( updateInfoBar )
        result = true;
    return result;
}

void Playground::handleDeletion()
{
    QList<Playblock*> *plist;
    bool wasDeleted;
    deletionCounter--;
    QList<Playblock*> tmpList;
    for (int i = 0; i < dBlocks.count(); ++i) {
        plist = dBlocks[i];
        wasDeleted = false;
        for (int j = 0; j < plist->count(); ++j) {
            Playblock *block = plist->at(j);
            if ( (block->type() < Playblock::STONE || block->type() > Playblock::TRANSITIONAL)
                 && block->counter() ) {
                block->setCounter(deletionCounter);
                levelPoints += Puzzle::blockBonus / Puzzle::deletionDelay; // won't work if deletionDelay > blockBonus (make it float)
                if ( !block->counter() ) {
                    wasDeleted = true;
                    if ( selectedBlock == block ) {
                        selectedBlock = 0;
                        selectedBlock = empty;
                        empty->setSelected(true);
                        selectedBlock->recalculatePos(block->bx(), block->by());
                    }
                    tmpList.append(block);
                }
                updateInfoBar = true;
                stripes.setBit(i);
            }
        }
        if (!tmpList.isEmpty())
            detachBlocks(i, tmpList.first());

        int remainder = Puzzle::blockBonus % Puzzle::deletionDelay;
        for (int j = 0; j < tmpList.count(); ++j) {
            Playblock *block = tmpList.at(j);
            levelPoints += remainder;
            plist->removeAt(plist->indexOf(block));
            delete block;
            totalNumBlocks--;
        }
        tmpList.clear();
        if ( wasDeleted ) {
            qSort(plist->begin(), plist->end(), Playblock::lessThan);
        }
    }
    if ( !totalNumBlocks ) {
        gState = OVER;
        gameWon = true;
        if ( Puzzle::timeLimit )
            levelPoints += Puzzle::timeBonus * timeLeft;
    }
}

// Update single stripe
void Playground::handleStripe(int stripeNum)
{
    QList<Playblock*> *plist = dBlocks[stripeNum];

    // clear update flag for all blocks
    for (int i = 0; i < plist->count(); ++i) {
        (*plist)[i]->setUpdated(false);
    }

    // Update all stones moving upwards
    for (int i = 0; i < plist->count(); ++i) {
        Playblock *block = plist->at(i);
        if (!block->updated() && block->type() == Playblock::MOVING_STONE) {
            if (!block->accelY()) {
                if (block->counter()) {
                    block->setCounter(block->counter() - 1);
                    block->setUpdated(true);
                } else {
                    if (block->lastDirection())
                        handleMovingBlockDown(stripeNum, block);
                    else
                        handleMovingBlockUp(stripeNum, block);
                }
            } else if ( block->accelY() < 0 ) {
                handleMovingBlockUp(stripeNum, block);
            }
        }
    }

    // now update all stones moving downwards as well as free faling blocks
    for (int i = plist->count() - 1; i >= 0; --i) {
        Playblock *block = plist->at(i);
        if (!block->updated()) {
            switch ( block->type() ) {
            case Playblock::MOVING_STONE:
                handleMovingBlockDown(stripeNum, block);
                break;
            case Playblock::BROKEN_STONE:
                if (handleBrokenBlock(stripeNum, block) )
                    stripes.setBit(stripeNum);
                break;
            default:
                if (!block->attached() && handleBlock(stripeNum, block) )
                    stripes.setBit(stripeNum);
                break;
            }
        }
    }
}

bool Playground::handleBrokenBlock(int stripeNum, Playblock *block)
{
    if (block->type() == Playblock::BROKEN_STONE) {
        QList<Playblock*> *plist = dBlocks[stripeNum];
        int index = plist->indexOf(block);
        if (index + 1 < plist->count())
            index++;
        Playblock *prev = plist->at(index);
        if (prev && (prev->type() < Playblock::STONE || prev->type() > Playblock::TRANSITIONAL)
            && prev->y() + Puzzle::blockPixelSize == block->y()) {
            if (block->counter()) {
                block->setCounter(block->counter() - 1);
            } else {
                block->setFrame(block->frame() + 1);
                if (block->frame() == 5) {
                    if (selectedBlock == block)
                        selectedBlock = 0;
                    grid[stripeNum+(block->by()*w)] = Playblock::EMPTY;
                    Playblock *removing = plist->takeAt(plist->indexOf(block));
                    delete removing;
                    return true;
                } else
                    block->setCounter(Puzzle::brokenDelay);
                return true;
            }
        } else {
            block->setCounter(0);
        }
    }
    return false;
}

bool Playground::handleBlock(int stripeNum, Playblock *block)
{
    bool justSet = false;
    block->setUpdated(true);
    if (!block->accelY()) {
        if (isEmpty(block->bx(), block->by() + 1)) {
            block->setAccelY(Puzzle::fallAccel);
            justSet = true;
        }
    }

    if (block->accelY()) {
        block->updatePos();
        if (isEmptyByPixel(block->x(), block->y() + Puzzle::blockPixelSize - 1)) {
            QList<Playblock*> *plist = dBlocks[stripeNum];
            Playblock *next = 0;
            int loc = plist->indexOf(block);
            if (loc > 0) {
                --loc;
                if (loc >= 0)
                    next = plist->at(loc);
            }

            if (next && block->y() + Puzzle::blockPixelSize > next->y()) {
                block->setY(next->y() - Puzzle::blockPixelSize);
                if (next->attached()) {
                    block->setAttached(true);
                    block->setAccelY(next->accelY());
                } else {
                    block->setAccelY(0);
                    if ( !justSet )
                        needsClearCheck = true;
                }
                if ( !justSet ) {
                    //if (next->type() >= Playblock::STONE && next->type() < Playblock::WHITE)
                    //    Puzzle::sounds->playSound("wall_hit");
                    //else
                    //    Puzzle::sounds->playSound("block_hit");
                }
            }
            block->recalculatePos();
        } else {
            block->setAccelY(0);
            QPoint p = blockCoordinatesAt(block->x(), block->y() + Puzzle::blockPixelSize - 1);
            block->recalculatePos(block->bx(), p.y() - 1);
            needsClearCheck = true;
            //Puzzle::sounds->playSound("wall_hit");
        }
        if (justSet && !block->accelY()) {
            return false;
        } else {
            return true;
        }
    }

    return false;
}

void Playground::handleTrapBlock(int stripeNum, Playblock *block)
{
    QList<Playblock*> *plist;
    plist = dBlocks.at(stripeNum);
    Playblock *cb;

    if ( isEmpty(block->bx(), block->by()) ) {
        for (int i = 0; i < plist->count(); ++i) {
            cb = plist->at(i);
            if ( cb->by() == block->by() ) {
                grid[block->by()*w+block->bx()] = Playblock::TRANSITIONAL;
                block->setCounter(3);
                break;
            }
        }
    } else {
        if ( !block->frame() ) {
            grid[block->by()*w+block->bx()] = Playblock::STONE;
            Playblock *removing = sBlocks.takeAt(sBlocks.indexOf(block));
            delete removing;
            stripes.setBit(stripeNum);
            return;
        }
        if ( block->counter() ) {
            block->setCounter(block->counter() - 1);
        } else {
            block->setFrame(block->frame() - 1);
            block->setCounter(0);
            stripes.setBit(stripeNum);
        }
    }
}

bool Playground::handleFireBlock(int stripeNum, Playblock *block)
{
    bool cleared = false;
    QList<Playblock*> *plist;
    plist = dBlocks.at(stripeNum);
    Playblock *cb;
    if ( isEmpty(block->bx(), block->by()) ) {
        for (int i = 0; i < plist->count(); ++i) {
            cb = plist->at(i);
            if ( cb->by() == block->by() ) {
                cb->setCounter(Puzzle::deletionDelay);
                grid[block->by()*w+block->bx()] = Playblock::TRANSITIONAL;
                cb->setGrabbed(false);
                block->setAccelY(6);
                cleared = true;
                Puzzle::sounds->playSound("delete");
                break;
            }
        }
    } else {
        if ( block->accelY() == 1 ) {
            grid[block->by()*w+block->bx()] = Playblock::STONE;
            Playblock *removing = sBlocks.takeAt(sBlocks.indexOf(block));
            delete removing;
            stripes.setBit(stripeNum);
            return false;
        }
    }
    if ( block->accelY() && !deletionCounter ) {
        block->setAccelY(block->accelY() - 1);
        block->setCounter(0);
    }
    if ( block->counter() ) {
        block->setCounter(block->counter() - 1);
    } else {
        block->setFrame(block->frame() + 1);
        if ( block->frame() == 10 ) {
            block->setFrame(0);
        }
        block->setCounter(2);
        stripes.setBit(stripeNum);
    }
    return cleared;
}

void Playground::handleMovingBlockDown( int stripeNum, Playblock *block)
{
    QList<Playblock*> *plist = dBlocks[stripeNum];
    Playblock *last;
    block->setLastDirection(0);
    if ( block->by() <= block->y2() ) {
        if ( !block->accelY() ) {
            applyAcceleration(stripeNum, block, Puzzle::movingAccel);
        }
        for (int i = plist->indexOf(block); i < plist->count(); ++i) {
            last = plist->at(i);
            if ( !last->attached() || (last->type() == Playblock::MOVING_STONE && last != block) ) {
                break;
            }

            last->setUpdated(true);
            int oldY = block->y();
            last->updatePos();
            if ( isEmptyByPixel(last->x(), last->y() + Puzzle::blockPixelSize - 1) ) {
                int index = plist->indexOf(last);
                Playblock *next = 0;
                if (index > 0)
                    next = plist->at(index - 1);
                if ( next && last->y() + Puzzle::blockPixelSize > next->y() ) {
                    last->setY(next->y() - Puzzle::blockPixelSize);
                    if ( oldY == last->y() )
                        last->setAccelY(0);
                }
                last->recalculatePos();
            } else {
                block->recalculatePos();
                applyAcceleration(stripeNum, block, 0, true);
            }
        }
        if ( block->type() == Playblock::MOVING_STONE && isMovingLimit(block) ) {
            applyAcceleration(stripeNum, block, 0);
        }
    }
    if ( !block->accelY() ) {
        block->setCounter(Puzzle::movingDelay);
    }

    needsClearCheck = true;
    stripes.setBit(stripeNum);
}

void Playground::handleMovingBlockUp(int stripeNum, Playblock *block)
{
    QList<Playblock*> *plist = dBlocks[stripeNum];
    Playblock *last;
    block->setLastDirection(1);
     
    if (block->by() >= block->y1()) {
         last = applyAcceleration(stripeNum, block, Puzzle::movingAccel * -1);
         int i = plist->indexOf(last);
         while ( true )
         {
             int oldY = last->y();
             last->updatePos();
             last->setUpdated(true);
             if (isEmptyByPixel(last->x(), last->y()) )
             {
                 int index = plist->indexOf(last);
                 Playblock *next = 0;
                 if (index + 1 < plist->count())
                        next = plist->at(index + 1);
                 if ( next )
                 {
                     if (next->y() + Puzzle::blockPixelSize > last->y())
                     {
                         last->setY(next->y()+Puzzle::blockPixelSize);           
                         if ( oldY==last->y() && ((next->attached() && next->accelY()==0) || next->type()==Playblock::BROKEN_STONE) )
                             last->setAccelY(0);
                     }
                 }
                 last->recalculatePos();
             } else {
                 last->recalculatePos(last->bx(),last->by());
                 last->setAccelY(0);
             }
             
             if (last == block)
                 break;
             else
             {
                 --i;
                 last = plist->at(i);
             }
         }               
         if (block->type() == Playblock::MOVING_STONE && isMovingLimit(block))
         {
             applyAcceleration(stripeNum,block,0,true); 
         }
     }
     if ( !block->accelY() )
     {
         block->setCounter(Puzzle::movingDelay);
     }
     
     needsClearCheck=true;
     stripes.setBit(stripeNum);
}

bool Playground::isEmptyByPixel(int px, int py) const
{
    if (py < 0) 
        return false;
    int bx = px / Puzzle::blockPixelSize;
    int by = py / Puzzle::blockPixelSize;
    return isEmpty(bx, by);
}

bool Playground::isEmpty(int bx, int by) const
{
    if (by < 0 || bx < 0)
        return false;
    /*for ( int i = 0; i < h; ++i) {
        QStringList list;
        for ( int j = 0; j < w; ++j) {
            list.append(QString("%1").arg(grid.at(i * w + j), 2));
        }
        qDebug() << list.join("|");
    }*/

    Playblock::GAME_BLOCK block = grid.at(by * w + bx);
    return !(block == Playblock::TRANSITIONAL
             || block == Playblock::STONE
             || block == Playblock::BROKEN_STONE);
}

// Verify if given MOVING_STONE block is at its limit ( top or bottom)
bool Playground::isMovingLimit(Playblock *block)
{
    bool result = false;

    QRect r;
    if ( block->accelY() > 0 ) {
        r = pixelCoordinatesAt(block->bx(), block->y2());
        if ( block->y() >= r.y() ) {
            block->setY(r.y());
            result = true;
        }
    } else {
        r = pixelCoordinatesAt(block->bx(), block->y1());
        if ( block->y() <= r.y() ) {
            block->setY(r.y());
            block->recalculatePos();
            result = true;
        }
    }
    return result;
}

// Detach *block and all blocks above it with attached() atribute being true
void  Playground::detachBlocks(int stripeNum, Playblock *block)
{
    QList<Playblock*> *plist = dBlocks[stripeNum];

    for (int i = plist->indexOf(block); i < plist->count(); ++i) {
        if ( !plist->at(i)->attached() || plist->at(i)->type() == Playblock::MOVING_STONE ) {
            break;
        }
        plist->at(i)->setAttached(false);
        plist->at(i)->setAccelY(0);
    }
}


// Apply acceleration to *block and all blocks above it with attached() atribute being true
Playblock* Playground::applyAcceleration(int stripeNum, Playblock *block, float ac, bool normalize)
{
    Playblock *last = 0;
    if (block->type() == Playblock::MOVING_STONE) {
        int cy = block->by();
        QList<Playblock*> *plist = dBlocks[stripeNum];
        for (int i = plist->indexOf(block); i>= 0 && i < plist->count(); ++i) {
            if (!plist->at(i)->attached() ||
                (plist->at(i)->type() == Playblock::MOVING_STONE && plist->at(i) != block) ) {
                break;
            }
            last = plist->at(i);
            last->setAccelY(ac);
            if ( normalize ) {
                last->recalculatePos(last->bx(), cy);
                cy--;
            }
        }
    }
    return last;
}

// Clear current selection
bool Playground::clearSelected()
{
    bool result = false;
    if ( gState != INVALID && selectedBlock ) {
        selectedBlock->setSelected(false);
        selectedBlock->setGrabbed(false);
        stripes.setBit(selectedBlock->bx());
        result = true;
    }
    return result;
}

// Move block horizontally
// Basically, do some checks and if the new position is valid, remove block from
// one stripe and insert it into another stripe.
// If user is trying to squeeze block between two wall blocks , given him some slack ( if at least 50% of the block is
// facing empty space, let the user move the block by snapping it into empty place.)
// Return true if move was successful.
bool Playground::moveBlock(int srcStripe, int destStripe, Playblock *block)
{
    if ( destStripe < w && destStripe >= 0 ) {
        QList<Playblock *> *dlist = dBlocks[destStripe];
        QList<Playblock *> *slist = dBlocks[srcStripe];

        int nx = block->x() + ((destStripe - srcStripe) * Puzzle::blockPixelSize);
        int ny = block->y();

        int addOffset = 0;

        bool needToSnap = false;
        if ( !isEmptyByPixel(nx, ny) )
            needToSnap = true;

        if ( !isEmptyByPixel(nx, ny + (Puzzle::blockPixelSize / 2)) )
            return false;
        if ( needToSnap )
            addOffset = 1;

        if ( !isEmptyByPixel(nx, ny + Puzzle::blockPixelSize - (Puzzle::blockPixelSize / 2)) ) {
            return false;
        }
        if ( !isEmptyByPixel(nx, ny + Puzzle::blockPixelSize) ) {
            needToSnap = true;
        }

        for (int i = 0; i < dlist->count(); ++i) {
            if ( dlist->at(i)->isPointInside(ny) || dlist->at(i)->isPointInside(ny + Puzzle::blockPixelSize - 1) ) {
                if ( !(dlist->at(i)->type() == Playblock::BROKEN_STONE && needToSnap) )
                    return false;
            }
        }
        if ( block->attached() ) {
            detachBlocks(srcStripe, block);
        }
        slist->takeAt(slist->indexOf(block));
        qSort(slist->begin(), slist->end(), Playblock::lessThan);
        dlist->append(block);
        stripes.setBit(srcStripe);
        stripes.setBit(destStripe);
        block->setX(nx);
        if ( needToSnap )
            block->recalculatePos(destStripe, block->by() + addOffset);
        else
            block->setCurrentBlockPos(destStripe, block->by());
        qSort(dlist->begin(), dlist->end(), Playblock::lessThan);
        if ( isEmpty(block->bx(), block->by() + 1) ) {
            block->setAccelY(Puzzle::fallAccel);
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
    vBlocks.fill(empty);
    bool cleared = false;
    int blocksCounted = 0;

    for (int i = 0; i < dBlocks.count(); ++i) {
        QList<Playblock*> *plist = dBlocks[i];
        for (int j = 0; j < plist->count(); ++j) {
            Playblock *block = plist->at(j);
            if ( (block->type() < Playblock::STONE || block->type() > Playblock::TRANSITIONAL)
                    && (block->attached() || block->accelY() == 0) && isBlockStatic(block) ) {
                vBlocks[block->bx() + block->by() * w] = block;
                blocksCounted++;
            }
        }
    }

    for (int i = 0; i < h; ++i) {
        for (int j = 0;j < w; ++j) {
            Playblock *block = 0;
            block  = getVBlock(j, i);
            if ( block && block->type() != Playblock::EMPTY ) {
                Playblock *next = getVBlock(j + 1, i);
                if ( next && block->type() == next->type() ) {
                    block->setCounter(Puzzle::deletionDelay);
                    next->setCounter(Puzzle::deletionDelay);
                    block->setGrabbed(false);
                    next->setGrabbed(false);
                    cleared = true;
                }
                next = getVBlock(j, i + 1);
                if ( next && block->type() == next->type() ) {
                    block->setCounter(Puzzle::deletionDelay);
                    next->setCounter(Puzzle::deletionDelay);
                    block->setGrabbed(false);
                    next->setGrabbed(false);
                    cleared = true;
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
    gState = s;
    if (s == INPROGRESS ) {
        tick = 0;
        gState = ANIMATE;
        needsClearCheck = true;

        QList<Playblock*> *plist;
        float sx, sy;
        float ex, ey;
        initTicks = Puzzle::initNumSteps;

        int aStart = rand() / (RAND_MAX / 12 + 1); // middle should happen three times as often as any other position

        for (int i = 0;i < dBlocks.count();i++ ) {
            plist = dBlocks.at(i);
            for (int j = 0; j < plist->count(); ++j) {
                Playblock *block = (*plist)[j];
                switch ( aStart ) {
                case    0:  // bottom right corner
                    sx = (w - 1) * Puzzle::blockPixelSize;
                    sy = (h - 1) * Puzzle::blockPixelSize;
                    break;
                case    1:  // bottom left corner
                    sx = 0;
                    sy = (h - 1) * Puzzle::blockPixelSize;
                    break;
                case    2:  // top left corner
                    sx = 0;
                    sy = 0;
                    break;
                case    3:  // top right corner
                    sx = (w - 1) * Puzzle::blockPixelSize;;
                    sy = 0;
                    break;
                default:    // middle
                    sx = (w * Puzzle::blockPixelSize - (Puzzle::blockPixelSize / 2)) / 2;
                    sy = (h * Puzzle::blockPixelSize - (Puzzle::blockPixelSize / 2)) / 2;
                    break;
                }

                ex = (float)block->x();
                ey = (float)block->y();
                block->setX((int)sx);
                block->setY((int)sy);
                block->setAccelY((int)((ey - sy) / (float)Puzzle::initNumSteps));
                block->setAccelX((int)((ex - sx) / (float)Puzzle::initNumSteps));
            }
        }
    }
}

bool Playground::savePlayground(const QString &fileName, int currPoints, const QString &levelFileName) const
{
    QFile file(fileName);
    bool result = false;
    if ( file.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
        QTextStream ts(&file);
        if ( gType == MOVES_BASED ) {
            ts << ".1\n";
        }
        ts << levelNumber << "-" << w << "-" << h << "-" << timeLeft << "\n";      // level levelNumberber, width, height, time left
        ts << levelFileName << "\n";                                                    // empty line or level file name
        ts << currPoints << "\n";
        ts << levelPoints << "\n";
        ts << "\n";
        int x, y;
        for (y = 0;y < h;y++ ) {
            for ( x = 0;x < w;x++ ) {
                switch ( grid.at(y*w + x) ) {
                case    Playblock::TRANSITIONAL:
                    ts << "t";
                    break;
                case    Playblock::EMPTY:
                    ts << ".";
                    break;
                case    Playblock::STONE:
                    ts << "w";
                    break;
                case    Playblock::BROKEN_STONE:
                    ts << "b";
                    break;
                default:
                    break;
                }
            }
            ts << "\n";
        }
        ts << "\n";

        QList<Playblock*> *plist;
        for (int x = 0;x < dBlocks.count();x++ ) {
            plist = dBlocks[x];
            ts << x << "-" << plist->count() << "\n";
            for (int i = 0; i < plist->count(); ++i) {
                ts << plist->at(i)->toString();
                ts << "\n";
            }
        }
        ts << "\n";

        ts << sBlocks.count() << "\n";
        for (int i = 0; i < sBlocks.count(); ++i) {
            ts << sBlocks.at(i)->toString();
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
// 1) Saved game file is corrupted
// 2) Saved game file is for level text file that is NOT present on this system

bool Playground::loadPlayground(const QString &fileName, int *currPoints, const char * level[])
{
    gState = INVALID;
    QFile f(fileName);

    if ( f.exists() && f.open(QIODevice::ReadOnly) ) {
        QString line;
        QTextStream stream( &f );

        // load basic level information
        line = stream.readLine();
        gType = TIME_BASED;
        if ( line.at(0) == '.' ) {
            if ( line.at(1) == '1' )
                gType = MOVES_BASED;
            line = stream.readLine();
        }

        if ( sscanf( line.toLatin1(), "%d-%d-%d-%d", &levelNumber, &w, &h, &timeLeft) != 4 )
            goto error;


        int savedLimit = timeLeft;

        // load level file name
        line = stream.readLine();
        if ( !line.isEmpty() ) {
#ifndef DEMO_VERSION
            if ( QFile::exists(line) )                             // see if this is full file path or just level set name
            {
                if ( !loadLevel(line, levelNumber) )
                    goto error;
            } else {
                Puzzle::currLevelsName = line;
                if (!loadLevel
                        (ROOTHOME + "pics/puzz-le/levels/" +
                         Puzzle::currLevelsName + "/levels", levelNumber))
                    goto error;
            }
#else
            goto error;
#endif
        } else {
            Puzzle::currLevelsName = "";
            loadLevel(level, levelNumber);
        }

        timeLeft = savedLimit;


        // load points information
        line = stream.readLine();
        initGrid(w, h);
        if ( currPoints ) {
            if ( sscanf( line.toLatin1(), "%d", currPoints) != 1 )
                goto error;
        }
        line = stream.readLine();
        if ( sscanf( line.toLatin1(), "%d", &levelPoints) != 1 )
            goto error;

        line = stream.readLine();

        // load grid entries
        int x, y;

        for ( y = 0;y < h;y++ ) {

            line = stream.readLine();
            if ( line.isEmpty() )
                goto error;
            for ( x = 0;x < w;x++ ) {
                switch ( line.at(x).toLatin1() ) {
                case    '.':
                    grid[y * w + x] = Playblock::EMPTY;
                    break;
                case    'w':
                    grid[y * w + x] = Playblock::STONE;
                    break;
                case    't':
                    grid[y * w + x] = Playblock::EMPTY;
                    break;
                case    'b':
                    grid[y * w + x] = Playblock::BROKEN_STONE;
                    break;
                default:
                    goto error;
                }
            }
        }

        line = stream.readLine();

        // get standard blocks
        int slevelNumber, blevelNumber;
        QList<Playblock*> *plist;
        Playblock *block;
        for ( x = 0;x < w;x++ ) {
            line = stream.readLine();
            if ( sscanf( line.toLatin1(), "%d-%d", &slevelNumber, &blevelNumber) != 2 )
                goto error;

            plist = dBlocks[x];
            for ( y = 0;y < blevelNumber;y++ ) {
                line = stream.readLine();
                block = new Playblock(0, 0);
                plist->append(block);
                if ( !block->fromString(line) )
                    goto error;
                if ( block->selected() )
                    selectedBlock = block;
            }
        }

        line = stream.readLine();

        // load special blocks
        line = stream.readLine();
        if ( sscanf( line.toLatin1(), "%d", &blevelNumber) != 1 )
            goto error;

        for ( y = 0;y < blevelNumber;y++ ) {
            line = stream.readLine();
            block = new Playblock(0, 0);
            sBlocks.append(block);
            if ( !block->fromString(line) )
                goto error;
        }

        line = stream.readLine();

        // load Playblock class variables
        line = stream.readLine();
        if ( sscanf( line.toLatin1(), "%d-%d-%d", &deletionCounter, &beat, &totalNumBlocks) != 3 )
            goto error;

        gState = INPROGRESS;
    }
error:

    if ( f.isOpen() )
        f.close();

    return (bool) gState;
}


Playblock* Playground::getVBlock(int bx, int by)
{
    if ( bx < w && by < h )
        return vBlocks.at(bx + by*w);
    else
        return 0;
}


// Scan saved game for levelNumber, and points
bool Playground::savedPlaygroundInfo(const QString &fileName, unsigned int *points, unsigned int *level)
{
    QFile f(fileName);
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return false;
    
    int wd, hg, tm, pt;
    QString line;
    QTextStream stream( &f );

    // load basic level information
    line = stream.readLine();
    if ( line.at(0) == '.' ) {
        line = stream.readLine();
    }

    if ( sscanf( line.toLatin1(), "%d-%d-%d-%d", level, &wd, &hg, &tm) != 4 )
        return false;
    // skip level file name
    line = stream.readLine();

    // get points information
    line = stream.readLine();

    if ( sscanf( line.toLatin1(), "%d", &pt) != 1 )
        return false;

    *points = pt;
    line = stream.readLine();
    if ( sscanf( line.toLatin1(), "%d", &pt) != 1 )
        return false;
    (*points) += pt;
    return true;
}
