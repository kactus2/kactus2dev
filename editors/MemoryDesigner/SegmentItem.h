//-----------------------------------------------------------------------------
// File: SegmentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 01.11.2012
//
// Description:
// Segment graphics item.
//-----------------------------------------------------------------------------

#ifndef SEGMENTITEM_H
#define SEGMENTITEM_H

#include "AddressSectionItem.h"

class Component;
class AddressSpace;
class AddressSubsection;

//-----------------------------------------------------------------------------
//! Segment graphics item.
//-----------------------------------------------------------------------------
class SegmentItem : public AddressSectionItem
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component       The parent component.
     *      @param [in] addressSpace    The parent address space.
     *      @param [in] name            The name of the section.
     *      @param [in] startAddress    The start address of the section.
     *      @param [in] range           The address range.
     *      @param [in] parent          The parent.
     */
    SegmentItem(QSharedPointer<Component> component,
                QSharedPointer<AddressSpace> addressSpace,
                QString const& name, quint64 startAddress,
                quint64 range, QGraphicsItem* parent);

    /*!
     *  Destructor.
     */
    ~SegmentItem();

    /*!
     *  Draws the segment item.
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    /*!
     *  Sets the height of the section item.
     *  
     *      @param [in] height  The height to set.
     */
    virtual void setHeight(int height);

private:
    // Disable copying.
    SegmentItem(SegmentItem const& rhs);
    SegmentItem& operator=(SegmentItem const& rhs);

    enum
    {
        SUBSECTION_HEIGHT = 40,
        SUBSECTION_SPACING = 30,
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent component.
    QSharedPointer<Component> component_;

    //! The parent address space.
    QSharedPointer<AddressSpace> addressSpace_;

    //! The subsections.
    QList< QSharedPointer<AddressSubsection> > subsections_;

};

//-----------------------------------------------------------------------------

#endif // SEGMENTITEM_H
