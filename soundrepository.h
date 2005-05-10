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
** Class for initialization, storing and quick retrieval of various game sounds.
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/

#ifndef SOUND_REPOSITORY_H
#define SOUND_REPOSITORY_H

#include <qstring.h>
#include <qdict.h>


class QSound;

class SoundRepository
{ 
public:

    SoundRepository(const QString &directory);
    ~SoundRepository();

   QSound* findSound(const QString& name);
	void playSound(const QString &name);
   void init();
   

private:


   QDict<QSound> sounds;
   QString	dir;
   bool initialized;
};

#endif // SOUND_REPOSITORY_H


