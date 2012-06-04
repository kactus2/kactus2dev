//-----------------------------------------------------------------------------
// File: DiagramDeleteCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.8.2011
//
// Description:
// Undo delete commands for the design widget.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMDELETECOMMANDS_H
#define DIAGRAMDELETECOMMANDS_H

#include <models/generaldeclarations.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

class BusInterface;
class Port;
class DiagramInterconnection;
class DiagramConnectionEndPoint;
class DiagramPort;
class DiagramComponent;
class DiagramInterface;
class GraphicsColumn;
class GraphicsColumnLayout;
class ComponentItem;

//-----------------------------------------------------------------------------
//! ColumnDeleteCommand class.
//-----------------------------------------------------------------------------
class ColumnDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout  The column layout.
     *      @param [in] column  The column to delete.
     */
    ColumnDeleteCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
                        QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ColumnDeleteCommand();

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
    ColumnDeleteCommand(ColumnDeleteCommand const& rhs);
    ColumnDeleteCommand& operator=(ColumnDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    GraphicsColumnLayout* layout_;

    //! The diagram column.
    GraphicsColumn* column_;

    //! Boolean flag for indicating if the column should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ComponentDeleteCommand class.
//-----------------------------------------------------------------------------
class ComponentDeleteCommand : public QObject, public QUndoCommand
{
	Q_OBJECT
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component    The component to delete.
     */
    ComponentDeleteCommand(DiagramComponent* component, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentDeleteCommand();

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
    ComponentDeleteCommand(ComponentDeleteCommand const& rhs);
    ComponentDeleteCommand& operator=(ComponentDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram component.
    DiagramComponent* component_;

    //! The component's parent column.
    GraphicsColumn* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the component should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ConnectionDeleteCommand class.
//-----------------------------------------------------------------------------
class ConnectionDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] conn    The interconnection to delete.
     */
    ConnectionDeleteCommand(DiagramInterconnection* conn, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionDeleteCommand();

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
    ConnectionDeleteCommand(ConnectionDeleteCommand const& rhs);
    ConnectionDeleteCommand& operator=(ConnectionDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    DiagramInterconnection* conn_;

    //! The interface modes for the endpoints.
    General::InterfaceMode mode1_;
    General::InterfaceMode mode2_;

    //! The port maps for the hierarchical end point (if any).
    QList< QSharedPointer<General::PortMap> > portMaps_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! PortDeleteCommand class.
//-----------------------------------------------------------------------------
class PortDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] port The port to delete.
     */
    PortDeleteCommand(DiagramConnectionEndPoint* port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PortDeleteCommand();

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
    PortDeleteCommand(PortDeleteCommand const& rhs);
    PortDeleteCommand& operator=(PortDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram port.
    DiagramConnectionEndPoint* port_;

    //! The port's parent.
    DiagramComponent* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! InterfaceDeleteCommand class.
//-----------------------------------------------------------------------------
class InterfaceDeleteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] interface    The interface to delete.
     *      @param [in] removePorts  If true, the ports that are part of the interface are also removed.
     */
    InterfaceDeleteCommand(DiagramInterface* interface, bool removePorts, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~InterfaceDeleteCommand();

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
    InterfaceDeleteCommand(InterfaceDeleteCommand const& rhs);
    InterfaceDeleteCommand& operator=(InterfaceDeleteCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The diagram interface.
    DiagramInterface* interface_;

    //! The bus interface.
    QSharedPointer<BusInterface> busIf_;

    //! The ports.
    QList< QSharedPointer<Port> > ports_;

    //! The port's parent.
    GraphicsColumn* parent_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;

    //! If true, the command also removes the ports that are part of the bus interface.
    bool removePorts_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMDELETECOMMANDS_H
