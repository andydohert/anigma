/*
* Copyright (C) 2005-2007 Benjamin C Meyer
* Copyright (C) 2001-2002 Walter Rawdanik
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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qevent.h>
#include <q3listbox.h>

class MenuButton;
class MenuButtonList;
class Playground;

class FileDialogItem: public Q3ListBoxItem
{
public:
    FileDialogItem(const QString &fileName, const QString &visibleFileNam, int levelNum, Q3ListBox *lb);

    virtual int height ( const Q3ListBox * ) const;
    virtual int width ( const Q3ListBox * ) const;

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

/*
 * Simple file requester for selecting saved games.
 */
class FileDialog : public QWidget
{
    Q_OBJECT

signals:
    void loadSavedGame(const QString &fileName);
    void done();

public:
    FileDialog(const QString &dir, QWidget *parent = 0);
    void refresh();

protected:
    virtual void paintEvent(QPaintEvent *) ;
    virtual void keyPressEvent(QKeyEvent * e);
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void deleteSelected();
    void load();
    void listBoxSelected(Q3ListBoxItem * );

private:
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
    Q3ListBox *lBox;
    int vMargin;   // horizontal margin
    int hMargin;   // vertical margin ( should include size of MenuButton
};

#endif // FILEDIALOG_H