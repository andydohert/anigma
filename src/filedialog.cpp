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

#include "filedialog.h"

#include <qdir.h>
#include <qpainter.h>
#include <qevent.h>
#include <qtreeview.h>
#include <qdebug.h>
#include <qstandarditemmodel.h>
#include <qheaderview.h>

#include "menubutton.h"
#include "playground.h"
#include "gamedialog.h"

FileDialog::FileDialog(const QString &dir, QWidget *parent): QWidget(parent)
{
    directory = dir;
    vMargin = 24;
    hMargin = 2;

    backButton = new MenuButton("Go Back", this);
    backButton->showFrame(true);
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(done()));
    backButton->setCentered(true);

    deleteButton = new MenuButton("Delete", this);
    deleteButton->showFrame(true);
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));
    deleteButton->setCentered(true);

    loadButton = new MenuButton("Load", this);
    loadButton->showFrame(true);
    connect(loadButton, SIGNAL(clicked()), this, SLOT(load()));
    loadButton->setCentered(true);

    treeView = new QTreeView(this);
    connect(treeView, SIGNAL(activated(const QModelIndex &)), this, SLOT(load()));
    treeView->setFrameShape(QFrame::NoFrame);
    model = new QStandardItemModel(0, 2, this);
    treeView->setModel(model);
    treeView->setIndentation(0);
    treeView->header()->setStretchLastSection(false);
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
        break;
    }
}

void FileDialog::resizeEvent(QResizeEvent *)
{
    backButton->setGeometry(0, height() - 24, 60, 20);
    deleteButton->setGeometry((width() - backButton->width()) / 2, backButton->y(), 60, 20);
    loadButton->setGeometry(width() - backButton->width(), backButton->y(), 60, 20);
    treeView->setGeometry(hMargin, vMargin*2 + 1, width() - (hMargin*2), height() - (vMargin*3) - 8);
}

void FileDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(QColor(0, 148, 255));
    p.drawLine(0, vMargin*2, width(), vMargin*2);
    p.drawLine(0, height() - vMargin - 6, width(), height() - vMargin - 6);
    //MenuButton::drawColorizedText("Name", treeView->x(), treeView->y() - vMargin + 4, &p, QColor(0, 148, 255), 150);
    //int offset = treeView->x() + treeView->header()->sectionSize(0);
    //MenuButton::drawColorizedText("Level", offset, treeView->y() - vMargin + 4, &p, QColor(0, 148, 255), 150);
    //offset += treeView->x() + treeView->header()->sectionSize(1);
    //MenuButton::drawColorizedText("Points", offset, treeView->y() - vMargin + 4, &p, QColor(0, 148, 255), 150);
    MenuButton::drawColorizedText("List of available saved games ...", hMargin, 2, &p, QColor(0, 148, 255), 150);
}

void FileDialog::refresh()
{
    QColor q(0, 148, 255);
    QDir d(directory);
    model->clear();
    model->insertColumns(0, 3);

    model->setHeaderData(0, Qt::Horizontal, "Name");
    model->setHeaderData(1, Qt::Horizontal, "Level");
    model->setHeaderData(2, Qt::Horizontal, "Points");
    model->setHeaderData(0, Qt::Horizontal,  q, Qt::ForegroundRole);
    model->setHeaderData(1, Qt::Horizontal,  q, Qt::ForegroundRole);
    model->setHeaderData(2, Qt::Horizontal,  q, Qt::ForegroundRole);
    model->setHeaderData(0, Qt::Horizontal,  Qt::black, Qt::BackgroundRole);
    model->setHeaderData(1, Qt::Horizontal,  Qt::black, Qt::BackgroundRole);
    model->setHeaderData(2, Qt::Horizontal,  Qt::black, Qt::BackgroundRole);

    if (d.exists()) {
        d.setFilter( QDir::Files | QDir::Readable | QDir::NoSymLinks );
        d.setSorting(QDir::Time);
        d.setNameFilters(QStringList("*.puzzle"));
        const QFileInfoList list = d.entryInfoList();
        for (int i = 0; i < list.count(); ++i) {
            QFileInfo info = list.at(i);
            unsigned int level;
            unsigned int points;
            if (Playground::savedPlaygroundInfo(info.absoluteFilePath(), &points, &level) ) {
                model->insertRow(model->rowCount());
                QModelIndex idx = model->index(model->rowCount() - 1, 0);
                model->itemFromIndex(idx)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                model->setData(idx, info.fileName(), 32);
                model->setData(idx, info.baseName());
                model->setData(idx, q, Qt::ForegroundRole);
                idx = model->index(model->rowCount() - 1, 1);
                model->itemFromIndex(idx)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                model->setData(idx, QString("%1").arg(level));
                model->setData(idx, q, Qt::ForegroundRole);
                idx = model->index(model->rowCount() - 1, 2);
                model->itemFromIndex(idx)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                model->setData(idx, QString("%1").arg(points));
                model->setData(idx, q, Qt::ForegroundRole);
            } else {
                qDebug() << "error reading" << info.absolutePath();
            }
        }
    }
    QModelIndex idx = model->index(0, 0);
    treeView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    treeView->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    treeView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    treeView->header()->setResizeMode(0, QHeaderView::Stretch);
}

void FileDialog::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if (visible) {
        treeView->setFocus();
    }
}

void FileDialog::load()
{
    QModelIndex idx = treeView->currentIndex();
    idx = idx.sibling(idx.row(), 0);
    if (idx.isValid())
        emit loadSavedGame(directory + "/" + idx.data(32).toString());
}

void FileDialog::deleteSelected()
{
    GameDialog *dlg = new GameDialog(this->parentWidget());
    dlg->configure("Delete selected game?", false, false, 0, "Yes", QString::null, "No");
    QModelIndex idx = treeView->currentIndex();
    idx = idx.sibling(idx.row(), 0);
    if (dlg->exec() == 0) {
        QFile f(directory + "/" + idx.data(32).toString());
        if (f.remove()) {
            model->removeRow(idx.row());
        }
    }
    delete dlg;
}

