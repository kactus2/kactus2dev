//-----------------------------------------------------------------------------
// File: MainMemoryGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.09.2016
//
// Description:
// Parent class for memory map and address space graphics items in memory designer.
//-----------------------------------------------------------------------------

#ifndef MAINMEMORYGRAPHICSITEM_H
#define MAINMEMORYGRAPHICSITEM_H

#include <designEditors/common/diagramgrid.h>

#include <designEditors/MemoryDesigner/MemoryDesignerGraphicsItem.h>
#include <designEditors/MemoryDesigner/SubMemoryLayout.h>

class MemoryItem;
class MemoryExtensionGraphicsItem;

//-----------------------------------------------------------------------------
//! Parent class for memory map and address space graphics items in memory designer.
//-----------------------------------------------------------------------------
class MainMemoryGraphicsItem : public MemoryDesignerGraphicsItem, public SubMemoryLayout
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] memoryItem      Memory item data.
     *      @param [in] instanceName    Name of the containing instance.
     *      @param [in] subItemType     Type of the memory sub items.
     *      @param [in] filterSubItems  Value for filtering sub items.
     *      @param [in] parent          The parent item.
     */
    MainMemoryGraphicsItem(QSharedPointer<MemoryItem> memoryItem, QString const& instanceName,
        QString const& subItemType, bool filterSubItems, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~MainMemoryGraphicsItem();

    /*!
     *  Add a memory connection to this item.
     *
     *      @param [in] connectionItem  The selected memory connection item.
     */
    virtual void addMemoryConnection(MemoryConnectionItem* connectionItem);

    /*!
     *  Add a memory connection collision to this item.
     *
     *      @param [in] collisionItem   The selected memory collision item.
     */
    void addConnectionCollision(MemoryCollisionItem* collisionItem);

    /*!
     *  Move the connected memory connections.
     *
     *      @param [in] beforePosition  The position of the graphics item before movement.
     */
    void moveConnectedConnections(QPointF beforePosition);

    /*!
     *  Move the connected memory connections in y-coordinate.
     *
     *      @param [in] yTransfer   The amount moved by.
     */
    void moveConnectedConnectionsInY(qreal yTransfer);

    /*!
     *  Move the memory item through a moving memory connection.
     *
     *      @param [in] movementOrigin  The origin of the movement.
     *      @param [in] movementDelta   The movement delta.
     */
    void moveByConnection(MemoryConnectionItem* movementOrigin, QPointF movementDelta);

    /*!
     *  Move the memory item in y-coordinate through a moving memory connection.
     *
     *      @param [in] movementOrigin  The origin of the movement.
     *      @param [in] yTransfer       The movement amount.
     */
    void moveByConnectionInY(MemoryConnectionItem* movementOrigin, qreal yTransfer);

    /*!
     *  Change the ranges of the child items.
     *
     *      @param [in] offset  Offset of this memory item.
     */
    virtual void changeChildItemRanges(quint64 offset);

    /*!
     *  Redraw the connected memory connections.
     */
    void reDrawConnections();

    /*!
     *  Compress the item and the contained sub items.
     *
     *      @param [in] movedConnections    Connection items that have already been moved.
     */
    virtual void condenseItemAndChildItems(QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections) = 0;

    /*!
     *  Move the connected items by y-coordinate.
     *
     *      @param [in] yTransfer   The movement amount in the y-coordinate.
     */
    void moveConnectedItems(qreal yTransfer);

    /*!
     *  Get the minimum required height of the memory item to fit the selected memory connection.
     *
     *      @param [in] connectionBaseAddress   Base address of the selected memory connection.
     *      @param [in] connectionEndAddress    End address of the selected memory connection.
     *
     *      @return The minimum required height of the memory item.
     */
    virtual qreal getMinimumRequiredHeight(quint64 connectionBaseAddress, quint64 connectionEndAddress) const = 0;

    /*!
     *  Get lowest point of the item, connection or connected item.
     *
     *      @return The lowest point.
     */
    quint64 getSceneEndPoint() const;

    /*!
     *  Get the last memory connection item connected to this memory graphics item.
     *
     *      @return The last memory connection item connected to this memory graphics item.
     */
    MemoryConnectionItem* getLastConnection() const;

    /*!
     *  Check if the memory graphics item has been compressed.
     *
     *      @return True, if the item has been compressed, false otherwise.
     */
    bool isCompressed() const;

    /*!
     *  Set a memory extension item for this graphics item.
     *
     *      @param [in] newExtensionItem    The new memory extension item.
     */
    void setExtensionItem(MemoryExtensionGraphicsItem* newExtensionItem);

    /*!
     *  Get the memory extension item contained within this graphics item.
     *
     *      @return The memory extension item contained within this graphics item.
     */
    MemoryExtensionGraphicsItem* getExtensionItem() const;

    /*!
     *  Check whether this item has a memory extension item.
     *
     *      @return True, if a memory extension item exists, false otherwise.
     */
    bool hasExtensionItem() const;

    /*!
     *  Hide the base address label of the first sub item and the last address label of the last sub item.
     */
    void hideFirstAndLastSegmentRange();

    /*!
     *  Get the memory item from which this graphics item has been constructed from.
     *
     *      @return The contained memory item.
     */
    QSharedPointer<MemoryItem> getMemoryItem() const;

protected:

    /*!
     *  Get the instance name label.
     *
     *      @return Graphics text item containing the name of the containing component instance.
     */
    QGraphicsTextItem* getInstanceNameLabel() const;

    /*!
     *  Called when the user moves the column with the mouse.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    /*!
     *  Called when the user release the mouse.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    /*!
     *  Set a new compression value.
     *
     *      @param [in] newCompressValue    The new memory compression value.
     */
    void setCompressed(bool newCompressValue);

private:
    // Disable copying.
    MainMemoryGraphicsItem(MainMemoryGraphicsItem const& rhs);
    MainMemoryGraphicsItem& operator=(MainMemoryGraphicsItem const& rhs);

    /*!
     *  Get the available width for this item.
     *
     *      @return Available width for this item.
     */
    virtual qreal getItemWidth() const;

    /*!
     *  Check if the selected label collides with range labels.
     *
     *      @param [in] label   The selected label.
     *
     *      @return True, if the selected label collides with range labels, false otherwise.
     */
    virtual bool labelCollidesWithRangeLabels(QGraphicsTextItem* label) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label containing the name of the containing component instance.
    QGraphicsTextItem* instanceNameLabel_;

    //! The memory item from which this graphics item has been constructed from.
    QSharedPointer<MemoryItem> memoryItem_;

    //! A list of memory collisions connected to this memory item.
    QVector<MemoryCollisionItem*> memoryCollisions_;

    //! Holds whether the graphics item has been compressed or not.
    bool compressed_;

    //! The memory extension item.
    MemoryExtensionGraphicsItem* extensionItem_;
};

//-----------------------------------------------------------------------------

#endif // MAINMEMORYGRAPHICSITEM_H
