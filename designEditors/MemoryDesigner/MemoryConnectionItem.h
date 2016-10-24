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
     *      @param [in] secondStartValue    Start address in the end item.
     *      @param [in] secondEndValue      End address in the end item.
     *      @param [in] containingScene     Graphics scene containing the memory connection item.
     *      @param [in] yTransfer           Y transfer of the memory connection.
     *      @param [in] parent              Parent item of the connection.
     */
    MemoryConnectionItem(MainMemoryGraphicsItem* startItem, QString const& firstStartValue,
        QString const& firstEndValue, MainMemoryGraphicsItem* endItem, QString const& secondStartValue,
        QString const& secondEndValue, QGraphicsScene* containingScene, int yTransfer = 0,
        QGraphicsItem* parent = 0);

	/*!
     *  The Destructor.
     */
    virtual ~MemoryConnectionItem();

    /*!
     *  Move a memory connection item.
     *
     *      @param [in] movementOrigin  The origin of the movement.
     *      @param [in] movementDelta   The difference in movement.
     */
    void onMoveConnection(MainMemoryGraphicsItem* movementOrigin, QPointF movementDelta);

    /*!
     *  Move a memory connection item by the y-coordinate.
     *
     *      @param [in] movementOrigin  The origin of the movement.
     *      @param [in] yTransfer       The movement amount.
     */
    void onMoveConnectionInY(MainMemoryGraphicsItem* movementOrigin, qreal yTransfer);

    /*!
     *  Get the start value of the connection.
     *
     *      @return The start value of the connection.
     */
    QString getRangeStartValue() const;

    /*!
     *  Get the end value of the connection.
     *
     *      @return The end value of the connection.
     */
    QString getRangeEndValue() const;

    /*!
     *  Redraw the path of this connection.
     */
    void reDrawConnection();

    /*!
     *  Move the other connected item.
     *
     *      @param [in] movementOrigin  The origin item of the movement.
     *      @param [in] yTransfer       The movement amount.
     */
    void moveConnectedItem(MainMemoryGraphicsItem* movementOrigin, qreal yTransfer);

    /*!
     *  Get the lowest point of the connection.
     *
     *      @return The lowest point of the connection item.
     */
    quint64 getSceneEndPoint() const;

    /*!
     *  Get the last address of the connected end item.
     *
     *      @return The last address of the connected end item.
     */
    quint64 getConnectedEndItemLastAddress() const;

    /*!
     *  Compress the end item.
     */
    void compressEndItem();

    /*!
     *  Get the height of the connected end memory item.
     *
     *      @return The height of the connected end memory item.
     */
    qreal getEndItemHeight() const;

private:
    // Disable copying.
    MemoryConnectionItem(MemoryConnectionItem const& rhs);
    MemoryConnectionItem& operator=(MemoryConnectionItem const& rhs);

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
    void avoidCollisionsOnPath(QPointF highStartPoint, QPointF highEndPoint, QPointF lowStartPoint, QPointF lowEndPoint);

    /*!
     *  Reposition the connection labels.
     */
    void repositionLabels();

    /*!
     *  Format a value to hexadecimal.
     *
     *      @param [in] value   The given value.
     */
    QString formatValueToHexadecimal(QString const& value) const;

    /*!
     *  Get the width of the connection.
     *
     *      @return The width of the connection.
     */
    qreal getConnectionWidth() const;

    /*!
     *  Get the amount of numbers used to display ranges.
     *
     *      @param [in] rangeStart  Range start.
     *      @param [in] rangeEnd    Range end.
     *
     *      @return The amount of numbers used to display ranges.
     */
    int getAmountOfNumbers(QString const& rangeStart, QString const& rangeEnd) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The amount of numbers used to display ranges.
    int amountOfNumbers_;

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
    QString rangeStart_;

    //! Connection end range.
    QString rangeEnd_;

    //! Y transfer of the connection.
    int yTransfer_;

    //! Width of the memory connection item.
    qreal connectionWidth_;
};

//-----------------------------------------------------------------------------


#endif // MEMORYCONNETIONITEM_H
