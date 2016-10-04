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

//-----------------------------------------------------------------------------
//! Parent class for memory map and address space graphics items in memory designer.
//-----------------------------------------------------------------------------
class MainMemoryGraphicsItem : public MemoryDesignerGraphicsItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] itemName            Name of the graphics item.
     *      @param [in] instanceName        Name of the containing instance.
     *      @param [in] addressUnitBits     Address unit bits of the memory item.
     *      @param [in] parent              The parent item.
     */
    MainMemoryGraphicsItem(QString const& itemName, QString const& instanceName, QString const& addressUnitBits,
        QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~MainMemoryGraphicsItem();

    /*!
     *  Get the name of the containing instance.
     *
     *      @return The name of the containing instance.
     */
    QString getContainingInstanceName() const;

    /*!
     *  Add a memory connection to this item.
     *
     *      @param [in] connectionItem  The selected memory connection item.
     */
    void addMemoryConnection(MemoryConnectionItem* connectionItem);

    /*!
     *  Add a memory connection collision to this item.
     *
     *      @param [in] collisionItem   The selected memory collision item.
     */
    void addConnectionCollision(MemoryCollisionItem* collisionItem);

    /*!
     *  Get the connected memory connections.
     *
     *      @return A list of the connected memory connections.
     */
    QVector<MemoryConnectionItem*> getMemoryConnections() const;

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
    virtual void changeChildItemRanges(quint64 offset) = 0;

    /*!
     *  Redraw the connected memory connections.
     */
    void reDrawConnections();

    /*!
     *  Compress the item and the contained sub items.
     */
    virtual void condenseItemAndChildItems() = 0;

    /*!
     *  Move the connected items by y-coordinate.
     *
     *      @param [in] yTransfer   The movement amount in the y-coordinate.
     */
    void moveConnectedItems(qreal yTransfer);

protected:

    /*!
     *  Get the instance name label.
     *
     *      @return Graphics text item containing the name of the containing component instance.
     */
    QGraphicsTextItem* getInstanceNameLabel() const;

    /*!
     *  Get the AUB label.
     *
     *      @return Graphics text item containing the address unit bits.
     */
    QGraphicsTextItem* getAUBLabel() const;

    /*!
     *  Called when the user moves the column with the mouse.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    /*!
     *  Called when the user release the mouse.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    // Disable copying.
    MainMemoryGraphicsItem(MainMemoryGraphicsItem const& rhs);
    MainMemoryGraphicsItem& operator=(MainMemoryGraphicsItem const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label containing the name of the containing component instance.
    QGraphicsTextItem* instanceNameLabel_;

    //! The label containing the address unit bits of the memory item.
    QGraphicsTextItem* aubLabel_;

    //! The name of the containing component instance.
    QString instanceName_;

    //! A list of memory connections to this memory item.
    QVector<MemoryConnectionItem*> memoryConnections_;

    //! A list of memory collisions connected to this memory item.
    QVector<MemoryCollisionItem*> memoryCollisions_;
};

//-----------------------------------------------------------------------------

#endif // MAINMEMORYGRAPHICSITEM_H
