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

#include "aboutdialog.h"

#include <qpainter.h>
#include <qevent.h>
#include <QFrame>
#include <QScrollBar>
#include <qtextedit.h>
#include <qurl.h>
#include <qdebug.h>

#include "menubutton.h"
#include "imagerepository.h"
#include "puzzle.h"

const char *historyData =
        "Changes:"
        "<p>"
        "*** VERSION 2.1.0 **************<br>"
        "- Ported to Qt 4<br>"
        "  Code cleanups and bug fixes.<br>"
        "<p>"
        "*** VERSION 2.0.0 **************<br>"
        "- added ability to create additional sets of<br>"
        "   colored blocks ( themes.)<br>"
        "- new themes ( Flags and Letters)<br>"
        "- 400 new levels courtesy of Vexed<br>"
        "   project ( ported to Puzz-le level<br>"
        "   format by Alexander Paersch )<br>"
        "- minor bug fixes ..<br>"
        "<p>"
        "*** VERSION 1.0.3 **************<br>"
        "- added option to enable/disable time limit<br>"
        "- added ability to pause game<br>"
        "- minor bug fixes ..<br>";

const char *infoData =
        "Puzz-le is an arcade puzzler."
        "<P>"
        "Your task is to eliminate all of"
        "the colored blocks by putting them"
        "together."
        "It is relatively easy with two blocks,"
        "but can be quite tricky with three"
        "or more."
        "Depending on the levels pack, there is "
        "either a time limit (time:000) or a limited"
        "number of moves (mov:00) you are allowed "
        "to make."
        "<P>"
        "Some of the objects you will encounter"
        "are:<br>"
        "<img src=\"yellow.png\">  &nbsp;   colored blocks<br>"
        "<img src=\"wall.png\"> &nbsp;    elevators<br>"
        "<img src=\"fire.png\">  &nbsp;     fire pits<br>"
        "<img src=\"broken.png\">   &nbsp;    damaged platforms<br>"
        "<img src=\"trap.png\">    &nbsp;  trap doors"
        "<P>"
        "To pick up object, click on it."
        "The cursor around the block turns white"
        "while you are carrying an object."
        "<P>"
        "Once you have picked up a block,"
        "you can move it horizontally."
        "When you touch two or more similar"
        "blocks together, they will disappear."
        "<P>"
        "Tap on the Demo Game button to watch "
        "computer solving each of the 50 available"
        "levels."
#ifdef DEMO_VERSION
        ""
        "<P>"
        "This is a demo version that contains"
        "a sample of 8 levels. The full version"
        "of the game contains all 50 levels"
#endif
        "<P>"
        "Have fun and good luck !";

AboutDialog::AboutDialog(QWidget * parent): QWidget(parent), loadedImages(false)
{
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setFocusPolicy(Qt::NoFocus);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setFrameStyle(QFrame::NoFrame);
    textEdit->setTextColor(Qt::white);
    textEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    QPalette pal = textEdit->palette();
    QColor blue(0, 148, 255);
    pal.setColor(QPalette::Text, blue);
    textEdit->setPalette(pal);

    backButton = new MenuButton("Go Back", this);
    backButton->showFrame(true);
    backButton->setCentered(true);
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(showWelcomeScreen()));
    demoButton = new MenuButton("Demo Game", this);
    demoButton->showFrame(true);
    demoButton->setCentered(true);
    connect(demoButton, SIGNAL(clicked()), this, SIGNAL(showDemo()));
    setMode(ABOUT);
}

void AboutDialog::setMode(MODE m)
{
    QString all;

    if (m != ABOUT)
        all = historyData;
    else {
        all = infoData;
    }

    ImageRepository *ir = Puzzle::images;
    if (ir && !loadedImages) {
        loadedImages = true;
        textEdit->document()->addResource(QTextDocument::ImageResource, QUrl("yellow.png") ,
         ir->findPixmap(ImageRepository::YELLOW));
        textEdit->document()->addResource(QTextDocument::ImageResource, QUrl("wall.png") ,
         ir->findPixmap(ImageRepository::WALL));
        textEdit->document()->addResource(QTextDocument::ImageResource, QUrl("fire.png") ,
         ir->findPixmap(ImageRepository::FIRE).copy(0,0,Puzzle::blockPixelSize, Puzzle::blockPixelSize));
        textEdit->document()->addResource(QTextDocument::ImageResource, QUrl("broken.png") ,
         ir->findPixmap(ImageRepository::BROKEN_WALL).copy(0,0,Puzzle::blockPixelSize, Puzzle::blockPixelSize));
        QPixmap left = ir->findPixmap(ImageRepository::TRAP_LEFT);
        QPixmap right = ir->findPixmap(ImageRepository::TRAP_RIGHT);
        QPainter p(&left);
        p.drawPixmap(0, 0, right);
        textEdit->document()->addResource(QTextDocument::ImageResource, QUrl("trap.png") ,
         left);
    }
    textEdit->setHtml(all);
}

void AboutDialog::resizeEvent(QResizeEvent *)
{
    textEdit->setGeometry(0, 1, width(), height() - backButton->height() - 9);
    textEdit->setLineWrapColumnOrWidth(width() - textEdit->horizontalScrollBar()->width());
    backButton->setGeometry(4,height() - backButton->height() - 4,
                                        110, backButton->height());
    demoButton->setGeometry(width() - 110 - 4,
                          height() - backButton->height() - 4, 110,
                          backButton->height());
}

void AboutDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(QColor(0, 148, 255));
    p.drawLine(0, 0, width(), 0);
    p.drawLine(0, height() - backButton->height() - 8, width(), height() - backButton->height() - 8);
}
