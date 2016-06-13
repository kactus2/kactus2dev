//-----------------------------------------------------------------------------
// File: AdHocConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.10.2015
//
// Description:
// <Short description of the class/file contents>
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


class AdHocConnectionItem : public GraphicsConnection
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_CONNECTION };
  
    AdHocConnectionItem(QPointF const& p1, QVector2D const& dir1,
                           QPointF const& p2, QVector2D const& dir2,
                           QString const& displayName,
                           QString const& description,
                           DesignDiagram* parent);

    AdHocConnectionItem(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
        QSharedPointer<AdHocConnection> adhocConnection, QSharedPointer<ConnectionRoute> route,
        DesignDiagram* parent);

    virtual ~AdHocConnectionItem();

    virtual void setName(QString const& name);

    virtual QString name() const;

    virtual void setDescription(QString const& description);

    virtual QString description() const;
    QSharedPointer<AdHocConnection> getAdHocConnection();

    QSharedPointer<ConnectionRoute> getRouteExtension() const;

    /*!
     *  Sets the bus width label visible/invisible.
     *
     *      @param [in] visible If true, the label is set visible. Otherwise false.
     */
    void setBusWidthVisible(bool visible);

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

    /*! \brief Set the routing of the interconnection
     *
     */
    virtual void setRoute(QList<QPointF> path);

    /*! \brief Update the end positions when connected endpoints are moved
     *
     */
    virtual void updatePosition();

    /*!
     *  Sets the left bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint is concerned. Either 1 or 2.
     *      @param [in] leftBound     The left bound.
     */
    void setAdHocLeftBound(int endpointIndex, QString const& leftBound);

    /*!
     *  Sets the right bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint is concerned. Either 1 or 2.
     *      @param [in] rightBound    The right bound.
     */
    void setAdHocRightBound(int endpointIndex, QString const& rightBound);

    /*!
     *  Returns the left bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint is concerned. Either 1 or 2.
     *
     *      @return The left bound of the endpoint.
     */
    QString getAdHocLeftBound(int endpointIndex) const;

    /*!
     *  Returns the left bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint data is concerned. Either 1 or 2.
     *
     *      @return The right bound of the endpoint.
     */
    QString getAdHocRightBound(int endpointIndex) const;

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
    //! Structure for storing ad-hoc port bounds.
    //-----------------------------------------------------------------------------
    struct AdHocPortBound
    {
        QString left_;
        QString right_;

        /*!
         *  Default constructor.
         */
        AdHocPortBound() : left_(), right_()
        {
        }
    };

    /*!
     *  Updates the width label based on the endpoints.
     */
    void updateWidthLabel();

    /*!
     *  Finds the position for bus width label.
     *     
     *      @return The optimal position of the bus width label.
     */
    QPointF findWidthLabelPosition() const;

    /*!
     *  Finds the position for bus width label above the longest horizontal segment.
     *     
     *      @return The optimal position of the bus width label.
     */
    QPointF findWidthLabelVertically() const;

    /*!
     *  Finds the position for bus width label next to the longest vertical segment.
     *     
     *      @return The optimal position of the bus width label.
     */
    QPointF findWidthLabelHorizontally() const;

    /*!
     *  Sets th default font for the bus width label.
     */
    void setWidthLabelDefaultFont();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection width label.
    QGraphicsTextItem* widthLabel_;

    //! The ad-hoc port bounds.
    AdHocPortBound portBounds_[2];

    QSharedPointer<AdHocConnection> adHocConnection_;

    QSharedPointer<ConnectionRoute> route_;
};

#endif // ADHOCCONNECTIONITEM_H
