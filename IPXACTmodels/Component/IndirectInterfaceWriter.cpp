//-----------------------------------------------------------------------------
// File: IndirectInterfaceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2017
//
// Description:
// XML writer class for IP-XACT Indirect Interface element.
//-----------------------------------------------------------------------------

#include "IndirectInterfaceWriter.h"
#include "IndirectInterface.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeIndirectInterface()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeIndirectInterface(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:indirectInterface"));

    NameGroupWriter::writeNameGroup(writer, indirectInterface);

    Details::writeIndirectFields(writer, indirectInterface);

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:memoryMapRef"), indirectInterface->getMemoryMapRef());

    Details::writeTransparentBridges(writer, indirectInterface, docRevision);

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:bitsInLau"), indirectInterface->getBitsInLau());

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:endianness"), indirectInterface->getEndianness());

    CommonItemsWriter::writeParameters(writer, indirectInterface->getParameters());

    CommonItemsWriter::writeVendorExtensions(writer, indirectInterface);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeIndirectFields()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::Details::writeIndirectFields(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface)
{
    writer.writeStartElement(QStringLiteral("ipxact:indirectAddressRef"));
    writer.writeCharacters(indirectInterface->getIndirectAddressRef());
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("ipxact:indirectDataRef"));
    writer.writeCharacters(indirectInterface->getIndirectDataRef());
    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeTransparentBridges()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::Details::writeTransparentBridges(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface, Document::Revision docRevision)
{    
    for (QSharedPointer<TransparentBridge> bridge : *indirectInterface->getTransparentBridges())
    {
        writer.writeStartElement(QStringLiteral("ipxact:transparentBridge"));

        QString referenceName;
        if (docRevision == Document::Revision::Std14)
        {
            referenceName = QStringLiteral("masterRef");
        }
        else if (docRevision == Document::Revision::Std22)
        {
            referenceName = QStringLiteral("initiatorRef");
        }

        writer.writeAttribute(referenceName, bridge->getInitiatorRef());

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeIsPresent(writer, bridge->getIsPresent());
        }

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeVendorExtensions(writer, bridge);
        }

        writer.writeEndElement();
    }
}
