//-----------------------------------------------------------------------------
// File: textedit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Timo Korpela
// Date: 28.05.2013
//
// Description:
// QTextEdit with custom functionality
//-----------------------------------------------------------------------------
#include <QTextCursor>
#include "textedit.h"


//-----------------------------------------------------------------------------
// Function: textEdit::textEdit()
//-----------------------------------------------------------------------------
textEdit::textEdit( QWidget *parent ) : QTextEdit( parent )
{
    setReadOnly( true );
    setCursorWidth(0);
}

//-----------------------------------------------------------------------------
// Function: textEdit::~textEdit()
//-----------------------------------------------------------------------------
textEdit::~textEdit()
{
}

//-----------------------------------------------------------------------------
// Function: textEdit::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void textEdit::mouseDoubleClickEvent(QMouseEvent *e) {

    QTextCursor textc = cursorForPosition( e->pos() );
    textc.select(QTextCursor::LineUnderCursor );
    QString worduc = textc.selectedText();
    if ( !worduc.isEmpty() ) {
         emit selectedWord( worduc );
    }
}
