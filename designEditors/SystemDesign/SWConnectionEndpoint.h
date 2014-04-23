//-----------------------------------------------------------------------------
// File: SWConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.5.2012
//
// Description:
// Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------

#ifndef SWCONNECTIONENDPOINT_H
#define SWCONNECTIONENDPOINT_H

#include <models/generaldeclarations.h>
#include <models/component.h>

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>

class ComponentItem;
class ApiInterface;
class ComInterface;
class VLNV;

//-----------------------------------------------------------------------------
//! Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------
class SWConnectionEndpoint : public ConnectionEndpoint
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent     The parent graphics item.
     *      @param [in] temporary  If true, the endpoint is marked as a temporary one.
     *      @param [in] dir        The initial direction for the endpoint.
     */
    SWConnectionEndpoint(QGraphicsItem* parent = 0,
                         bool temporary = false, QVector2D const& rhs = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    virtual ~SWConnectionEndpoint();

    /*!
     *  Sets the COM/API type. The type of the VLNV determines the type of the endpoint.
     *
     *      @param [in] type The VLNV of the COM/API definition.
     */
    virtual void setTypeDefinition(VLNV const& type) = 0;

    /*!
     *  Returns the currently set COM/API definition.
     */
    virtual VLNV getTypeDefinition() const = 0;

    /*!
     *  Called when creating of a connection for this port has begun.
     */
    virtual void onBeginConnect() = 0;

    /*!
     *  Called when creating of a connection has ended.
     */
    virtual void onEndConnect() = 0;

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
     *  Updates the endpoint interface.
     */
    virtual void updateInterface();

private:
    // Disable copying.
    SWConnectionEndpoint(SWConnectionEndpoint const& rhs);
    SWConnectionEndpoint& operator=(SWConnectionEndpoint const& rhs);
};

//-----------------------------------------------------------------------------

#endif // SWCONNECTIONENDPOINT_H
