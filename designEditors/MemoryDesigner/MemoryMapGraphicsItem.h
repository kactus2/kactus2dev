//-----------------------------------------------------------------------------
// File: MemoryMapGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.08.2016
//
// Description:
// Graphics item for visualizing a memory map in the memory designer.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPGRAPHICSITEM_H
#define MEMORYMAPGRAPHICSITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>

class MemoryItem;
class ConnectivityComponent;
class AddressBlockGraphicsItem;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing a memory map in the memory designer.
//-----------------------------------------------------------------------------
class MemoryMapGraphicsItem : public MainMemoryGraphicsItem
{

public:

    //! Type of the memory map graphics item.
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] memoryItem          Memory item containing memory map data.
     *      @param [in] containingInstance  Instance containing the memory map.
     *      @param [in] parent              The parent item.
     */
    MemoryMapGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
        QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    ~MemoryMapGraphicsItem();

    /*!
     *  Get the type of the memory item.
     */
    int type() const { return Type; }

    /*!
     *  Change the ranges of the containing child items.
     *
     *      @param [in] offset  The offset of the memory map item.
     */
    virtual void changeChildItemRanges(quint64 offset);

private:
    // Disable copying.
    MemoryMapGraphicsItem(MemoryMapGraphicsItem const& rhs);
    MemoryMapGraphicsItem& operator=(MemoryMapGraphicsItem const& rhs);

    /*!
     *  Get the memory map start.
     *
     *      @param [in] memoryItem  The memory item containing memory map data.
     *
     *      @return The memory map start address.
     */
    quint64 getMemoryMapStart(QSharedPointer<MemoryItem> memoryItem) const;

    /*!
     *  Get the memory map end address.
     *
     *      @param [in] memoryItem  The memory item containing memory map data.
     *
     *      @return The memory map end address.
     */
    quint64 getMemoryMapEnd(QSharedPointer<MemoryItem> memoryItem) const;

    /*!
     *  Set the label positions.
     */
    virtual void setLabelPositions();

    /*!
     *  Setup the contained address block items.
     *
     *      @param [in] memoryItem  Memory item containing the memory map data.
     */
    void setupAddressBlocks(QSharedPointer<MemoryItem> memoryItem);

    /*!
     *  Create an empty address block.
     *
     *      @param [in] blockPositionX  The x position of an address block.
     *      @param [in] beginAddress    Begin address of the empty address block.
     *      @param [in] rangeEnd        End address of the empty address block.
     *      @param [in] mapAUB          Address unit bits of the containing memory map.
     */
    void createEmptyBlock(qreal blockPositionX, quint64 beginAddress, quint64 rangeEnd, QString const& mapAUB);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The base address of the memory map item.
    quint64 baseAddress_;

    //! The end address of the memory map item.
    quint64 lastAddress_;

    //! The contained address block items.
    QVector<AddressBlockGraphicsItem*> addressBlocks_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYMAPGRAPHICSITEM_H
