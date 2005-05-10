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
** Quck and dirty class for custom (flashing) groups of buttons.
**
**
** Copyright (C) 2001 Walter Rawdanik.  All rights reserved.
**
****************************************************************************/

#ifndef MENU_BUTTON_H
#define MENU_BUTTON_H

#include <qbutton.h>
#include <qlist.h>


class QTimer;

class MenuButton : public QButton
{ 
    Q_OBJECT

public:

    MenuButton( int id,const QString& label,QWidget* parent = 0, bool df=false, const char* name = 0, WFlags fl = 0 );
    ~MenuButton();
		void setHighlighted(bool flag);  
	void setColors(const QColor& foreground, const QColor& background);
	inline bool isHighlighted()
	{
		return iHighlighted;
	}

	inline int id()
	{
		return iId;
	}

	inline void setCentered(bool f)
	{
		centered=f;
		repaint();
	}
	inline bool isCentered()
	{
		return centered;
	}
	static void drawColorizedText(const QString& text, int x, int y, QPainter *p,const QColor& clr,int min=0);

protected:
	void drawButton ( QPainter * );
    void focusInEvent ( QFocusEvent * );
    void focusOutEvent ( QFocusEvent * ); 



private:
	bool centered;
    bool iHighlighted;
	int	iId;
	bool drawFrame;

};

class MenuButtonList:public QObject
{
	Q_OBJECT
public:
	MenuButtonList(QObject * parent=0, const char * name=0 );
	
	void appendMenuButton(MenuButton *btn);
	void removeMenuButton(MenuButton *btn);

	void setHighlighted(int index);
    inline int highlighted()
    {
        return curHighlighted;
    }

	void configureButtons(bool show, bool highlight);

signals:

	void clicked(int id);

private slots:
	void buttonClicked();
	void timeout();

private:
	QList<MenuButton> list;
	QTimer	*timer;
	bool doHighlighting;
	int	 curHighlighted;	
};


#endif // MENU_BUTTON_H


