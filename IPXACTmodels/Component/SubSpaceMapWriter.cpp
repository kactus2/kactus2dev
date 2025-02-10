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
// Function: SubSpaceMapWriter::writeSubSpaceMap()
//-----------------------------------------------------------------------------
void SubSpaceMapWriter::writeSubSpaceMap(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap, 
    Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:subspaceMap"));

    Details::writeAttributes(writer, subMap, docRevision);

    MemoryBlockBaseWriter::writeNameGroup(writer, subMap, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, subMap->getIsPresent());
    }

    MemoryBlockBaseWriter::writeBaseAddress(writer, subMap);

    CommonItemsWriter::writeParameters(writer, subMap->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, subMap);

    writer.writeEndElement(); // ipxact:subspaceMap
}

//-----------------------------------------------------------------------------
// Function: SubSpaceMapWriter::Details::writeAttributes()
//-----------------------------------------------------------------------------
void SubSpaceMapWriter::Details::writeAttributes(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap, 
    Document::Revision docRevision)
{
    QString initiatorAttrName = docRevision == Document::Revision::Std22
        ? QStringLiteral("initiatorRef") : QStringLiteral("masterRef");

    writer.writeAttribute(initiatorAttrName, subMap->getInitiatorReference());

    if (!subMap->getSegmentReference().isEmpty())
    {
        writer.writeAttribute(QStringLiteral("segmentRef"), subMap->getSegmentReference());
    }
}
