//-----------------------------------------------------------------------------
// File: ScriptingTextEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write and run.
//-----------------------------------------------------------------------------

#include "ScriptingTextEditor.h"

#include "ScriptingHistory.h"

#include <QTextBlock>
#include <QScrollBar>
#include <QMenu>
#include <QKeySequence>
#include <QPainter>
#include <QSettings>
#include <QVariant>
#include <QTextStream>

#include <PythonAPI/WriteChannel.h>

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::ScriptingTextEditor()
//-----------------------------------------------------------------------------
ScriptingTextEditor::ScriptingTextEditor(ScriptingHistory* history, QWidget* parent):
    QPlainTextEdit(parent),
    promtSideArea_(new ScriptingSideArea(this)),
    history_(history),
    textLockPosition_(0), 
    promptText_(),
    copyAction_(tr("Copy"), this),
    useTabs_(false)
{    
    setContentsMargins(0, 0, 0, 0);
   
    setAcceptDrops(false);
    setUndoRedoEnabled(false);
    setWordWrapMode(QTextOption::NoWrap);

    applySettings();

    copyAction_.setDisabled(true);

    connect(&copyAction_, SIGNAL(triggered()), 
        this, SLOT(copy()), Qt::UniqueConnection);
    connect(this, SIGNAL(copyAvailable(bool)), 
        &copyAction_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    connect(this, SIGNAL(updateRequest(QRect const&, int)),
        this, SLOT(updateSideArea(QRect const&, int)), Qt::UniqueConnection);

}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::applySettings()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::applySettings()
{
    QSettings settings;

    settings.beginGroup("Editor");
    useTabs_ = settings.value("IndentStyle", true).toBool() == false;
    setFont(settings.value("Font").value<QFont>());
    settings.endGroup();

    setViewportMargins(sideAreaWidth(), 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::insertInput()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::insertInput(QString const& input)
{
    QTextCursor lastCursor = textCursor();
    lastCursor.movePosition(QTextCursor::StartOfBlock);
    lastCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    lastCursor.insertText(input);
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::print()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::print(QString const& input)
{
    static QStringList prompts({ ">>> ", "... " });

    if (prompts.contains(input))
    {
        promptText_ = input;
        return;
    }

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextBlock block = cursor.block();
    
    QTextCharFormat format = block.charFormat();
    format.setForeground(QBrush(Qt::black));

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
    QTextCursor cursor = textCursor();

    // Do not allow removing locked section text.
    if (e->key() == Qt::Key_Backspace && cursor.position() <= textLockPosition_ && !cursor.hasSelection())
    {
        return;
    }

    // Do not allow editing in the locked section, so move to end of the input.
    if ((cursor.position() < textLockPosition_) &&
        ((e->text().isEmpty() == false && !(e->modifiers() | Qt::ControlModifier))||
         e->key() == Qt::Key_Backspace ||
         e->key() == Qt::Key_Delete ||
         e->key() == Qt::Key_Enter ||
         e->key() == Qt::Key_Return))
    {
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
    }

    if (e->key() == Qt::Key_Tab && useTabs_ == false)
    {
        insertPlainText("    ");
        return;
    }

    if (textCursor().block().blockNumber() == blockCount() - 1 && e->key() == Qt::Key_Up)
    {
        insertInput(history_->previous());
        return;
    }
    else if (textCursor().block().blockNumber() == blockCount() - 1 && e->key() == Qt::Key_Down)
    {
        insertInput(history_->next());
        return;
    }

    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        cursor.setPosition(textLockPosition_);
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        QString command = cursor.selectedText();

        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
        QPlainTextEdit::keyPressEvent(e);

        textLockPosition_ = textCursor().position();

        emit write(command);

        history_->push(command);
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
    textLockPosition_ = 0;
    print(promptText_);
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::lineNumberAreaWidth()
//-----------------------------------------------------------------------------
int ScriptingTextEditor::sideAreaWidth() const
{
    int space =  fontMetrics().width(">") * 5;
    return space;
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::updateSideArea()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::updateSideArea(QRect const& rect, int dy)
{
    if (dy)
    {
        promtSideArea_->scroll(0, dy);
    }
    else
    {
        promtSideArea_->update(0, rect.y(), promtSideArea_->width(), rect.height());
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::resizeEvent()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    promtSideArea_->setGeometry(QRect(cr.left(), cr.top(), sideAreaWidth(), cr.height()));
}

//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::sideAreaPaintEvent()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::sideAreaPaintEvent()
{
    QPainter painter(promtSideArea_);

    QTextCursor cursor = textCursor();
    cursor.setPosition(textLockPosition_);

    QTextBlock lastBlock = cursor.block();

    int top = qRound(blockBoundingGeometry(lastBlock).translated(contentOffset()).top());

    if (lastBlock.isVisible())
    {
        painter.setPen(Qt::black);
        painter.setFont(font());
        painter.drawText(4, top, promtSideArea_->width()-4, fontMetrics().height(),
            Qt::AlignLeft, promptText_);
    }
}
