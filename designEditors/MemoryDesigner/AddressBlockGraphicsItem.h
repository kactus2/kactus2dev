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

#include <common/graphicsItems/GraphicsItemTypes.h>

class MemoryItem;
class MemoryMapGraphicsItem;
class RegisterGraphicsItem;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing an address block in the memory designer.
//-----------------------------------------------------------------------------
class AddressBlockGraphicsItem : public MemoryDesignerChildGraphicsItem
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
     *  Setup registers contained within the address block.
     *
     *      @param [in] blockItem   The selected address block item.
     */
    void setupRegisters(QSharedPointer<MemoryItem> blockItem);

    /*!
     *  Creates an empty register item for the address block item.
     *
     *      @param [in] beginAddress        The base address of the register item.
     *      @param [in] rangeEnd            The end address of the register item.
     *      @param [in] addressUnitBits     The address unit bits of the memory map.
     *      @param [in] blockBaseAddress    The base address of the address block.
     */
    void createEmptyRegister(quint64 beginAddress, quint64 rangeEnd, QString const& addressUnitBits,
        quint64 blockBaseAddress);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! A list of registers contained within the address block.
    QVector<RegisterGraphicsItem*> registers_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSBLOCKGRAPHICSITEM_H
