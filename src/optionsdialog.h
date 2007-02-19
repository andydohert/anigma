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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <qdialog.h>
#include <qstring.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>


class QLineEdit;
class MenuButton;
class MenuButtonList;
class QComboBox;

/**
 *  Options dialog - nothing fancy , just no frames and black background
 */
class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    OptionsDialog( QString *currLevel, QString *thm, bool *se, bool *ar, bool *tl, int *gs, QWidget *parent = 0, const char *name = 0);

protected:
    virtual void paintEvent(QPaintEvent *) ;
    virtual void resizeEvent(QResizeEvent *);

protected slots:
    virtual void accept();

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
    MenuButton *OKButton;
    MenuButton *cancelButton;
    MenuButtonList *dButtonList;

    static int sOptions[5];
};



#endif // OPTIONSDIALOG_H



