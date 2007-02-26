/*
* Copyright (C) 2005-2007 Benjamin C Meyer
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

#include "soundrepository.h"

#include <qsound.h>
#include <qfile.h>
#include <qdir.h>
#include <qdebug.h>
#include "puzzle.h"

SoundRepository::SoundRepository(const QString &directory)
{
    findSound(directory, "delete");
    findSound(directory, "button");
}

SoundRepository::~SoundRepository()
{
    QHashIterator<QString, QSound*> i(sounds);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
}

void SoundRepository::playSound(const QString &name) const
{
    if (Puzzle::playSound) {
        if (sounds.contains(name))
            sounds[name]->play();
        else
            qWarning() << "sound not loaded" << name;
    }
}

void SoundRepository::findSound(const QString &directory, const QString &name)
{
    if (sounds.contains(name))
        return;

    QString fileName(directory + QDir::separator() + name + ".dav");
    if (QFile::exists(fileName)) {
        QSound *newSound = new QSound(fileName);
        sounds.insert(name, newSound);
    } else {
        qWarning() << "missing sound file:" << fileName;
    }
}

