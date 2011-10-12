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

class BusInterface;
class DiagramInterconnection;
class DiagramComponent;
class LibraryInterface;

/*! \brief DiagramInterconnection represents graphically an IP-XACT port
 *
 */
class DiagramPort : public DiagramConnectionEndPoint
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

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
    // DiagramConnectionEndPoint implementation.
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

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(DiagramConnectionEndPoint const* other);

    bool askCompatibleMode(QSharedPointer<BusInterface> otherBusIf, General::InterfaceMode &mode);

    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(DiagramConnectionEndPoint const* other);

    /*! 
     *  Returns true if this port can be connected to the given end point.
     *
     *      @param [in] other The end point to which to connect.
     */
    virtual bool canConnect(DiagramConnectionEndPoint const* other) const;

    /*! 
     *  Returns the encompassing port, if this DiagramComponent represents
     *  a bus interface on a component
     */
    virtual DiagramComponent *encompassingComp() const;

    /*! 
     *  Returns the IP-XACT bus interface model of the port.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*! \brief Returns true if the port represents a hierarchical connection
     *
     */
    virtual bool isHierarchical() const;

	/*! \brief Set the interface mode for the end point.
	 *
	 * \param mode The interface mode to set.
	 *
	*/
	virtual void setInterfaceMode(General::InterfaceMode mode);

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    bool hierarchical_;
    QGraphicsTextItem *nameLabel_;
    QSharedPointer<BusInterface> busInterface_;
    LibraryInterface* lh_;

    //! Boolean flag for determining if the port is temporary or not.
    bool temp_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<DiagramPort*, QPointF> oldPortPositions_;
};

#endif // DIAGRAMPORT_H
