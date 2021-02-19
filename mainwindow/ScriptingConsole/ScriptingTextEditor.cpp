//-----------------------------------------------------------------------------
// File: ScriptingTextEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ScriptingTextEditor.h"

#include <QTextBlock>
#include <QScrollBar>
#include <QMenu>
#include <QKeySequence>

#include <PythonAPI/WriteChannel.h>

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::ScriptingTextEditor()
//-----------------------------------------------------------------------------
ScriptingTextEditor::ScriptingTextEditor(WriteChannel* outputChannel, QWidget* parent): 
    QPlainTextEdit(parent),
    textLockPosition_(0), 
    promptText_(),
    outputChannel_(outputChannel),
    fontFamily_(),
    copyAction_(tr("Copy"), this)
{    
    setContentsMargins(0, 0, 0, 0);
   
    setAcceptDrops(false);
    setUndoRedoEnabled(false);

    QFont font;
    font.setStyleHint(QFont::Monospace);
     
    fontFamily_ = font.defaultFamily();

    copyAction_.setDisabled(true);

    connect(&copyAction_, SIGNAL(triggered()), this, SLOT(copy()), Qt::UniqueConnection);
    connect(this, SIGNAL(copyAvailable(bool)), &copyAction_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::print()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::print(QString const& input)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextBlock block = cursor.block();
    
    QTextCharFormat format = block.charFormat();
    format.setForeground(QBrush(Qt::black));
    format.setFontFamily(fontFamily_);

    cursor.insertText(input, format);
    textLockPosition_ = cursor.position();
    promptText_ = cursor.block().text();

    format.setForeground(QBrush(Qt::black));
    setCurrentCharFormat(format);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::printError()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::printError(QString const& input)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextBlock block = cursor.block();

    QTextCharFormat format = block.charFormat();
    format.setForeground(QBrush(Qt::red));
    format.setFontFamily(fontFamily_);
    
    cursor.insertText(input, format);
    textLockPosition_ = cursor.position();
    promptText_ = cursor.block().text();

    format.setForeground(QBrush(Qt::black));
    setCurrentCharFormat(format);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::keyPressEvent(QKeyEvent *e)
{
    if ((textCursor().position() < textLockPosition_) &&
        (e->text().isEmpty() == false ||
         e->key() == Qt::Key_Backspace ||
         e->key() == Qt::Key_Delete ||
         e->key() == Qt::Key_Enter ||
         e->key() == Qt::Key_Return) || 
        (e->key() == Qt::Key_Backspace && textCursor().position() == textLockPosition_))
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

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::contextMenuEvent()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu;
    menu.addAction(&copyAction_);
    QAction* paste = menu.addAction(QStringLiteral("Paste"), this, SLOT(paste()), QKeySequence::Paste);
    menu.addAction(QStringLiteral("Select all"), this, SLOT(selectAll()), QKeySequence::SelectAll);
    menu.addAction(QStringLiteral("Clear"), this, SLOT(onClear()));

    paste->setEnabled(canPaste());

    menu.exec(event->globalPos());
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::canPaste()
//-----------------------------------------------------------------------------
bool ScriptingTextEditor::canPaste() const
{
    QTextCursor cursor = textCursor();    
    return (cursor.selectionStart() >= textLockPosition_ && cursor.selectionEnd() >= textLockPosition_);
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::onClear()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::onClear()
{
    clear();
    print(promptText_);
}
