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

class SystemColumnLayout;
class SystemColumn;
class ComponentItem;
class IComponentStack;
class SWConnection;
class SWComponentItem;
class SWPortItem;

//-----------------------------------------------------------------------------
//! SystemColumnAddCommand class.
//-----------------------------------------------------------------------------
class SystemColumnAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout        The column layout.
     *      @param [in] name          The name of the column.
     *      @param [in] parent        The parent command.
     */
    SystemColumnAddCommand(SystemColumnLayout* layout, QString const& name,
                           QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemColumnAddCommand();

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
    SystemColumnAddCommand(SystemColumnAddCommand const& rhs);
    SystemColumnAddCommand& operator=(SystemColumnAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    SystemColumnLayout* layout_;

    //! The name of the column.
    QString name_;

    //! Created diagram column.
    SystemColumn* column_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SWConnectionAddCommand class.
//-----------------------------------------------------------------------------
class SWConnectionAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] scene  The scene.
     *      @param [in] conn   The connection to add.
     *      @param [in] parent The parent command.
     */
    SWConnectionAddCommand(QGraphicsScene* scene, SWConnection* conn,
                           QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWConnectionAddCommand();

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
    SWConnectionAddCommand(SWConnectionAddCommand const& rhs);
    SWConnectionAddCommand& operator=(SWConnectionAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The SW connection.
    SWConnection* conn_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! SystemItemAddCommand class.
//-----------------------------------------------------------------------------
class SystemItemAddCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] stack   The component stack to which to add the componetn.
     *      @param [in] item    The mapping component item to add.
     *      @param [in] parent  The parent command.
     */
    SystemItemAddCommand(IComponentStack* stack, ComponentItem* item, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemItemAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

signals:

	//! Signaled when the component is instantiated to the design.
	void componentInstantiated(ComponentItem* comp);

	//! Signaled when the component instance is removed from the design.
	void componentInstanceRemoved(ComponentItem* comp);

private:
    // Disable copying.
    SystemItemAddCommand(SystemItemAddCommand const& rhs);
    SystemItemAddCommand& operator=(SystemItemAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item.
    ComponentItem* item_;

    //! The item's parent component stack.
    IComponentStack* stack_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

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
    SWPortAddCommand(SWComponentItem* component, QPointF const& pos, QUndoCommand* parent = 0);

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
    SWComponentItem* component_;

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

#endif // SYSTEMADDCOMMANDS_H
