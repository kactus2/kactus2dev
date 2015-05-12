//-----------------------------------------------------------------------------
// File: localmemorymapgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 17.12.2012
//
// Description:
// The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------

#ifndef LOCALMEMORYMAPGRAPHITEM_H
#define LOCALMEMORYMAPGRAPHITEM_H

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>
#include <IPXACTmodels/addressspace.h>

//-----------------------------------------------------------------------------
//! The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------
class LocalMemoryMapGraphItem : public MemoryMapGraphItem
{
    Q_OBJECT

public:

    /*! The constructor
     *
     *      @param [in] addrSpace       The address space that contains the local memory map.
     *      @param [in] localMemoryMap  The local memory map.
     *      @param [in] parent          The owner of the graphics item.
     *
    */
    LocalMemoryMapGraphItem(QSharedPointer<AddressSpace> addrSpace,
        QSharedPointer<MemoryMap> localMemoryMap, 
        QGraphicsItem* parent = 0);
    
    //! The destructor
    virtual ~LocalMemoryMapGraphItem();

    //! Refresh the item and sub-items.
    virtual void refresh();

    /*! Get number of bits the addressable unit contains.
     *
     *      @return The size of least addressable unit.
    */
    virtual unsigned int getAddressUnitSize() const;

private:
    
    //! No copying
    LocalMemoryMapGraphItem(const LocalMemoryMapGraphItem& other);

    //! No assignment
    LocalMemoryMapGraphItem& operator=(const LocalMemoryMapGraphItem& other);

    //! The address space containing the local memory map.
    QSharedPointer<AddressSpace> addrSpace_;
};

#endif // LOCALMEMORYMAPGRAPHITEM_H
