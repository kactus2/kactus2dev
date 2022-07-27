//-----------------------------------------------------------------------------
// File: ScriptInputEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write.
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
    QPlainTextEdit(parent),
    editorSideArea_(new ScriptingSideArea(this))
{
    setWordWrapMode(QTextOption::NoWrap);

    connect(this, SIGNAL(updateRequest(QRect const&, int)),
        this, SLOT(updateSideArea(QRect const&, int)), Qt::UniqueConnection);
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
    QPlainTextEdit::paintEvent(e);

    highlightSelectedLines();
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::resizeEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    editorSideArea_->setGeometry(QRect(cr.left(), cr.top(), sideAreaWidth(), cr.height()));
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

    int space = 2 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * (digits + 1) + 4;

    return space;
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::sideAreaPaintEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::sideAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(editorSideArea_);

    //! Draw background.
    QColor backgroundColor = QColor(Qt::lightGray).lighter(120);
    painter.setPen(backgroundColor);
    painter.setBrush(backgroundColor);
    painter.drawRect(event->rect());

    //! Draw line numbers.
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int cursorBlock = textCursor().blockNumber();

    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            if (blockNumber == cursorBlock)
            {
                painter.setPen(Qt::black);
            }
            else
            {
                painter.setPen(Qt::darkGray);
            }

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
// Function: ScriptInputEditor::updateSideArea()
//-----------------------------------------------------------------------------
void ScriptInputEditor::updateSideArea(QRect const& rect, int dy)
{
    if (dy)
    {
        editorSideArea_->scroll(0, dy);
    }
    else
    {
        editorSideArea_->update(0, rect.y(), editorSideArea_->width(), rect.height());
    }
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
