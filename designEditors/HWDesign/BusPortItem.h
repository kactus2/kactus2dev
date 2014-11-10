/* 
 *
 * 		filename: BusPortItem.h
 */

#ifndef BUSPORTITEM_H
#define BUSPORTITEM_H

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

#include "HWConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

class BusInterface;
class HWComponentItem;
class OffPageConnectorItem;
class LibraryInterface;

/*! \brief HWConnection represents graphically an IP-XACT port
 *
 */
class BusPortItem : public HWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_PORT };

    BusPortItem(QSharedPointer<BusInterface> busIf, LibraryInterface* lh,
                bool packetized, QGraphicsItem *parent = 0);

	//! \brief The destructor
	virtual ~BusPortItem();

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

	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*! \brief Returns the name of this port
     *
     */
    virtual QString name() const;

	/*! \brief Set the name for the port.
	 *
	 * \param name The name to set for the port.
	 *
	*/
	virtual void setName(const QString& name);

	/*! \brief Get the description of the port.
	 *
	 *
	 * \return QString contains the description.
	*/
	virtual QString description() const;

	/*! \brief Set the description for the port.
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
     *  Finds the mode for the interface and generated ports (if any).
     *
     *      @param [in] other The other end point of the connection.
     *      @param [out] mode The interface mode determined or selected by user.
     *      @param [out] ports Ports generated with the connection.
     *      @param [out] portMaps Port maps generated with the connection.
     *
     *      @return False if there was an error in the connection or user rejected 
     *              the mode dialog. Otherwise true.
     */
    bool getModeAndPorts(ConnectionEndpoint const* other, General::InterfaceMode& mode, 
                           QList< QSharedPointer<Port> >& ports,
                           QList< QSharedPointer<PortMap> >& portMaps);

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
	
	/*!
	 *  Set the position of the name label.
	 */
	void setLabelPosition();

	/*!
	 *  Check the direction of the port and change it if necessary.
	 */
	void checkDirection();

	/*!
	 *  Return the correct length of the name label.
	 */
	qreal getNameLength();

	/*!
	 *  Shorten the name label to better fit the component.
	 *  
	 *      @param [in] width   The width of the shortened name.
	 */
	void shortenNameLabel( qreal width );

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    
    /*!
     *  Finds the possible opposing modes for a bus interface.
     *
     *      @param [in] busIf   The interface for which the opposing mode is searched.
     *    
     *      @return The possible opposing modes.
     */
    QList<General::InterfaceMode> getOpposingModes(QSharedPointer<BusInterface> busIf);

	//! The name label.
    QGraphicsTextItem nameLabel_;

	//! The bus interface.
	QSharedPointer<BusInterface> busInterface_;

	//! The library interface.
    LibraryInterface* lh_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<ConnectionEndpoint*, QPointF> oldPortPositions_;

    //! The off-page connector.
    OffPageConnectorItem* offPageConnector_;

    //! Name of the bus interface before injection from another.
    QString oldName_;
};

#endif // BUSPORTITEM_H
