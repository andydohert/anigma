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
** Options dialog - nothing fancy , just no frames and black background
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
***************************************************************************/


#ifndef OPTIONS_DIALOG_H
#define OPTIONS_DIALOG_H

#include <qdialog.h>
#include <qstring.h>


class QLineEdit;
class MenuButton;
class MenuButtonList;
class QComboBox;

class OptionsDialog : public QDialog
{ 
    Q_OBJECT

public:

    OptionsDialog( QString *currLevel,QString *thm,bool *se, bool *ar,bool *tl,int *gs,QWidget *parent=0,const char *name=0);
	


protected:
	virtual void paintEvent ( QPaintEvent * ) ; 
	virtual void resizeEvent ( QResizeEvent * );

protected slots:
    virtual void accept(); 


	
private slots:
	void buttonClicked(int);
	

private:	

    bool *ise;
    bool *iar;
    bool *itl;
    int  *igs;
    QString *ile;
    QString *the;
	QComboBox   *soundEffects;
    QComboBox   *abortConfirmation;
    QComboBox   *gameSpeed;
    QComboBox   *customLevel;
    QComboBox   *timeLimit;
    QComboBox   *themes;
	Key dko;
	Key dkc;
	MenuButton *OKButton;
	MenuButton *cancelButton;
	MenuButtonList *dButtonList; 

    static int sOptions[5];
};



#endif // OPTIONS_DIALOG_H



