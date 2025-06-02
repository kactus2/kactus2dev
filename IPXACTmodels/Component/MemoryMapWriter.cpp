//-----------------------------------------------------------------------------
// File: MemoryMapWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Writer for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#include "MemoryMapWriter.h"
#include "MemoryMap.h"
#include "MemoryRemap.h"
#include "MemoryMapBase.h"

#include "AddressBlock.h"
#include "MemoryMapBaseWriter.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/AddressBlockWriter.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::writeMemoryMap()
//-----------------------------------------------------------------------------
void MemoryMapWriter::writeMemoryMap(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap, 
    Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:memoryMap"));

    if (docRevision == Document::Revision::Std14)
    {
	    MemoryMapBaseWriter::writeMemoryMapBase(writer, memoryMap, docRevision);

        Details::writeMemoryRemaps(writer, memoryMap, docRevision);

        Details::writeAddressUnitBits(writer, memoryMap);

        Details::writeShared(writer, memoryMap);

        CommonItemsWriter::writeVendorExtensions(writer, memoryMap);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        NameGroupWriter::writeNameGroup(writer, memoryMap, docRevision);

        Details::writeMemoryMapDefinitionReference(writer, memoryMap);

        MemoryMapBaseWriter::Details::writeMemoryBlocks(writer, memoryMap, docRevision);

        Details::writeMemoryRemaps(writer, memoryMap, docRevision);

        Details::writeAddressUnitBits(writer, memoryMap);

        Details::writeShared(writer, memoryMap);

        CommonItemsWriter::writeVendorExtensions(writer, memoryMap);
    }

    writer.writeEndElement(); // ipxact:memoryMap
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::Details::writeMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMapWriter::Details::writeMemoryRemaps(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap, 
    Document::Revision docRevision)
{
    for (auto const& memoryRemap : *memoryMap->getMemoryRemaps())
    {
        writer.writeStartElement(QStringLiteral("ipxact:memoryRemap"));

        if (docRevision == Document::Revision::Std14)
        {
            writer.writeAttribute(QStringLiteral("state"), memoryRemap->getRemapState());

            NameGroupWriter::writeNameGroup(writer, memoryRemap, docRevision);

            CommonItemsWriter::writeIsPresent(writer, memoryRemap->getIsPresent());

            MemoryMapBaseWriter::Details::writeMemoryBlocks(writer, memoryRemap, docRevision);
        }
        else if (docRevision == Document::Revision::Std22)
        {
            NameGroupWriter::writeNameGroup(writer, memoryRemap, docRevision);

            CommonItemsWriter::writeModeReferences(writer, memoryRemap->getModeReferences());

            writeRemapDefinitionReference(writer, memoryRemap);

            MemoryMapBaseWriter::Details::writeMemoryBlocks(writer, memoryRemap, docRevision);

            CommonItemsWriter::writeVendorExtensions(writer, memoryRemap);
        }

        writer.writeEndElement(); // ipxact:memoryRemap
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::Details::writeAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryMapWriter::Details::writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:addressUnitBits"),
        memoryMap->getAddressUnitBits(false));
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::Details::writeShared()
//-----------------------------------------------------------------------------
void MemoryMapWriter::Details::writeShared(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap)
{
    if (!memoryMap->getShared().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:shared"));

        if (memoryMap->getShared() == QLatin1String("true"))
        {
            writer.writeCharacters(QStringLiteral("yes"));
        }
        else
        {
            writer.writeCharacters(QStringLiteral("no"));
        }

        writer.writeEndElement(); // ipxact:shared
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::Details::writeMemoryMapDefinitionReference()
//-----------------------------------------------------------------------------
void MemoryMapWriter::Details::writeMemoryMapDefinitionReference(QXmlStreamWriter& writer, 
    QSharedPointer<MemoryMap> memoryMap)
{
    if (!memoryMap->getMemoryMapDefinitionReference().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:memoryMapDefinitionRef"));

        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("typeDefinitions"),
            memoryMap->getTypeDefinitionsReference());

        writer.writeCharacters(memoryMap->getMemoryMapDefinitionReference());

        writer.writeEndElement(); // ipxact:remapDefinitionRef
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapWriter::Details::writeRemapDefinitionReference()
//-----------------------------------------------------------------------------
void MemoryMapWriter::Details::writeRemapDefinitionReference(QXmlStreamWriter& writer, 
    QSharedPointer<MemoryRemap> memoryRemap)
{
    if (!memoryRemap->getMemoryRemapDefinitionReference().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:remapDefinitionRef"));

        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("typeDefinitions"),
            memoryRemap->getTypeDefinitionsReference());

        writer.writeCharacters(memoryRemap->getMemoryRemapDefinitionReference());

        writer.writeEndElement(); // ipxact:remapDefinitionRef
    }
}
