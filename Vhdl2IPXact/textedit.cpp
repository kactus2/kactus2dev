#include <QTextCursor>
#include "textedit.h"

textEdit::textEdit( QWidget *parent ) : QTextEdit( parent )
{
    setReadOnly( true );
    setCursorWidth(0);

}

textEdit::~textEdit()
{
}

void textEdit::mouseDoubleClickEvent(QMouseEvent *e) {

    QTextCursor textc = cursorForPosition( e->pos() );
    textc.select(QTextCursor::LineUnderCursor );
    QString worduc = textc.selectedText();
    if ( !worduc.isEmpty() ) {
         emit selectedWord( worduc );
}
}
