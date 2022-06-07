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
ScriptingTextEditor::ScriptingTextEditor(QWidget* parent):
    QPlainTextEdit(parent),
    promtSideArea_(new ScriptingSideArea(this)),
    promptText_(),
    useTabs_(false)
{       
    applySettings();

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

    if (e->key() == Qt::Key_Tab && useTabs_ == false)
    {
        insertPlainText("    ");
        return;
    }

    if (e->modifiers() == Qt::ShiftModifier &&
        (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return))
    {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        QString command = cursor.selectedText();

        emit write(command);
    }
    else
    {
        QPlainTextEdit::keyPressEvent(e);
    }
}


//-----------------------------------------------------------------------------
// Function: ScriptingTextEditor::onClear()
//-----------------------------------------------------------------------------
void ScriptingTextEditor::onClear()
{
    clear();
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
