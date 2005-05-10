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
** Quck and dirty adaptation of standard modal QDialog for the game.
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/


#ifndef GAME_DIALOG_H
#define GAME_DIALOG_H

#include <qdialog.h>
#include <qstring.h>
#include <qpixmap.h>
#include "Fixed.h"


class QTimer;
class QLineEdit;
class MenuButton;
class MenuButtonList;

class GameDialog : public QDialog
{ 
    Q_OBJECT

public:

    GameDialog( QWidget *parent=0,const char *name=0);
	

	virtual void show();
	virtual void hide (); 

	void configure(QPixmap *background,const QString &text,bool fancy,bool requester=false,int timeout=0, const QString &ls=QString::null,	
	                 const QString &ms=QString::null,const QString &rs=QString::null);

	inline QString requesterText()
	{
		return reqText;
	}


protected:
	virtual void paintEvent ( QPaintEvent * ) ; 
	virtual void keyPressEvent ( QKeyEvent * e );
	virtual void resizeEvent ( QResizeEvent * );

	
private slots:
	void buttonClicked(int);
	void timeSynch();

private:
 
   enum DEFINES
   {
		NUM_FW=3,
		NUM_FP=32,
   };

	void updateFireworks();
	void createTintedBackground(QPixmap *back);

	QString dText;
	bool needsButtons;
	Key dkl;
	Key dkr;
	Key dkm;
	MenuButton *leftButton;
	MenuButton *middleButton;
	MenuButton *rightButton;
	MenuButtonList *dButtonList;
	QLineEdit *lEdit;
		int tCounter;
	int		margin;
	int		textWidth;
	int fireWorks[NUM_FW];
	int firePoints[NUM_FP*NUM_FW];
	int fcount[NUM_FW];
	fixed xc[NUM_FP*NUM_FW];
	fixed yc[NUM_FP*NUM_FW];
	fixed xa[NUM_FP*NUM_FW];
	fixed ya[NUM_FP*NUM_FW];
	QTimer *timer;
	static int numFW;
	static int numFP;
	bool doFancy;
	QPixmap pix;
	QPixmap tinted;
	bool doTinted;
	QString reqText;

};



#endif // GAME_DIALOG_H


