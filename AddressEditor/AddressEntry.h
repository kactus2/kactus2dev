//-----------------------------------------------------------------------------
// File: AddressEntry.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 01.09.2012
//
// Description:
// Address entry class.
//-----------------------------------------------------------------------------

#ifndef ADDRESSENTRY_H
#define ADDRESSENTRY_H

#include <QString>

class ComponentItem;
class BusPortItem;
class ConnectionEndpoint;
class GenericEditProvider;

//-----------------------------------------------------------------------------
//! Address entry class.
//-----------------------------------------------------------------------------
class AddressEntry
{
public:
    /*!
     *  Constructor.
     */
    AddressEntry(ComponentItem* component, BusPortItem* mirSlavePort);

    /*!
     *  Sets the start address.
     *
     *      @param [in] startAddress The start address to set.
     *
     *      @remarks This function also affects the end address.
     */
    void setStartAddress(unsigned int startAddress);

    /*!
     *  Sets the address entry locked/unlocked.
     *
     *      @param [in] locked If true, the entry is set locked.
     */
    void setLocked(bool locked);

    /*!
     *  Returns the start address.
     */
    unsigned int getStartAddress() const;

    /*!
     *  Returns true if the address entry is locked.
     */
    bool isLocked() const;

    /*! 
     *  Returns the end address.
     */
    unsigned int getEndAddress() const;

    /*!
     *  Returns the range in AUB.
     */
    unsigned int getRange() const;

    /*!
     *  Returns the range in bytes.
     */
    unsigned int getRangeInBytes();

    /*!
     *  Returns the number of memory map's addressable unit bits.
     */
    unsigned int getMemoryMapAUB();

    /*!
     *  Returns the name of the slave interface.
     */
    QString getInterfaceName() const;

    /*!
     *  Returns the name of the memory map.
     */
    QString getMemoryMapName() const;

    /*!
     *  Returns true if the mirror slave interface holding the address information is connected to
     *  a slave interface and information can be retrieved.
     */
    bool hasValidConnection() const;

    /*!
     *  Returns true if the entry's address range overlaps with the given entry.
     */
    bool overlaps(AddressEntry const& other) const;

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The owner component.
    ComponentItem* component_;           

    //! The mirrored slave bus interface.
    BusPortItem* port_;                

    //! The connected port containing the memory map.
    ConnectionEndpoint* connectedPort_;

    //! The data range.
    unsigned int range_;

    //! The base end address.
    unsigned int baseEndAddress_;

    //! The edit provider.
    GenericEditProvider& editProvider_;
    unsigned int aub_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSENTRY_H
