//-----------------------------------------------------------------------------
// File: BusInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEITEM_H
#define BUSINTERFACEITEM_H

#include <QGraphicsPolygonItem>
#include <QSharedPointer>
#include <QVector2D>

#include "HWConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <IPXACTmodels/common/DirectionTypes.h>

class Port;
class BusInterface;
class OffPageConnectorItem;
class HWComponentItem;
class HWColumn;
class Component;
class LibraryInterface;
class InterfaceGraphicsData;

//-----------------------------------------------------------------------------
//! BusInterfaceItem class.
//-----------------------------------------------------------------------------
class BusInterfaceItem : public HWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERFACE };

    /*!
     *  The constructor.
     *
     *      @param [in] lh          The library in use.
     *      @param [in] component   The component containing the bus interface this item represents.
     *      @param [in] busIf       The bus interface this item represents.
     *      @param [in] dataGroup   The container for the item data.
     *      @param [in] parent      The parent object.
     */
    BusInterfaceItem(LibraryInterface* lh, QSharedPointer<Component> component,
                     QSharedPointer<BusInterface> busIf,
                     QSharedPointer<InterfaceGraphicsData> dataGroup,
                     QGraphicsItem *parent = 0);

	//! The destructor
	virtual ~BusInterfaceItem();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the end point.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    void setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode);

    /*!
     *  Update the graphics to match the IP-XACT bus interface
     */
    void updateInterface();

    /*!
     *  Defines the interface.
     *
     *      @param [in] busIf     The bus interface.
     *      @param [in] addPorts  If true, the given ports are added to the parent component.
     *      @param [in] ports     The related ports.
     */
    void define(QSharedPointer<BusInterface> busIf, bool addPorts,
                QList< QSharedPointer<Port> > ports = QList< QSharedPointer<Port> >());

    /*!
     *  Sets the interface undefined.
     *
     *      @param [in] removePorts If true, the ports that are part of the bus interface are
     *                  removed from the parent component.
     */
    void undefine(bool removePorts);

    /*!
     *  Returns the ports in the top-level component that are related to the bus interface.
     */
	QList<QSharedPointer<Port> > getPorts() const;

    /*!
     *  Returns true if the bus interface has been created by copying the ports.
     */
    bool hasPortsCopied() const;

    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns the name of this port
     */
    virtual QString name() const;

	/*!
     *  Set the name for the interface.
	 *
	 *      @param [in] name The name to set for the interface.
	 */
	virtual void setName(const QString& name);

	/*!
     *  Get the description of the interface.
	 *
	 *      @return QString contains the description.
	 */
	virtual QString description() const;

	/*!
     *  Set the description for the interface.
	 *
	 *      @param [in] description Contains the description to set.
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
     *  Returns pointer to the top component that owns this interface.
	 *
	 *      @return The component to which this interface belongs to.
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
     *  Returns true if the port represents a hierarchical connection
     */
    virtual bool isHierarchical() const;

    /*!
     *  Returns true if the end point is a bus interface end point.
     */
    virtual bool isBus() const;

    /*!
     *  Set the direction for the bus interface item.
     *
     *      @param [in] dir     The new direction.
     */
    void setDirection(QVector2D const& dir);

	/*!
     *  Set the interface mode for the end point.
	 *
	 *      @param [in] mode The interface mode to set.
	 */
	virtual void setInterfaceMode(General::InterfaceMode mode);

    /*!
     *  Returns the corresponding off-page connector or a null pointer if the end point does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

	/*!
	 * Set the position of the name label.
	 */
	void setLabelPosition();

    /*!
     *  Gets the data extension for the bus interface.
     *
     *      @return The data vendor extension.
     */
    QSharedPointer<VendorExtension> getDataExtension() const;

signals:
    //! Send an error message to the user.
    void errorMessage(const QString& errorMessage);

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
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    /*!
     *  Event for mouse button release.
     *
     *      @param [in] event   The release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);


private:

    /*!
     *  Clones the port maps and required ports from the source end point.
     *
     *      @param [out] busIf   The destination bus interface.
     *      @param [in]  source  The source end point.
     */
    bool clonePortMaps(QSharedPointer<BusInterface> busIf, ConnectionEndpoint const* source);

    /*!
     *  Creates the shape of a up-pointing arrow for the item.
     *
     *      @return The shape for the item.
     */
    QPolygonF arrowUp() const;
    
    /*!
     *  Creates the shape of a down-pointing arrow for the item.
     *
     *      @return The shape for the item.
     */
    QPolygonF arrowDown() const;
    
    /*!
     *  Creates the shape of a double-headed arrow for the item.
     *
     *      @return The shape for the item.
     */
    QPolygonF doubleArrow() const;

    /*!
     *  Gets the direction for the bus interface.
     *
     *      @return The direction of the bus interface.
     */
    DirectionTypes::Direction getInterfaceDirection() const;
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the interface before connecting.
    QString oldName_;

    //! The library interface.
    LibraryInterface* library_;

    //! The name label.
	QGraphicsTextItem nameLabel_;

    //! The bus interface.
    QSharedPointer<BusInterface> busInterface_;

    //! The top-level component.
    QSharedPointer<Component> component_;

    //! The data extension for the item.
    QSharedPointer<InterfaceGraphicsData> dataGroup_;

    //! The old column from where the mouse drag event began.
    HWColumn* oldColumn_;

    //! The position of the interface before a mouse move.
    QPointF oldPos_;

    //! The old positions of the other interfaces before mouse move.
    QMap<QGraphicsItem*, QPointF> oldInterfacePositions_;

    //! The off-page connector.
    OffPageConnectorItem* offPageConnector_;

    //! If true, the ports were copied during end point connection.
    bool portsCopied_;
};

#endif // BUSINTERFACEITEM_H
