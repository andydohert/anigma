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

#ifndef DEMO_H
#define DEMO_H

#include <qobject.h>
#include <qpoint.h>

class DemoMove
{
public:
    DemoMove(int x = 0, int y = 0, int t = 0);

    inline QString toString() const
    {
        return QString("%1 %2 %3").arg(ix).arg(iy).arg(it);
    }

    inline QPoint getPos() const
    {
        return QPoint(ix, iy);
    }

    inline int getTick() const
    {
        return it;
    }

private:
    int ix;
    int iy;
    int it;
};

class Playground;

class Demo : public QObject
{
Q_OBJECT

public:
    bool initDemo();
    bool timeout(int tick);
    void recordDemo(const DemoMove &move) { demoList.append(move); }
    void clearRecording() { demoList.clear(); }
    void saveDemo() {
        saveDemoToFile();
        demoList.clear();
    }

    Playground *play;
private:
    void loadDemo(const QString &fileName);
    void saveDemoToFile();
    
    int currentDemoMove;
    int demoLevel;
    DemoMove curMove;
    QList <DemoMove> demoList;
};

#endif
