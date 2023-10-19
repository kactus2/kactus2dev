//-----------------------------------------------------------------------------
// File: AdHocConnectionItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.10.2015
//
// Description:
// Graphical item to represent ad-hoc connections in a design.
//-----------------------------------------------------------------------------

#ifndef ADHOCCONNECTIONITEM_H
#define ADHOCCONNECTIONITEM_H

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

#include <common/graphicsItems/ConnectionItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <IPXACTmodels/Design/AdHocConnection.h>

class ConnectionRoute;

//-----------------------------------------------------------------------------
//! Graphical item to represent ad-hoc connections in a design.
//-----------------------------------------------------------------------------
class AdHocConnectionItem : public ConnectionItem<AdHocConnection>
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_CONNECTION };
  
    AdHocConnectionItem(QPointF const& p1, QVector2D const& dir1, QPointF const& p2,
        QVector2D const& dir2, DesignDiagram* parent);

    AdHocConnectionItem(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
        QSharedPointer<AdHocConnection> adhocConnection, QSharedPointer<ConnectionRoute> route,
        DesignDiagram* parent);

    virtual ~AdHocConnectionItem() = default;

    // Disable copying.
    AdHocConnectionItem(AdHocConnectionItem const& rhs) = delete;
    AdHocConnectionItem& operator=(AdHocConnectionItem const& rhs) = delete;

    /*!
     *  Returns true if the connection is a bus connection. Returns false if it isn't (i.e. it is an ad-hoc
     *  connection).
     */
    bool isBus() const noexcept;

    /*!
     *  Get the type of this connection item.
     *
     *      @return Ad hoc connection item type.
     */
    int type() const final { return Type; }

};

#endif // ADHOCCONNECTIONITEM_H
