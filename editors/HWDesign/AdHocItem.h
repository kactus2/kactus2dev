//-----------------------------------------------------------------------------
// File: AdHocItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.05.2016
//
// Description:
// Parent graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCITEM_H
#define ADHOCITEM_H

#include "HWConnectionEndpoint.h"

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

class Port;
class GraphicsItemLabel;

//-----------------------------------------------------------------------------
//! Parent graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------
class AdHocItem : public HWConnectionEndpoint
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] port                    The selected port.
     *      @param [in] containingComponent     Component containing the port.
     *      @param [in] parent                  The owner of this item.
     *      @param [in] dir                     The direction of the item.
     */
    AdHocItem(QSharedPointer<Port> port, QSharedPointer<Component> containingComponent, QGraphicsItem* parent = 0,
        QVector2D const& dir = QVector2D(0.0f, -1.0f));

	/*!
     *  The destructor.
     */
	~AdHocItem() override = default;

    // Disable copying.
    AdHocItem(AdHocItem const& rhs) = delete;
    AdHocItem& operator=(AdHocItem const& rhs) = delete;

    /*!
     *  Check if the ad hoc port is valid or not.
     *
     *      @return True, if the port is valid, otherwise false.
     */
    bool adhocPortIsValid() const;

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns the name of the ad-hoc port.
     *
     *      @return The name of the ad hoc port.
     */
    virtual QString name() const;

	/*!
     *  Returns the description of the port.
     *
     *      @return The description of the port.
     */
	virtual QString description() const;

	/*!
     *  Sets the description of the port.
     *
     *      @param [in] description The description to set.
     */
	virtual void setDescription(QString const& description);

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(ConnectionEndpoint const* other);

    /*!
     *  Check if the port is exclusive, i.e. can only have one connection.
     *
     *      @return False. Ad hoc ports are always non-exclusive.
     */
    virtual bool isExclusive() const;

    /*! 
     *  Returns the IP-XACT bus interface model of the port.
     *
     *      @return Ad hoc interfaces do not have a bus interface, returns an empty pointer.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*!
     *  Returns the ad-hoc port of the end point.
     *
     *      @remarks The function returns a null pointer if the end point is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     *
     *      @return The ad hoc port of the item.
     */
    virtual QSharedPointer<Port> getPort() const;

    /*!
     *  Check if the end point is an ad hoc port.
     *
     *      @return True, if the end point is an ad hoc port, false otherwise.
     */
    virtual bool isAdHoc() const;

    /*!
     *  Returns the type of the endpoint (API/COM/bus/ad-hoc/undefined).
     */
    virtual ConnectionEndpoint::EndpointType getType() const;

    /*!
     *  Create or change the current tie off label.
     *
     *      @param [in] tieOffExpression    The expression containing the tie off.
     *      @param [in] tieOffValue         The formatted tie off value.
     *      @param [in] tieOffIsSupported   Flag for determining whether the tie off is supported or not.
     */
    void changeTieOffLabel(QString const& tieOffExpression, QString const& tieOffValue, bool tieOffIsSupported);

    /*!
     *  Remove the tie off symbol.
     */
    void removeTieOffItem();

    /*!
     *  Check if a connection to the selected connection end point is valid.
     *
     *      @param [in] other   The selected connection end point.
     *
     *      @return True, if the connection can be created, false otherwise.
     */
    virtual bool isConnectionValid(ConnectionEndpoint const* other) const;

protected:

    /*!
     *  Get the shape of the item.
     *
     *      @return Polygon containing the shape of the item.
     */
    QPolygonF getPortShape() const;

    /*!
     *  Get the tie off label of the item.
     *
     *      @return The label item containing the current tie off.
     */
    GraphicsItemLabel* getTieOffLabel();

    /*!
     *  Set the position for the tie off label.
     */
    void setTieOffLabelPosition();

    /*!
     *  Event handling for mouse press.
     *
     *      @param [in] event   The mouse press event.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    /*!
     *  Get the shape of an in port.
     *
     *      @param [in] squareSize  The size of a square in the design grid.
     *
     *      @return Polygon containing the shape of an in port.
     */
    virtual QPolygonF getInPortShape(const int squareSize) const;

    /*!
     *  Get the shape of an out port.
     *
     *      @param [in] squareSize  The size of a square in the design grid.
     *
     *      @return Polygon containing the shape of an out port.
     */
    virtual QPolygonF getOutPortShape(const int squareSize) const;

    /*!
     *  Create the tie off label.
     */
    void createTieOffLabel();

    /*!
     *  Create the graphical path connecting the tie off to the selected port.
     */
    void createTieOffPath();

    /*!
     *  Check whether the label should be drawn on the left side of the interface.
     *
     *      @return True, if the label should be drawn to the left side.
     */
    virtual bool labelShouldBeDrawnLeft() const = 0;

    /*!
     *  Store the positions of the associated end points.
     */
    virtual void saveOldPortPositions() = 0;

    /*!
     *  Update the graphics of the end point.
     */
    virtual void updateEndPointGraphics();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The port contained within the item.
    QSharedPointer<Port> port_ = nullptr;

    //! The tie off label.
    GraphicsItemLabel* tieOffLabel_ = nullptr;

    //! The path connecting the tie off label to this item.
    QGraphicsItem* tieOffPath_ = nullptr;

};

//-----------------------------------------------------------------------------

#endif // ADHOCITEM_H
