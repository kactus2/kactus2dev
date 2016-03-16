//-----------------------------------------------------------------------------
// File: MemoryMapWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Writer class for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#include "MemoryMapWriter.h"
#include "MemoryMap.h"
#include "MemoryRemap.h"
#include "MemoryMapBase.h"

#include "AddressBlock.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/AddressBlockWriter.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::MemoryMapWriter()
//-----------------------------------------------------------------------------
MemoryMapWriter::MemoryMapWriter(QObject* parent /* = 0 */):
MemoryMapBaseWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::~MemoryMapWriter()
//-----------------------------------------------------------------------------
MemoryMapWriter::~MemoryMapWriter()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::writeMemoryMap()
//-----------------------------------------------------------------------------
void MemoryMapWriter::writeMemoryMap(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const
{
    writer.writeStartElement("ipxact:memoryMap");

	writeMemoryMapBase(writer, memoryMap);

    writeMemoryRemaps(writer, memoryMap);

    writeAddressUnitBits(writer, memoryMap);

    writeShared(writer, memoryMap);

    writeVendorExtensions(writer, memoryMap);

    writer.writeEndElement(); // ipxact:memoryMap
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::writeMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMapWriter::writeMemoryRemaps(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const
{
    foreach (QSharedPointer<MemoryRemap> memoryRemap, *memoryMap->getMemoryRemaps())
    {
        writer.writeStartElement("ipxact:memoryRemap");

        writer.writeAttribute("state", memoryRemap->getRemapState());

        writeNameGroup(writer, memoryRemap);

        writeIsPresent(writer, memoryRemap->getIsPresent());

        writeMemoryBlocks(writer, memoryRemap);

        writer.writeEndElement(); // ipxact:memoryRemap
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::writeAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryMapWriter::writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const
{
    if (!memoryMap->getAddressUnitBits().isEmpty())
    {
        writer.writeTextElement("ipxact:addressUnitBits", memoryMap->getAddressUnitBits());
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::writeShared()
//-----------------------------------------------------------------------------
void MemoryMapWriter::writeShared(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap) const
{
    if (!memoryMap->getShared().isEmpty())
    {
        writer.writeStartElement("ipxact:shared");

        if (memoryMap->getShared() == "true")
        {
            writer.writeCharacters("yes");
        }
        else
        {
            writer.writeCharacters("no");
        }

        writer.writeEndElement(); // ipxact:shared
    }
}