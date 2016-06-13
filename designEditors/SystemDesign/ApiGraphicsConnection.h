//-----------------------------------------------------------------------------
// File: ApiGraphicsConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

class ApiInterconnection;
class ConnectionRoute;


//-----------------------------------------------------------------------------
//! A graphical representation of a Kactus2 API interconnection.
//-----------------------------------------------------------------------------
class ApiGraphicsConnection : public GraphicsConnection
{
    Q_OBJECT

public:

    //! Type of the API interconnection.
    enum { Type = GFX_TYPE_SW_API_CONNECTION };

    /*!
     *  The constructor.
     *
     *      @param [in] endpoint1       The start point of the connection.
     *      @param [in] endpoint2       The end point of the connection.
     *      @param [in] autoConnect     If true, automatically connects the two end points.
     *      @param [in] name            Name of the connection.
     *      @param [in] displayName     Display name of the connection.
     *      @param [in] description     Description of the connection.
     *      @param [in] parent          The design diagram containing the connection.
     */
    ApiGraphicsConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2, bool autoConnect,
                          QString const& name, QString const& displayName, QString const& description,
                          DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     *
     *      @param [in] p1              The start point of the connection.
     *      @param [in] dir1            Direction of the start point.
     *      @param [in] p2              The end point of the connection.
     *      @param [in] dir2            Direction of the end point.
     *      @param [in] displayName     Display name of the connection.
     *      @param [in] description     Description of the connection.
     *      @param [in] parent          The design diagram containing the connection.
     */
    ApiGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
                          QString const& displayName, QString const& description, DesignDiagram* parent);

    /*!
     *  The constructor containing the API interconnection.
     *
     *      @param [in] endpoint1           The start point of the connection.
     *      @param [in] endpoint2           The end point of the connection.
     *      @param [in] apiInterconnection  The API interconnection being connected.
     *      @param [in] route               The route of the connection.
     *      @param [in] autoConnect         If true, automatically connects the two end points.
     *      @param [in] parent              The design diagram containing the connection.
     */
    ApiGraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                          QSharedPointer<ApiInterconnection> apiInterconnection,
                          QSharedPointer<ConnectionRoute> route, bool autoConnect, DesignDiagram* parent);

    /*!
     *  The destructor.
     */
    virtual ~ApiGraphicsConnection();

    /*!
     *  Set the name of the connection.
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
     *  Set the description of the connection.
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
     *  Get the connected API interconnection.
     *
     *      @return The connected API interconnection.
     */
    QSharedPointer<ApiInterconnection> getApiInterconnection();

    /*!
     *  Get the route used by the connection.
     *
     *      @return The route of the connection.
     */
    QSharedPointer<ConnectionRoute> getRouteExtension() const;

    /*!
     *  Toggles the connection between normal and off-page.
     */
    virtual void toggleOffPage();

    /*!
     *  Check if the connection is off-page.
     *
     *      @return True if the connection is off-page, otherwise false.
     */
    bool isOffPage() const;

    /*!
     *  Set the routing of the interconnection
     *
     *      @param [in] path    List of points forming the connection route.
     */
    virtual void setRoute(QList<QPointF> path);

    /*!
     *  Get the type of the graphics item.
     *
     *      @return GFX_TYPE_SW_API_CONNECTION.
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

    //! The connected API interconnection.
    QSharedPointer<ApiInterconnection> apiInterconnection_;

    //! Route used by the connection.
    QSharedPointer<ConnectionRoute> route_;
};

#endif // APIGRAPHICSCONNECTION_H
