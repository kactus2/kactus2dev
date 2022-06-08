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
    ScriptInputEditor::applySettings();

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSideAreaWidth(int)), Qt::UniqueConnection);
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::ScriptInputEditor()
//-----------------------------------------------------------------------------
void ScriptInputEditor::applySettings()
{
    QSettings settings;

    settings.beginGroup("Editor");
    useTabs_ = settings.value("IndentStyle", true).toBool() == false;
    settings.endGroup();

    ScriptingTextEditor::applySettings();
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::keyPressEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Tab && useTabs_ == false)
    {
        insertPlainText("    ");
        return;
    }

    QPlainTextEdit::keyPressEvent(e);
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

    int space = 2 + fontMetrics().width(QLatin1Char('9')) * digits + 4;

    return space;
}

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::sideAreaPaintEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::sideAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(editorSideArea_);
    painter.setPen(Qt::black);

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
// Function: ScriptInputEditor::updateSideAreaWidth()
//-----------------------------------------------------------------------------
void ScriptInputEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(KactusColors::SW_COMPONENT).lighter(120);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    setExtraSelections(extraSelections);
}
