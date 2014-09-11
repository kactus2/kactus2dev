//-----------------------------------------------------------------------------
// File: SourceFileDisplayer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "SourceFileDisplayer.h"

#include <QObject>

//-----------------------------------------------------------------------------
// Function: SourceFileDisplayer::SourceFileDisplayer()
//-----------------------------------------------------------------------------
SourceFileDisplayer::SourceFileDisplayer(QWidget* parent)
    :  QPlainTextEdit(parent)
{
    QFont font("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(9);
    setFont(font);

    int tabStop = 4;
    setTabStopWidth(tabStop * fontMetrics().width(' '));

    setReadOnly(true);
    setCursorWidth(0);
}

//-----------------------------------------------------------------------------
// Function: SourceFileDisplayer::~SourceFileDisplayer()
//-----------------------------------------------------------------------------
SourceFileDisplayer::~SourceFileDisplayer()
{

}

//-----------------------------------------------------------------------------
// Function: SourceFileDisplayer::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void SourceFileDisplayer::mouseDoubleClickEvent(QMouseEvent *e)
{
    int characterPosition = cursorForPosition(e->pos()).position();
    emit doubleClicked(characterPosition);
}
