//-----------------------------------------------------------------------------
// File: GraphicsColumnUndoCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2012
//
// Description:
// Undo commands for graphics columns.
//-----------------------------------------------------------------------------

#include "GraphicsColumnUndoCommands.h"

#include "GraphicsColumn.h"
#include "GraphicsColumnLayout.h"

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: GraphicsColumnMoveCommand::GraphicsColumnMoveCommand()
//-----------------------------------------------------------------------------
GraphicsColumnMoveCommand::GraphicsColumnMoveCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
    QPointF const& oldPos, QUndoCommand* parent):
QUndoCommand(parent),
    layout_(layout),
    column_(column),
    oldPos_(oldPos),
    newPos_(column->pos())
{
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnMoveCommand::~GraphicsColumnMoveCommand()
//-----------------------------------------------------------------------------
GraphicsColumnMoveCommand::~GraphicsColumnMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnMoveCommand::undo()
//-----------------------------------------------------------------------------
void GraphicsColumnMoveCommand::undo()
{
    column_->setPos(oldPos_);
    layout_->onMoveColumn(column_);
    layout_->onReleaseColumn(column_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnMoveCommand::redo()
//-----------------------------------------------------------------------------
void GraphicsColumnMoveCommand::redo()
{
    column_->setPos(newPos_);
    layout_->onMoveColumn(column_);
    layout_->onReleaseColumn(column_);

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnAddCommand()
//-----------------------------------------------------------------------------
GraphicsColumnAddCommand::GraphicsColumnAddCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
    QSharedPointer<Design> design, QUndoCommand* parent)
    : QUndoCommand(parent),
      layout_(layout),
      column_(column),
      del_(false),
      design_(design)
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

    design_->removeColumn(column_->getColumnDesc());
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void GraphicsColumnAddCommand::redo()
{
    // Add the column to the layout.
    layout_->addColumn(column_, column_->pos().isNull());
    del_ = false;
    design_->addColumn(column_->getColumnDesc());

    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnChangeCommand()
//-----------------------------------------------------------------------------
GraphicsColumnChangeCommand::GraphicsColumnChangeCommand(GraphicsColumn* column,
                                                         QSharedPointer<ColumnDesc> newDesc,
                                                         QSharedPointer<Design> containingDesign,
                                                         QUndoCommand* parent):
QUndoCommand(parent),
columnItem_(column),
columnDesc_(column->getColumnDesc()),
oldDesc_(new ColumnDesc(*column->getColumnDesc().data())),
newDesc_(newDesc),
containingDesign_(containingDesign)
{
}

//-----------------------------------------------------------------------------
// Function: ~GraphicsColumnChangeCommand()
//-----------------------------------------------------------------------------
GraphicsColumnChangeCommand::~GraphicsColumnChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void GraphicsColumnChangeCommand::undo()
{
    changeModifiedColumnInDesign(oldDesc_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void GraphicsColumnChangeCommand::redo()
{
    changeModifiedColumnInDesign(newDesc_);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnUndoCommands::changeModifiedColumnInDesign()
//-----------------------------------------------------------------------------
void GraphicsColumnChangeCommand::changeModifiedColumnInDesign(QSharedPointer<ColumnDesc> newColumn)
{
    columnDesc_->setAllowedItems(newColumn->getAllowedItems());
    columnDesc_->setContentType(newColumn->getContentType());
    columnDesc_->setName(newColumn->name());
    //columnDesc_->setPosition(newColumn->getPosition());
    columnDesc_->setWidth(newColumn->getWidth());

    columnItem_->setColumnDesc(columnDesc_);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnResizeCommand::GraphicsColumnResizeCommand()
//-----------------------------------------------------------------------------
GraphicsColumnResizeCommand::GraphicsColumnResizeCommand(GraphicsColumn* column, unsigned int oldWidth,
                                                         QUndoCommand* parent)
    : QUndoCommand(parent),
      column_(column),
      oldWidth_(oldWidth),
      newWidth_(column_->getColumnDesc()->getWidth())
{
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnResizeCommand::~GraphicsColumnResizeCommand()
//-----------------------------------------------------------------------------
GraphicsColumnResizeCommand::~GraphicsColumnResizeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnResizeCommand::undo()
//-----------------------------------------------------------------------------
void GraphicsColumnResizeCommand::undo()
{
    column_->setWidth(oldWidth_);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnResizeCommand::redo()
//-----------------------------------------------------------------------------
void GraphicsColumnResizeCommand::redo()
{
    column_->setWidth(newWidth_);
}
