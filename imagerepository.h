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

/****************************************************************************
** Class for initialization, storing and quick retrieval of various game images.
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/

#ifndef IMAGE_REPOSITORY_H
#define IMAGE_REPOSITORY_H

#include <qpixmap.h>
#include <qimage.h>
#include <qdict.h>
#include <qdir.h>

#ifdef KDE_RELEASE
#define ROOTHOME (QDir::currentDirPath()+"/Qtopia/puzz-le/opt/QtPalmtop/")
#else
#include <qpe/qpeapplication.h>
#define ROOTHOME QPEApplication::qpeDir()
#endif

class ImageRepository
{ 

public:
	
		enum I_NAMES
	{
		RED,
		GREEN,
        BLUE,
		PURPLE,
		YELLOW,
        CYAN,
		WHITE,
        GREY,       
		WALL,
		FIRE,
		TRAP_RIGHT,
		TRAP_LEFT,
		BROKEN_WALL,
		FRONT_1,
		BACKGROUND,
		FRONT_TITLE,
		FREEZING_ANIMATION
	};


    ImageRepository();
    ~ImageRepository();

	void init(int sw, int sh);

    bool initTheme( const QString &theme);
    void shuffleTheme();

    QPixmap* findPixmap(const QString& name);
	QPixmap* findPixmap(I_NAMES id);


private:

   enum I_TYPES 
   {
      NORMAL,
      HIGHLIGHT,
      RESIZE
   };
  
   void createHighlighted(QImage &img);

   QDict<QPixmap> pixmaps;
   QList<QPixmap> cTheme;
   QList<QPixmap> cHTheme;
	 QList<QPicture> svgTheme;
};

#endif // IMAGE_REPOSITORY_H

