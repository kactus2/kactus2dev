//-----------------------------------------------------------------------------
// File: GraphicsConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.6.2012
//
// Description:
// Base class for graphical connections.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCONNECTION_H
#define GRAPHICSCONNECTION_H

#include <designEditors/common/Associable.h>

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

class DesignDiagram;

//-----------------------------------------------------------------------------
//! Base class for graphical connections.
//-----------------------------------------------------------------------------
class GraphicsConnection : public QObject, public QGraphicsPathItem, public Associable
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! RoutingMode enumeration.
    //-----------------------------------------------------------------------------
    enum RoutingMode
    {
        ROUTING_MODE_NORMAL = 0,  //!< The connection uses rectlinear routing.
        ROUTING_MODE_OFFPAGE      //!< The connection goes straight from the location of the first
                                  //!< endpoint to the location of the second endpoint.
    };

    /*!
     *  Constructor.
     */
    GraphicsConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2,
                       bool autoConnect, QString const& name, 
                       QString const& displayName, QString const& description,
                       DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended connection.
     */
    GraphicsConnection(QPointF const& p1, QVector2D const& dir1,
                       QPointF const& p2, QVector2D const& dir2,
                       QString const& displayName, QString const& description,
                       DesignDiagram* parent);

    /*!
     *  Destructor.
     */
    virtual ~GraphicsConnection();

    /*!
     *  Sets the routing mode.
     *
     *      @param [in] mode The routing mode.
     */
    void setRoutingMode(RoutingMode mode);

    /*!
     *  Sets the line width.
     *
     *      @param [in] width The line width in pixels.
     */
    void setLineWidth(int width);

    /*! 
     *  Connects the ends of the connection.
     *
     *      @remarks There must be valid endpoints below the start and end positions
     *               of the connection.
     */
    virtual bool connectEnds();

    /*!
     *  Sets the first endpoint. If the connection is already connected, the connection
     *  is disconnected from the old endpoint and connected to the new given endpoint, updating the
     *  route too.
     *
     *      @param [in] endpoint1 The first endpoint to set.
     */
    void setEndpoint1(ConnectionEndpoint* endpoint1);

    /*!
     *  Sets the first endpoint. If the connection is already connected, the connection
     *  is disconnected from the old endpoint and connected to the new given endpoint, updating the
     *  route too.
     *
     *      @param [in] endpoint2 The second endpoint to set.
     */
    void setEndpoint2(ConnectionEndpoint* endpoint2);

    /*!
     *  Validates the connection and draws it in red if it is not valid.
     */
    void validate();

    /*! 
     *  Disconnects the ends of the connection.
     */
    void disconnectEnds();

    /*!
     *  Toggles the connection between normal and off-page.
     */
    void toggleOffPage();

    /*!
     *  Sets the routing of the connection.
     *
     *      @param [in] path The route to set.
     */
    virtual void setRoute(QList<QPointF> path);

    /*!
     *  Returns the route of this connection.
     */
    QList<QPointF> const& route() const;

    /*! 
     *  Updates the end positions when connected endpoints are moved.
     */
    virtual void updatePosition();

    /*!
     *  Applies clearance algorithm to prevent connections from intersecting.
     */
    void fixOverlap();

    /*!
     *  Updates the name of the connection according to the default formatting.
     */
    QString createDefaultName() const;

    /*!
     *  Begins the position update of the connection.
     */
    void beginUpdatePosition();

    /*!
     *  Ends the position update of the connection and creates an undo command.
     *
     *      @param [in] parent The parent command.
     *
     *      @return The created (child) command.
     */
    QUndoCommand* endUpdatePosition(QUndoCommand* parent);

    /*!
     *  Set the name for the connection.
	 *
	 *      @param [in] name The name to set for the connection.
	 */
	void setName(QString const& name);

    /*!
     *  Sets the description for the connection.
	 *
     *      @param [in] description The description to set.
	 */
	void setDescription(const QString& description);

    /*!
     *  Sets the imported state.
     *
     *      @param [in] imported If true, the connection is an imported one.
     */
    void setImported(bool imported);

    /*!
     *  Returns the name of this connection
     */
    QString const& name() const;

	/*!
     *  Returns the description of the connection.
	 */
	QString const& description() const;

    /*!
     *  Returns true if the connection is an imported one.
     */
    bool isImported() const;

	/*!
     *  Returns the first endpoint connected.
     */
    ConnectionEndpoint* endpoint1() const;

    /*!
     *  Return the second endpoint connected.
     */
    ConnectionEndpoint* endpoint2() const;

    /*!
     *  Returns the type of the connection.
     */
    ConnectionEndpoint::EndpointType getConnectionType() const;

    /*!
     *  Returns the used routing mode.
     */
    RoutingMode getRoutingMode() const;

    /*!
     *  Returns true if the connection is invalid.
     */
    bool isInvalid() const;

    /*!
     *  Returns true if the connection uses the default name.
     */
    bool hasDefaultName() const;

    /*!
     *  Defines the connection point for associations in scene coordinates.
     *     
     *      @param [in] otherEnd   The position of the other end connection point.
     *
     *      @return The connection point of the item.
     */
    virtual QPointF connectionPoint(QPointF const& otherEnd) const;


