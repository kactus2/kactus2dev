//-----------------------------------------------------------------------------
// File: SystemAddCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.10.2011
//
// Description:
// Undo add commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMADDCOMMANDS_H
#define SYSTEMADDCOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class ComponentItem;
class IGraphicsItemStack;
class SystemComponentItem;
class SWPortItem;
class SWInterfaceItem;
class ApiInterface;
class ComInterface;

//-----------------------------------------------------------------------------
//! SWPortAddCommand class.
//-----------------------------------------------------------------------------
class SWPortAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component to which to add a port.
     *      @param [in] pos        The position where to add the port.
     *      @param [in] parent     The parent command.
     */
    SWPortAddCommand(SystemComponentItem* component, QPointF const& pos, QUndoCommand* parent = 0);

    /*!
     *  Constructor.
     *
     *      @param [in] component  The component to which to add a port.
     *      @param [in] port       The port to add.
     *      @param [in] parent     The parent command.
     */
    SWPortAddCommand(SystemComponentItem* component, SWPortItem* port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWPortAddCommand();

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
    SWPortAddCommand(SWPortAddCommand const& rhs);
    SWPortAddCommand& operator=(SWPortAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent component.
    SystemComponentItem* component_;

    //! The port position.
    QPointF pos_;

    //! The diagram port.
    SWPortItem* port_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SWInterfacePasteCommand class.
//-----------------------------------------------------------------------------
class SWInterfacePasteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] stack   The graphics item stack to which to paste the item.
     *      @param [in] item    The SW interface item to paste.
     *      @param [in] parent  The parent command.
     */
    SWInterfacePasteCommand(IGraphicsItemStack* stack, SWInterfaceItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWInterfacePasteCommand();

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
    SWInterfacePasteCommand(SWInterfacePasteCommand const& rhs);
    SWInterfacePasteCommand& operator=(SWInterfacePasteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The SW interface item.
    SWInterfaceItem* interface_;

    //! The API interface (if API).
    QSharedPointer<ApiInterface> apiInterface_;

    //! The COM interface (if COM).
    QSharedPointer<ComInterface> comInterface_;

    //! The item's parent component stack.
    IGraphicsItemStack* stack_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMADDCOMMANDS_H
