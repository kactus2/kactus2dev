//-----------------------------------------------------------------------------
// File: ComGraphicsConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.02.2016
//
// Description:
// A graphical representation of a Kactus2 com interconnection.
//-----------------------------------------------------------------------------

#ifndef COMGRAPHICSCONNECTION_H
#define COMGRAPHICSCONNECTION_H

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

#include <common/graphicsItems/ConnectionItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <IPXACTmodels/kactusExtensions/ComInterconnection.h>

class ConnectionRoute;

//-----------------------------------------------------------------------------
//! A graphical representation of a Kactus2 com interconnection.
//-----------------------------------------------------------------------------
class ComGraphicsConnection : public ConnectionItem<ComInterconnection>
{
    Q_OBJECT

public:

    //! The type of the COM connection item.
    enum { Type = GFX_TYPE_SW_COM_CONNECTION };

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     *
     *      @param [in] p1              The start point.
     *      @param [in] dir1            Direction of the start point.
     *      @param [in] p2              The end point.
     *      @param [in] dir2            Direction of the end point.
     *      @param [in] parent          The design diagram containing the connection.
     */
    ComGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
                          DesignDiagram* parent);

    /*!
     *  The constructor containing the connected COM interconnection.
     *
     *      @param [in] endpoint1           The start point of the connection.
     *      @param [in] endpoint2           The end point of the connection.
     *      @param [in] comInterconnection  The connected COM interconnection.
     *      @param [in] route               Route of the interconnection.
     *      @param [in] autoConnect         If true, the end points are automatically connected.
     *      @param [in] parent              The containing design diagram.
     */
    ComGraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                          QSharedPointer<ComInterconnection> comInterconnection,
                          QSharedPointer<ConnectionRoute> route, bool autoConnect, DesignDiagram* parent);

    /*!
     *  The destructor.
     */
    virtual ~ComGraphicsConnection() = default;

    /*!
     *  Get the type of the graphical COM connection.
     *
     *      @return The type of the COM connection item.
     */
    int type() const { return Type; }

};

#endif // COMGRAPHICSCONNECTION_H
