//-----------------------------------------------------------------------------
// File: MemoryColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 17.10.2012
//
// Description:
// Memory column.
//-----------------------------------------------------------------------------

#ifndef MEMORYCOLUMN_H
#define MEMORYCOLUMN_H

#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/GraphicsColumn.h>

class GraphicsColumnLayout;
class GraphicsConnection;
class MainMemoryGraphicsItem;
class MemoryItem;
class MemoryConnectionItem;
class MemoryMapGraphicsItem;

#include <QGraphicsRectItem>
#include <QSet>
#include <QCursor>
#include <QMultiMap>

//-----------------------------------------------------------------------------
//! MemoryColumn class.
//-----------------------------------------------------------------------------
class MemoryColumn : public GraphicsColumn
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SYSTEM_COLUMN };

    /*!
     *  Constructor.
     *
     *      @param [in] name            The column description.
     *      @param [in] layout          The parent column layout.
     *      @param [in] itemSpacing     Spacing for the graphics items.
     */
    MemoryColumn(QSharedPointer<ColumnDesc> desc, GraphicsColumnLayout* layout, int itemSpacing = 30);

    /*!
     *  The destructor.
     */
    virtual ~MemoryColumn() = default;

    /*!
     *  Find a graphics item by a given memory item.
     *
     *      @param [in] containedMemoryItem     The selected memory item.
     *
     *      @return Memory graphics item matching the selected memory item.
     */
    MainMemoryGraphicsItem* findGraphicsItemByMemoryItem(QSharedPointer<MemoryItem const> containedMemoryItem)
        const;

    /*!
     *  Get the type of this item.
     */
    int type() const { return Type; }

    /*!
     *  Get the main memory graphics items contained within this column in the order of their scene positions.
     *
     *      @return Map containing y-coordinate, main memory graphics item pairs.
     */
    QMultiMap<qreal, MainMemoryGraphicsItem*> getGraphicsItemInOrder() const;

    /*!
     *  Check if the memory column contains memory map graphics items.
     *
     *      @return True, if the column contains memory map graphics items, false otherwise.
     */
    bool containsMemoryMapItems() const;

    /*!
     *  Change the width of the memory column and the contained memory map graphics items.
     *
     *      @param [in] deltaWidth  Change of width.
     */
    void changeWidth(qreal deltaWidth);

    /*!
     *  Get the maximum needed change in width to display the name labels of all the field items contained within
     *  the memory map items.
     *
     *      @return The maximum needed change in width to display the name labels of all the field items.
     */
    qreal getMaximumNeededChangeInWidth() const;

    /*!
     *  Move the unconnected memory items to the bottom of the column.
     *
     *      @param [in] placedItems     List of the connected items of the column.
     */
    void moveUnconnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems) const;

    /*!
     *  Check if a memory map overlaps within this column.
     *
     *      @param [in] connectionBaseAddress   Lowest base address of the connections in the selected memory item.
     *      @param [in] connectionLastAddress   Highest last address of the connections in the selected memory item.
     *      @param [in] memoryGraphicsItem      The selected memory map item.
     *      @param [in] memoryItemRect          Bounding rectangle of the selected memory map item.
     *      @param [in] memoryPenWidth          Line width of the memory map item.
     *      @param [in] connectedSpaceItems     List of address space items connected to the memory map item.
     *      @param [in] placedMaps              List of the placed memory map items.
     *
     *      @return True if the memory map overlaps another item in this column, false otherwise.
     */
    bool memoryMapOverlapsInColumn(quint64 connectionBaseAddress, quint64 connectionLastAddress,
        MainMemoryGraphicsItem const* memoryGraphicsItem, QRectF memoryItemRect, int memoryPenWidth,
        QVector<MainMemoryGraphicsItem*> connectedSpaceItems,
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps) const;

    /*!
     *  Check if the item overlaps another placed column item.
     *	
     *      @param [in] memoryItem      The selected memory item.
     *      @param [in] itemRectangle   Scene rectangle of the memory item.
     *      @param [in] lineWidth       Line width of the rectangle.
     *      @param [in] placedItems     List of the placed memory items.
     *
     *      @return True, if the item overlaps another placed item, false otherwise.
     */
    bool itemOverlapsAnotherPlacedColumnItem(MainMemoryGraphicsItem const* memoryItem, QRectF const& itemRectangle, int lineWidth, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems)
        const;

    /*!
     *  Compress the unconnected memory items.
     *	
     *      @param [in] compressMemoryItems     Flag for compressing the memory items.
     *      @param [in] placedItems             List of the placed memory items.
     */
    void compressUnconnectedMemoryItems(bool compressMemoryItems, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems);

protected:

    /*!
     *  Returns true if the given item is allowed to reside in the column based on the allowed items.
     *
     *      @param [in] item          The item.
     *      @param [in] allowedItems  The allowed items flags.
     */
    virtual bool isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const;

private:
    // Disable copying.
    MemoryColumn(MemoryColumn const& rhs);
    MemoryColumn& operator=(MemoryColumn const& rhs);

    /*!
     *  Get the position for the unconnected memory items.
     *
     *      @param [in] placedItems     List of the placed memory graphics items.
     *
     *      @return Y-coordinate for the unconnected memory items.
     */
    quint64 getUnconnectedItemPosition(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems) const;
};

//-----------------------------------------------------------------------------

#endif // MEMORYCOLUMN_H
