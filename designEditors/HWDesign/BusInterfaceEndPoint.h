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

#include <designEditors/HWDesign/HWConnectionEndpoint.h>

#include <QSharedPointer>
#include <QGraphicsTextItem>

class InterfaceGraphicsData;
class Component;
class BusInterface;

class OffPageConnectorItem;
class Port;

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
     *      @param [in] busIf   The bus interface this item represents.
     *      @param [in] parent  The parent object.
     *      @param [in] dir     Direction for the end point graphics.
     */
    BusInterfaceEndPoint(QSharedPointer<BusInterface> busIf, QGraphicsItem *parent = 0,
        QVector2D const& dir = QVector2D(0.0f, -1.0f));

	/*!
     *  The destructor.
     */
    virtual ~BusInterfaceEndPoint();

    /*!
     *  Update the bus interface graphics item.
     */
    virtual void updateInterface();

    /*!
     *  Set the position for the bus interface name label.
     */
    virtual void setLabelPosition() = 0;

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
     *  Get the port for the end point.
     *
     *      @return Empty port for bus interfaces.
     */
    virtual QSharedPointer<Port> getPort() const;

    /*!
     *  Check if the end point is a bus interface.
     *
     *      @return True.
     */
    virtual bool isBus() const;

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
     *  Get the off page connector item.
     *
     *      @return The corresponding off-page connector or a null pointer if the bus interface does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

protected:

    /*!
     *  Set a new bus interface.
     *
     *      @param [in] newBus  The selected bus interface.
     */
    void setBusInterface(QSharedPointer<BusInterface> newBus);

    /*!
     *  Get the label containing the name of the bus interface.
     *
     *      @return The label containing the name of the bus interface.
     */
    QGraphicsTextItem* getNameLabel() const;

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
     *  Check if the containing diagram is locked.
     *
     *      @return True, if the diagram is locked, false otherwise.
     */
    bool sceneIsLocked() const;

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
     *  Get the shape of the bus interface with direction in.
     *
     *      @return The shape of the bus interface with direction in.
     */
    virtual QPolygonF getDirectionInShape() const = 0;

    /*!
     *  Get the shape of the bus interface with direction out.
     *
     *      @return The shape of the bus interface with direction out.
     */
    virtual QPolygonF getDirectionOutShape() const = 0;

    /*!
     *  Get the shape of the bus interface with direction in/out.
     *
     *      @return The shape of the bus interface with direction in/out.
     */
    virtual QPolygonF getDirectionInOutShape() const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Label containing the bus interface name.
    QGraphicsTextItem* nameLabel_;

    //! The bus interface.
    QSharedPointer<BusInterface> busInterface_;

    //! Old position of the interface item.
    QPointF oldPos_;

    //! Old positions of the associated end points.
    QMap<ConnectionEndpoint*, QPointF> oldPortPositions_;

    //! The off page connector item.
    OffPageConnectorItem* offPageConnector_;
};

#endif // BUSINTERFACEENDPOINT_H
