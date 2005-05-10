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

TEMPLATE	= app
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= PuzzleWindow.h ImageRepository.h MenuButton.h Playground.h GameDialog.h FileDialog.h AboutDialog.h SoundRepository.h Puzzle.h\
		  OptionsDialog.h block.h
SOURCES		= main.cpp PuzzleWindow.cpp ImageRepository.cpp MenuButton.cpp Playground.cpp GameDialog.cpp FileDialog.cpp AboutDialog.cpp \
		  SoundRepository.cpp Puzzle.cpp OptionsDialog.cpp block.cpp
#-Wunused-function
#INCLUDEPATH	+= $(QPEDIR)/include
#DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lkdecore 
TARGET		= puzz-le
DEFINES = KDE_RELEASE
INCLUDEPATH	+= $(KDEDIR)/include
DEPENDPATH	+= $(KDEDIR)/include
#DEFINES		= DEMO_VERSION
