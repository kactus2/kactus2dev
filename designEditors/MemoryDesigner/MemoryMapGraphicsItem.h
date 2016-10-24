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
     *  Compress the item and the contained sub items.
     */
    virtual void condenseItemAndChildItems();

    /*!
     *  Compress the memory map item.
     */
    void compressMapItem();

    /*!
     *  Get the minimum required height of the memory map item to fit the selected memory connection.
     *
     *      @param [in] connectionBaseAddress   Base address of the selected memory connection.
     *      @param [in] connectionEndAddress    End address of the selected memory connection.
     *
     *      @return The minimum required height of the memory map item.
     */
    virtual qreal getMinimumRequiredHeight(quint64 connectionBaseAddress, quint64 connectionEndAddress) const;

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
     *  Create a new address block graphics item.
     *
     *      @param [in] subMemoryItem   Memory item containing address block data.
     *      @param [in] isEmpty         Boolean value for an empty address block.
     *
     *      @return The created address block graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem> subMemoryItem,
        bool isEmpty);

    /*!
     *  Create an empty address block graphics item.
     *
     *      @param [in] beginAddress    Base address of the empty address block graphics item.
     *      @param [in] rangeEnd        End address of the empty address block graphics item.
     *
     *      @return The created address block graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createEmptySubItem(quint64 beginAddress, quint64 rangeEnd);

    /*!
     *  Check if the memory map item has an extension item.
     *
     *      @return True, if the memory map item has an extension item, false otherwise.
     */
    bool hasExtensionItem() const;

    /*!
     *  Get the minimum height for the memory map sub items.
     *
     *      @return The minimum height for the memory map sub items.
     */
    int getMinimumHeightForSubItems() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The address unit bits of a memory map.
    QString addressUnitBits_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYMAPGRAPHICSITEM_H
