//-----------------------------------------------------------------------------
// File: diagraminterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMINTERFACE_H
#define DIAGRAMINTERFACE_H

#include <QGraphicsPolygonItem>
#include <QSharedPointer>
#include <QVector2D>

#include "DiagramConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

class Port;
class BusInterface;
class DiagramInterconnection;
class DiagramOffPageConnector;
class DiagramComponent;
class DiagramColumn;
class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! DiagramInterface class.
//-----------------------------------------------------------------------------
class DiagramInterface : public DiagramConnectionEndPoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERFACE };

    DiagramInterface(LibraryInterface* lh, QSharedPointer<Component> component,
                     QSharedPointer<BusInterface> busIf, QGraphicsItem *parent = 0);

	//! \brief The destructor
	virtual ~DiagramInterface();

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
    QList<Port*> getPorts() const;

    /*!
     *  Sets the interface temporary or not temporary. Temporary interface destroys
     *  its bus interface automatically if the connections are removed.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    void setTemporary(bool temp);

	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // DiagramConnectionEndPoint implementation.
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
    virtual bool onConnect(DiagramConnectionEndPoint const* other);

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

	/*! \brief Returns pointer to the top component that owns this interface.
	 *
	 *
	 * \return QSharedPointer<Component> Pointer to the component to which this 
	 * interface belongs to.
	*/
	virtual QSharedPointer<Component> ownerComponent() const;

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
    virtual DiagramConnectionEndPoint* getOffPageConnector();

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
    DiagramColumn* oldColumn_;

    //! Boolean flag for determining if this interface is temporary or not.
    bool temp_;

    //! The position of the interface before a mouse move.
    QPointF oldPos_;

    //! The old positions of the other interfaces before mouse move.
    QMap<QGraphicsItem*, QPointF> oldInterfacePositions_;

    //! The off-page connector.
    DiagramOffPageConnector* offPageConnector_;
};

#endif // DIAGRAMINTERFACE_H
