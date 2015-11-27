//-----------------------------------------------------------------------------
// File: MemoryMap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Describes the ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "MemoryMapBase.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/BooleanValue.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QTextStream>

class MemoryRemap;

//-----------------------------------------------------------------------------
//! Describes the ipxact:memoryMap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMap : public MemoryMapBase, public Extendable
{

public:

	/*!
	 *  The default constructor.
	 */
	MemoryMap(QString const& name = QString());

	/*!
	 *  Copy constructor.
	 *
	 *      @param [in] other   The memory map to be copied.
	 */
	MemoryMap(const MemoryMap& other);

	/*!
	 *  Assignment operator.
	 *
	 *      @param [in] other   The memory map being assigned.
	 */
	MemoryMap& operator=(const MemoryMap& other);

	/*!
	 *  The destructor.
	 */
	~MemoryMap();

    /*!
     *  Get a pointer to the list containing the memory remaps of this memory map.
     *
     *      @return Pointer to the list containing the memory remaps of this memory map.
     */
    QSharedPointer<QList<QSharedPointer<MemoryRemap> > > getMemoryRemaps() const;

    /*!
     *  Set the memory remaps.
     *
     *      @param [in] newMemoryRemaps     Pointer to a list containing the new memory remaps.
     */
    void setMemoryRemaps(QSharedPointer<QList<QSharedPointer<MemoryRemap> > > newMemoryRemaps);

    /*!
	 *  Get the address unit bits value.
	 *
     *      @return The address unit bit value.
	 */
    QString getAddressUnitBits() const;

	/*!
	 *  Set the address unit bits for this memory map.
	 *
	 *      @param [in] newAddressUnitBits  Value to be set.
	 */
    void setAddressUnitBits(QString const& newAddressUnitBits);

    /*!
     *  Get the shared value.
     *
     *      @return The shared value.
     */
    QString getShared() const;

    /*!
     *  Set the shared value.
     *
     *      @param [in] newSharedStatus     The new shared value.
     */
    void setShared(bool newSharedStatus);

    /*!
     *  Set the shared value to unspecified.
     */
    void clearShared();

private:

    /*!
     *  Copy the memory remaps.
     *
     *      @param [in] other   The memory map being copied.
     */
    void copyMemoryRemaps(const MemoryMap& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Memory remaps of the memory map.
    QSharedPointer<QList<QSharedPointer<MemoryRemap> > > memoryRemaps_;

	//! Defines the number of data bits in each address increment of the memory map.
    QString addressUnitBits_;

    //! Defines the shared status.
    BooleanValue shared_;
};

#endif // MEMORYMAP_H
