//-----------------------------------------------------------------------------
// File: ClearboxElementWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.06.2026
//
// Description:
// Writer for the ipxact:clearboxElement/whiteboxElement elements.
//-----------------------------------------------------------------------------

#include "ClearboxElementWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

void ClearboxElementWriter::writeClearboxElement(QXmlStreamWriter& writer, QSharedPointer<ClearboxElement> clearboxElement, Document::Revision docRevision)
{

    if (docRevision == Document::Revision::Std22)
    {
        writer.writeStartElement(QStringLiteral("ipxact:clearboxElement"));
    }
    else
    {
        writer.writeStartElement(QStringLiteral("ipxact:whiteboxElement")); 
    }

    NameGroupWriter::writeNameGroup(writer, clearboxElement, docRevision);

    CommonItemsWriter::writeIsPresent(writer, clearboxElement->getIsPresent());

    if (docRevision == Document::Revision::Std14)
    {
        writer.writeTextElement(QStringLiteral("ipxact:whiteboxType"), ClearboxElement::typeToString(clearboxElement->getType()));
    }
    else
    {
        writer.writeTextElement(QStringLiteral("ipxact:clearboxType"), ClearboxElement::typeToString(clearboxElement->getType()));
    }

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:driveable"), clearboxElement->getDriveable());

    CommonItemsWriter::writeParameters(writer, clearboxElement->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, clearboxElement);

    writer.writeEndElement(); // ipxact:clear/whiteboxElement
}