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
** Simple file requester for selecting saved games.
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/


#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlistbox.h> 


class MenuButton;
class MenuButtonList;
class Playground;

class FileDialogItem: public QListBoxItem
{
public:
	FileDialogItem(const QString &fileName,const QString &visibleFileNam,int levelNum, QListBox *lb);
	
	virtual int height ( const QListBox * ) const;
	virtual int width ( const QListBox * ) const;

	inline QString fileName()
	{
		return QString(fName);
	}


protected:
	virtual void paint ( QPainter * );


private:
	QString fName;
	int number;
	int numOffset;
	int fHeight;
};

class FileDialog : public QWidget
{ 
    Q_OBJECT

public:

    FileDialog( const QString &dir,QWidget *parent=0,const char *name=0);

	unsigned int refresh(Playground *play);



signals:

	void loadSavedGame(const QString &fileName);
	void done(void);

protected: 
	virtual void paintEvent ( QPaintEvent * ) ; 
	virtual void keyPressEvent ( QKeyEvent * e );
	virtual void resizeEvent ( QResizeEvent * );    

	
private slots:
	void buttonClicked(int);
	void listBoxSelected(QListBoxItem * );

private:
	
	void deleteSelected();

	enum BUTTONS 
	{
		GO_BACK,
		DELETE,
		LOAD
	};

	QString dDir;				
	MenuButton *backButton;
	MenuButton *loadButton;
	MenuButton *deleteButton;
	MenuButtonList *dButtonList;
	QListBox	*lBox;
	int vMargin;			// horizontal margin
	int	hMargin;			// vertical margin ( should include size of MenuButton

};



#endif // FILE_DIALOG_H
