//-----------------------------------------------------------------------------
// File: HWConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

class Interconnection;
class ConnectionRoute;
class ActiveInterface;

//-----------------------------------------------------------------------------
//! HWConnection represents graphically an IP-XACT interconnection
//-----------------------------------------------------------------------------
class HWConnection : public GraphicsConnection
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
    HWConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
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
    virtual ~HWConnection();

    /*!
     *  Set the name for the connection item.
     *
     *      @param [in] name    The new connection item name.
     */
    virtual void setName(QString const& name);

    /*!
     *  Get the name of the connection item.
     *
     *      @return The name of the connection item.
     */
    virtual QString name() const;

    /*!
     *  Set the description for the connection item.
     *
     *      @param [in] description     The new description.
     */
    virtual void setDescription(QString const& description);

    /*!
     *  Get the description for the connection item.
     *
     *      @return The description of the connection item.
     */
    virtual QString description() const;
        
    /*!
     *  Get the contained interconnection.
     *
     *      @return The interconnection contained within this item.
     */
    QSharedPointer<Interconnection> getInterconnection();

    /*!
     *  Get the route of the connection.
     *
     *      @return The connection route.
     */
    QSharedPointer<ConnectionRoute> getRouteExtension() const;

    /*!
     *  Toggles the connection between normal and off-page.
     */
    virtual void toggleOffPage();

    /*!
     *  Set the routing of the interconnection
     *
     *      @param [in] path    List of points for the path.
     */
    virtual void setRoute(QList<QPointF> path);

    /*!
     *  Returns true if the connection is a bus connection. Returns false if it isn't (i.e. it is an ad-hoc
     *  connection).
     */
    bool isBus() const;

    /*!
     *  Get the type of this connection.
     *
     *      @return HW connection.
     */
    int type() const { return Type; }

    /*!
     *  Change the component reference of a contained interface.
     *
     *      @param [in] oldName     The old component reference.
     *      @param [in] newName     The new component reference.
     */
    virtual void changeConnectionComponentReference(QString const& oldName, QString const& newName);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The contained interconnection.
    QSharedPointer<Interconnection> interconnection_;

    //! The route of the interconnection.
    QSharedPointer<ConnectionRoute> route_;
};

#endif // HWCONNECTION_H
