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

#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>

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
     *      @param [in] memoryItem          Memory item containing address space data.
     *      @param [in] containingInstance  Data of the containing component instance.
     *      @param [in] parent              The parent item.
     */
    AddressSpaceGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
        QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~AddressSpaceGraphicsItem();

    /*!
     *  Get the type of the address space graphics item.
     */
    int type() const { return Type; }

    /*!
     *  Change the ranges of the contained address segment items.
     *
     *      @param [in] offset  The new offset of the address space item.
     */
    virtual void changeChildItemRanges(quint64 offset);

private:
    // Disable copying.
    AddressSpaceGraphicsItem(AddressSpaceGraphicsItem const& rhs);
    AddressSpaceGraphicsItem& operator=(AddressSpaceGraphicsItem const& rhs);

    /*!
     *  Set the positions of the labels.
     */
    virtual void setLabelPositions();

    /*!
     *  Setup the address segment items.
     *
     *      @param [in] spaceItem   Memory item containing the address space data.
     */
    void setupSegments(QSharedPointer<MemoryItem> spaceItem);

    /*!
     *  Create an empty address segment.
     *
     *      @param [in] segmentXPosition    X position of the segment.
     *      @param [in] beginAddress        The base address of the segment.
     *      @param [in] rangeEnd            The end address of the segment.
     */
    void createEmptySegment(qreal segmentXPosition, quint64 beginAddress, quint64 rangeEnd);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The icon for an address space with a CPU.
    QGraphicsPixmapItem* cpuIcon_;

    //! A list of address segment items.
    QVector<AddressSegmentGraphicsItem*> segments_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSSPACEGRAPHICSITEM_H
