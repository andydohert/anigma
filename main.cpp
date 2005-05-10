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

#ifdef KDE_RELEASE
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#else
#include <qpe/qpeapplication.h>
#endif
#include <qwindowsstyle.h>
#include <qmotifstyle.h>
#include <qcdestyle.h>
#include <qplatinumstyle.h>
#include "Puzzle.h"
#include "PuzzleWindow.h"
#include <stdlib.h>
#include <time.h>

int main( int argc, char ** argv )
{
    srand(time(NULL));

#ifdef KDE_RELEASE
    // About data
    KAboutData aboutData("puzz-le", I18N_NOOP("Kinkatta"), "1.0",
    I18N_NOOP("Block game"), KAboutData::License_LGPL, "(c) 2000-2003, Benjamin Meyer",
    0, 0);
    aboutData.addAuthor("Benjamin Meyer", I18N_NOOP("Original Author"), "ben@meyerhome.net");
    KCmdLineArgs::init(argc, argv, &aboutData);
		
    KApplication a(argc, argv);
    PuzzleWindow pw(0,"Main Puzzle Window");
#else
		QPEApplication a( argc, argv );
    PuzzleWindow pw(0,"Main Puzzle Window",Qt::WStyle_Customize | Qt::WStyle_NoBorder);
#endif
    Puzzle::initVariables(argc,argv);
#ifndef KDE_RELEASE
		a.showMainWidget( &pw );
#endif
		pw.resize(240,320);
    pw.show();
    return a.exec();
}

