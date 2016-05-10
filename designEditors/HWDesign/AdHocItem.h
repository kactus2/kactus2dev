//-----------------------------------------------------------------------------
// File: AdHocItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

class OffPageConnectorItem;
class Port;

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
     *      @param [in] port    The selected port.
     *      @param [in] parent  The owner of this item.
     *      @param [in] dir     The direction of the item.
     */
    AdHocItem(QSharedPointer<Port> port, QGraphicsItem* parent = 0, QVector2D const& dir = QVector2D(0.0f, -1.0f));

	/*!
     *  The destructor.
     */
	virtual ~AdHocItem();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the end point.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    virtual void setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode);

    /*!
     *  Updates the graphics to match the IP-XACT port.
     */
    virtual void updateInterface() = 0;

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
     *  Sets the name of the ad-hoc port.
     *
     *      @param [in] name The name to set.
     */
	virtual void setName(const QString& name);

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
	virtual void setDescription(const QString& description);

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(ConnectionEndpoint const* other);

    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(ConnectionEndpoint const* other);

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
     *  Check if the item is a bus interface.
     *
     *      @return False, ad hoc ports are not bus interfaces.
     */
    virtual bool isBus() const;

	/*!
     *  Sets the interface mode for the port.
     *
     *      @param [in] mode The mode to set.
     */
	virtual void setInterfaceMode(General::InterfaceMode mode);

    /*!
     *  Get the off page connector item.
     *
     *      @return The corresponding off-page connector or a null pointer if the end point does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

    /*!
     *  Create a symbol for a non resolvable tie off value.
     */
    void createNonResolvableTieOff();

    /*!
     *  Create a symbol for a numbered tie off value.
     */
    void createNumberedTieOff();

    /*!
     *  Create a symbol for a tie off value of 1.
     */
    void createHighTieOff();

    /*!
     *  Create a symbol for a tie off value of 0.
     */
    void createLowTieOff();

    /*!
     *  Remove the tie off symbol.
     */
    void removeTieOffItem();

protected:

    /*!
     *  Get the name label of the item.
     *
     *      @return The text item containing the name of the port.
     */
    QGraphicsTextItem& getNameLabel();

    /*!
     *  Get the shape of the item.
     *
     *      @return Polygon containing the shape of the item.
     */
    QPolygonF getPortShape();

private:

    /*!
     *  Get the shape of an in port.
     *
     *      @param [in] squareSize  The size of a square in the design grid.
     *
     *      @return Polygon containing the shape of an in port.
     */
    virtual QPolygonF getInPortShape(const int squareSize) const = 0;

    /*!
     *  Get the shape of an out port.
     *
     *      @param [in] squareSize  The size of a square in the design grid.
     *
     *      @return Polygon containing the shape of an out port.
     */
    virtual QPolygonF getOutPortShape(const int squareSize) const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The name label.
	QGraphicsTextItem nameLabel_;

    //! The port contained within the item.
    QSharedPointer<Port> port_;

    //! The off-page connector.
    OffPageConnectorItem* offPageConnector_;

    //! The symbol for the tie off.
    QGraphicsItem* tieoffItem_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCITEM_H
