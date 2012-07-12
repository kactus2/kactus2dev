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
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class GraphicsConnection;
class GraphicsColumnLayout;
class SystemColumn;
class ComponentItem;
class ApplicationItem;
class ProgramEntityItem;
class IGraphicsItemStack;
class SWPortItem;
class SWInterfaceItem;
class SystemComponentItem;

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
     */
    SystemColumnDeleteCommand(GraphicsColumnLayout* layout, SystemColumn* column,
                              QUndoCommand* parent = 0);

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
//! SystemComponentDeleteCommand class.
//-----------------------------------------------------------------------------
class SystemComponentDeleteCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item The component item to delete.
     */
    SystemComponentDeleteCommand(ComponentItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemComponentDeleteCommand();

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
	void componentInstantiated(ComponentItem* comp);

	//! \brief Emitted when a component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem* comp);

private:
    // Disable copying.
    SystemComponentDeleteCommand(SystemComponentDeleteCommand const& rhs);
    SystemComponentDeleteCommand& operator=(SystemComponentDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item.
    ComponentItem* item_;

    //! The component's parent column.
    IGraphicsItemStack* stack_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;

    //! If true, the command has not been run previously.
    bool firstRun_;
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
     *      @param [in] port The port to delete.
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
     *      @param [in] interface    The interface to delete.
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

    //! The parent stack.
    IGraphicsItemStack* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDELETECOMMANDS_H
