//-----------------------------------------------------------------------------
// File: ConsoleEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ConsoleEditor.h"

#include <QTextBlock>
#include <QScrollBar>
#include <QFontDatabase>

#include <PythonAPI/WriteChannel.h>

//-----------------------------------------------------------------------------
// Function: ConsoleEditor::ConsoleEditor()
//-----------------------------------------------------------------------------
ConsoleEditor::ConsoleEditor(WriteChannel* outputChannel, QWidget* parent): 
    QPlainTextEdit(parent),
    lockedLines_(0), 
    promptText_(),
    outputChannel_(outputChannel),
    fontFamily_()
{    
    setContentsMargins(0, 0, 0, 0);
    setTabStopWidth(32);
    setAcceptDrops(false);
    setUndoRedoEnabled(false);

    QFont font;
    font.setStyleHint(QFont::Monospace);
     
    fontFamily_ = font.defaultFamily();
}

//-----------------------------------------------------------------------------
// Function: ConsoleEditor::print()
//-----------------------------------------------------------------------------
void ConsoleEditor::print(QString const& input)
{

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextBlock block = cursor.block();
    
    QTextCharFormat format = block.charFormat();
    format.setForeground(QBrush(Qt::black));
    format.setFontFamily(fontFamily_);

    cursor.insertText(input, format);
    lockedLines_ = cursor.blockNumber() - 1;
    promptText_ = cursor.block().text();

    format.setForeground(QBrush(Qt::black));
    setCurrentCharFormat(format);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

//-----------------------------------------------------------------------------
// Function: ConsoleEditor::printError()
//-----------------------------------------------------------------------------
void ConsoleEditor::printError(QString const& input)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextBlock block = cursor.block();

    QTextCharFormat format = block.charFormat();
    format.setForeground(QBrush(Qt::red));
    format.setFontFamily(fontFamily_);
    
    cursor.insertText(input, format);
    lockedLines_ = cursor.blockNumber() - 1;
    promptText_ = cursor.block().text();

    format.setForeground(QBrush(Qt::black));
    setCurrentCharFormat(format);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

//-----------------------------------------------------------------------------
// Function: ConsoleEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ConsoleEditor::keyPressEvent(QKeyEvent *e)
{
    if ((textCursor().blockNumber() <= lockedLines_ || textCursor().positionInBlock() < promptText_.length()) &&
        (e->text().isEmpty() == false ||
         e->key() == Qt::Key_Backspace ||
         e->key() == Qt::Key_Delete ||
         e->key() == Qt::Key_Enter ||
         e->key() == Qt::Key_Return) || 
        (e->key() == Qt::Key_Backspace && textCursor().positionInBlock() == promptText_.length()))
    {
        return;
    }
    
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)        
    {
        QTextBlock block = textCursor().block();
        QString line = block.text();
        line.remove(0, promptText_.length());

        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
        QPlainTextEdit::keyPressEvent(e);

        outputChannel_->write(line);
    }
    else
    {
        QPlainTextEdit::keyPressEvent(e);
    }
}
