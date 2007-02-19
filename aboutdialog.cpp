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
#include "menubutton.h"
#include "imagerepository.h"
#include "puzzle.h"

static const char *historyData[] =
    {
        "",
        "Changes:",
        "",
        "*** VERSION 2.1.0 **************",
        "- Ported to Qt 4",
        "  Code cleanups and bug fixes.",
        "",
        "*** VERSION 2.0.0 **************",
        "- added ability to create additional sets of",
        "   colored blocks ( themes.)",
        "- new themes ( Flags and Letters)",
        "- 400 new levels courtesy of Vexed",
        "   project ( ported to Puzz-le level",
        "   format by Alexander Paersch )",
        "- minor bug fixes ..",
        "",
        "*** VERSION 1.0.3 **************",
        "- added option to enable/disable time limit",
        "- added ability to pause game",
        "- minor bug fixes ..",
        "",
        "END"
    };

static const char *infoData[] =
    {
        "",
        "Puzz-le is an arcade puzzler.",
        "",
        "Your task is to eliminate all of",
        "the colored blocks by putting them",
        "together.",
        "It is relatively easy with two blocks,",
        "but can be quite tricky with three",
        "or more.",
        "Depending on the levels pack, there is ",
        "either a time limit (time:000) or a limited",
        "number of moves (mov:00) you are allowed ",
        "to make.",
        "",
        "Some of the objects you will encounter",
        "are:",
        "|B   -  colored blocks",
        "|E   -  elevators",
        "|F   -  fire pits",
        "|D   -  damaged platforms",
        "|T   -  trap doors",
        "",
        "To pick up object, click on it.",
        "The cursor around the block turns white",
        "while you are carrying an object.",
        "",
        "Once you have picked up a block,",
        "you can move it horizontally.",
        "When you touch two or more similar",
        "blocks together, they will disappear.",
        "",
        "Tap on the Demo Game button to watch ",
        "computer solving each of the 50 available",
        "levels.",
#ifdef DEMO_VERSION
        "",
        "",
        "This is a demo version that contains",
        "a sample of 8 levels. The full version",
        "of the game contains all 50 levels",
#endif
        "",
        "Have fun and good luck !",
        "",
        "END"
    };

class AboutDialogItem: public Q3ListBoxItem
{
public:
    AboutDialogItem(const QString & textLine, Q3ListBox * lb);

    virtual int height(const Q3ListBox *) const;
    virtual int width(const Q3ListBox *) const;

protected:
    virtual void paint(QPainter *);

private:
    int fHeight;
};

AboutDialog::AboutDialog(QWidget * parent): QWidget(parent)
{
    QFont f = font();
    f.setBold(true);
    setFont(f);

    textEdit = new QTextEdit(this);
    textEdit->setBackgroundColor(QColor(0, 0, 0));
    
    lBox = new Q3ListBox(this);
    
    //lBox->hide();
    textEdit->hide();
    lBox->setFrameStyle(QFrame::NoFrame);
    lBox->setFocusPolicy(Qt::NoFocus);
    lBox->setHScrollBarMode(Q3ScrollView::AlwaysOff);

    setBackgroundColor(QColor(0, 0, 0));
    textEdit->setFrameStyle(QFrame::NoFrame);
    textEdit->setTextColor(QColor(255, 255, 255));
    setMode(ABOUT);
}

void AboutDialog::setMode(MODE m)
{
    lBox->clear();
    QString all;
    QString tmp;
    int i = 0;
    while (true) {
        if (m == ABOUT)
            tmp = infoData[i];
        else
            tmp = historyData[i];
        if (tmp == "END")
            break;
        all += tmp;
        all += '\n';
        lBox->insertItem(new AboutDialogItem(tmp, lBox));
        i++;
    }
    textEdit->setPlainText(all);
}

void AboutDialog::resizeEvent(QResizeEvent *)
{
    textEdit->setGeometry(0, 1, width(), height() - 2);
    lBox->setGeometry(0, 1, width(), height() - 2);
}

void AboutDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.setPen(QColor(0, 148, 255));
    p.drawLine(0, 0, width(), 0);
    p.drawLine(0, height() - 1, width(), height() - 1);
}

AboutDialogItem::AboutDialogItem(const QString & textLine, Q3ListBox * lb)
{
    fHeight = 0;
    setText(textLine);
    setCustomHighlighting(true);

    if (lb) {
        if (!textLine.isEmpty() && textLine.at(0) == '|')
            fHeight = Puzzle::blockPixelSize + 4;
        else {
            QFontMetrics fm(lb->font());
            fHeight = fm.height() + 6;
        }
    }
}

int AboutDialogItem::height(const Q3ListBox *) const
{
    return fHeight;
}

int AboutDialogItem::width(const Q3ListBox * l) const
{
    return l->width();
}

void AboutDialogItem::paint(QPainter * p)
{
    QColor q(0, 148, 255);
    QString txt = text();

    if (!txt.isEmpty() && txt.at(0) == '|') {
        ImageRepository::IMAGE_NAMES iname = ImageRepository::YELLOW;
        switch (txt.at(1).latin1()) {
        case 'B':
            iname = ImageRepository::YELLOW;
            break;
        case 'E':
            iname = ImageRepository::WALL;
            break;
        case 'F':
            iname = ImageRepository::FIRE;
            break;
        case 'D':
            iname = ImageRepository::BROKEN_WALL;
            break;
        case 'T':
            iname = ImageRepository::TRAP_LEFT;
            break;
        }
        QPixmap pix = Puzzle::images->findPixmap(iname);
        p->drawPixmap(0, 2, pix, 0, 0, Puzzle::blockPixelSize, Puzzle::blockPixelSize);
        if (txt.at(1) == 'T') {
            pix =
                Puzzle::images->
                findPixmap(ImageRepository::TRAP_RIGHT);
            if (p)
                p->drawPixmap(0, 2, pix, 0, 0, Puzzle::blockPixelSize,
                              Puzzle::blockPixelSize);
        }
        MenuButton::drawColorizedText(txt.remove(0, 2), Puzzle::blockPixelSize + 2, 3, p, q, 150);
    } else {
        MenuButton::drawColorizedText(text(), 0, 3, p, q, 150);
    }
}
