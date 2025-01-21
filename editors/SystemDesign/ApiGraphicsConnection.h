//-----------------------------------------------------------------------------
// File: ApiGraphicsConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// A graphical representation of a Kactus2 API interconnection.
//-----------------------------------------------------------------------------

#ifndef APIGRAPHICSCONNECTION_H
#define APIGRAPHICSCONNECTION_H

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

#include <common/graphicsItems/ConnectionItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <IPXACTmodels/kactusExtensions/ApiInterconnection.h>

class ConnectionRoute;

//-----------------------------------------------------------------------------
//! A graphical representation of a Kactus2 API interconnection.
//-----------------------------------------------------------------------------
class ApiGraphicsConnection : public ConnectionItem<ApiInterconnection>
{
    Q_OBJECT

public:

    //! Type of the API interconnection.
    enum { Type = GFX_TYPE_SW_API_CONNECTION };


    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     *
     *    @param [in] p1              The start point of the connection.
     *    @param [in] dir1            Direction of the start point.
     *    @param [in] p2              The end point of the connection.
     *    @param [in] dir2            Direction of the end point.
     *    @param [in] parent          The design diagram containing the connection.
     */
    ApiGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
                          DesignDiagram* parent);

    /*!
     *  The constructor containing the API interconnection.
     *
     *    @param [in] endpoint1           The start point of the connection.
     *    @param [in] endpoint2           The end point of the connection.
     *    @param [in] apiInterconnection  The API interconnection being connected.
     *    @param [in] route               The route of the connection.
     *    @param [in] autoConnect         If true, automatically connects the two end points.
     *    @param [in] parent              The design diagram containing the connection.
     */
    ApiGraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                          QSharedPointer<ApiInterconnection> apiInterconnection,
                          QSharedPointer<ConnectionRoute> route, bool autoConnect, DesignDiagram* parent);

    /*!
     *  The destructor.
     */
    virtual ~ApiGraphicsConnection() = default;

    /*!
     *  Get the type of the graphics item.
     *
     *    @return GFX_TYPE_SW_API_CONNECTION.
     */
    int type() const final { return Type; }
    
};

#endif // APIGRAPHICSCONNECTION_H
