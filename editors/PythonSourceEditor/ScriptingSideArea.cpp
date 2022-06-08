//-----------------------------------------------------------------------------
// File: ScriptingConsole.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// Widget for showing info besides scripting text editor.
//-----------------------------------------------------------------------------

#include "ScriptingSideArea.h"

#include "ScriptingTextEditor.h"

//-----------------------------------------------------------------------------
// Function: ScriptingSideArea::ScriptingSideArea()
//-----------------------------------------------------------------------------
ScriptingSideArea::ScriptingSideArea(ScriptingTextEditor *editor) : QWidget(editor), editor_(editor)
{

}

//-----------------------------------------------------------------------------
// Function: ScriptingSideArea::sizeHint()
//-----------------------------------------------------------------------------
QSize ScriptingSideArea::sizeHint() const
{
    return QSize(editor_->sideAreaWidth(), 0);
}

//-----------------------------------------------------------------------------
// Function: ScriptingSideArea::paintEvent()
//-----------------------------------------------------------------------------
void ScriptingSideArea::paintEvent(QPaintEvent *event)
{
    editor_->sideAreaPaintEvent(event);
    event->accept();
}
