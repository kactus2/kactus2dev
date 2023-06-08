//-----------------------------------------------------------------------------
// File: HWConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT interconnection.
//-----------------------------------------------------------------------------

#ifndef HWCONNECTION_H
#define HWCONNECTION_H

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

#include <common/graphicsItems/ConnectionItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <IPXACTmodels/Design/Interconnection.h>

class ConnectionRoute;
class ActiveInterface;

//-----------------------------------------------------------------------------
//! HWConnection represents graphically an IP-XACT interconnection
//-----------------------------------------------------------------------------
class HWConnection : public ConnectionItem<Interconnection>
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERCONNECTION };

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     *
     *      @param [in] p1              The first end point.
     *      @param [in] dir1            Direction of the first end point.
     *      @param [in] p2              The second end point.
     *      @param [in] dir2            Direction of the second end point.
     *      @param [in] parent          Owner of the connection item.
     */
    explicit HWConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
         DesignDiagram* parent);

    /*!
     *  Constructor.
     *
     *      @param [in] endpoint1           The first end point.
     *      @param [in] endpoint2           The second end point.
     *      @param [in] interconnection     The contained interconnection.
     *      @param [in] route               Route for the connection item.
     *      @param [in] parent              Owner of the connection item.
     */
    HWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
        QSharedPointer<Interconnection> interconnection, QSharedPointer<ConnectionRoute> route,
        DesignDiagram* parent);

    /*!
     *  The destructor.
     */
    virtual ~HWConnection() = default;

    /*!
     *  Get the type of this connection.
     *
     *      @return HW connection.
     */
    int type() const final { return Type; }

    bool isBus() const;

    QSharedPointer<ConnectionRoute> route_;
};

#endif // HWCONNECTION_H
