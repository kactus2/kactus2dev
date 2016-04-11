//-----------------------------------------------------------------------------
// File: AdHocInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCINTERFACEITEM_H
#define ADHOCINTERFACEITEM_H

#include <QSharedPointer>
#include <QVector2D>

#include "HWConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

class GraphicsColumn;
class HWComponentItem;
class Kactus2Placeholder;
class LibraryInterface;
class OffPageConnectorItem;
class Port;

//-----------------------------------------------------------------------------
//! AdHocInterfaceItem class.
//-----------------------------------------------------------------------------
class AdHocInterfaceItem : public HWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_INTERFACE };

    /*!
     *  Constructor.
     */
    AdHocInterfaceItem(QSharedPointer<Component> component, QSharedPointer<Port> port,
        QSharedPointer<Kactus2Placeholder> dataGroup, QGraphicsItem* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~AdHocInterfaceItem();

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

	/*!
	 *  Get the type of this graphics item.
	 *
     *      @return AdHoc interface item type.
	 */
	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

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
     *      @remarks Does not take existing connections into account but simply validates whether a connection 
     *               between the endpoints would be valid in a general case.
     */
    virtual bool isConnectionValid(ConnectionEndpoint const* other) const;

    /*!
     *  Returns true if the endpoint is exclusive, i.e. can only have one connection.
     */
    virtual bool isExclusive() const;

    /*! 
     *  Returns the encompassing component. if this port represents a bus interface on a component.
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
    virtual QSharedPointer<Port> getPort() const;

    /*! 
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

    /*!
     *  Returns true if the end point is a bus interface end point.
     */
    virtual bool isBus() const;

    /*!
     *  Set the direction for this ad hoc interface item.
     *
     *      @param [in] dir     The new item direction.
     */
    void setDirection(QVector2D const& dir);

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

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

protected:

    /*!
     *  Notifies item state changes.
     *
     *      @param [in] change  The change.
     *      @param [in] value   The new value.
     */
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);

    /*!
     *  Event for mouse press.
     *
     *      @param [in] event   The pressed mouse button.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Event for mouse move.
     *
     *      @param [in] event   The movement event.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Event for mouse button release.
     *
     *      @param [in] event   The release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    //! The name label.
	QGraphicsTextItem nameLabel_;

    //! The top-level component.
    QSharedPointer<Component> component_;

    //! The port.
    QSharedPointer<Port> port_;


    QSharedPointer<Kactus2Placeholder> dataGroup_;

    //! The old column from where the mouse drag event began.
    GraphicsColumn* oldColumn_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other interfaces before mouse move.
    QMap<QGraphicsItem*, QPointF> oldInterfacePositions_;

    //! The off-page connector.
    OffPageConnectorItem* offPageConnector_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCINTERFACEITEM_H
