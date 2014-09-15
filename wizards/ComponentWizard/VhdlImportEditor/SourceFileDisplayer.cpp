//-----------------------------------------------------------------------------
// File: SourceFileDisplayer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.09.2014
//
// Description:
// Display widget for source files.
//-----------------------------------------------------------------------------

#include "SourceFileDisplayer.h"

#include <QObject>
#include <QScrollBar>
#include <QTextBlock>

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
    
    setTabStopWidth(4 * fontMetrics().width(' '));

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

//-----------------------------------------------------------------------------
// Function: SourceFileDisplayer::scrollToCharacterPosition()
//-----------------------------------------------------------------------------
void SourceFileDisplayer::scrollToCharacterPosition(int characterPosition)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(characterPosition);

    int rowNumber = cursor.block().firstLineNumber();
    verticalScrollBar()->setSliderPosition(rowNumber);
}
