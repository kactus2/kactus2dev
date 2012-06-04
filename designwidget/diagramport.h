/* 
 *
 * 		filename: diagramport.h
 */

#ifndef DIAGRAMPORT_H
#define DIAGRAMPORT_H

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

#include "DiagramConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

class BusInterface;
class DiagramInterconnection;
class DiagramComponent;
class DiagramOffPageConnector;
class LibraryInterface;

/*! \brief DiagramInterconnection represents graphically an IP-XACT port
 *
 */
class DiagramPort : public DiagramConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_PORT };

    DiagramPort(QSharedPointer<BusInterface> busIf, LibraryInterface* lh,
                QGraphicsItem *parent = 0);

	//! \brief The destructor
	virtual ~DiagramPort();

    /*!
     *  Sets the port temporary or not temporary. Temporary port set its bus interface undefined
     *  automatically if the connections are removed.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    void setTemporary(bool temp);

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
    // DiagramConnectionEndpoint implementation.
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
    virtual bool onConnect(DiagramConnectionEndpoint const* other);

    bool askCompatibleMode(QSharedPointer<BusInterface> otherBusIf, General::InterfaceMode &mode);

    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(DiagramConnectionEndpoint const* other);

    /*! 
     *  Returns true if this port can be connected to the given end point.
     *
     *      @param [in] other The end point to which to connect.
     */
    virtual bool canConnect(DiagramConnectionEndpoint const* other) const;

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
    virtual DiagramConnectionEndpoint* getOffPageConnector();

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsTextItem *nameLabel_;
    QSharedPointer<BusInterface> busInterface_;
    LibraryInterface* lh_;

    //! Boolean flag for determining if the port is temporary or not.
    bool temp_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<DiagramConnectionEndpoint*, QPointF> oldPortPositions_;

    //! The off-page connector.
    DiagramOffPageConnector* offPageConnector_;
};

#endif // DIAGRAMPORT_H
