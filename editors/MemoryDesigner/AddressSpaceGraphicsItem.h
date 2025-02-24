//-----------------------------------------------------------------------------
// File: AddressSpaceGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Graphics item for visualizing an address space in the memory designer.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACEGRAPHICSITEM_H
#define ADDRESSSPACEGRAPHICSITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <editors/MemoryDesigner/MainMemoryGraphicsItem.h>

class AddressSpace;
class MemoryItem;
class ConnectivityComponent;
class AddressSegmentGraphicsItem;

#include <QSharedPointer>
#include <QGraphicsPixmapItem>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing address spaces in the memory designer.
//-----------------------------------------------------------------------------
class AddressSpaceGraphicsItem : public MainMemoryGraphicsItem
{

public:

    //! The type of the address space graphics item.
    enum { Type = GFX_TYPE_ADDRESS_SPACE_ITEM };

    /*!
     *  The constructor.
     *
     *    @param [in] memoryItem          Memory item containing address space data.
     *    @param [in] identifierChain     Chain of strings identifying this item.
     *    @param [in] containingInstance  Data of the containing component instance.
     *    @param [in] filterSegments      Value for filtering address space segments.
     *    @param [in] parent              The parent item.
     */
    AddressSpaceGraphicsItem(QSharedPointer<MemoryItem const> memoryItem, QVector<QString> identifierChain,
        QSharedPointer<ConnectivityComponent const> containingInstance, bool filterSegments,
        QGraphicsItem* parent = 0);

    // Disable copying.
    AddressSpaceGraphicsItem(AddressSpaceGraphicsItem const& rhs) = delete;
    AddressSpaceGraphicsItem& operator=(AddressSpaceGraphicsItem const& rhs) = delete;

	/*!
     *  The destructor.
     */
    virtual ~AddressSpaceGraphicsItem() = default;

    /*!
     *  Get the type of the address space graphics item.
     */
    int type() const { return Type; }

    /*!
     *  Get all the address space graphics items connected to the origin item.
     */
    QVector<MainMemoryGraphicsItem*> getAllConnectedSpaceItems() const;

private:

    /*!
     *  Set the positions of the labels.
     */
    virtual void setLabelPositions() override;

    /*!
     *  Create a new address segment graphics item.
     *
     *    @param [in] subMemoryItem   Memory item containing address segment data.
     *    @param [in] isEmpty         Boolean value for an empty address segment.
     *
     *    @return The created address segment graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createNewSubItem(QSharedPointer<MemoryItem const> subMemoryItem,
        bool isEmpty) override;

    /*!
     *  Create an empty address segment graphics item.
     *
     *    @param [in] beginAddress    Base address of the empty address segment graphics item.
     *    @param [in] rangeEnd        End address of the empty address segment graphics item.
     *
     *    @return The created address segment graphics item.
     */
    virtual MemoryDesignerChildGraphicsItem* createEmptySubItem(quint64 beginAddress, quint64 rangeEnd) override;

    /*!
     *  Get the minimum height required to display for the memory item.
     *	
     *    @return The minimum height required to display the memory item.
     */
    virtual qreal getMinimumHeightForSubItems() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The icon for an address space with a CPU.
    QGraphicsPixmapItem* cpuIcon_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSSPACEGRAPHICSITEM_H
