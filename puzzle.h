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

<<<<<<< .mine
/****************************************************************************
** Bunch of global variables used to modify behavior of the program.
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/
#ifndef PUZZLE_H
#define PUZZLE_H

#include <qstring.h>
#include "ImageRepository.h"
#include "SoundRepository.h"

class Puzzle
{
public:

	static int startLevel;			// starting level
	static int bSize;				// block size (pixels)
	static int brokenDelay;			// delay between frames for breaking stone animation (ticks)
	static int movingDelay;			// delay accured by moving stone at each of frames for breaking wall animation ticks)
	static float fallAccel;			// acceleration (or rather velocity) value for free falling blocks (pixels/ticks)
	static float movingAccel;		// acceleration (or rather velocity) value for attached/moving blocks (pixels/ticks)
	static int deletionDelay;		// deletionDelay/2 specifies number of times block will flash before being destroyed (ticks)
	static int blockBonus;			// number of points added to total score is increased after destruction of a block
	static int timeBonus;			// number of points per time unit added to total score after each level
	static int timeoutValue;		// defines tick in 1/1000 sec
	static int initNumSteps;		// number of steps it should take for a block to move from initial to final position during init animation
	static QString levelFileName;	// holds name of external level definition file
	static QString demoFileName;	// holds name of external demo file
	static bool progressSaving;		// if true, user will be prompted with "Game Save" dialog
	static bool recordGames;		// if true, games will be saved in both CPP and stand alone file formats
	static SoundRepository *sounds;
	static ImageRepository *images;
	static bool demoLoop;					// if true , demo level will loop around
   static bool playSound;        // play sound effects
   static bool isBigEndian;
   static QString currLevelsName;   // name of current levels
   static QString gameVersion;
   static bool timeLimit;       // false - no  time limit
   static QString theme;        // theme dir name

	static void initVariables(int & argc, char ** argv);
	static bool initImages(int w, int h);
	static void initSounds(const QString &directory);
};

#endif // PUZZLE_H


=======
/****************************************************************************
** Bunch of global variables used to modify behavior of the program.
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/
#ifndef PUZZLE_H
#define PUZZLE_H

#include <qstring.h>
#include "imagerepository.h"
#include "aoundrepository.h"

class Puzzle
{
public:

	static int startLevel;			// starting level
	static int bSize;				// block size (pixels)
	static int brokenDelay;			// delay between frames for breaking stone animation (ticks)
	static int movingDelay;			// delay accured by moving stone at each of frames for breaking wall animation ticks)
	static float fallAccel;			// acceleration (or rather velocity) value for free falling blocks (pixels/ticks)
	static float movingAccel;		// acceleration (or rather velocity) value for attached/moving blocks (pixels/ticks)
	static int deletionDelay;		// deletionDelay/2 specifies number of times block will flash before being destroyed (ticks)
	static int blockBonus;			// number of points added to total score is increased after destruction of a block
	static int timeBonus;			// number of points per time unit added to total score after each level
	static int timeoutValue;		// defines tick in 1/1000 sec
	static int initNumSteps;		// number of steps it should take for a block to move from initial to final position during init animation
	static QString levelFileName;	// holds name of external level definition file
	static QString demoFileName;	// holds name of external demo file
	static bool progressSaving;		// if true, user will be prompted with "Game Save" dialog
	static bool recordGames;		// if true, games will be saved in both CPP and stand alone file formats
	static SoundRepository *sounds;
	static ImageRepository *images;
	static bool demoLoop;					// if true , demo level will loop around
   static bool playSound;        // play sound effects
   static bool isBigEndian;
   static QString currLevelsName;   // name of current levels
   static QString gameVersion;
   static bool timeLimit;       // false - no  time limit
   static QString theme;        // theme dir name

	static void initVariables(int & argc, char ** argv);
	static bool initImages(int w, int h);
	static void initSounds(const QString &directory);
};

#endif // PUZZLE_H


>>>>>>> .r164
