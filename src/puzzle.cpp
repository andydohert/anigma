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

#include <stdio.h>
#include "puzzle.h"
#include <qsettings.h>
#include <qdebug.h>
#include <qapplication.h>

#include "imagerepository.h"
#include "soundrepository.h"

int Puzzle::startLevel = 1;     // cmd -z
int Puzzle::blockPixelSize = 20;
int Puzzle::brokenDelay = 8;    // cmd -h
int Puzzle::movingDelay = 16;   // cmd -l
float Puzzle::fallAccel = 3.0;  // cmd -f
float Puzzle::movingAccel = 2.0;// cmd -m
int Puzzle::deletionDelay = 16; // cmd -d
int Puzzle::blockBonus = 32;    // cmd -b
int Puzzle::timeBonus = 4;      // cmd -t
int Puzzle::timeoutValue = 40;  // cmd -v
int Puzzle::initNumSteps = 5;   // cmd -s
QString Puzzle::levelFileName = "";     // cmd -n
QString Puzzle::demoFileName = "";      // cmd -x
bool Puzzle::progressSaving = true;     // cmd -p
bool Puzzle::recordGames = false;       // cmd -r
ImageRepository *Puzzle::images = 0;
SoundRepository *Puzzle::sounds = 0;
bool Puzzle::demoLoop = false;  // cmd -w
bool Puzzle::playSound = true;  // cmd -a
bool Puzzle::isBigEndian = false;
bool Puzzle::timeLimit = false;
QString Puzzle::theme = "default";

QString Puzzle::currLevelsName = "";
#ifdef DEMO_VERSION
QString Puzzle::gameVersion = "2.1.0d";
#else
QString Puzzle::gameVersion = "2.1.0";
#endif

#include <qicon.h>

bool Puzzle::initImages()
{
    Puzzle::images = new ImageRepository();
    qApp->setWindowIcon(QIcon(images->findPixmap("puzz-le")));
    bool result = Puzzle::images->initTheme(Puzzle::theme);
    if (!result) {
        Puzzle::theme = "Default";
        result = Puzzle::images->initTheme(Puzzle::theme);
    }
    return result;
}

void Puzzle::initSounds(const QString & directory)
{
    Puzzle::sounds = new SoundRepository(directory);
}

void Puzzle::initVariables(int &argc, char **argv)
{
    QString cmd;
    QString val;
    int intVal;
    float floatVal;

    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        isBigEndian = true;

    Puzzle::demoFileName = ROOTHOME + "/pics/ldemos/Level_01.demo";

    Puzzle::demoLoop = true;
    QSettings cfg("Puzz_le");
    cfg.beginGroup("Puzz_le");
    Puzzle::timeoutValue = cfg.value("timeoutValue", 40).toInt();
    Puzzle::progressSaving = cfg.value("progressSaving", true).toBool();
    Puzzle::playSound = cfg.value("playSounds", true).toBool();
    Puzzle::timeLimit = cfg.value("timeLimit", true).toBool();
    Puzzle::currLevelsName = cfg.value("currentLevelName", "").toString();
    Puzzle::theme = cfg.value("theme", "Default").toString();

    int i = 1;  // skip executable name
    while (i < argc) {
        cmd = argv[i];
        if (!cmd.isEmpty()) {
            i++;
            switch (cmd.at(1).toLatin1()) {
            case 'z':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::startLevel = intVal;
                break;
            case 'h':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::brokenDelay = intVal;
                break;
            case 'l':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::movingDelay = intVal;
                break;
            case 'f':
                if (sscanf(argv[i], "%f", &floatVal) == 1)
                    Puzzle::fallAccel = floatVal;
                break;
            case 'm':
                if (sscanf(argv[i], "%f", &floatVal) == 1)
                    Puzzle::movingAccel = floatVal;
                break;
            case 'd':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::deletionDelay = intVal;
                break;
            case 'b':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::blockBonus = intVal;
                break;
            case 't':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::timeBonus = intVal;
                break;
            case 'v':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::timeoutValue = intVal;
                break;
            case 's':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::initNumSteps = intVal;
                break;
            case 'p':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::progressSaving = (bool) intVal;
                break;
            case 'r':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::recordGames = (bool) intVal;
                break;
            case 'w':
                if (sscanf(argv[i], "%d", &intVal) == 1)
                    Puzzle::demoLoop = (bool) intVal;
                break;
            case 'a':
                if (sscanf(argv[i], "%d", &intVal) == 1) {
                    Puzzle::playSound = (bool) intVal;
                }
                break;
            case 'n':
                Puzzle::levelFileName = argv[i];
                break;
            case 'x':
                Puzzle::demoFileName = argv[i];
                break;
            }
            i++;
        }
    }
}
