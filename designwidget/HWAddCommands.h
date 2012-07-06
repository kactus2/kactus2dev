//-----------------------------------------------------------------------------
// File: HWAddCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.8.2011
//
// Description:
// Undo add commands for the design diagram.
//-----------------------------------------------------------------------------

#ifndef HWADDCOMMANDS_H
#define HWADDCOMMANDS_H

#include <models/generaldeclarations.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

class HWConnection;
class BusPortItem;
class HWComponentItem;
class BusInterfaceItem;
class GraphicsColumn;
class GraphicsColumnLayout;
class ComponentItem;
class IGraphicsItemStack;

//-----------------------------------------------------------------------------
//! PortAddCommand class.
//-----------------------------------------------------------------------------
class PortAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component to which to add a port.
     *      @param [in] pos        The position where to add the port.
     *      @param [in] parent     The parent command.
     */
    PortAddCommand(HWComponentItem* component, QPointF const& pos, QUndoCommand* parent = 0);

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
    BusPortItem* port_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
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
     *      @param [in] scene  The scene.
     *      @param [in] conn   The interconnection to add.
     *      @param [in] parent The parent command.
     */
    ConnectionAddCommand(QGraphicsScene* scene, HWConnection* conn,
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
    HWConnection* conn_;

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

#endif // HWADDCOMMANDS_H
