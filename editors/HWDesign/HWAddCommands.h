//-----------------------------------------------------------------------------
// File: HWAddCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 3.8.2011
//
// Description:
// Undo add commands for the design diagram.
//-----------------------------------------------------------------------------

#ifndef HWADDCOMMANDS_H
#define HWADDCOMMANDS_H

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

class ActiveBusInterfaceItem;
class HierarchicalBusInterfaceItem;
class BusInterface;
class Component;
class ComponentItem;
class Design;
class DesignDiagram;
class GraphicsColumn;
class GraphicsColumnLayout;
class HWConnection;
class HWComponentItem;
class IGraphicsItemStack;
class Port;

//-----------------------------------------------------------------------------
//! PortAddCommand class.
//-----------------------------------------------------------------------------
class PortAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] component   The component to which to add a port.
     *    @param [in] pos         The position where to add the port.
     *    @param [in] diagram     Diagram containing the scene of the port.
     *    @param [in] parent      The parent command.
     */
    PortAddCommand(HWComponentItem* component, QPointF const& pos, DesignDiagram* diagram,
        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PortAddCommand();

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
    PortAddCommand(PortAddCommand const& rhs);
    PortAddCommand& operator=(PortAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The item.
    HWComponentItem* component_;

    //! The port position.
    QPointF pos_;

    //! The diagram port.
    ActiveBusInterfaceItem* port_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;

    //! Diagram containing the scene of the port.
    DesignDiagram* diagram_;
};

//-----------------------------------------------------------------------------
//! ConnectionAddCommand class.
//-----------------------------------------------------------------------------
class ConnectionAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] scene           The scene.
     *    @param [in] connectionItem  The interconnection to add.
     *    @param [in] parent          The parent command.
     */
    ConnectionAddCommand(QGraphicsScene* scene, HWConnection* connectionItem, QSharedPointer<Design> design,
                         QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionAddCommand();

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
    ConnectionAddCommand(ConnectionAddCommand const& rhs);
    ConnectionAddCommand& operator=(ConnectionAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    HWConnection* connection_;

    QSharedPointer<Design> design_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! BusInterfacePasteCommand class.
//-----------------------------------------------------------------------------
class BusInterfacePasteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
	 *      Creates the child commands for adding physical ports to the component model. 
	 *
     *    @param [in] destComponent  The component to which to copy an interface.
	 *    @param [in] interface      The interface item to paste.
	 *    @param [in] column         The target column.
     *    @param [in] diagram        The target design diagram.
     *    @param [in] parent         The parent command.
     */
    BusInterfacePasteCommand(QSharedPointer<Component> destComponent, HierarchicalBusInterfaceItem* interfaceItem,
        GraphicsColumn* column, DesignDiagram* diagram, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~BusInterfacePasteCommand();

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
    BusInterfacePasteCommand(BusInterfacePasteCommand const& rhs);
    BusInterfacePasteCommand& operator=(BusInterfacePasteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component to copy to.
    QSharedPointer<Component> destComponent_;

    //! The bus interface to copy.
    QSharedPointer<BusInterface> busInterface_;

    //! The interface position.
    QPointF pos_;

    //! The interface item.
    HierarchicalBusInterfaceItem* interfaceItem_;

    //! The target column.
    GraphicsColumn* column_;

    //! The target design diagram.
    DesignDiagram* diagram_;

    //! If true, the bus interface item is deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! AddPhysicalPortCommand class.
//-----------------------------------------------------------------------------
class AddPhysicalPortCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] component  The component to which to copy a physical port.
	 *    @param [in] port       The port to paste.
     *    @param [in] parent     The parent command.
     */
   AddPhysicalPortCommand(QSharedPointer<Component>, QSharedPointer<Port> port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AddPhysicalPortCommand();

    // Disable copying.
    AddPhysicalPortCommand(AddPhysicalPortCommand const& rhs) = delete;
    AddPhysicalPortCommand& operator=(AddPhysicalPortCommand const& rhs) = delete;

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The target component model.
    QSharedPointer<Component> component_;

    //! The port to paste.
	QSharedPointer<Port> port_;
};

//-----------------------------------------------------------------------------

#endif // HWADDCOMMANDS_H
