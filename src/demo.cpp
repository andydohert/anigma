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

#include "demo.h"
#include <qdebug.h>
#include <qtextstream.h>
#include <qfile.h>
#include "imagerepository.h"
#include "puzzle.h"
#include "playground.h"

#include "levels.dat"

DemoMove::DemoMove(int x, int y, int t): ix(x), iy(y), it(t)
{
}

void Demo::loadDemo(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qWarning() << "can't load demo" << fileName;
        return;
    }

    demoList.clear();
    QTextStream ts(&file);
    QString line;
    
    int dx, dy, dt;
    int totalNumberOfLines = ts.readLine().toInt();
    demoLevel = ts.readLine().toInt();
    currentDemoMove = 0;
    while (!ts.atEnd()) {
        line = ts.readLine();
        sscanf(line.toLatin1(), "%d %d %d ", &dx, &dy, &dt);
        demoList.append(DemoMove(dx, dy, dt));
    }
    
    if (totalNumberOfLines != demoList.count()) {
        qWarning() << "error reading demo" << fileName;
    }
}

bool Demo::initDemo()
{
    if (Puzzle::demoFileName.isEmpty()) {
        qWarning() << "demo FileName is empty()";
        return false;
    }

    if (Puzzle::demoLoop) {
        QString nextName = Puzzle::demoFileName;
        if (play->currentLevel == -1) {
            QString tmp = (QString) Puzzle::demoFileName.at(Puzzle::demoFileName.length() - 7);
            tmp.append(Puzzle::demoFileName.at(Puzzle::demoFileName.length() - 6));
            sscanf(tmp.toLatin1(), "%d", &play->currentLevel);
        } else {
            QString tmp;
            tmp.sprintf("%02d", play->currentLevel);
            nextName.replace(nextName.length() - 7, 2, tmp);
        }
        loadDemo(nextName);
    } else {
        loadDemo(Puzzle::demoFileName);
    }

    if (demoList.count()) {
        curMove = demoList.first();
        bool result;
        Puzzle::images->shuffleTheme();
        if (Puzzle::levelFileName.isEmpty())
            result = play->loadLevel(levels, demoLevel);
        else
            result = play->loadLevel(Puzzle::levelFileName, demoLevel);
        if (result) {
            return true;
        }
    }
    return false;
}

// Save demo into plain ASCII text file.
// Demo file is simply recording of timestamped player moves ( mouse click coordinates )
void Demo::saveDemoToFile()
{
    QString fileName = QString("Level_%1.demo").arg(play->currentLevel);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "can't save demo" << fileName;
    }

    QTextStream ts(&file);
    QString header = QString("%1\n").arg(demoList.count());
    ts << header;
    header.sprintf("%d\n", play->currentLevel);
    ts << header;
    for (int i = 0; i < demoList.count(); ++i) {
        ts << demoList.at(i).toString() + "\n";
    }
    file.close();
}

bool Demo::timeout(int tick)
{
    bool update = false;
    if (curMove.getTick() == tick) {
        QPoint p = curMove.getPos();
        if (play->setSelected(p.x(), p.y())) {
            update = true;
        }
        if (demoList.count() > currentDemoMove + 1)
            curMove = demoList.at(++currentDemoMove);
    }
    return update;
}
