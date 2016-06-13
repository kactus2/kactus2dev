//-----------------------------------------------------------------------------
// File: ComGraphicsConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

class ComInterconnection;
class ConnectionRoute;

//-----------------------------------------------------------------------------
//! A graphical representation of a Kactus2 com interconnection.
//-----------------------------------------------------------------------------
class ComGraphicsConnection : public GraphicsConnection
{
    Q_OBJECT

public:

    //! The type of the COM connection item.
    enum { Type = GFX_TYPE_SW_COM_CONNECTION };

    /*!
     *  The constructor.
     *
     *      @param [in] endpoint1       The start point of the connection.
     *      @param [in] endpoint2       The end point of the connection.
     *      @param [in] autoConnect     If true, automatically connects the end points.
     *      @param [in] name            Name of the connection.
     *      @param [in] displayName     Display name of the connection.
     *      @param [in] description     Description of the connection.
     *      @param [in] parent          The design diagram containing the connection.
     */
    ComGraphicsConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2, bool autoConnect,
                          QString const& name, QString const& displayName, QString const& description,
                          DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     *
     *      @param [in] p1              The start point.
     *      @param [in] dir1            Direction of the start point.
     *      @param [in] p2              The end point.
     *      @param [in] dir2            Direction of the end point.
     *      @param [in] displayName     Display name of the connection.
     *      @param [in] description     Description of the connection.
     *      @param [in] parent          The design diagram containing the connection.
     */
    ComGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
                          QString const& displayName, QString const& description, DesignDiagram* parent);

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
    virtual ~ComGraphicsConnection();

    /*!
     *  Set the name of the COM connection.
     *
     *      @param [in] name    The new name of the connection.
     */
    virtual void setName(QString const& name);

    /*!
     *  Get the name of the connection.
     *
     *      @return The name of the connection.
     */
    virtual QString name() const;

    /*!
     *  Set the description of the COM connection.
     *
     *      @param [in] description     The new description of the connection.
     */
    virtual void setDescription(QString const& description);

    /*!
     *  Get the description of the connection.
     *
     *      @return The description of the connection.
     */
    virtual QString description() const;

    /*!
     *  Get the connected COM interconnection.
     *
     *      @return The connected COM interconnection.
     */
    QSharedPointer<ComInterconnection> getComInterconnection();

    /*!
     *  Get the route used by the connection.
     *
     *      @return The route used by the connection.
     */
    QSharedPointer<ConnectionRoute> getRouteExtension() const;

    /*!
     *  Toggles the connection between normal and off-page.
     */
    virtual void toggleOffPage();

    /*!
     *  Check if the connection is off-page.
     *
     *      @return True, if the connection is off-page, otherwise false.
     */
    bool isOffPage() const;

    /*!
     *  Set the routing of the interconnection.
     *
     *      @parameter [in] path    List containing the points used as the route for the connection.
     */
    virtual void setRoute(QList<QPointF> path);

    /*!
     *  Get the type of the graphical COM connection.
     *
     *      @return The type of the COM connection item.
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

    //! The connected COM interconnection.
    QSharedPointer<ComInterconnection> comInterconnection_;

    //! The route used by the connection.
    QSharedPointer<ConnectionRoute> route_;
};

#endif // COMGRAPHICSCONNECTION_H
