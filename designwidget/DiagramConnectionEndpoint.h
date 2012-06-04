//-----------------------------------------------------------------------------
// File: DiagramConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMCONNECTIONENDPOINT_H
#define DIAGRAMCONNECTIONENDPOINT_H

#include <models/generaldeclarations.h>
#include <models/component.h>

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>

class DiagramInterconnection;
class ComponentItem;
class BusInterface;
class VLNV;

//-----------------------------------------------------------------------------
//! DiagramConnectionEndpoint interface.
//-----------------------------------------------------------------------------
class DiagramConnectionEndpoint : public ConnectionEndpoint
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent graphics item.
     *      @param [in] dir    The initial direction for the endpoint.
     */
    DiagramConnectionEndpoint(QGraphicsItem* parent = 0,
                              QVector2D const& dir = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    virtual ~DiagramConnectionEndpoint();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the endpoint.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    virtual void setTypes(VLNV const& busType, VLNV const& absType,
                          General::InterfaceMode mode) = 0;

    /*!
     *  Attaches the endpoint to a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void addInterconnection(DiagramInterconnection* connection);

    /*!
     *  Unattaches the endpoint from a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void removeInterconnection(DiagramInterconnection* connection);

    /*!
     *  Returns the list of connections that are connected to this endpoint.
     */
    virtual QList<DiagramInterconnection*> const& getInterconnections() const;

    /*!
     *  Returns true if the endpoint has at least one connection.
     */
    bool isConnected() const;

    /*!
     *  Called when a connection between this and another endpoint is done.
     *
     *      @param [in] other The other endpoint of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(DiagramConnectionEndpoint const* other) = 0;

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     *
     *      @param [in] other The other endpoint of the connection.
     */
    virtual void onDisconnect(DiagramConnectionEndpoint const* other) = 0;

    /*! 
     *  Returns true if this endpoint can be connected to the given endpoint.
     *
     *      @param [in] other The endpoint to which to connect.
     */
    virtual bool canConnect(DiagramConnectionEndpoint const* other) const = 0;

    /*!
     *  Returns the ad-hoc port of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     */
    virtual Port* getPort() const = 0;

    /*!
     *  Returns true if the endpoint is an ad-hoc port.
     */
    bool isAdHoc() const;

	/*! \brief Set the interface mode for the endpoint.
	 *
	 * \param mode The interface mode to set.
	 *
	*/
	virtual void setInterfaceMode(General::InterfaceMode mode) = 0;

    /*!
     *  Returns the corresponding off-page connector or a null pointer if the endpoint does not have one.
     */
    virtual DiagramConnectionEndpoint* getOffPageConnector();

private:
    // Disable copying.
    DiagramConnectionEndpoint(DiagramConnectionEndpoint const& rhs);
    DiagramConnectionEndpoint& operator=(DiagramConnectionEndpoint const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connections to this endpoint.
    QList<DiagramInterconnection*> connections_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMCONNECTIONENDPOINT_H
