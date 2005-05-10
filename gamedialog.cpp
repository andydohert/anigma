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

#include "gamedialog.h"
#include "menubutton.h"    
#include <qtimer.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <stdlib.h>
#include "puzzle.h"


GameDialog::GameDialog( QWidget *parent, const char *name):QDialog(parent,name,true,WStyle_Customize|WStyle_NoBorderEx )
{
    margin=16;
    leftButton=0;
    rightButton=0;
    middleButton=0;
    dButtonList=0;
    lEdit=0;

    QFont f=font();
    f.setBold(true);
    setFont(f);     


    setBackgroundMode(NoBackground);

    //configure(0,text,fancy,timeout,ls,hl,kl,ms,hm,km,rs,hr,kr);

    timer=new QTimer(this);
    connect( timer, SIGNAL(timeout()),this, SLOT(timeSynch())); 

}


void GameDialog::paintEvent ( QPaintEvent * ) 
{
    updateFireworks();
}

void GameDialog::resizeEvent ( QResizeEvent * ) 
{
    pix.resize(width(),height());
    tinted.resize(width(),height());
}

void GameDialog::buttonClicked(int b)
{
    done(b);
}

void GameDialog::keyPressEvent ( QKeyEvent * e )
{
    switch ( e->key() )
    {
        case    Key_Return:
            done(0);
            break;
        case    Key_Escape:
            done(2);
            break;                  
    }
}

void GameDialog::timeSynch()
{
    updateFireworks();
    if ( tCounter!=-1 )
    {
        if ( tCounter==0 )
            accept();
        else
            tCounter--;
    }
}
void GameDialog::updateFireworks()
{
    QPainter p(this);
    int i,j;
    int index;

    QPainter px(&pix);

    if ( doTinted )
    {
        px.drawPixmap(0,0,tinted);
    }
    else
    {
        pix.fill(QColor(0,0,0));
    }

    if ( doFancy )
    {
        for ( i=0;i<NUM_FW;i++ )
        {
            if ( fireWorks[i]==0 )
            {
                fireWorks[i]=1;

                int x=(rand() / (RAND_MAX / (width()-40) + 1))+20;
                int y=(rand() / (RAND_MAX / (height()-40) + 1))+20;

                for ( j=0; j<NUM_FP; j++ )
                {
                    index=i*NUM_FP+j;
                    xc[index]=x;
                    yc[index]=y;      

                    xa[index]=((((float)rand())/RAND_MAX)*6)-3;
                    ya[index]=((((float)rand())/RAND_MAX)*7)-3.5;
                }

            }

            fixed fround=0.5;
            fixed adv=0.1;

            for ( j=0;j<NUM_FP;j++ )
            {
                index=i*NUM_FP+j;           
                switch ( i )
                {
                    case 0:
                        px.setPen(QColor(255,fcount[i]*3,fcount[i]*3));
                        break;
                    case 1:
                        px.setPen(QColor(fcount[i]*3,255,fcount[i]*3));
                        break;
                    case 2:
                        px.setPen(QColor(fcount[i]*3,fcount[i]*3,255));
                        break;
                }

                px.drawLine((xc[index]+fround),(yc[index]+fround),
                            ((xc[index]-xa[index])+fround),((yc[index]-ya[index])-fround));
                xc[index]+=xa[index];
                yc[index]+=ya[index];
                ya[index]=ya[index]+adv;            
            }
            fcount[i]--;
            if ( fcount[i]<=0 )
            {
                fcount[i]=(rand() / (RAND_MAX / 54))+32;
                fireWorks[i]=0;     
            }

        }
    }
    px.setFont(font());

    int topPos;
    if ( leftButton || rightButton )
        topPos=margin*2 ;
    else
        topPos=margin*3;

    if ( lEdit )
    {
        topPos-=10;
    }

    MenuButton::drawColorizedText(dText,(width()-textWidth)/2+1,topPos+1,&px,QColor(0,0,0),150);
    MenuButton::drawColorizedText(dText,(width()-textWidth)/2,topPos,&px,QColor(255,255,255),180);

    px.setPen(QColor(0,148,255));
    px.drawRect(0,0,width(),height());
    p.drawPixmap(0,0,pix);
}

void GameDialog::show()
{
    if ( doFancy )
        timer->start(33);
    QDialog::show();
}

void GameDialog::hide ()
{
    delete leftButton;
    delete rightButton;
    delete middleButton;
    delete dButtonList;
    if ( lEdit )
        reqText=lEdit->text();
    delete lEdit;
    leftButton=0;
    rightButton=0;
    middleButton=0;
    dButtonList=0;
    lEdit=0;
    if ( doFancy )
        timer->stop();
    QDialog::hide();
}

