//-----------------------------------------------------------------------------
// File: CommonGraphicsUndoCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.7.2012
//
// Description:
// Common graphics item undo commands.
//-----------------------------------------------------------------------------

#ifndef COMMONGRAPHICSUNDOCOMMANDS_H
#define COMMONGRAPHICSUNDOCOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

class IGraphicsItemStack;
class ComponentItem;

//-----------------------------------------------------------------------------
//! ItemAddCommand class.
//-----------------------------------------------------------------------------
class ItemAddCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] stack   The stack to which to add the item.
     *      @param [in] item    The item to add.
     *      @param [in] parent  The parent command.
     */
    ItemAddCommand(IGraphicsItemStack* stack, QGraphicsItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ItemAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:

	//! \brief Emitted when a new component is instantiated to the design.
	void componentInstantiated(ComponentItem*);

	//! \brief Emitted when a component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem*);

private:
    // Disable copying.
    ItemAddCommand(ItemAddCommand const& rhs);
    ItemAddCommand& operator=(ItemAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The item's parent column.
    IGraphicsItemStack* stack_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ItemMoveCommand class.
//-----------------------------------------------------------------------------
class ItemMoveCommand : public QUndoCommand
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
    ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos,
                    IGraphicsItemStack* oldStack, QUndoCommand* parent = 0);

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
    ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
                    QPointF const& newPos, IGraphicsItemStack* newStack, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ItemMoveCommand();

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
    ItemMoveCommand(ItemMoveCommand const& rhs);
    ItemMoveCommand& operator=(ItemMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The old position of the item.
    QPointF oldPos_;

    //! The old parent stack.
    IGraphicsItemStack* oldStack_;

    //! The new position of the item.
    QPointF newPos_;

    //! The new parent stack.
    IGraphicsItemStack* newStack_;
};

//-----------------------------------------------------------------------------

#endif // COMMONGRAPHICSUNDOCOMMANDS_H