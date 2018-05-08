//-----------------------------------------------------------------------------
// File: GraphicsColumnAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.10.2017
//
// Description:
// Undo command for adding graphics columns.
//-----------------------------------------------------------------------------

#include "GraphicsColumnAddCommand.h"

#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <editors/common/DesignDiagram.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: GraphicsColumnAddCommand()
//-----------------------------------------------------------------------------
GraphicsColumnAddCommand::GraphicsColumnAddCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
    DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
layout_(layout),
column_(column),
diagram_(diagram),
del_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ~GraphicsColumnAddCommand()
//-----------------------------------------------------------------------------
GraphicsColumnAddCommand::~GraphicsColumnAddCommand()
{
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void GraphicsColumnAddCommand::undo()
{
    QUndoCommand::undo();

    Q_ASSERT(column_ != 0);

    // Remove the column from the layout.
    layout_->removeColumn(column_);
    del_ = true;

    diagram_->getDesign()->removeColumn(column_->getColumnDesc());
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void GraphicsColumnAddCommand::redo()
{
    // Add the column to the layout.
    layout_->appendColumn(column_);
    del_ = false;
    diagram_->getDesign()->addColumn(column_->getColumnDesc());

    QUndoCommand::redo();

    diagram_->resetSceneRectangleForItems();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnAddCommand::getColumn()
//-----------------------------------------------------------------------------
GraphicsColumn* GraphicsColumnAddCommand::getColumn() const
{
    return column_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnAddCommand::getLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout* GraphicsColumnAddCommand::getLayout() const
{
    return layout_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnAddCommand::getDiagram()
//-----------------------------------------------------------------------------
DesignDiagram* GraphicsColumnAddCommand::getDiagram() const
{
    return diagram_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnAddCommand::columnShouldBeDeleted()
//-----------------------------------------------------------------------------
bool GraphicsColumnAddCommand::columnShouldBeDeleted() const
{
    return del_;
}
