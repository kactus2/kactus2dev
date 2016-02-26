//-----------------------------------------------------------------------------
// File: SWInterfaceMoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.02.2016
//
// Description:
// Undo move command for SW interface.
//-----------------------------------------------------------------------------

#ifndef SWINTERFACEMOVECOMMAND_H
#define SWINTERFACEMOVECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QString>

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

class SWInterfaceItem;

//-----------------------------------------------------------------------------
//! Undo move command for SW interface.
//-----------------------------------------------------------------------------
class SWInterfaceMoveCommand : public ItemMoveCommand
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] item      The item to move.
     *      @param [in] oldPos    The item's old position.
     *      @param [in] oldStack  The item's old stack.
     *      @param [in] parent    The parent command.
     */
    SWInterfaceMoveCommand(SWInterfaceItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
        QUndoCommand* parent = 0);

    /*!
     *  Constructor.
     *
     *      @param [in] item      The item to move.
     *      @param [in] oldPos    The item's old position.
     *      @param [in] oldStack  The item's old stack.
     *      @param [in] oldPos    The item's new position.
     *      @param [in] oldStack  The item's new stack.
     *      @param [in] parent    The parent command.
     */
    SWInterfaceMoveCommand(SWInterfaceItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
                    QPointF const& newPos, IGraphicsItemStack* newStack, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWInterfaceMoveCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    SWInterfaceMoveCommand(SWInterfaceMoveCommand const& rhs);
    SWInterfaceMoveCommand& operator=(SWInterfaceMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The item being moved.
    SWInterfaceItem* interfaceItem_;

    //! The old position of the item.
    QPointF oldPosition_;

    //! The new position of the item.
    QPointF newPosition_;
};

//-----------------------------------------------------------------------------

#endif // SWINTERFACEMOVECOMMAND_H
