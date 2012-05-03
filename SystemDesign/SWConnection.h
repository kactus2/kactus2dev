//-----------------------------------------------------------------------------
// File: SWConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.5.2012
//
// Description:
// Graphics item for SW connections (API/COM).
//-----------------------------------------------------------------------------

#ifndef SWCONNECTION_H
#define SWCONNECTION_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

class SWConnectionEndpoint;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! Graphics item for SW connections (API/COM).
//-----------------------------------------------------------------------------
class SWConnection : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SW_CONNECTION };

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
    SWConnection(SWConnectionEndpoint *endPoint1, SWConnectionEndpoint *endPoint2,
                 bool autoConnect, const QString &displayName, const QString &description,
                 DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended connection.
     */
    SWConnection(QPointF p1, QVector2D const& dir1,
                 QPointF p2, QVector2D const& dir2,
                 const QString &displayName, const QString &description,
                 DesignDiagram* parent);

    /*!
     *  Destructor.
     */
    ~SWConnection();

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
    bool connectEnds();

    /*! 
     *  Disconnects the ends of the connection.
     */
    void disconnectEnds();

    /*!
     *  Sets the routing of the connection.
     *
     *      @param [in] path The route to set.
     */
    void setRoute(QList<QPointF> path);

    /*!
     *  Returns the route of this connection.
     */
    QList<QPointF> const& route();

    /*! 
     *  Updates the end positions when connected endpoints are moved.
     */
    void updatePosition();

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
     *  Returns the name of this connection
     */
    QString const& name() const;

	/*!
     *  Set the name for the connection.
	 *
	 *      @param [in] name The name to set for the connection.
	 */
	void setName(QString const& name);

	/*!
     *  Returns the description of the connection.
	 */
	QString const& description() const;

	/*!
     *  Sets the description for the connection.
	 *
     *      @param [in] description The description to set.
	 */
	void setDescription(const QString& description);

    /*!
     *  Returns the first endpoint connected.
     */
    SWConnectionEndpoint* endPoint1() const;

    /*!
     *  Return the second endpoint connected.
     */
    SWConnectionEndpoint* endPoint2() const;

    /*!
     *  Returns true if the connection is a bus connection. Returns false if it isn't (i.e. it is an ad-hoc
     *  connection).
     */
    bool isBus() const;

    int type() const { return Type; }

signals:
    //! Signals that the connection has changed.
    void contentChanged();

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

	//! \brief This signal is emitted when this connection is destroyed.
	void destroyed(SWConnection* connection);

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
    static qreal const MIN_LENGTH;

    /*!
     *  Sets the default graphics item settings.
     */
    void setItemSettings();

    /*!
     *  Creates an optimal route from point p1 to point p2, considering the given start and exit directions.
     */
    void createRoute(QPointF p1, QPointF p2, QVector2D const& dir1, QVector2D const& dir2);

    /*!
     *  Creates an optimal route between the given endpoints.
     */
    void createRoute(SWConnectionEndpoint* endPoint1, SWConnectionEndpoint* endPoint2);

    /*!
     *  Simplifies the path by removing parallel consecutive segments.
     */
    void simplifyPath();

    /*!
     *  Sets the default color based on the routing mode.
     */
    void setDefaultColor();

    /*!
     *  Updates the name of the connection.
     */
    void updateName();

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
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent diagram.
    DesignDiagram* parent_;

    //! The name of the connection.
    QString name_;

    //! The description of the connection.
	QString description_;

    //! The first endpoint.
    SWConnectionEndpoint *endPoint1_;

    //! The second endpoint.
    SWConnectionEndpoint *endPoint2_;

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
};

//-----------------------------------------------------------------------------

#endif // SWCONNECTION_H
