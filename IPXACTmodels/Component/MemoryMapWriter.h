//-----------------------------------------------------------------------------
// File: MemoryMapWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Writer class for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPWRITER_H
#define MEMORYMAPWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include "MemoryMapBaseWriter.h"

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//class AddressBlock;
class MemoryMap;
class MemoryMapBase;

//-----------------------------------------------------------------------------
//! Writer class for ipxact:memoryMap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapWriter : public MemoryMapBaseWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    MemoryMapWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~MemoryMapWriter();

    /*!
     *  Write a memory map to an XML file.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] memoryMap   The memory map to be written.
     */
    void writeMemoryMap(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const;

private:

    //! No copying allowed.
    MemoryMapWriter(MemoryMapWriter const& rhs);
    MemoryMapWriter& operator=(MemoryMapWriter const& rhs);

    /*!
     *  Write the memory remaps.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] memoryMap   The selected memory map.
     */
    void writeMemoryRemaps(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Write the address unit bits.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] memoryMap   The selected memory map.
     */
    void writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Write the shared value.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] memoryMap   The selected memory map.
     */
    void writeShared(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const;
};

#endif // MEMORYMAPWRITER_H