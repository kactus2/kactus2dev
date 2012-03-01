/* 
 *
 * 		filename: diagraminterconnection.h
 */

#ifndef DIAGRAMINTERCONNECTION_H
#define DIAGRAMINTERCONNECTION_H

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

class DiagramConnectionEndPoint;
class BlockDiagram;

/*! \brief DiagramInterconnection represents graphically an IP-XACT interconnection
 *
 */
class DiagramInterconnection : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 2 };

    //-----------------------------------------------------------------------------
    //! RoutingMode enumeration.
    //-----------------------------------------------------------------------------
    enum RoutingMode
    {
        ROUTING_MODE_NORMAL = 0,  //!< The interconnection uses rectlinear routing.
        ROUTING_MODE_OFFPAGE      //!< The interconnection goes straight from the location of the first
                                  //!< end point to the location of the second end point.
    };

    /*!
     *  Constructor.
     */
    DiagramInterconnection(DiagramConnectionEndPoint *endPoint1,
                           DiagramConnectionEndPoint *endPoint2,
                           bool autoConnect,
                           const QString &displayName,
                           const QString &description,
                           BlockDiagram* parent);

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     */
    DiagramInterconnection(QPointF p1, QVector2D const& dir1,
                           QPointF p2, QVector2D const& dir2,
                           const QString &displayName,
                           const QString &description,
                           BlockDiagram* parent);

    virtual ~DiagramInterconnection();

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
     *  Sets the bus width label visible/invisible.
     *
     *      @param [in] visible If true, the label is set visible. Otherwise false.
     */
    void setBusWidthVisible(bool visible);

    /*! 
     *  Connects the ends of the interconnection.
     *
     *      @remarks There must be valid end points below the start and end positions
     *               of the interconnection.
     */
    bool connectEnds();

    /*! 
     *  Disconnects the ends of the interconnection.
     */
    void disconnectEnds();

    /*! \brief Set the routing of the interconnection
     *
     */
    void setRoute(QList<QPointF> path);

    /*! \brief Returns the route of this interconnection
     *
     */
    const QList<QPointF> &route();

    /*! \brief Update the end positions when connected end points are moved
     *
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

    /*! \brief Return the name of this interconnection
     *
     */
    QString name() const;

	/*! \brief Set the name for the connection.
	 *
	 * \param name The name to set for the connection.
	 *
	*/
	void setName(const QString& name);

	/*! \brief Get the description of the connection.
	 *
	 *
	 * \return QString contains the description.
	*/
	QString description() const;

	/*! \brief Set the description for the connection.
	 *
	 * \param description contains the description to set.
	 *
	*/
	void setDescription(const QString& description);

    /*! \brief Return the first end point connected
     *
     */
    DiagramConnectionEndPoint *endPoint1() const;

    /*! \brief Return the second end point connected
     *
     */
    DiagramConnectionEndPoint *endPoint2() const;

    /*!
     *  Sets the left bound of the ad-hoc end point involved in the connection.
     *
     *      @param [in] endPointIndex Which endpoint is concerned. Either 1 or 2.
     *      @param [in] leftBound     The left bound.
     */
    void setAdHocLeftBound(int endPointIndex, int leftBound);

    /*!
     *  Sets the right bound of the ad-hoc end point involved in the connection.
     *
     *      @param [in] endPointIndex Which endpoint is concerned. Either 1 or 2.
     *      @param [in] rightBound    The right bound.
     */
    void setAdHocRightBound(int endPointIndex, int rightBound);

    /*!
     *  Returns the left bound of the ad-hoc end point involved in the connection.
     *
     *      @param [in] endPointIndex Which endpoint is concerned. Either 1 or 2.
     *
     *      @return The left bound of the end point.
     */
    int getAdHocLeftBound(int endPointIndex) const;

    /*!
     *  Returns the left bound of the ad-hoc end point involved in the connection.
     *
     *      @param [in] endPointIndex Which endpoint data is concerned. Either 1 or 2.
     *
     *      @return The right bound of the end point.
     */
    int getAdHocRightBound(int endPointIndex) const;

    /*!
     *  Returns true if the connection is a bus connection. Returns false if it isn't (i.e. it is an ad-hoc
     *  connection).
     */
    bool isBus() const;

    int type() const { return Type; }

signals:
    //! Signals that the interconnection has changed.
    void contentChanged();

	//! \brief This signal is emitted when this connection is destroyed.
	void destroyed(DiagramInterconnection* connection);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    enum SelectionType {END, SEGMENT, NONE};
    static qreal const MIN_LENGTH;

    //-----------------------------------------------------------------------------
    //! Structure for storing ad-hoc port bounds.
    //-----------------------------------------------------------------------------
    struct AdHocPortBound
    {
        int left_;
        int right_;

        /*!
         *  Default constructor.
         */
        AdHocPortBound() : left_(-1), right_(-1)
        {
        }
    };

    /*!
     *  Sets the default graphics item settings.
     */
    void setItemSettings();

    void createRoute(QPointF p1, QPointF p2, QVector2D const& dir1, QVector2D const& dir2);
    void createRoute(DiagramConnectionEndPoint* endPoint1, DiagramConnectionEndPoint* endPoint2);

    void simplifyPath();

    /*!
     *  Sets the default color based on the routing mode.
     */
    void setDefaultColor();

    /*!
     *  Updates the name of the interconnection.
     */
    void updateName();

    /*!
     *  Updates the width label based on the end points.
     */
    void updateWidthLabel();

    /*!
     *  Calculates the total bus width.
     *
     *      @return The bus width.
     */
    int calculateBusWidth() const;

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

    //! The parent block diagram.
    BlockDiagram* parent_;

    QString name_;
	QString description_;
    DiagramConnectionEndPoint *endPoint1_;
    DiagramConnectionEndPoint *endPoint2_;
    QList<QPointF> pathPoints_;
    int selected_;
    SelectionType selectionType_;
    QList<QPointF> oldRoute_;

    //! The routing mode.
    RoutingMode routingMode_;

    //! The connection width label.
    QGraphicsTextItem* widthLabel_;

    //! The ad-hoc port bounds.
    AdHocPortBound portBounds_[2];
};

#endif // DIAGRAMINTERCONNECTION_H
