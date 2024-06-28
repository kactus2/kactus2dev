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

#include <editors/common/diagramgrid.h>

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
    QString name() const override;

	/*!
     *  Returns the description of the port.
     *
     *      @return The description of the port.
     */
	QString description() const override;

	/*!
     *  Sets the description of the port.
     *
     *      @param [in] description The description to set.
     */
	void setDescription(QString const& description) override;

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    bool onConnect(ConnectionEndpoint const* other) override;

    /*!
     *  Check if the port is exclusive, i.e. can only have one connection.
     *
     *      @return False. Ad hoc ports are always non-exclusive.
     */
    bool isExclusive() const final;

    /*! 
     *  Returns the IP-XACT bus interface model of the port.
     *
     *      @return Ad hoc interfaces do not have a bus interface, returns an empty pointer.
     */
    QSharedPointer<BusInterface> getBusInterface() const override;

    /*!
     *  Returns the ad-hoc port of the end point.
     *
     *      @remarks The function returns a null pointer if the end point is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     *
     *      @return The ad hoc port of the item.
     */
    QSharedPointer<Port> getPort() const override;

    void setPort(QSharedPointer<Port> newPort);

    /*!
     *  Check if the end point is an ad hoc port.
     *
     *      @return True, if the end point is an ad hoc port, false otherwise.
     */
    bool isAdHoc() const noexcept override;

    /*!
     *  Returns the type of the endpoint (API/COM/bus/ad-hoc/undefined).
     */
    ConnectionEndpoint::EndpointType getType() const noexcept override;

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
    bool isConnectionValid(ConnectionEndpoint const* other) const override;

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
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:

    /*!
     *  Get the shape of an in port.
     *
     *      @return Polygon containing the shape of an in port.
     */
    static QPolygonF getInPortShape();

    /*!
     *  Get the shape of an out port.
     *
     *      @return Polygon containing the shape of an out port.
     */
    static QPolygonF getOutPortShape();

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

    static constexpr int SQUARE_SIZE = GridSize - 4;
    static constexpr int TIEOFFITEM_DISTANCE = 20;

    //! The port contained within the item.
    QSharedPointer<Port> port_ = nullptr;

    //! The tie off label.
    GraphicsItemLabel* tieOffLabel_ = nullptr;

    //! The path connecting the tie off label to this item.
    QGraphicsItem* tieOffPath_ = nullptr;

};

//-----------------------------------------------------------------------------

#endif // ADHOCITEM_H
