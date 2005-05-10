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

#include <qsound.h>
#include <qfile.h>
#include "soundrepository.h"
#include "puzzle.h"
#include <unistd.h>



SoundRepository::SoundRepository(const QString &directory)
{
	sounds.setAutoDelete(true);	
	dir=directory;
   initialized=false;
}
SoundRepository::~SoundRepository()
{
}

QSound* SoundRepository::findSound(const QString& name)
{
	QSound *s=sounds.find(name);
	if(!s && !initialized)
	{	
		QString fileName(dir + "/" + name + ".dav");
		QFile file(fileName);
		if(file.exists())
		{
			s=new QSound(fileName);
			sounds.insert(name,s);
		}
	}
	
	return s;
}
void SoundRepository::playSound(const QString & name)
{
    if (Puzzle::playSound)
    {
        QSound *s=findSound(name);
        if(s)
        {
            s->play();
        }
    }
}
void SoundRepository::init()
{     
    findSound("delete");
    findSound("button");
    findSound("block_hit");
    findSound("move");
    findSound("wall_hit");
    initialized=true;
}


