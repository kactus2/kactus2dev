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

/*! \brief HWConnection represents graphically an IP-XACT interconnection
 *
 */
class HWConnection : public GraphicsConnection
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERCONNECTION };

    /*!
     *  Constructor.
     */
    HWConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2,
                           bool autoConnect, QString const& name, QString const& displayName,
                           QString const& description, DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     */
    HWConnection(QPointF const& p1, QVector2D const& dir1,
                           QPointF const& p2, QVector2D const& dir2,
                           QString const& displayName,
                           QString const& description,
                           DesignDiagram* parent);

    HWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
        QSharedPointer<Interconnection> interconnection, QSharedPointer<ConnectionRoute> route,
        DesignDiagram* parent);

    virtual ~HWConnection();

    virtual void setName(QString const& name);

    virtual QString name() const;

    virtual void setDescription(QString const& description);

    virtual QString description() const;

        
    QSharedPointer<Interconnection> getInterconnection();

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
     *  Returns true if the connection is a bus connection. Returns false if it isn't (i.e. it is an ad-hoc
     *  connection).
     */
    bool isBus() const;

    int type() const { return Type; }

private:

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
     *  Calculates the total bus width.
     *
     *      @return The bus width.
     */
    int calculateBusWidth() const;

    /*!
     *  Sets th default font for the bus width label.
     */
    void setWidthLabelDefaultFont();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection width label.
    QGraphicsTextItem* widthLabel_;

    QSharedPointer<Interconnection> interconnection_;

    QSharedPointer<ConnectionRoute> route_;
};

#endif // HWCONNECTION_H
