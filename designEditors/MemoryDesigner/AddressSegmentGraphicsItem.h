//-----------------------------------------------------------------------------
// File: AddressSegmentGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.09.2016
//
// Description:
// Graphics item for visualizing an address segment in the memory designer.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSEGMENTGRAPHICSITEM_H
#define ADDRESSSEGMENTGRAPHICSITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>
#include <designEditors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>

class MemoryItem;
class AddressSpaceGraphicsItem;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing an address segment in the memory designer.
//-----------------------------------------------------------------------------
class AddressSegmentGraphicsItem : public MemoryDesignerChildGraphicsItem
{

public:
    //! The type of the address segment graphics item.
    enum { Type = GFX_TYPE_ADDRESS_SECTION_ITEM };

    /*!
     *  The constructor.
     *
     *      @param [in] segmentItem     The memory item containing address segment data.
     *      @param [in] isEmptySegment  Boolean value to inform of an empty segment.
     *      @param [in] spaceItem       The address space item containing the address segment.
     */
    AddressSegmentGraphicsItem(QSharedPointer<MemoryItem> segmentItem, bool isEmptySegment,
        AddressSpaceGraphicsItem* spaceItem);

	/*!
     *  The destructor.
     */
    virtual ~AddressSegmentGraphicsItem();

    /*!
     *  Get the type of the address segment graphics item.
     */
    int type() const { return Type; }

private:
    // Disable copying.
    AddressSegmentGraphicsItem(AddressSegmentGraphicsItem const& rhs);
    AddressSegmentGraphicsItem& operator=(AddressSegmentGraphicsItem const& rhs);

    /*!
     *  Get the width of the address segment item.
     *
     *      @param [in] addressSpaceItem   [Description].
     */
    qreal getSegmentItemWidth(AddressSpaceGraphicsItem* addressSpaceItem) const;

    /*!
     *  Set the positions for the labels.
     */
    virtual void setLabelPositions();
};

//-----------------------------------------------------------------------------


#endif // ADDRESSSPACEGRAPHICSITEM_H
