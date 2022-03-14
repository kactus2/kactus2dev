//-----------------------------------------------------------------------------
// File: SubSpaceMapWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.01.2022
//
// Description:
// Writer class for ipxact:subSpaceMap element.
//-----------------------------------------------------------------------------

#include "SubSpaceMapWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>

//-----------------------------------------------------------------------------
// Function: SubSpaceMapWriter::SubSpaceMapWriter()
//-----------------------------------------------------------------------------
SubSpaceMapWriter::SubSpaceMapWriter():
MemoryBlockBaseWriter()
{

}

//-----------------------------------------------------------------------------
// Function: SubSpaceMapWriter::writeSubSpaceMap()
//-----------------------------------------------------------------------------
void SubSpaceMapWriter::writeSubSpaceMap(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap) const
{
    writer.writeStartElement(QStringLiteral("ipxact:subspaceMap"));

    writeAttributes(writer, subMap);

    writeNameGroup(writer, subMap);

    writeIsPresent(writer, subMap->getIsPresent());

    writeBaseAddress(writer, subMap);

    writeParameters(writer, subMap->getParameters());

    writeVendorExtensions(writer, subMap);

    writer.writeEndElement(); // ipxact:subspaceMap
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMapWriter::writeAttributes()
//-----------------------------------------------------------------------------
void SubSpaceMapWriter::writeAttributes(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap) const
{
    writer.writeAttribute(QStringLiteral("masterRef"), subMap->getMasterReference());

    if (!subMap->getSegmentReference().isEmpty())
    {
        writer.writeAttribute(QStringLiteral("segmentRef"), subMap->getSegmentReference());
    }
}
