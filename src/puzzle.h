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

#ifndef PUZZLE_H
#define PUZZLE_H

#define ROOTHOME QString(":")

#include <qstring.h>

class SoundRepository;
class ImageRepository;

/*
 * Bunch of global variables used to modify behavior of the program.
 */
class Puzzle
{

public:
    static int startLevel;
    static int blockPixelSize;
    static int brokenDelay;     // delay between frames for breaking stone animation (ticks)
    static int movingDelay;     // delay accured by moving stone at each of frames for breaking wall animation ticks)
    static float fallAccel;     // acceleration (or rather velocity) value for free falling blocks (pixels/ticks)
    static float movingAccel;   // acceleration (or rather velocity) value for attached/moving blocks (pixels/ticks)
    static int deletionDelay;   // deletionDelay/2 specifies number of times block will flash before being destroyed (ticks)
    static int blockBonus;      // number of points added to total score is increased after destruction of a block
    static int timeBonus;       // number of points per time unit added to total score after each level
    static int timeoutValue;    // defines tick in 1/1000 sec
    static int initNumSteps;    // number of steps it should take for a block to move from initial to final position during init animation
    static QString levelFileName;  // holds name of external level definition file
    static QString demoFileName;   // holds name of external demo file
    static bool progressSaving; // if true, user will be prompted with "Game Save" dialog
    static bool recordGames;    // if true, games will be saved in both CPP and stand alone file formats
    static SoundRepository *sounds;
    static ImageRepository *images;
    static bool demoLoop;       // if true , demo level will loop around
    static bool playSound;      // play sound effects
    static bool isBigEndian;
    static QString currLevelsName;      // name of current levels
    static QString gameVersion;
    static bool timeLimit;      // false - no  time limit
    static QString theme;       // theme dir name

    static void initVariables(int &argc, char **argv);
    static bool initImages();
    static void initSounds(const QString & directory);
};

#endif // PUZZLE_H
