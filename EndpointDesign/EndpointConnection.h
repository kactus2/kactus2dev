//-----------------------------------------------------------------------------
// File: EndpointConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2011
//
// Description:
// Endpoint connection class.
//-----------------------------------------------------------------------------

#ifndef ENDPOINTCONNECTION_H
#define ENDPOINTCONNECTION_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QList>
#include <QPointF>

class EndpointItem;

//-----------------------------------------------------------------------------
//! EndpointConnection class.
//-----------------------------------------------------------------------------
class EndpointConnection : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 11 };

    /*!
     *  Constructor which creates a connection between two MCAPI endpoints.
     */
    EndpointConnection(EndpointItem* endpoint1, EndpointItem* endpoint2,
                       bool autoConnect = true, QGraphicsItem* parent = 0);

    /*!
     *  Constructor which creates an open-ended endpoint connection.
     */
    EndpointConnection(QPointF p1, QVector2D const& dir1,
                       QPointF p2, QVector2D const& dir2,
                       QGraphicsItem* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~EndpointConnection();

    /*!
     *  Sets the route from a collection of points.
     *
     *      @param [in] path The route to set.
     */
    void setRoute(QList<QPointF> path);

    /*! 
     *  Updates the end positions when connected endpoints are moved.
     */
    void updatePosition();

    /*! 
     *  Connects the ends of the interconnection.
     *
     *      @remarks There must be valid end points below the start and end positions
     *               of the interconnection.
     */
    bool connectEnds();

    /*!
     *  Returns the name of the connection.
     */
    QString const& getName() const;

    /*!
     *  Returns the route as a collection of points.
     */
    QList<QPointF> const& getRoute() const;

    /*!
     *  Returns the first connected endpoint or null if the connection is incomplete.
     */
    EndpointItem* getEndpoint1();

    /*!
     *  Returns the second connected endpoint or null if the connection is incomplete.
     */
    EndpointItem* getEndpoint2();

    int type() const { return Type; }

signals:
    //! Signals that the interconnection has changed.
    void contentChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    // Disable copying.
    EndpointConnection(EndpointConnection const& rhs);
    EndpointConnection& operator=(EndpointConnection const& rhs);

    /*!
     *  Sets the common graphics item settings.
     */
    void setItemSettings();

    /*!
     *  Creates a route between two points.
     *
     *      @param [in] p1    The start point.
     *      @param [in] p2    The end point.
     *      @param [in] dir1  The required line direction on the start point.
     *      @param [in] dir2  The required line direction on the end point.
     */
    void createRoute(QPointF p1, QPointF p2, QVector2D const& dir1, QVector2D const& dir2);

    /*!
     *  Creates a route between two MCAPI endpoints.
     *
     *      @param [in] endpoint1 The first endpoint.
     *      @param [in] endpoint2 The second endpoint.
     */
    void createRoute(EndpointItem* endpoint1, EndpointItem* endpoint2);

    /*!
     *  Simplifies the path removing adjacent parallel segments.
     */
    void simplifyPath();

    /*!
     *  Disconnects the endpoint of the connection.
     */
    void disconnectEnds();

    /*!
     *  Updates the name of the connection.
     */
    void updateName();

    //-----------------------------------------------------------------------------
    //! SelectionType enumeration.
    //-----------------------------------------------------------------------------
    enum SelectionType
    {
        END,
        SEGMENT,
        NONE
    };

    static qreal const MIN_LENGTH;
    static qreal const MIN_START_LENGTH;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the connection.
    QString name_;

    //! The endpoints of the connection.
    EndpointItem* endpoint1_;
    EndpointItem* endpoint2_;

    //! All points along the path from one endpoint to the other.
    QList<QPointF> pathPoints_;

    int selected_;
    SelectionType selectionType_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTCONNECTION_H
