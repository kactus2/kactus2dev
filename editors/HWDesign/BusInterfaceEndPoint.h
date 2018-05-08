//-----------------------------------------------------------------------------
// File: BusInterfaceEndPoint.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.02.2018
//
// Description:
// Parent interface class for HW bus interfaces.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEENDPOINT_H
#define BUSINTERFACEENDPOINT_H

#include <editors/HWDesign/HWConnectionEndpoint.h>

#include <QSharedPointer>
#include <QGraphicsTextItem>
#include <QUndoCommand>

class InterfaceGraphicsData;
class Component;
class BusInterface;
class Port;
class DesignDiagram;
class HWComponentItem;

//-----------------------------------------------------------------------------
//! Parent interface class for HW bus interfaces.
//-----------------------------------------------------------------------------
class BusInterfaceEndPoint : public HWConnectionEndpoint
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] busIf       The bus interface this item represents.
     *      @param [in] component   Component containing the bus interface.
     *      @param [in] parent      The parent object.
     *      @param [in] dir         Direction for the end point graphics.
     */
    BusInterfaceEndPoint(QSharedPointer<BusInterface> busIf, QSharedPointer<Component> component,
        QGraphicsItem *parent = 0, QVector2D const& dir = QVector2D(0.0f, -1.0f));

	/*!
     *  The destructor.
     */
    virtual ~BusInterfaceEndPoint();

    /*!
     *  Get the name of the bus interface.
     */
    virtual QString name() const;

    /*!
     *  Set a new name for the bus interface.
     *
     *      @param [in] name    The new bus interface name.
     */
    virtual void setName(QString const& name);
    
    /*!
     *  Get the description of the bus interface.
	 *
	 *      @return The bus interface description.
     */
	virtual QString description() const;

	/*!
     *  Set a new description for the bus interface.
	 *
	 *      @param [in] description     The selected description to set.
     */
	virtual void setDescription(QString const& description);

    /*!
     *  Get the contained bus interface.
     *
     *      @return The contained bus interface.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*!
     *  Check if the bus interface can only have one connection.
     *
     *      @return True, if the bus interface is exclusive, false otherwise.
     */
    virtual bool isExclusive() const;

    /*!
     *  Check if the end point is a bus interface.
     *
     *      @return True.
     */
    virtual bool isBus() const;

    /*!
     *  Get the type of the connection end point.
     *
     *      @return This end point is a bus interface.
     */
    virtual ConnectionEndpoint::EndpointType getType() const;

    /*!
     *  Set the interface mode for the end point.
	 *
	 *      @param mode [in]    The interface mode to set.
     */
	virtual void setInterfaceMode(General::InterfaceMode mode);

    /*!
     *  Get the mode of the contained bus interface.
     *
     *      @return The bus interface mode.
     */
    virtual General::InterfaceMode getInterfaceMode() const;

    /*!
     *  Check if a connection can be made to the selected end point.
     *
     *      @param [in] other   The end point to be connected.
     *
     *      @return True, if the connection can be made, false otherwise.
     */
    virtual bool isConnectionValid(ConnectionEndpoint const* other) const;

    /*!
     *  Handles the disconnecting of the connection end points.
     *
     *      @param [in] other   The other end of the connection.
     */
    virtual void onDisconnect(ConnectionEndpoint const* other);

    /*!
     *  Handles the connecting of the connection end points.
     *
     *      @param [in] other   The other end of the conenction.
     *
     *      @return True, if the connection was made successfully, false otherwise.
     */
    virtual bool onConnect(ConnectionEndpoint const* other);

    /*!
     *  Set a new bus interface.
     *
     *      @param [in] newBus  The selected bus interface.
     */
    void setBusInterface(QSharedPointer<BusInterface> newBus);

protected:

    /*!
     *  Get the position of the bus interface item before moving it.
     *
     *      @return The old position of the bus interface item.
     */
    QPointF getOldPosition() const;

    /*!
     *  Store the given position as the old position of the item.
     *
     *      @param [in] newPosition     The position to be stored.
     */
    void setOldPosition(QPointF const& newPosition);

    /*!
     *  Set the positions of the end point items from the same level as this item.
     *
     *      @param [in] items   List of the associated end point items.
     */
    void saveOldPortPositions(QList<QGraphicsItem*> items);

    /*!
     *  Get the stored positions of the associated end point items.
     *
     *      @return The stored positions of the associated end point items.
     */
    QMap<ConnectionEndpoint*, QPointF> getOldPortPositions() const;

    /*!
     *  Clear the stored positions of the associated end points.
     */
    void clearOldPortPositions();

    /*!
     *  Update the connection positions of the associated end points.
     */
    void beginUpdateConnectionPositions();

    /*!
     *  Handle the mouse release events.
     *
     *      @param [in] event   The mouse release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
    /*!
     *  Get the shape of the bus interface with direction in.
     *
     *      @return The shape of the bus interface with direction in.
     */
    virtual QPolygonF getDirectionInShape() const;

    /*!
     *  Get the shape of the bus interface with direction out.
     *
     *      @return The shape of the bus interface with direction out.
     */
    virtual QPolygonF getDirectionOutShape() const;

    /*!
     *  Get the shape of the bus interface with direction in/out.
     *
     *      @return The shape of the bus interface with direction in/out.
     */
    virtual QPolygonF getDirectionInOutShape() const;

private:

    // Disable copying.
    BusInterfaceEndPoint(BusInterfaceEndPoint const& rhs);
    BusInterfaceEndPoint& operator=(BusInterfaceEndPoint const& rhs);

    /*!
     *  Get the graphical shape for the interface item.
     *
     *      @param [in] direction   Direction of the interface item.
     *
     *      @return The new shape for the interface item.
     */
    QPolygonF getInterfaceShape(DirectionTypes::Direction direction) const;

    /*!
     *  Handle the name change of the bus interface.
     *
     *      @param [in] previousName    The previous name.
     *      @param [in] newName         The new name.
     */
    virtual void updateName(QString const& previousName, QString const& newName) = 0;

    /*!
     *  Create a move command for this end point item.
     *
     *      @param [in] diagram     The containing design diagram.
     *
     *      @return The created move command.
     */
    virtual QSharedPointer<QUndoCommand> createMouseMoveCommand(DesignDiagram* diagram) = 0;

    /*!
     *  Create move command for an end point that has been moved by the movement of this end point.
     *
     *      @param [in] endPoint            The selected end point.
     *      @param [in] endPointPosition    The new position of the end point.
     *      @param [in] diagram             Design diagram containing the end point.
     *      @param [in] parentCommand       Parent command.
     */
    virtual void createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
        DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand) = 0;

    /*!
     *  Check if a connection can be made to the selected connection end point.
     *
     *      @param [in] otherEndPoint   The selected connection end point.
     *
     *      @return True, if the connection can be made, false otherwise.
     */
    virtual bool canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const = 0;

    /*!
     *  Update the end point item graphics.
     */
    virtual void updateEndPointGraphics();

    /*!
     *  Get the current position of the end point.
     *
     *      @return The current position of the end point.
     */
    virtual QPointF getCurrentPosition() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The bus interface.
    QSharedPointer<BusInterface> busInterface_;

    //! Old position of the interface item.
    QPointF oldPos_;

    //! Old positions of the associated end points.
    QMap<ConnectionEndpoint*, QPointF> oldPortPositions_;
};

#endif // BUSINTERFACEENDPOINT_H
