//-----------------------------------------------------------------------------
// File: SystemDeleteCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.10.2011
//
// Description:
// Undo delete commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDELETECOMMANDS_H
#define SYSTEMDELETECOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QObject>

class GraphicsConnection;
class GraphicsColumnLayout;
class SystemColumn;
class ComponentItem;
class IGraphicsItemStack;
class SWPortItem;
class SWInterfaceItem;
class SystemComponentItem;
class ApiInterface;
class ComInterface;

//-----------------------------------------------------------------------------
//! SystemColumnDeleteCommand class.
//-----------------------------------------------------------------------------
class SystemColumnDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout  The column layout.
     *      @param [in] column  The column to delete.
     *      @param [in] parent  The owner of this command.
     */
    SystemColumnDeleteCommand(GraphicsColumnLayout* layout, SystemColumn* column, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemColumnDeleteCommand();

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
    SystemColumnDeleteCommand(SystemColumnDeleteCommand const& rhs);
    SystemColumnDeleteCommand& operator=(SystemColumnDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    GraphicsColumnLayout* layout_;

    //! The diagram column.
    SystemColumn* column_;

    //! Boolean flag for indicating if the column should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SWConnectionDeleteCommand class.
//-----------------------------------------------------------------------------
class SWConnectionDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] conn    The connection to delete.
     *      @param [in] parent  The parent undo command.
     */
    SWConnectionDeleteCommand(GraphicsConnection* conn, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWConnectionDeleteCommand();

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
    SWConnectionDeleteCommand(SWConnectionDeleteCommand const& rhs);
    SWConnectionDeleteCommand& operator=(SWConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    GraphicsConnection* conn_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SWPortDeleteCommand class.
//-----------------------------------------------------------------------------
class SWPortDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] port    The port to delete.
     *      @param [in] parent  The owner of this command.
     */
    SWPortDeleteCommand(SWPortItem* port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWPortDeleteCommand();

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
    SWPortDeleteCommand(SWPortDeleteCommand const& rhs);
    SWPortDeleteCommand& operator=(SWPortDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram port.
    SWPortItem* port_;

    //! The port's parent.
    SystemComponentItem* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SWInterfaceDeleteCommand class.
//-----------------------------------------------------------------------------
class SWInterfaceDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] interface   The interface to delete.
     *      @param [in] parent      The owner of this command.
     */
    SWInterfaceDeleteCommand(SWInterfaceItem* interface, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWInterfaceDeleteCommand();

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
    SWInterfaceDeleteCommand(SWInterfaceDeleteCommand const& rhs);
    SWInterfaceDeleteCommand& operator=(SWInterfaceDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interface item.
    SWInterfaceItem* interface_;

    //! The API interface (if API).
    QSharedPointer<ApiInterface> apiInterface_;

    //! The COM interface (if COM).
    QSharedPointer<ComInterface> comInterface_;

    //! The parent stack.
    IGraphicsItemStack* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDELETECOMMANDS_H
