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

#include <PythonAPI/WriteChannel.h>

//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::ScriptInputEditor()
//-----------------------------------------------------------------------------
ScriptInputEditor::ScriptInputEditor(QWidget* parent):
    ScriptingTextEditor(parent)
{       
    ScriptInputEditor::applySettings();
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
// Function: ScriptInputEditor::lineNumberAreaWidth()
//-----------------------------------------------------------------------------
int ScriptInputEditor::sideAreaWidth() const
{
    int space =  fontMetrics().width("0") * 5;
    return space;
}


//-----------------------------------------------------------------------------
// Function: ScriptInputEditor::sideAreaPaintEvent()
//-----------------------------------------------------------------------------
void ScriptInputEditor::sideAreaPaintEvent()
{
    QPainter painter(promtSideArea_);

  
    QTextCursor cursor = textCursor();

    QTextBlock lastBlock = cursor.block();

    int top = qRound(blockBoundingGeometry(lastBlock).translated(contentOffset()).top());


        painter.setPen(Qt::black);
        painter.setFont(font());
        painter.drawText(4, top, promtSideArea_->width()-4, fontMetrics().height(),
            Qt::AlignLeft, QString());
    
}
