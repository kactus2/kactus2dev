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

#include <editors/MemoryDesigner/MainMemoryGraphicsItem.h>

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
     *      @param [in] memoryItem              Memory item containing memory map data.
     *      @param [in] identifierChain         List of string identifying the memory item.
     *      @param [in] filterAddressBlocks     Value for filtering memory map address blocks.
     *      @param [in] filterRegisters         Value for filtering address block registers.
     *      @param [in] filterFields            Value for filtering register fields.
     *      @param [in] containingInstance      Instance containing the memory map.
     *      @param [in] parent                  The parent item.
     */
    MemoryMapGraphicsItem(QSharedPointer<MemoryItem> memoryItem, QVector<QString> identifierChain,
        bool filterAddressBlocks, bool filterRegisters, bool filterFields,
        QSharedPointer<ConnectivityComponent const> containingInstance, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    ~MemoryMapGraphicsItem();

    /*!
     *  Get the type of the memory item.
     */
    int type() const { return Type; }

    /*!
     *  Change the width of this item and the contained register graphics items.
     *
     *      @param [in] widthChange     The change in width.
     */
    void changeWidth(qreal widthChange);

    /*!
     *  Get the maximum needed change in width to fully display the name labels of all the field items within the
     *  address block items.
     *
     *      @return The maximum needed change in width to display the name labels of all the field items.
     */
    qreal getMaximumNeededChangeInFieldWidth() const;

    /*!
     *  Create field overlap items for register fields.
     */
    void createFieldOverlapItems();

private:
    // Disable copying.
    MemoryMapGraphicsItem(MemoryMapGraphicsItem const& rhs);
    MemoryMapGraphicsItem& operator=(MemoryMapGraphicsItem const& rhs);

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
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem const> subMemoryItem,
        bool isEmpty) override;

    /*!
     *  Get the name of the address block containing the selected register.
     *
     *      @param [in] registerBaseAddress     Base address of the selected register.
     *      @param [in] registerLastAddress     Last address of the selected register.
     *
     *      @return The name of the address block containing the selected register.
     */
    QString getContainingAddressBlockName(quint64 registerBaseAddress, quint64 registerLastAddress) const;

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
     *  Get the minimum height for the memory map sub items.
     *
     *      @return The minimum height for the memory map sub items.
     */
    virtual qreal getMinimumHeightForSubItems() const override final;

    /*!
     *  Setup sub items for the memory map.
     *
     *      @param [in] blockXPosition  X coordinate of the memory sub items.
     *      @param [in] memoryItem      The memory map memory item.
     */
    void setupSubItems(qreal blockXPosition, QSharedPointer<MemoryItem> memoryItem);

    /*!
     *  Get the width of the memory map sub items.
     *
     *      @return Width of the memory map sub items.
     */
    qreal getSubItemWidth() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Information of the filtered address blocks.
    struct FilteredBlock
    {
        //! Name of the filtered address blocks.
        QString blockName_;

        //! Base address of the filtered address block.
        quint64 blockBaseAddress_;

        //! Last address of the filtered address block.
        quint64 blockLastAddress_;
    };

    //! The address unit bits of a memory map.
    QString addressUnitBits_;

    //! Value for filtering address blocks.
    bool filterAddressBlocks_;

    //! Value for filtering address block registers.
    bool filterRegisters_;

    //! Value for filtering register fields.
    bool filterFields_;

    //! Width of the contained memory sub items.
    qreal subItemWidth_;

    //! List of filtered address blocks.
    QVector<MemoryMapGraphicsItem::FilteredBlock> filteredBlocks_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYMAPGRAPHICSITEM_H
