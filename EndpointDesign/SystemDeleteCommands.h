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

class SystemColumnLayout;
class SystemColumn;
class ComponentItem;
class ApplicationItem;
class ProgramEntityItem;
class IComponentStack;
class SWConnection;

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
    SystemColumnDeleteCommand(SystemColumnLayout* layout, SystemColumn* column,
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
    SystemColumnLayout* layout_;

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
    SWConnectionDeleteCommand(SWConnection* conn, QUndoCommand* parent = 0);

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
    SWConnection* conn_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SystemItemDeleteCommand class.
//-----------------------------------------------------------------------------
class SystemItemDeleteCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] item The component item to delete.
     */
    SystemItemDeleteCommand(ComponentItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemItemDeleteCommand();

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
    SystemItemDeleteCommand(SystemItemDeleteCommand const& rhs);
    SystemItemDeleteCommand& operator=(SystemItemDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item.
    ComponentItem* item_;

    //! The component's parent column.
    IComponentStack* stack_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDELETECOMMANDS_H
