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

#include <qdir.h>
#include <qfontmetrics.h>
#include <qapplication.h>
#include <qpalette.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <Q3Frame>
#include <QKeyEvent>
#include <stdlib.h>
#include "filedialog.h"
#include "menubutton.h"
#include "playground.h"
#include "gamedialog.h"

FileDialog::FileDialog( const QString &dir, QWidget *parent): QWidget(parent)
{
    QFont f = font();
    f.setBold(true);
    setFont(f);
    setBackgroundColor(QColor(0, 0, 0));
    dDir = dir;
    vMargin = 24;
    hMargin = 2;

// setFocusPolicy(QWidget::StrongFocus);

    dButtonList = new MenuButtonList(this);;

    backButton = new MenuButton(GO_BACK, "Go Back", this, true);
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(done()));
    backButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
    backButton->setCentered(true);

    deleteButton = new MenuButton(DELETE, "Delete", this, true);
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));
    deleteButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
    deleteButton->setCentered(true);

    loadButton = new MenuButton(LOAD, "Load", this, true);
    connect(loadButton, SIGNAL(clicked()), this, SLOT(load()));
    loadButton->setColors(QColor(0, 148, 255), QColor(0, 0, 0));
    loadButton->setCentered(true);

    dButtonList->appendMenuButton(backButton);
    dButtonList->appendMenuButton(deleteButton);
    dButtonList->appendMenuButton(loadButton);
    dButtonList->setVisible(true);

    lBox = new Q3ListBox(this);
    lBox->setFrameStyle ( Q3Frame::NoFrame ) ;
    lBox->setFocusPolicy(Qt::NoFocus);

    connect( lBox, SIGNAL(selected(Q3ListBoxItem *)), this, SLOT(listBoxSelected(Q3ListBoxItem * )));
}

void FileDialog::keyPressEvent( QKeyEvent *e )
{
    switch(e->key()) {
    case  Qt::Key_F33:
        load();
        break;
    case  Qt::Key_Escape:
        emit done();
        break;
    default:
        qApp->sendEvent(lBox, e);
        break;
    }
}

void FileDialog::resizeEvent( QResizeEvent * )
{
    backButton->setGeometry(0, height() - 24, 60, 20);
    deleteButton->setGeometry((width() - backButton->width()) / 2, backButton->y(), 60, 20);
    loadButton->setGeometry(width() - backButton->width(), backButton->y(), 60, 20);
    lBox->setGeometry(hMargin, vMargin*2 + 1, width() - (hMargin*2), height() - (vMargin*3) - 8);
}

void FileDialog::paintEvent ( QPaintEvent * )
{
    QPainter p(this);
    p.setPen(QColor(0, 148, 255));
    p.drawLine(0, vMargin*2, width(), vMargin*2);
    p.drawLine(0, height() - vMargin - 6, width(), height() - vMargin - 6);
    MenuButton::drawColorizedText("Name", hMargin, lBox->y() - vMargin + 4, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("Level", width() - 40, lBox->y() - vMargin + 4, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("List of available saved games ...", hMargin, 2, &p, QColor(0, 148, 255), 150);
}

void FileDialog::load()
{
    listBoxSelected(lBox->item(lBox->currentItem()));
}

void FileDialog::deleteSelected()
{
    FileDialogItem *item = (FileDialogItem*)lBox->item(lBox->currentItem());

    if(item) {
        GameDialog *dlg = new GameDialog(this->parentWidget());
        dlg->configure(0, "Delete selected game ?", false, false, 0, "Yes", QString::null, "No");
        if(dlg->exec() == 0) {
            QFile f(QString(getenv("HOME")) + QString("/puzz-le/") + item->fileName());
            if(f.remove()) {
                lBox->removeItem(lBox->currentItem());
            }
        }
        delete dlg;
    }

}

void FileDialog::refresh()
{
    QDir d(dDir);
    lBox->clear();
    if (d.exists()) {
        d.setFilter( QDir::Files | QDir::Readable | QDir::NoSymLinks ); 
        d.setSorting(QDir::Time);
        d.setNameFilter("*.puzzle");
        const QFileInfoList list = d.entryInfoList();
        for (int i = 0; i < list.count(); ++i)
        {
            QFileInfo info = list.at(i);
            unsigned int level;
            unsigned int points;
            if (Playground::savedPlaygroundInfo(info.absFilePath(), &points, &level) )
            {
                lBox->insertItem(new FileDialogItem(info.fileName(), info.baseName(),level,lBox));
            }
        }
        if ( lBox->count() )
            lBox->setSelected(0,true);
    }
}

void FileDialog::listBoxSelected( Q3ListBoxItem *i)
{
    if(i) {
        FileDialogItem *item = (FileDialogItem*)i;
        emit loadSavedGame(item->fileName());
    }
}

FileDialogItem::FileDialogItem(const QString &fileName, const QString &visibleFileName, int levelNum, Q3ListBox *lb)
{
    number = levelNum;
    numOffset = 0;
    fHeight = 0;

    fName = fileName;
    QString tmp = visibleFileName;
    tmp.truncate(36);
    setText(tmp);
    setCustomHighlighting ( true );

    if(lb) {
        QFontMetrics fm(lb->font());
        numOffset = fm.width("000") + 2;
        fHeight = fm.height() + 6;
    }
}

int FileDialogItem::height ( const Q3ListBox * ) const
{
    return fHeight;
}

int FileDialogItem::width ( const Q3ListBox *l ) const
{
    return l->width();
}

void FileDialogItem::paint ( QPainter *p)
{
    QColor q(0, 148, 255);
    if(listBox()->isSelected(this)) {
        QFont f = p->font();
        f.setUnderline(true);
        p->setFont(f);
        q.setRgb(255, 255, 255);
    }

    MenuButton::drawColorizedText(text(), 0, 3, p, q, 150);
    MenuButton::drawColorizedText(QString::number(number), listBox()->width() - numOffset, 3, p, q, 150);
}