void GameDialog::configure(QPixmap *background ,const QString &text,bool fancy,bool requester,int timeout, const QString &ls,
                           const QString &ms,const QString &rs )
{

    reqText.truncate(0);
    dText=text;
    doFancy=fancy;
    tCounter=timeout/33;    
    if ( !tCounter )
        tCounter=-1;

    QFontMetrics fm(this->font());
    QRect r=fm.boundingRect(text);
    textWidth=r.width();

   // don't bother with parent widget, just hardcode the size ( otherwise if on screen keyboard is present our dialog gets too small)

    //if(parentWidget())
    //	setFixedSize(parentWidget()->width()-10,parentWidget()->height()/3);
    //else
    setFixedSize(220,110);

    if ( parentWidget() )
    {
        QRect pr=parentWidget()->frameGeometry ();
        move(pr.x()+(pr.width()-width())/2,(pr.y()+(pr.height()-height())/2)-10);
    }

    if ( fancy )
    {
        int i;
        for ( i=0;i<NUM_FW;i++ )
        {
            fireWorks[i]=0;
            fcount[i]=(rand()%53)+32;
        }
        for ( i=0;i<NUM_FP;i++ )
        {
            xc[i]=-1;
            yc[i]=0;
            xa[i]=0;
            ya[i]=0;
        }
    }

    if ( requester )
    {
        QString fileName;

        if ( !Puzzle::levelFileName.isEmpty() )
            fileName.append(Puzzle::levelFileName);
        else
        {
            if(!Puzzle::currLevelsName.isEmpty())
                fileName.append(Puzzle::currLevelsName);
            else
                fileName.append("Default");
        }
        
        QDate cdate=QDate::currentDate();
        QTime ctime=QTime::currentTime();
        fileName.sprintf("%s-%d_%d_%d_%d_%d",fileName.latin1(),cdate.day(),cdate.month(),ctime.hour(),ctime.minute(),ctime.second());
        lEdit=new QLineEdit(this);                  
        lEdit->setMaxLength (36); 
        lEdit->setGeometry((width()-160)/2,(height()-20)/2,160,20);

        lEdit->setText(fileName);

        lEdit->setSelection (0,20);
        lEdit->setFocus();
    }

    if ( !ls.isNull() || !rs.isNull() )
    {
        dButtonList=new MenuButtonList(this);;
        if ( !ls.isNull() )
        {
            leftButton=new MenuButton(0,ls,this,true);          
            leftButton->setColors(QColor(0,148,255),QColor(0,0,0));
            leftButton->setGeometry(120,280,60,20);
            dButtonList->appendMenuButton(leftButton);          
            leftButton->setCentered(true);
        }
        if ( !ms.isNull() )
        {
            middleButton=new MenuButton(1,ms,this,true);
            middleButton->setColors(QColor(0,148,255),QColor(0,0,0));
            middleButton->setGeometry(120,280,60,20);
            dButtonList->appendMenuButton(middleButton);            
            middleButton->setCentered(true);
        }
        if ( !rs.isNull() )
        {
            rightButton=new MenuButton(2,rs,this,true);
            rightButton->setColors(QColor(0,148,255),QColor(0,0,0));
            rightButton->setGeometry(120,280,60,20);
            dButtonList->appendMenuButton(rightButton);         
            rightButton->setCentered(true);
        }
    }
    if ( leftButton )
    {
        if ( rightButton || middleButton )
        {
            leftButton->move(margin,height()-leftButton->height()-margin);
        }
        else
        {
            leftButton->move(((width()-leftButton->width())/2),height()-leftButton->height()-margin);
        }
    }
    if ( middleButton )
    {

        middleButton->move(((width()-middleButton->width())/2),height()-middleButton->height()-margin);

    }
    if ( rightButton || middleButton )
    {
        if ( leftButton )
        {
            rightButton->move(width()-rightButton->width()-margin,height()-rightButton->height()-margin);
        }
        else
        {
            rightButton->move(((width()-rightButton->width())/2),height()-rightButton->height()-margin);
        }
    }

    if ( dButtonList )
    {
        dButtonList->configureButtons(true,false);
        connect( dButtonList, SIGNAL(clicked(int)),this, SLOT(buttonClicked(int)));     
    }

    if ( background && parentWidget() )
    {
        doTinted=true;
        createTintedBackground(background);
    }
    else
    {
        doTinted=false;
    }
}

// Make the source image only half as bright

void GameDialog::createTintedBackground(QPixmap *back)
{
    QPainter p(&tinted);
    QRect r=parentWidget()->geometry(); 
    p.drawPixmap(0,0,*back,x()-r.x(),y()-r.y());
    QImage img=tinted.convertToImage();     // this one is slow like hell !!!!


    QRgb *pixel=(QRgb*)img.bits();
    QRgb *pixel2=pixel+(img.width()*img.height());

    while ( pixel!=pixel2 )
    {
        *pixel++=qRgb(qRed(*pixel)>>1,qGreen(*pixel)>>1,qBlue(*pixel)>>1);
    }
    tinted.convertFromImage(img);
}



