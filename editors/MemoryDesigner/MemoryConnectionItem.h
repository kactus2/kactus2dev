//-----------------------------------------------------------------------------
// File: MemoryConnectionItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.08.2016
//
// Description:
// Memory designer connection item between address spaces and memory maps.
//-----------------------------------------------------------------------------

#ifndef MEMORYCONNETIONITEM_H
#define MEMORYCONNETIONITEM_H

class MainMemoryGraphicsItem;
class ConnectivityInterface;

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
//! Memory designer connection item between address spaces and memory maps.
//-----------------------------------------------------------------------------
class MemoryConnectionItem : public QGraphicsPathItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] startItem           The start item of the connection (address space).
     *      @param [in] firstStartValue     Start address in the start item.
     *      @param [in] firstEndValue       End address in the start item.
     *      @param [in] endItem             The end item of the connection (memory map).
     *      @param [in] containingScene     Graphics scene containing the memory connection item.
     *      @param [in] yTransfer           Y transfer of the memory connection.
     *      @param [in] parent              Parent item of the connection.
     */
    MemoryConnectionItem(MainMemoryGraphicsItem* startItem,
        quint64 firstStartValue,
        quint64 firstEndValue,
        MainMemoryGraphicsItem* endItem,
        QGraphicsScene* containingScene,
        qreal yTransfer = 0,
        QGraphicsItem* parent = 0);

    // Disable copying.
    MemoryConnectionItem(MemoryConnectionItem const& rhs) = delete;
    MemoryConnectionItem& operator=(MemoryConnectionItem const& rhs) = delete;

	/*!
     *  The Destructor.
     */
    virtual ~MemoryConnectionItem() = default;

    /*!
     *  Change the position relative to the connection start item.
     *	
     *      @param [in] movementInY     Movement in the containig scene.
     */
    void moveItemBy(qreal const& movementInY);

    /*!
     *  Get the start value of the connection.
     *
     *      @return The start value of the connection.
     */
    quint64 getRangeStartValue() const;

    /*!
     *  Get the end value of the connection.
     *
     *      @return The end value of the connection.
     */
    quint64 getRangeEndValue() const;

    /*!
     *  Redraw the path of this connection.
     */
    void reDrawConnection();

    /*!
     *  Get the lowest point of the connection.
     *
     *      @return The lowest point of the connection item.
     */
    quint64 getSceneEndPoint() const;

    /*!
     *  Get the connected start memory item.
     *
     *      @return The connected start memory item.
     */
    MainMemoryGraphicsItem* getConnectionStartItem() const;

    /*!
     *  Get the connected end memory item.
     *
     *      @return The connected end memory item.
     */
    MainMemoryGraphicsItem* getConnectionEndItem() const;

    /*!
     *  Reposition range labels that are colliding with range labels of other memory connection items.
     */
    void repositionCollidingRangeLabels();

    /*!
     *  Check if the selected label collides with the range labels.
     *
     *      @param [in] label           The selected label.
     *      @param [in] fontHeight      Height of the font used in the selected label.
     *      @param [in] connectedItem   Owner of the selected label.
     *
     *      @return True, if the selected label collides with the range labels, false otherwise.
     */
    bool labelCollidesWithRanges(QGraphicsTextItem* label, qreal fontHeight,
        const MainMemoryGraphicsItem* connectedItem) const;

    /*!
     *  Compress this and the connected end items to the selected coordinates.
     *	
     *      @param [in] condensedItems              List of memory items that have been compressed.
     *      @param [in] unCutCoordinates            List of the visible coordinates.
     *      @param [in] CUTMODIFIER                 Maximum height for a compressed area.
     *      @param [in] memoryItemsAreCompressed    Flag for compressing the memory items.
     */
    void compressToUnCutCoordinates(QVector<MainMemoryGraphicsItem*>& condensedItems,
        QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER, bool memoryItemsAreCompressed);

    /*!
     *  Move the connected memory items.
     *
     *      @param [in] movedItems      Memory items that have already been moved.
     *      @param [in] movementOrigin  Origin of the movement.
     *      @param [in] transferY       The amount to move in y-coordinate.
     */
    void moveConnectedItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems,
        MainMemoryGraphicsItem* movementOrigin, qreal transferY);

    /*!
     *  Get the lowest point of the memory connection and the other connected memory item.
     *
     *      @param [in] originItem  The origin memory item.
     */
    qreal getConnectionLowPoint(MainMemoryGraphicsItem const* originItem) const;

