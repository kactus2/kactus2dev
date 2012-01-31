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
                           bool autoConnect = true,
                           const QString &displayName = QString(),
                           const QString &description = QString(),
                           QGraphicsScene* scene = 0);

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     */
    DiagramInterconnection(QPointF p1, QVector2D const& dir1,
                           QPointF p2, QVector2D const& dir2,
                           const QString &displayName = QString(),
                           const QString &description = QString(),
                           QGraphicsScene* scene = 0);

    virtual ~DiagramInterconnection();

    /*!
     *  Sets the routing mode.
     *
     *      @param [in] mode The routing mode.
     */
    void setRoutingMode(RoutingMode mode);

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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

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
};

#endif // DIAGRAMINTERCONNECTION_H
