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

#include <editors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>
#include <editors/MemoryDesigner/SubMemoryLayout.h>

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
     *    @param [in] blockItem               Memory item containing the address block data.
     *    @param [in] identifierChain         Chain of strings identifying this item.
     *    @param [in] isEmptyBlock            Boolean value for existing blocks.
     *    @param [in] filterRegisters         Filtering value for address block registers.
     *    @param [in] filterFields            Filtering value for register fields.
     *    @param [in] addressBlockWidth       Width of the address block graphics item.
     *    @param [in] containingInstanceName  Name of the containing component instance.
     *    @param [in] memoryMapItem           The parent memory map item.
     */
    AddressBlockGraphicsItem(QSharedPointer<MemoryItem const> blockItem, QVector<QString> identifierChain,
        bool isEmptyBlock, bool filterRegisters, bool filterFields, qreal addressBlockWidth,
        QSharedPointer<ConnectivityComponent const> containingInstance, MemoryMapGraphicsItem* memoryMapItem);

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
     *    @param [in] offset  The new offset of the memory map.
     */
    virtual void changeAddressRange(quint64 memoryMapOffset);

    /*!
     *  Add a memory connection to address block item and to the correct sub items.
     *
     *    @param [in] connectionItem  The selected memory connection item.
     */
    virtual void addMemoryConnection(MemoryConnectionItem* connectionItem);

    /*!
     *  Change the width of the contained register graphics items.
     *
     *    @param [in] widthChange     The given change in width.
     */
    void changeWidth(qreal widthChange);

    /*!
     *  Get the maximum needed change in width to display the name labels of all the field items within the
     *  register items.
     *
     *    @return The maximum needed change in width to display the name labels of all the field items.
     */
    qreal getMaximumNeededChangeInFieldWidth() const;

    /*!
     *  Create overlapping register field items.
     */
    void createFieldOverlapItems();

private:
    // Disable copying.
    AddressBlockGraphicsItem(AddressBlockGraphicsItem const& rhs);
    AddressBlockGraphicsItem& operator=(AddressBlockGraphicsItem const& rhs);

    /*!
     *  Get the width of the block item.
     *
     *    @param [in] memoryMapItem           The parent item of the address block item.
     *    @param [in] registersAreFiltered    Filtering value for address block registers.
     */
    qreal getBlockWidth(MemoryMapGraphicsItem* memoryMapItem, bool registersAreFiltered) const;

    /*!
     *  Set the positions for the labels.
     */
    virtual void setLabelPositions();

    /*!
     *  Create a new register graphics item.
     *
     *    @param [in] subMemoryItem   Memory item containing register data.
     *    @param [in] isEmpty         Boolean value for an empty register.
     *
     *    @return The created register graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem const> subMemoryItem,
        bool isEmpty);

    /*!
     *  Create an empty register graphics item.
     *
     *    @param [in] beginAddress    Base address of the empty register graphics item.
     *    @param [in] rangeEnd        End address of the empty register graphics item.
     *
     *    @return The created register graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createEmptySubItem(quint64 beginAddress, quint64 rangeEnd);

    /*!
     *  Get the width of the available area used by the address block item.
     *
     *    @return The width of the available area used by the address block item.
     */
    virtual qreal getItemWidth() const;

    /*!
     *  Get the width for the register graphics items.
     *
     *    @return Width for the register graphics items.
     */
    qreal getRegisterWidth() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Address unit bits of the containing memory map.
    QString addressUnitBits_;

    //! Filtering value for register fields.
    bool filterFields_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSBLOCKGRAPHICSITEM_H
