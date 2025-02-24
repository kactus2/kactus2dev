//-----------------------------------------------------------------------------
// File: localmemorymapgraphitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 17.12.2012
//
// Description:
// The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------

#ifndef LOCALMEMORYMAPGRAPHITEM_H
#define LOCALMEMORYMAPGRAPHITEM_H

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

class ExpressionParser;
class AddressSpace;
class MemoryMapBase;

//-----------------------------------------------------------------------------
//! The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------
class LocalMemoryMapGraphItem : public MemoryMapGraphItem
{
    Q_OBJECT

public:

    /*! The constructor
     *
     *    @param [in] addrSpace       The address space that contains the local memory map.
     *    @param [in] localMemoryMap  The local memory map.
     *    @param [in] parent          The owner of the graphics item.
     *
    */
    LocalMemoryMapGraphItem(QSharedPointer<AddressSpace> addrSpace,
        QSharedPointer<MemoryMapBase> localMemoryMap,
        QSharedPointer<ExpressionParser> expressionParser,
        QGraphicsItem* parent = 0);

    //! The destructor.
    virtual ~LocalMemoryMapGraphItem() = default;

    //! No copying.
    LocalMemoryMapGraphItem(const LocalMemoryMapGraphItem& other) = delete;

    //! No assignment.
    LocalMemoryMapGraphItem& operator=(const LocalMemoryMapGraphItem& other) = delete;

    /*!
     *  Get number of bits the addressable unit contains.
     *
     *    @return The size of least addressable unit.
     */
    virtual unsigned int getAddressUnitSize() const override final;
 

private:
    
    //! The address space containing the local memory map.
    QSharedPointer<AddressSpace> addrSpace_;
};

#endif // LOCALMEMORYMAPGRAPHITEM_H