signals:
    //! Signals that the connection has changed.
    void contentChanged();

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

    //! \brief This signal is emitted when this connection is destroyed.
    void destroyed(GraphicsConnection* connection);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    //-----------------------------------------------------------------------------
    //! Selection type enumeration.
    //-----------------------------------------------------------------------------
    enum SelectionType
    {
        END,
        SEGMENT,
        NONE
    };

    //! Minimum length for a line segment.
    static float const MIN_LENGTH;
    static float const MIN_START_LENGTH;

    /*!
     *  Sets the default graphics item settings.
     */
    void setItemSettings();

    /*!
     *  Creates an optimal route from point p1 to point p2, considering the given start and exit directions.
     */
    void createRoute(QPointF p1, QPointF p2, QVector2D const& dir1, QVector2D const& dir2);

    /*!
     *  Paints the current path of the connection.     
     */
    void paintConnectionPath();

    /*!
     *  Creates an optimal route between the given endpoints.
     */
    void createRoute(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2);

    /*!
     *  Simplifies the path by removing parallel consecutive segments.
     */
    void simplifyPath();

    /*!
     *  Sets the default color based on the routing mode.
     */
    void setDefaultColor();

    /*!
     *  Draws specific helper graphics for overlapping graphics items.
     *
     *      @param [in] painter The painter.
     */
    void drawOverlapGraphics(QPainter* painter);

    /*!
     *  Draws a "gap" to a line with the currently selected pen.
     *
     *      @param [in] painter The painter.
     *      @param [in] line    The line to which to draw the gap.
     *      @param [in] pt      The intersection point where to draw the gap.
     */
    void drawLineGap(QPainter* painter, QLineF const& line, QPointF const& pt);

    //-----------------------------------------------------------------------------
    //! Segment bound structure.
    //-----------------------------------------------------------------------------
    struct SegmentBound
    {
        qreal minX;
        qreal minY;
        qreal maxX;
        qreal maxY;

        /*!
         *  Constructor.
         */
        SegmentBound(QPointF const& p1, QPointF const& p2)
            : minX(qMin(p1.x(), p2.x())),
              minY(qMin(p1.y(), p2.y())),
              maxX(qMax(p1.x(), p2.x())),
              maxY(qMax(p1.y(), p2.y()))
        {
        }
    };

    /*!
     *  Creates segment bounds from the existing connections.
     *
     *      @param [out] verBounds    The list of vertical segment bounds.
     *      @param [out] horizontalBounds  The list of horizontal bounds.
     */
    void createSegmentBounds(QList<SegmentBound>& verBounds, QList<SegmentBound>& horizontalBounds);

    /*!
     *  Retrieves the minimum and maximum X coordinates for the horizontal segment identified by index i.
     *
     *      @param [in]  i      The segment index.
     *      @param [out] minY   The minimum X coordinate.
     *      @param [out] maxY   The maximum X coordinate.
     */
    void getSegmentLimitsX(int i, qreal& minX, qreal& maxX);

    /*!
     *  Retrieves the minimum and maximum Y coordinates for the vertical segment identified by index i.
     *
     *      @param [in]  i      The segment index.
     *      @param [out] minY   The minimum Y coordinate.
     *      @param [out] maxY   The maximum Y coordinate.
     */
    void getSegmentLimitsY(int i, qreal& minY, qreal& maxY);

    /*!
     *  Searches for a vertical segment overlap.
     *
     *      @param [in] verBounds  The collection of vertical segment bounds to test against.
     *      @param [in] bounds     The vertical segment being tested for overlap.
     *
     *      @return The index of the first found overlapping bound in the collection, or -1 if not found.
     */
    int findVerticalSegmentOverlap(QList<SegmentBound> const& verBounds, SegmentBound const& bounds);
    
    /*!
     *  Fixes vertical segment overlap issues for the vertical segment identified by index i.
     *
     *      @param [in] verBounds  The collection of vertical segment bounds to test against.
     *      @param [in] i          The index of the segment to fix.
     *
     *      @return True if modifications were made; false if nothing was changed.
     */
    bool fixVerticalSegmentClearance(QList<SegmentBound> const& verBounds, int i);

    /*!
     *  Searches for a horizontal segment overlap.
     *
     *      @param [in] horBounds  The collection of horizontal segment bounds to test against.
     *      @param [in] bounds     The horizontal segment being tested for overlap.
     *
     *      @return The index of the first found overlapping bound in the collection, or -1 if not found.
     */
    int findHorizontalSegmentOverlap(QList<SegmentBound> const& horBounds, SegmentBound const& bounds);
    
    /*!
     *  Fixes horizontal segment overlap issues for the horizontal segment identified by index i.
     *
     *      @param [in] horBounds  The collection of horizontal segment bounds to test against.
     *      @param [in] i          The index of the segment to fix.
     *
     *      @return True if modifications were made; false if nothing was changed.
     */
    bool fixHorizontalSegmentClearance(QList<SegmentBound> const& horBounds, int i);

    /*!
     *  Tests for horizontal overlap between two segment bounds.
     *
     *      @param [in] bound1 The first segment bound.
     *      @param [in] bound2 The second segment bound.
     *
     *      @return True if the bounds overlap by X coordinate.
     */
    static bool testSegmentOverlapX(SegmentBound const& bound1, SegmentBound const& bound2);

    /*!
     *  Tests for vertical overlap between two segment bounds.
     *
     *      @param [in] bound1 The first segment bound.
     *      @param [in] bound2 The second segment bound.
     *
     *      @return True if the bounds overlap by Y coordinate.
     */
    static bool testSegmentOverlapY(SegmentBound const& bound1, SegmentBound const& bound2);

    /*!
     *  Sort operator for sorting segment bounds by X coordinate.
     */
    static bool sortBoundsByX(SegmentBound const& lhs, SegmentBound const& rhs);

    /*!
     *  Sort operator for sorting segment bounds by Y coordinate.
     */
    static bool sortBoundsByY(SegmentBound const& lhs, SegmentBound const& rhs);
   

    /*!
     *  Updates the endpoint direction to given direction.
     *
     *      @param [in] endpoint   The endpoint to update.
     *      @param [in] dir        The direction to change to.
     *
     *      @return 
     */
    void updateEndpointDirection(ConnectionEndpoint* endpoint, QVector2D dir);


    /*!
     *  Finds the point closest to the given point from a set of points.
     *
     *      @param [in] sourcePoints    The points to search through.
     *      @param [in] destination     The point get closest to.
     *
     *      @return The point closest to destination.
     */
    QPointF findClosestPoint(QList<QPointF> const& sourcePoints, QPointF const& destination) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent diagram.
    DesignDiagram* parent_;

    //! The name of the connection.
    QString name_;

    //! The description of the connection.
    QString description_;

    //! The first endpoint.
    ConnectionEndpoint* endpoint1_;

    //! The second endpoint.
    ConnectionEndpoint* endpoint2_;

    //! The route path points.
    QList<QPointF> pathPoints_;

    //! The index of the segment that is currently selected.
    int selected_;

    //! The type of selection.
    SelectionType selectionType_;

    //! The old route for creating undo commands.
    QList<QPointF> oldRoute_;

    //! The routing mode.
    RoutingMode routingMode_;

    //! If true, the connection is an imported one.
    bool imported_;

    //! The default color.
    bool invalid_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCONNECTION_H
