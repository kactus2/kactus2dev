//-----------------------------------------------------------------------------
// File: AddressBlockItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 02.11.2012
//
// Description:
// Address block graphics item.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKITEM_H
#define ADDRESSBLOCKITEM_H

#include "AddressSectionItem.h"

class Component;
class MemoryMap;

//-----------------------------------------------------------------------------
//! Segment graphics item.
//-----------------------------------------------------------------------------
class AddressBlockItem : public AddressSectionItem
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component       The parent component.
     *      @param [in] memoryMap       The parent memory map.
     *      @param [in] name            The name of the section.
     *      @param [in] startAddress    The start address of the section.
     *      @param [in] range           The address range.
     *      @param [in] parent          The parent.
     */
    AddressBlockItem(QSharedPointer<Component> component,
                     QSharedPointer<MemoryMap> memoryMap,
                     QString const& name, quint64 startAddress,
                     quint64 range, QGraphicsItem* parent);

    /*!
     *  Destructor.
     */
    ~AddressBlockItem();

private:
    // Disable copying.
    AddressBlockItem(AddressBlockItem const& rhs);
    AddressBlockItem& operator=(AddressBlockItem const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent component.
    QSharedPointer<Component> component_;

    //! The parent memory map.
    QSharedPointer<MemoryMap> memoryMap_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSBLOCKITEM_H
