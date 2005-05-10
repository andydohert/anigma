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

#include <stdio.h>
#include <stdlib.h>
#include "puzzle.h"

#ifdef KDE_RELEASE
#include <kconfig.h>
#define Config KConfig
#else
#include <qpe/config.h>
#endif


int Puzzle::startLevel=1;           // cmd -z
int Puzzle::bSize=20;               
int Puzzle::brokenDelay=8;          // cmd -h
int Puzzle::movingDelay=16;         // cmd -l
float Puzzle::fallAccel=3.0;        // cmd -f
float Puzzle::movingAccel=2.0;      // cmd -m
int Puzzle::deletionDelay=16;       // cmd -d
int Puzzle::blockBonus=32;          // cmd -b
int Puzzle::timeBonus=4;            // cmd -t
int Puzzle::timeoutValue=40;        // cmd -v
int Puzzle::initNumSteps=5;         // cmd -s
QString Puzzle::levelFileName="";  // cmd -n
QString Puzzle::demoFileName="";   // cmd -x
bool Puzzle::progressSaving=true;           // cmd -p
bool Puzzle::recordGames=false;             // cmd -r
ImageRepository *Puzzle::images=0;
SoundRepository *Puzzle::sounds=0;
bool Puzzle::demoLoop=false;				// cmd -w
bool Puzzle::playSound=true;				// cmd -a
bool Puzzle::isBigEndian=false;
bool Puzzle::timeLimit=false; 
QString Puzzle::theme="default";

QString Puzzle::currLevelsName="";
#ifdef DEMO_VERSION
    QString Puzzle::gameVersion="2.0.0d";
#else
    QString Puzzle::gameVersion="2.0.0";
#endif


bool Puzzle::initImages(int w, int h)
{
    Puzzle::images=new ImageRepository();
    Puzzle::images->init(w,h);

    bool result=Puzzle::images->initTheme(Puzzle::theme);
    if(!result)
    {
        Puzzle::theme="Default";
        result=Puzzle::images->initTheme(Puzzle::theme);
    }
    return result;
}

void Puzzle::initSounds(const QString &directory)
{
    Puzzle::sounds= new SoundRepository(directory);
    Puzzle::sounds->init();

}

void Puzzle::initVariables(int & argc, char ** argv)
{
    int i=1; // skip executable name

    QString cmd;
    QString val;
    int intVal;
    float floatVal;

    int wsize;
    bool bendian;

    qSysInfo(&wsize,&bendian);
    if(bendian)
       isBigEndian=true;

    Puzzle::demoFileName=QString(getenv("QPEDIR")) + "/pics/puzz-le/ldemos/Level_01.demo";
    Puzzle::demoLoop=true;

    Config cfg("Puzz_le");
    cfg.setGroup("Puzz_le");
    Puzzle::timeoutValue=cfg.readNumEntry("timeoutValue",40);
    Puzzle::progressSaving=cfg.readBoolEntry("progressSaving",true);
    Puzzle::playSound=cfg.readBoolEntry("playSounds",true);
    Puzzle::timeLimit=cfg.readBoolEntry("timeLimit",true);    
    Puzzle::currLevelsName=cfg.readEntry("currentLevelName","");
    Puzzle::theme=cfg.readEntry("theme","Default");



    while ( i<argc )
    {
        cmd=argv[i];
        if ( !cmd.isEmpty() )
        {
            i++;
            switch ( cmd.at(1).latin1() )
            {
                case    'z':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::startLevel=intVal;
                    break;
                case    'h':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::brokenDelay=intVal;
                    break;
                case    'l':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::movingDelay=intVal;
                    break;
                case    'f':
                    if ( sscanf( argv[i], "%f",&floatVal)==1 )
                        Puzzle::fallAccel=floatVal;
                    break;
                case    'm':
                    if ( sscanf( argv[i], "%f",&floatVal)==1 )
                        Puzzle::movingAccel=floatVal;
                    break;
                case    'd':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::deletionDelay=intVal;
                    break;  
                case    'b':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::blockBonus=intVal;
                    break;
                case    't':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::timeBonus=intVal;
                    break;  
                case    'v':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::timeoutValue=intVal;
                    break;                      
                case    's':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::initNumSteps=intVal;
                    break;
                case    'p':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::progressSaving=(bool)intVal;
                    break;
                case    'r':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::recordGames=(bool)intVal;
                    break;
                case    'w':
                    if ( sscanf( argv[i], "%d",&intVal)==1 )
                        Puzzle::demoLoop=(bool)intVal;
                    break;
               case    'a':
                  if ( sscanf( argv[i], "%d",&intVal)==1 )
                  {
                       Puzzle::playSound=(bool)intVal;
                  }
                   break;
                case    'n':
                    Puzzle::levelFileName=argv[i];  
                    break;
                case    'x':
                    Puzzle::demoFileName=argv[i];   
                    break;                  
            }
            i++;
        }
    }
}



