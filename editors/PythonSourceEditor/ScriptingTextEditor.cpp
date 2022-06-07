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
    promtSideArea_(new ScriptingSideArea(this))
{
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
    setFont(settings.value("Font").value<QFont>());
    settings.endGroup();

    setViewportMargins(sideAreaWidth(), 0, 0, 0);
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
    return 0;
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
