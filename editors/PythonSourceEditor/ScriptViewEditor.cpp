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
    ScriptingTextEditor(parent),
    promptText_()
{       
    setReadOnly(true);
    
    ScriptingTextEditor::applySettings();

    connect(this, SIGNAL(updateRequest(QRect const&, int)),
        this, SLOT(updateSideArea(QRect const&, int)), Qt::UniqueConnection);
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
// Function: ScriptViewEditor::printError()
//-----------------------------------------------------------------------------
void ScriptViewEditor::printError(QString const& input)
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
// Function: ScriptViewEditor::onClear()
//-----------------------------------------------------------------------------
void ScriptViewEditor::onClear()
{
    clear();
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
void ScriptViewEditor::sideAreaPaintEvent()
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
