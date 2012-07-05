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

class Port;
class BusInterface;
class OffPageConnectorItem;
class HWComponentItem;
class HWColumn;
class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! BusInterfaceItem class.
//-----------------------------------------------------------------------------
class BusInterfaceItem : public HWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERFACE };

    BusInterfaceItem(LibraryInterface* lh, QSharedPointer<Component> component,
                     QSharedPointer<BusInterface> busIf, QGraphicsItem *parent = 0);

	//! \brief The destructor
	virtual ~BusInterfaceItem();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the end point.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    void setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode);

    /*! \brief Update the graphics to match the IP-XACT bus interface
     *
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

    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*! \brief Returns the name of this port
     *
     */
    virtual QString name() const;

	/*! \brief Set the name for the interface.
	 *
	 * \param name The name to set for the interface.
	 *
	*/
	virtual void setName(const QString& name);

	/*! \brief Get the description of the interface.
	 *
	 *
	 * \return QString contains the description.
	*/
	virtual QString description() const;

	/*! \brief Set the description for the interface.
	 *
	 * \param description Contains the description to set.
	 *
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

	/*! \brief Returns pointer to the top component that owns this interface.
	 *
	 *
	 * \return QSharedPointer<Component> Pointer to the component to which this 
	 * interface belongs to.
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

    /*! \brief Returns true if the port represents a hierarchical connection
     *
     */
    virtual bool isHierarchical() const;

    /*!
     *  Returns true if the end point is a bus interface end point.
     */
    virtual bool isBus() const;

    void setDirection(QVector2D const& dir);

	/*! \brief Set the interface mode for the end point.
	 *
	 * \param mode The interface mode to set.
	 *
	*/
	virtual void setInterfaceMode(General::InterfaceMode mode);

    /*!
     *  Returns the corresponding off-page connector or a null pointer if the end point does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

signals:
    //! \brief Send an error message to the user.
    void errorMessage(const QString& errorMessage);

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    //! The library interface.
    LibraryInterface* lh_;

    //! The name label.
    QGraphicsTextItem *nameLabel_;

    //! The bus interface.
    QSharedPointer<BusInterface> busInterface_;

    //! The top-level component.
    QSharedPointer<Component> component_;

    //! The old column from where the mouse drag event began.
    HWColumn* oldColumn_;

    //! The position of the interface before a mouse move.
    QPointF oldPos_;

    //! The old positions of the other interfaces before mouse move.
    QMap<QGraphicsItem*, QPointF> oldInterfacePositions_;

    //! The off-page connector.
    OffPageConnectorItem* offPageConnector_;
};

#endif // BUSINTERFACEITEM_H
