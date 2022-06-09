//-----------------------------------------------------------------------------
// File: ScriptInputEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write and run.
//-----------------------------------------------------------------------------

#include "ScriptInputEditor.h"

#include <QPainter>
#include <QTextCursor>
#include <QTextBlock>
#include <QSettings>

#include <common/KactusColors.h>

#include <PythonAPI/WriteChannel.h>

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::ScriptInputEditor()
//-----------------------------------------------------------------------------
ScriptInputEditor::ScriptInputEditor(QWidget* parent):
    ScriptingTextEditor(parent)
{
    setWordWrapMode(QTextOption::NoWrap);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSideAreaWidth(int)), Qt::UniqueConnection);
    connect(this, SIGNAL(cursorPositionChanged()), viewport(), SLOT(update()), Qt::UniqueConnection);

    setViewportMargins(sideAreaWidth(), 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Tab && useTabs_ == false)
    {
        insertPlainText(QString(indentWidth_, QChar(' ')));
        return;
    }

    QPlainTextEdit::keyPressEvent(e);
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::paintEvent(QPaintEvent *e)
{
    ScriptingTextEditor::paintEvent(e);

    highlightSelectedLines();
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::getSelectedLines()
//-----------------------------------------------------------------------------
QString ScriptInputEditor::getSelectedLines() const
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection() == false)
    {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }
    else
    {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();

        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::StartOfLine);

        cursor.setPosition(end, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }

    return cursor.selectedText().replace(QChar(QChar::ParagraphSeparator), QChar('\n'));
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::setIndentStyle()
//-----------------------------------------------------------------------------
void ScriptInputEditor::setIndentStyle(bool useTabs, unsigned int width)
{
    useTabs_ = useTabs;
    indentWidth_ = width;
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::lineNumberAreaWidth()
//-----------------------------------------------------------------------------
int ScriptInputEditor::sideAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 2 + fontMetrics().width(QLatin1Char('9')) * (digits + 1) + 4;

    return space;
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::sideAreaPaintEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::sideAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(editorSideArea_);
    painter.setPen(Qt::darkGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            painter.drawText(0, top, editorSideArea_->width()-4, fontMetrics().height(),
                Qt::AlignRight, QString::number(blockNumber + 1));
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::updateSideAreaWidth()
//-----------------------------------------------------------------------------
void ScriptInputEditor::updateSideAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(sideAreaWidth(), 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::highlightSelectedLines()
//-----------------------------------------------------------------------------
void ScriptInputEditor::highlightSelectedLines() const
{
    QPainter painter(viewport());
    painter.setPen(QColor(Qt::lightGray).lighter(120));

    QTextCursor cursor = textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    cursor.setPosition(start);
    auto startBlock = cursor.block();

    if (startBlock.isVisible())
    {
        auto startRect = blockBoundingGeometry(startBlock).translated(contentOffset());
        painter.drawLine(startRect.topLeft(), startRect.topRight());
    }

    cursor.setPosition(end);
    auto endBlock = cursor.block();

    if (endBlock.isVisible())
    {
        auto endRect = blockBoundingGeometry(endBlock).translated(contentOffset());
        painter.drawLine(endRect.bottomLeft(), endRect.bottomRight());
    }
}
