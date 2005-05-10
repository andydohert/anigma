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

#include "imagerepository.h"
#include "graphics/wall_small.xpm"
#include "graphics/trap_left_small.xpm"
#include "graphics/trap_right_small.xpm"
#include "graphics/fire_animation.xpm"
#include "graphics/broken_wall_animation.xpm"
#include "graphics/freezing_animation.xpm"
#include "graphics/images.h"
#include <qdir.h>
#include "Puzzle.h"
#include <qarray.h>
#include <qbitmap.h>
#include <stdlib.h>
#include <qpicture.h>

static const char *imageNames[]= {
    "red",
    "green",
    "blue",
    "purple",
    "yellow",
    "cyan",
    "white",
    "grey",
    "wall",
    "fire",
    "trap_right",
    "trap_left",
    "broken_wall",
    "front_1",
    "background",
    "front_title",
    "freezing_animation"
};

static const char *pieceNames[]= {
    "red",
    "green",
    "blue",
    "cyan",
    "white",
    "grey",
    "purple",
    "yellow"
};


ImageRepository::ImageRepository()
{
    cTheme.setAutoDelete(true);
    cHTheme.setAutoDelete(true);
    pixmaps.resize(31);
}

ImageRepository::~ImageRepository()
{
    
}

QPixmap* ImageRepository::findPixmap(const QString& name)
{
    return pixmaps.find(name);
}

QPixmap* ImageRepository::findPixmap(I_NAMES id)
{
    return pixmaps.find(imageNames[id]);
}

bool ImageRepository::initTheme( const QString &theme)
{
    bool result=false;

    if ( !theme.isEmpty() )
    {
        cTheme.clear();
        cHTheme.clear();
				svgTheme.clear();
        QDir d(ROOTHOME+"pics/puzz-le/themes/"+theme);
        if ( d.exists() )
        {
            d.setFilter( QDir::Files);
            d.setSorting( QDir::Size | QDir::Reversed );
            d.setNameFilter("*.png_;*.svg");

            const QFileInfoList *list = d.entryInfoList();
            QFileInfoListIterator it( *list );      // create list iterator
            QFileInfo *fi;                          // pointer for traversing

            int count=0;
            QPixmap *pix;
            QPixmap *hPix;
            QImage img;

            while ( (fi=it.current()) )
            {

                
								QImage img(fi->filePath());
                if ( !img.isNull() )
                {
                    if ( img.width()!=Puzzle::bSize || img.height()!=Puzzle::bSize )
                    {
                        img=img.smoothScale(Puzzle::bSize,Puzzle::bSize);
                    }

                    if ( img.depth()<32 )
                        img=img.convertDepth(32);
                    pix = new QPixmap();
                    pix->convertFromImage(img); 
                    if ( !pix->mask() )
                    {
                        pix->setMask(pix->createHeuristicMask());
                    }

                    cTheme.append(pix);
                    createHighlighted(img);
                    hPix=new QPixmap();                
                    hPix->convertFromImage(img);            
                    hPix->setMask(*pix->mask());                
                    cHTheme.append(hPix);
                    count++;
                }
								else{
									qDebug("Unable to load image %s", fi->filePath().latin1());
									QPicture *p = new QPicture();
									if(!p->load(fi->filePath(), "svg")){
										qDebug("Can't load svg");
										delete p;
									}
									else {
										svgTheme.append(p);
										count++;
									}
								}
                ++it;                               // goto next list element
            }
            if ( count>=8 )
                result=true;
        }
				else{
          qDebug("Unable to load theme: %s", QString(ROOTHOME+"pics/puzz-le/themes/"+theme).latin1());
				}

    }
    if(result)
        shuffleTheme();        
    return result;
}

void ImageRepository::shuffleTheme()
{
    QArray<int> ns;

    ns.resize(8);
    ns.fill(-1);

    int n;
    int cnt=0;

    while ( cnt<8 )
    {
        n=(rand() / (RAND_MAX / cTheme.count()));
        if ( ns.find(n)==-1 )
        {
            ns[cnt]=n;
            QString name=pieceNames[cnt];
            QString hName=name+"h";
            pixmaps.remove(name);
            pixmaps.remove(hName);            
            pixmaps.insert(name,cTheme.at(n));            
            pixmaps.insert(hName,cHTheme.at(n));
            cnt++;
        }
    }


}


void ImageRepository::init(int sw, int sh)
{
    struct EmbededPixmaps
    {
        char** data;
        const char* name;  
        I_TYPES type;
    } embed_data[] =
    {
        //{red_small_xpm,"red",HIGHLIGHT},
        //{blue_small_xpm,"blue",HIGHLIGHT},
        //{green_small_xpm,"green",HIGHLIGHT},
        //{purple_small_xpm,"purple",HIGHLIGHT},
        //{yellow_small_xpm,"yellow",HIGHLIGHT},
        {wall_small_xpm,"wall",NORMAL},
        {fire_animation_xpm,"fire",NORMAL},
        {trap_right_small_xpm,"trap_right",NORMAL},
        {trap_left_small_xpm,"trap_left",NORMAL},
        {broken_wall_animation_xpm,"broken_wall",NORMAL},
        {freezing_animation_xpm,"freezing_animation",NORMAL},
        {0,0,NORMAL}
    };


    int i;
    QPixmap *pix;
    QString nName;



    for ( i=0;embed_data[i].data;i++ )
    {
        QImage img((const char **)embed_data[i].data);
        if ( img.depth()<32 )
            img=img.convertDepth(32); // we MUST have 32 bit image for createHighlight to work.
        pix= new QPixmap();
        switch ( embed_data[i].type )
        {
            case  RESIZE:            // resizing disabled because it ends up screwed up when onscreen keyboard is on
                pix->convertFromImage(img.smoothScale(sw,sh));
                pixmaps.insert(embed_data[i].name,pix);
                break;
            case  NORMAL:
                pix->convertFromImage(img);
                pixmaps.insert(embed_data[i].name,pix);
                break;
        }         
    }

    QByteArray bytes=qembed_findData("background.png");
    QImage img(bytes);
    if ( img.depth()<32 )
        img=img.convertDepth(32);
    pix = new QPixmap();
    //pix->convertFromImage(img.smoothScale(sw,sh));
    pix->convertFromImage(img);
    pixmaps.insert("background",pix);
    bytes=qembed_findData("front_title.png");
    pix = new QPixmap(bytes);
    pixmaps.insert("front_title",pix);
    bytes=qembed_findData("front_1.png");
    pix = new QPixmap(bytes);    
    pixmaps.insert("front_1",pix);

}

void ImageRepository::createHighlighted(QImage &img)
{
    int x,y;

    for ( y=0;y<img.height();y++ )
    {
        for ( x=0;x<img.width();x++ )
        {
            // Create 60% lighter image
            // Seems like QColor does not retain alpha info which means we have to restore it later ...
            QRgb s=img.pixel(x,y);
            QColor clr(s);
            QRgb d=clr.light(160).rgb();
            img.setPixel(x,y,qRgba(qRed(d),qGreen(d),qBlue(d),qAlpha(s)));         
        }
    }

}




