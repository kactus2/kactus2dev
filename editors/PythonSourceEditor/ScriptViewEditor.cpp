//-----------------------------------------------------------------------------
// File: ScriptViewEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// Text editor for script write and run.
//-----------------------------------------------------------------------------

#include "ScriptViewEditor.h"

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
// Function: ScriptViewEditor::ScriptViewEditor()
//-----------------------------------------------------------------------------
ScriptViewEditor::ScriptViewEditor(QWidget* parent):
    ScriptingTextEditor(parent)
{       
    setReadOnly(true);

    setViewportMargins(ScriptViewEditor::sideAreaWidth(), 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: ScriptViewEditor::printInput()
//-----------------------------------------------------------------------------
void ScriptViewEditor::printInput(QString const& input)
{
    printWithColor(input, QColor(Qt::blue));

    if (input.endsWith(QChar('\n')) == false)
    {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptViewEditor::print()
//-----------------------------------------------------------------------------
void ScriptViewEditor::print(QString const& input)
{
    static QStringList prompts({ ">>> ", "... " });

    if (prompts.contains(input))
    {
        promptText_ = input;
        return;
    }

    printWithColor(input, QColor(Qt::black));
}

//-----------------------------------------------------------------------------
// Function: ScriptViewEditor::printError()
//-----------------------------------------------------------------------------
void ScriptViewEditor::printError(QString const& input)
{
    printWithColor(input, QColor(Qt::red));
}

//-----------------------------------------------------------------------------
// Function: ScriptViewEditor::lineNumberAreaWidth()
//-----------------------------------------------------------------------------
int ScriptViewEditor::sideAreaWidth() const
{
    int space =  fontMetrics().width(">") * 5;
    return space;
}

//-----------------------------------------------------------------------------
// Function: ScriptViewEditor::sideAreaPaintEvent()
//-----------------------------------------------------------------------------
void ScriptViewEditor::sideAreaPaintEvent(QPaintEvent* /* event */)
{
    QPainter painter(editorSideArea_);

    QTextCursor cursor = textCursor();

    cursor.movePosition(QTextCursor::End);
    QTextBlock lastBlock = cursor.block();

    int top = qRound(blockBoundingGeometry(lastBlock).translated(contentOffset()).top());

    if (lastBlock.isVisible())
    {
        painter.setPen(Qt::black);
        painter.setFont(font());
        painter.drawText(4, top, editorSideArea_->width()-4, fontMetrics().height(),
            Qt::AlignLeft, promptText_);
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptViewEditor::printWithColor()
//-----------------------------------------------------------------------------
void ScriptViewEditor::printWithColor(QString const& input, QColor const& textColor)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);

    QTextBlock block = cursor.block();

    QTextCharFormat format = block.charFormat();
    format.setForeground(QBrush(textColor));

    cursor.insertText(input, format);

    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}