private:

    /*!
     *  Compress the contained end item using coordinates.
     *	
     *      @param [in] condensedItems              Memory items that have already been condensed.
     *      @param [in] unCutCoordinates            The addresses tha remain after the compression.
     *      @param [in] CUTMODIFIER                 Modifier for the cut areas.
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     */
    void compressEndItemToCoordinates(QVector<MainMemoryGraphicsItem*>& compressedItems,
        QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER, bool memoryItemsAreCompressed);

    /*!
     *  Set a new height for the memory connection.
     *
     *      @param [in] newHeight   The new height.
     */
    void setCondensedHeight(qreal newHeight);

    /*!
     *  Setup the range labels.
     *
     *      @param [in] startValue  Base address of the connection.
     *      @param [in] endValue    Last address of the connection.
     */
    void setupLabels(quint64 startValue, quint64 endValue);

    /*!
     *  Create the connection path.
     */
    void createPath();

    /*!
     *  Avoid collisions on the connection path.
     *
     *      @param [in] highStartPoint  The top start point of the connection.
     *      @param [in] highEndPoint    The top end point of the connection.
     *      @param [in] lowStartPoint   The low start point of the connection.
     *      @param [in] lowEndPoint     The low end point of the connection.
     */
    void avoidCollisionsOnPath(QPointF const& highStartPoint, QPointF const& highEndPoint,
        QPointF const& lowStartPoint, QPointF const& lowEndPoint);

    /*!
     *  Create a collision path according to the collision points.
     *
     *      @param [in] highStartPoint          The top start point of the connection.
     *      @param [in] highEndPoint            The top end point of the connection.
     *      @param [in] lowStartPoint           The low start point of the connection.
     *      @param [in] lowEndPoint             The low end point of the connection.
     *      @param [in] highCollisionPoints     Map containing the top collision points of the connection item.
     *      @param [in] lowCollisionPoints      Map containing the low collision points of the connection item.
     */
    void createCollisionPath(QPointF highStartPoint, QPointF highEndPoint, QPointF lowStartPoint,
        QPointF lowEndPoint, QMultiMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints,
        QMultiMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints);

    /*!
     *  Reposition the connection labels.
     */
    void repositionLabels();

    /*!
     *  Get the width of the connection.
     *
     *      @return The width of the connection.
     */
    qreal getConnectionWidth() const;

    /*!
     *  Reposition a single colliding range label.
     *
     *      @param [in] textLabel   The selected range label.
     */
    void repositionSingleRangeLabel(QGraphicsTextItem* rangeLabel) const;

    /*!
     *  Create a path for the memory connection.
     *
     *      @param [in] highStartPoint  High start point of the memory connection.
     *      @param [in] highEndPoint    High end point of the memory connection.
     *      @param [in] lowStartPoint   Low start point of the memory connection.
     *      @param [in] lowEndPoint     Low end point of the memory connection.
     *      @param [in] LINEWIDTH       Line width of the memory connection.
     *
     *      @return The created painter path.
     */
    QPainterPath createConnectionPath(QPointF const& highStartPoint, QPointF const& highEndPoint,
        QPointF const& lowStartPoint, QPointF const& lowEndPoint, int const& LINEWIDTH);

    /*!
     *  Move the connected items without moving connected memory connections.
     *
     *      @param [in] movedItems      Memory items that have already been moved.
     *      @param [in] movementOrigin  Origin of the movement.
     *      @param [in] connectedItem   The item to be moved.
     *      @param [in] transferY       The amount to move in y-coordinate.
     */
    void moveConnectedItemWithoutConnections(QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems,
        MainMemoryGraphicsItem const* movementOrigin, MainMemoryGraphicsItem* connectedItem, qreal transferY);

    /*!
     *  Check if this connection is between an address space and its local memory map.
     *
     *      @return True, if the connection is a local memory map connection.
     */
    bool isLocalMapConnection() const;

    /*!
     *  Create a path for a local memory map connection or an address space connection avoiding colliding items.
     *
     *      @param [in] connectionIsLocal       Flag for identifying the connection type.
     *      @param [in] highStartPoint          High start point of the connection.
     *      @param [in] highEndPoint            High end point of the connection.
     *      @param [in] lowStartPoint           Low start point of the connection.
     *      @param [in] lowEndPoint             Low end point of the connection.
     *      @param [in] highCollisionPoints     High collision points.
     *      @param [in] lowCollisionPoints      Low collision points.
     *
     *      @return The memory connection path for local memory connection and address space connection with the
     *              colliding lines removed.
     */
    QPainterPath createCollidingPathForUnusualConnection(bool connectionIsLocal, QPointF highStartPoint,
        QPointF highEndPoint, QPointF lowStartPoint, QPointF lowEndPoint,
        QMultiMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints = QMultiMap<qreal, QPair<QPointF, QPointF> >(),
        QMultiMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints = QMultiMap<qreal, QPair<QPointF, QPointF> >());

    /*!
     *  Erase the colliding lines from the memory connection.
     *
     *      @param [in] collisionPath       The selected memory connection path.
     *      @param [in] collisionBegin      Start point of the colliding connection line.
     *      @param [in] collisionPoints     List of the colliding points.
     *
     *      @return The memory connection path with the colliding lines removed.
     */
    QPainterPath eraseCollisionsFromPath(QPainterPath collisionPath, QPointF collisionBegin,
        QMultiMap<qreal, QPair<QPointF, QPointF> > collisionPoints);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label containing the first item start range.
    QGraphicsTextItem* firstItemStartLabel_;

    //! The label containing the first item end range.
    QGraphicsTextItem* firstItemEndLabel_;

    //! The label containing the second item start range.
    QGraphicsTextItem* secondItemStartLabel_;

    //! The label containing the second item end range.
    QGraphicsTextItem* secondItemEndLabel_;

    //! The connection start memory graphics item.
    MainMemoryGraphicsItem* startItem_;

    //! The connection end memory graphics item.
    MainMemoryGraphicsItem* endItem_;

    //! Connection start range.
    quint64 connectionBaseAddress_;

    //! Connection end range.
    quint64 connectionLastAddress_;

    //! Width of the memory connection item.
    qreal connectionWidth_;

    //! Y transfer of the connection.
    qreal yTransfer_;

    //! The connection line modifier used in the unusual memory connections.
    const int BRIDGEMODIFIER_ = 10;
};

//-----------------------------------------------------------------------------


#endif // MEMORYCONNETIONITEM_H
