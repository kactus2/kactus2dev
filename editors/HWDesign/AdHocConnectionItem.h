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

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

class AdHocConnection;
class ConnectionRoute;

//-----------------------------------------------------------------------------
//! Graphical item to represent ad-hoc connections in a design.
//-----------------------------------------------------------------------------
class AdHocConnectionItem : public GraphicsConnection
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_CONNECTION };
  
    explicit AdHocConnectionItem(QPointF const& p1, QVector2D const& dir1, QPointF const& p2,
        QVector2D const& dir2, DesignDiagram* parent);

    AdHocConnectionItem(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
        QSharedPointer<AdHocConnection> adhocConnection, QSharedPointer<ConnectionRoute> route,
        DesignDiagram* parent);

    virtual ~AdHocConnectionItem() = default;

    // Disable copying.
    AdHocConnectionItem(AdHocConnectionItem const& rhs) = delete;
    AdHocConnectionItem& operator=(AdHocConnectionItem const& rhs) = delete;

    virtual void setName(QString const& name);

    virtual QString name() const;

    virtual void setDescription(QString const& description);

    virtual QString description() const;

    QSharedPointer<AdHocConnection> getAdHocConnection();

    QSharedPointer<ConnectionRoute> getRouteExtension() const;

    /*! 
     *  Connects the ends of the interconnection.
     *
     *      @remarks There must be valid endpoints below the start and end positions
     *               of the interconnection.
     */
    virtual bool connectEnds();

    /*!
     *  Toggles the connection between normal and off-page.
     */
    virtual void toggleOffPage();

    /*! Set the routing of the interconnection
     *
     */
    virtual void setRoute(QList<QPointF> path);

    /*!
     *  Returns true if the connection is a bus connection. Returns false if it isn't (i.e. it is an ad-hoc
     *  connection).
     */
    bool isBus() const;

    /*!
     *  Get the type of this connection item.
     *
     *      @return Ad hoc connection item type.
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

    QSharedPointer<AdHocConnection> adHocConnection_ = nullptr;

    QSharedPointer<ConnectionRoute> route_ = nullptr;
};

#endif // ADHOCCONNECTIONITEM_H
