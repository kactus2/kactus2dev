//-----------------------------------------------------------------------------
// File: ComponentItemMoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 17.09.2015
//
// Description:
// Undo command for moving component instances in a design.
//-----------------------------------------------------------------------------

#ifndef COMPONENT_ITEM_MOVE_COMMAND_H
#define COMPONENT_ITEM_MOVE_COMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

class IGraphicsItemStack;
class ComponentItem;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! Undo command for moving component instances in a design.
//-----------------------------------------------------------------------------
class ComponentItemMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] item        The component item to move.
     *      @param [in] oldPos      The item's old position.
     *      @param [in] oldStack    The item's old stack.
     *      @param [in] diagram     Diagram containing the scene of the component item.
     *      @param [in] parent      The parent command.
     */
    ComponentItemMoveCommand(ComponentItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
        DesignDiagram* diagram, QUndoCommand* parent = 0);

    //! The destructor.
    ~ComponentItemMoveCommand();

    //! Undoes the move.
    virtual void undo();

    //! Redoes the move.
    virtual void redo();

private:
    // Disable copying.
    ComponentItemMoveCommand(ComponentItemMoveCommand const& rhs);
    ComponentItemMoveCommand& operator=(ComponentItemMoveCommand const& rhs);

    /*!
     *  Reset the scene area.
     */
    void resetSceneRectangle();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item representing the instance.
    ComponentItem* item_;

    //! The old position of the component item.
    QPointF oldPos_;

    //! The old parent stack.
    IGraphicsItemStack* oldStack_;

    //! The new position of the item.
    QPointF newPos_;

    //! The new parent stack.
    IGraphicsItemStack* newStack_;

    //! Diagram containing the scene of the component item.
    DesignDiagram* diagram_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENT_ITEM_MOVE_COMMAND_H