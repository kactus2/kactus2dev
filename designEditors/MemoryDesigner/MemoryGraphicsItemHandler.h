//-----------------------------------------------------------------------------
// File: MemoryGraphicsItemHandler.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.01.2017
//
// Description:
// Constructs the memory graphics items for the memory design diagram.
//-----------------------------------------------------------------------------

#ifndef MEMORYGRAPHICSITEMHANDLER_H
#define MEMORYGRAPHICSITEMHANDLER_H

class ConnectivityGraph;
class MemoryColumn;
class MemoryItem;
class ConnectivityComponent;

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Constructs the memory graphics items for the memory design diagram.
//-----------------------------------------------------------------------------
class MemoryGraphicsItemHandler
{

public:

    /*!
     *  The constructor.
     */    
    MemoryGraphicsItemHandler();

    /*!
     *  The destructor.
     */
    ~MemoryGraphicsItemHandler();
    
    /*!
     *  Set a new value for filtering the address space segments.
     *
     *      @param [in] filterSegments  Value for filtering address space segments.
     */
    void setFilterAddressSpaceSegments(bool filterSegments);

    /*!
     *  Check if the address space segments are filtered.
     *
     *      @return True, if the address space segments are filtered, false otherwise.
     */
    bool addressSpaceSegmentsAreFiltered() const;

    /*!
     *  Set a new value for filtering the address blocks.
     *
     *      @param [in] filterBlocks    Value for filtering address blocks.
     */
    void setFilterAddressBlocks(bool filterBlocks);

    /*!
     *  Check if the address blocks are filtered.
     *
     *      @return True, if the address blocks are filtered, false otherwise.
     */
    bool addressBlocksAreFiltered() const;

    /*!
     *  Set a new value for filtering the address block registers.
     *
     *      @param [in] filterSegments  Value for filtering address block registers.
     */
    void setFilterAddressBlockRegisters(bool filterRegisters);

    /*!
     *  Check if the address block registers are filtered.
     *
     *      @return True, if the address block registers are filtered, false otherwise.
     */
    bool addressBlockRegistersAreFiltered() const;

    /*!
     *  Set a new value for filtering the register fields.
     *
     *      @param [in] filterFields    Value for filtering register fields.
     */
    void setFilterFields(bool filterFields);

    /*!
     *  Check if the register fields are filtered.
     *
     *      @return True, if the register fields are filtered, false otherwise.
     */
    bool fieldsAreFiltered() const;

    /*!
     *  Create the memory items found in the design.
     *
     *      @param [in] connectionGraph     Graph containing the connection paths.
     *      @param [in] spaceColumn         Column for the address space graphics items.
     *      @param [in] memoryMapColumn     Column for the memory map graphics items.
     */
    void createMemoryItems(QSharedPointer<ConnectivityGraph> connectionGraph, MemoryColumn* spaceColumn,
        MemoryColumn* memoryMapColumn);

private:
    // Disable copying.
    MemoryGraphicsItemHandler(MemoryGraphicsItemHandler const& rhs);
    MemoryGraphicsItemHandler& operator=(MemoryGraphicsItemHandler const& rhs);

        /*!
     *  Create an address space item.
     *
     *      @param [in] spaceItem           Memory item containing address space data.
     *      @param [in] containingInstance  The component instance containing the selected address space.
     *      @param [in] containingColumn    The address space column.
     */
    void createAddressSpaceItem(QSharedPointer<MemoryItem> spaceItem,
        QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn);

    /*!
     *  Create a memory map item.
     *
     *      @param [in] mapItem             Memory item containing memory map data.
     *      @param [in] containingInstance  The component instance containing the selected memory map.
     *      @param [in] containingColumn    The memory map column.
     */
    void createMemoryMapItem(QSharedPointer<MemoryItem> mapItem,
        QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Value for filtering address space segments.
    bool filterAddressSpaceSegments_;

    //! Value for filtering memory map address blocks.
    bool filterAddressBlocks_;

    //! Value for filtering address block registers.
    bool filterRegisters_;

    //! Value for filtering register fields.
    bool filterFields_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYCOLUMNHANDLER_H
