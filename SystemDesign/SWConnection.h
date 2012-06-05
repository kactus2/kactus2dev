//-----------------------------------------------------------------------------
// File: SWConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.5.2012
//
// Description:
// Graphics item for SW connections (API/COM).
//-----------------------------------------------------------------------------

#ifndef SWCONNECTION_H
#define SWCONNECTION_H

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

class DesignDiagram;

//-----------------------------------------------------------------------------
//! Graphics item for SW connections (API/COM).
//-----------------------------------------------------------------------------
class SWConnection : public GraphicsConnection
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SW_CONNECTION };

    /*!
     *  Constructor.
     */
    SWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                 bool autoConnect, QString const& name, 
                 QString const& displayName, QString const& description,
                 DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended connection.
     */
    SWConnection(QPointF const& p1, QVector2D const& dir1,
                 QPointF const& p2, QVector2D const& dir2,
                 const QString &displayName, const QString &description,
                 DesignDiagram* parent);

    /*!
     *  Destructor.
     */
    ~SWConnection();

    int type() const { return Type; }
};

//-----------------------------------------------------------------------------

#endif // SWCONNECTION_H
