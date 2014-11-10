//-----------------------------------------------------------------------------
// File: AdHocPortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCPORTITEM_H
#define ADHOCPORTITEM_H

#include <QSharedPointer>
#include <QVector2D>

#include "HWConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

class OffPageConnectorItem;
class LibraryInterface;
class Port;

//-----------------------------------------------------------------------------
//! AdHocPortItem class.
//-----------------------------------------------------------------------------
class AdHocPortItem : public HWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_PORT };

    /*!
     *  Constructor.
     */
    AdHocPortItem(Port* port, LibraryInterface* lh, QGraphicsItem* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~AdHocPortItem();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the end point.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    void setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode);

    /*!
     *  Updates the graphics to match the IP-XACT port.
     */
    void updateInterface();

	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*!
     *  Returns the name of the ad-hoc port.
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
     *  Returns true if a connection is valid between the two endpoints.
     *
     *      @param [in] other The other endpoint.
     *
     *      @remarks Does not take existing connections into account but simply
     *               validates whether a connection between the endpoints would be valid
     *               in a general case.
     */
    virtual bool isConnectionValid(ConnectionEndpoint const* other) const;

    /*!
     *  Returns true if the endpoint is exclusive, i.e. can only have one connection.
     */
    virtual bool isExclusive() const;

    /*! 
    *  Returns the encompassing component. if this port represents
    *  a bus interface on a component.
     */
    virtual ComponentItem* encompassingComp() const;

	/*!
     *  Returns a pointer to the top component that owns this interface
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const;

    /*! 
     *  Returns the IP-XACT bus interface model of the port.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*!
     *  Returns the ad-hoc port of the end point.
     *
     *      @remarks The function returns a null pointer if the end point is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     */
    virtual Port* getPort() const;

    /*! 
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

    /*!
     *  Returns true if the end point is a bus interface end point.
     */
    virtual bool isBus() const;

	/*!
     *  Sets the interface mode for the port.
     *
     *      @param [in] mode The mode to set.
     */
	virtual void setInterfaceMode(General::InterfaceMode mode);

    /*!
     *  Returns the corresponding off-page connector or a null pointer if the end point does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

	/*!
	 *  Set the position of the name label.
	 */
	void setLabelPosition();

	/*!
	 *  Check the direction of the port and change it if necessary.
	 */
	void checkDirection();

	/*!
	 *  Gives the length of the text in the name label.
	 */
	qreal getNameLength();
	
	/*!
	 *  Shortens the name label to better fit in the component.
	 *
	 *      @param [in] width   The width of the shortened name.
	 */
    void shortenNameLabel(qreal width);

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    
	//! The name label.
	QGraphicsTextItem nameLabel_;

    Port* port_;

	//! Library interface.
    LibraryInterface* lh_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<ConnectionEndpoint*, QPointF> oldPortPositions_;

    //! The off-page connector.
    OffPageConnectorItem* offPageConnector_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCPORTITEM_H
