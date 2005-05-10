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

#include <qdir.h>
#include <qfontmetrics.h>
#include <qapplication.h>
#include <qpalette.h>
#include <qpainter.h>
#include <stdlib.h>
#include "FileDialog.h"
#include "MenuButton.h"
#include "Playground.h"
#include "GameDialog.h"

FileDialog::FileDialog( const QString &dir,QWidget *parent,const char *name):QWidget(parent,name)
{
	QFont f=font();
	f.setBold(true);
	setFont(f);	
	setBackgroundColor(QColor(0,0,0));
	dDir=dir;
	vMargin=24;
	hMargin=2;

	setFocusPolicy(QWidget::StrongFocus);


	
	dButtonList=new MenuButtonList(this);;
	
	backButton=new MenuButton(GO_BACK,"Go Back",this,true);			
	backButton->setColors(QColor(0,148,255),QColor(0,0,0));	
	backButton->setCentered(true);	
	
	deleteButton=new MenuButton(DELETE,"Delete",this,true);
	deleteButton->setColors(QColor(0,148,255),QColor(0,0,0));
	deleteButton->setCentered(true);

	loadButton=new MenuButton(LOAD,"Load",this,true);
	loadButton->setColors(QColor(0,148,255),QColor(0,0,0));
	loadButton->setCentered(true);
	
	dButtonList->appendMenuButton(backButton);
	dButtonList->appendMenuButton(deleteButton);
	dButtonList->appendMenuButton(loadButton);
	dButtonList->configureButtons(true,false);
	connect( dButtonList, SIGNAL(clicked(int)),this, SLOT(buttonClicked(int)));


	lBox=new QListBox(this);
	lBox->setFrameStyle ( QFrame::NoFrame ) ;
	lBox->setFocusPolicy(QWidget::NoFocus);

	connect( lBox, SIGNAL(selected(QListBoxItem *)),this, SLOT(listBoxSelected(QListBoxItem * )));

}

void FileDialog::keyPressEvent ( QKeyEvent *e )
{	
	switch(e->key())
	{
      case  Key_F33:
			buttonClicked(LOAD);
			break;
      case  Key_Escape:
         buttonClicked(GO_BACK);
         break;               
      default:    
			qApp->sendEvent(lBox,e);
			break;
	}

}

void FileDialog::resizeEvent ( QResizeEvent * )
{

	backButton->setGeometry(0,height()-24,60,20);
	deleteButton->setGeometry((width()-backButton->width())/2,backButton->y(),60,20);
	loadButton->setGeometry(width()-backButton->width(),backButton->y(),60,20);
	lBox->setGeometry(hMargin,vMargin*2+1,width()-(hMargin*2),height()-(vMargin*3)-8);

}

void FileDialog::paintEvent ( QPaintEvent * ) 
{
	QPainter p(this);
	p.setPen(QColor(0,148,255));
	p.drawLine(0,vMargin*2,width(),vMargin*2);
	p.drawLine(0,height()-vMargin-6,width(),height()-vMargin-6);
	MenuButton::drawColorizedText("Name",hMargin,lBox->y()-vMargin+4,&p,QColor(0,148,255),150);
	MenuButton::drawColorizedText("Level",width()-40,lBox->y()-vMargin+4,&p,QColor(0,148,255),150);
	MenuButton::drawColorizedText("List of available saved games ...",hMargin,2,&p,QColor(0,148,255),150);
}

void FileDialog::buttonClicked(int id)
{
	switch(id)
	{
		case	GO_BACK:
			emit done();
			break;
		case	DELETE:
			deleteSelected();
			break;
		case	LOAD:
			listBoxSelected(lBox->item(lBox->currentItem()));
			break;
	}
}

void FileDialog::deleteSelected()
{
	FileDialogItem *item=(FileDialogItem*)lBox->item(lBox->currentItem());	
	
	if(item)
	{
		GameDialog *dlg= new GameDialog(this->parentWidget());
		dlg->configure(0,"Delete selected game ?",false,false,0,"Yes",QString::null,"No");
		if(dlg->exec()==0)
		{
			QFile f(getenv("HOME")+QString("/puzz-le/")+item->fileName());
			if(f.remove())
			{
				lBox->removeItem(lBox->currentItem());
			}
		}
		delete dlg;
	}
	
}

unsigned int FileDialog::refresh(Playground *play)
{
    QDir d(dDir);
    lBox->clear();

    if ( d.exists() )
    {        
        d.setFilter( QDir::Files | QDir::Readable | QDir::NoSymLinks ); 
        d.setSorting( QDir::Time  );
        d.setNameFilter("*.puzzle");

        const QFileInfoList *list = d.entryInfoList();
        QFileInfoListIterator it( *list );      
        QFileInfo *fi;                   
        unsigned int level;
        unsigned int points;

        while ( (fi=it.current()) )
        {
            if ( play->savedPlaygroundInfo(fi->absFilePath(),&points,&level) )
            {
                lBox->insertItem(new FileDialogItem(fi->fileName(),fi->baseName(),level,lBox));
            }
            ++it;                              
        }
        if ( lBox->count() )
            lBox->setSelected(0,true);
    }
    return lBox->count();
}

void FileDialog::listBoxSelected( QListBoxItem *i)
{
	if(i)
	{
		FileDialogItem *item=(FileDialogItem*)i;
		emit loadSavedGame(item->fileName());
	}
}


FileDialogItem::FileDialogItem(const QString &fileName,const QString &visibleFileName,int levelNum, QListBox *lb)
{
	number=levelNum;
	numOffset=0;
	fHeight=0;

	fName=fileName;
	QString tmp=visibleFileName;
	tmp.truncate(36);
	setText(tmp);
	setCustomHighlighting ( true ); 

	if(lb)
	{
		QFontMetrics fm(lb->font());
		numOffset=fm.width("000")+2;
		fHeight=fm.height()+6;
	}
}
	
int FileDialogItem::height ( const QListBox * ) const
{
	return fHeight;
}
int FileDialogItem::width ( const QListBox *l ) const
{
	return l->width();
}

void FileDialogItem::paint ( QPainter *p) 
{
	QColor q(0,148,255);
	if(listBox()->isSelected(this))
	{
		QFont f=p->font();
		f.setUnderline(true);
		p->setFont(f);
		q.setRgb(255,255,255);
	}

	MenuButton::drawColorizedText(text(),0,3,p,q,150);	
	MenuButton::drawColorizedText(QString::number(number),listBox()->width()-numOffset,3,p,q,150);	
}


