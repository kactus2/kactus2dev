//-----------------------------------------------------------------------------
// File: GraphicsConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 5.6.2012
//
// Description:
// Base class for graphical connections.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCONNECTION_H
#define GRAPHICSCONNECTION_H

#include <editors/common/Association/Associable.h>

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
                       QString const& displayName,
                       QString const& description,
                       DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended connection.
     */
    GraphicsConnection(QPointF const& p1, QVector2D const& dir1,
                       QPointF const& p2, QVector2D const& dir2,
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
    virtual void toggleOffPage();

    /*!
     *  Sets the routing of the connection.
     *
     *      @param [in] path The route to set.
     */
    virtual void setRoute(QList<QPointF> path);

    /*!
     *  Returns the route of this connection.
     */
    QList<QPointF> route() const;

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
	virtual void setName(QString const& name);

    /*!
     *  Sets the description for the connection.
	 *
     *      @param [in] description The description to set.
	 */
	virtual void setDescription(QString const& description);

    /*!
     *  Sets the imported state.
     *
     *      @param [in] imported If true, the connection is an imported one.
     */
    void setImported(bool imported);

    /*!
     *  Returns the name of this connection
     */
    virtual QString name() const;

	/*!
     *  Returns the description of the connection.
	 */
	virtual QString description() const;

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

    /*!
     *  Change the component reference of a contained interface.
     *
     *      @param [in] oldName     The old component reference.
     *      @param [in] newName     The new component reference.
     */
    virtual void changeConnectionComponentReference(std::string const& oldName, std::string const& newName);

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

    /*!
     *  Sets the default graphics item settings.
     */
    void setItemSettings();

    /*!
     *  Paints the current path of the connection.     
     */
    void paintConnectionPath();

    /*!
     *  Creates an optimal route between the given endpoints.
     */
    void createRoute(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2);

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
     *  Draws overlapping graphics with another connection.
     *
     *      @param [in] painter     The painter to use.
     *      @param [in] connection  The connection who overlaps with this.
     */
    void drawOverlapWithConnection(QPainter* painter, GraphicsConnection* connection);

    /*!
     *  Draws an undercrossing with another connection.
     *
     *      @param [in] painter                 The painter to use.
     *      @param [in] path                    The segment of the connection intersecting with other connection.
     *      @param [in] crossingPoint           The point where the connections intersect.
     *      @param [in] crossConnectionWidth    The withd of the intersecting connection.
     */
    void drawUndercrossing(QPainter* painter, QLineF const& path, QPointF const& crossingPoint, int crossConnectionWidth);

    /*!
     *  Draws a junction point on the connection.
     *
     *      @param [in] painter             The painter to use.
     *      @param [in] intersectionPoint   The point where the connections intersect.
     */
    void drawJunctionPoint(QPainter* painter, QPointF const& intersectionPoint);

    /*!
     *  Draws overlapping with a component item.
     *
     *      @param [in] painter     The painter to use.
     *      @param [in] item        The component item overlapping the connection.     
     */
    void drawOverlapWithComponent(QPainter* painter, QGraphicsItem* item);

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
     *  Sort operator for sorting segment bounds by X coordinate.
     */
    static bool sortBoundsByX(SegmentBound const& lhs, SegmentBound const& rhs);

    /*!
     *  Sort operator for sorting segment bounds by Y coordinate.
     */
    static bool sortBoundsByY(SegmentBound const& lhs, SegmentBound const& rhs);

    /*!
     *  Simplifies the current path.
     */
    void simplifyPath();

    /*!
     *  Retrieves the minimum and maximum X coordinates for the horizontal segment identified by index i.
     *
     *      @param [in]  i      The segment index.
     *
     *      @return The <min, max> limits for the segment.
     */
    QPair<qreal, qreal> getSegmentLimitsX(int i) const;

    /*!
     *  Retrieves the minimum and maximum Y coordinates for the vertical segment identified by index i.
     *
     *      @param [in]  i      The segment index.
     *
     *      @return The <min, max> limits for the segment.
     */
    QPair<qreal, qreal> getSegmentLimitsY(int i) const;

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
    bool testSegmentOverlapX(SegmentBound const& bound1, SegmentBound const& bound2) const;

    /*!
     *  Tests for vertical overlap between two segment bounds.
     *
     *      @param [in] bound1 The first segment bound.
     *      @param [in] bound2 The second segment bound.
     *
     *      @return True if the bounds overlap by Y coordinate.
     */
    bool testSegmentOverlapY(SegmentBound const& bound1, SegmentBound const& bound2) const;

    /*!
     *  Updates the endpoint direction to given direction.
     *
     *      @param [in] endpoint   The endpoint to update.
     *      @param [in] dir        The direction to change to.
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

    /*!
     *  Creates lines connecting the given points.
     *
     *      @param [in] points   The line endpoints.
     *
     *      @return The lines connecting the given points.
     */
    QList<QLineF> pointsToLines(QList<QPointF> points) const;

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

    //! The lines connecting the path points.
    QList<QLineF> pathLines_;

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

    //! If true, connection is being moved.
    bool positionUpdateInProcess_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCONNECTION_H
