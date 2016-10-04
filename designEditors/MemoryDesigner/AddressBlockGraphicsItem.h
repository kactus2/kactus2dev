//-----------------------------------------------------------------------------
// File: AddressBlockGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.09.2016
//
// Description:
// Graphics item for visualizing an address block in the memory designer.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKGRAPHICSITEM_H
#define ADDRESSBLOCKGRAPHICSITEM_H

#include <designEditors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>
#include <designEditors/MemoryDesigner/SubMemoryLayout.h>

#include <common/graphicsItems/GraphicsItemTypes.h>

class MemoryItem;
class MemoryMapGraphicsItem;
class RegisterGraphicsItem;
class MemoryConnectionItem;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing an address block in the memory designer.
//-----------------------------------------------------------------------------
class AddressBlockGraphicsItem : public MemoryDesignerChildGraphicsItem, public SubMemoryLayout
{

public:

    //! The type for the address block graphics item.
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] blockItem       Memory item containing the address block data.
     *      @param [in] isEmptyBlock    Boolean value for existing blocks.
     *      @param [in] memoryMapItem   The parent memory map item.
     */
    AddressBlockGraphicsItem(QSharedPointer<MemoryItem> blockItem, bool isEmptyBlock,
        MemoryMapGraphicsItem* memoryMapItem);

	/*!
     *  The destructor.
     */
    virtual ~AddressBlockGraphicsItem();

    /*!
     *  Get the type of the address block graphics item.
     */
    int type() const { return Type; }

    /*!
     *  Change the address block ranges.
     *
     *      @param [in] offset  The new offset of the memory map.
     */
    virtual void changeAddressRange(quint64 memoryMapOffset);

    /*!
     *  Compress the registers contained within the address block to match the selected connection item.
     *
     *      @param [in] connectionItem          The selected connection item.
     *      @param [in] connectionBaseAdddress  Base address of the connection.
     *      @param [in] connectionLastAddress   Last address of the connection.
     *      @param [in] minimumRegisterHeight   Minimum height of the registers.
     *
     *      @return New height of the compressed address block.
     */
    quint64 condenseRegistersToConnection(MemoryConnectionItem* connectionItem, quint64 connectionBaseAdddress,
        quint64 connectionLastAddress, qreal minimumRegisterHeight);

private:
    // Disable copying.
    AddressBlockGraphicsItem(AddressBlockGraphicsItem const& rhs);
    AddressBlockGraphicsItem& operator=(AddressBlockGraphicsItem const& rhs);

    /*!
     *  Get the width of the block item.
     *
     *      @param [in] memoryMapItem   The parent item of the address block item.
     */
    qreal getBlockWidth(MemoryMapGraphicsItem* memoryMapItem) const;

    /*!
     *  Set the positions for the labels.
     */
    virtual void setLabelPositions();

    /*!
     *  Create a new register graphics item.
     *
     *      @param [in] subMemoryItem   Memory item containing register data.
     *      @param [in] isEmpty         Boolean value for an empty register.
     *
     *      @return The created register graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem> subMemoryItem,
        bool isEmpty);

    /*!
     *  Create an empty register graphics item.
     *
     *      @param [in] beginAddress    Base address of the empty register graphics item.
     *      @param [in] rangeEnd        End address of the empty register graphics item.
     *
     *      @return The created register graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createEmptySubItem(quint64 beginAddress, quint64 rangeEnd);

    /*!
     *  Get the height to fit the condensed registers within the selected connection item.
     *
     *      @param [in] connectionItem          The selected memory connection item.
     *      @param [in] registersInConnection   The registers contained within the selected connection.
     *      @param [in] registerStartPositionY  Y position of the first register.
     *      @param [in] minimumHeight           Minimum height of the registers.
     *
     *      @return The height for the condensed registers.
     */
    qreal getCondensedRegisterHeightForConnection(MemoryConnectionItem* connectionItem,
        QVector<MemoryDesignerChildGraphicsItem*> registersInConnection, qreal registerStartPositionY,
        qreal minimumHeight) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Address unit bits of the containing memory map.
    QString addressUnitBits_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSBLOCKGRAPHICSITEM_H
