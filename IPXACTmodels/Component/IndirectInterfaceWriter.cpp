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
// Function: IndirectInterfaceWriter::IndirectInterfaceWriter()
//-----------------------------------------------------------------------------
IndirectInterfaceWriter::IndirectInterfaceWriter()
{

}


//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::~IndirectInterfaceWriter()
//-----------------------------------------------------------------------------
IndirectInterfaceWriter::~IndirectInterfaceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeIndirectInterface()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeIndirectInterface(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    writer.writeStartElement(QStringLiteral("ipxact:indirectInterface"));

    writeNameGroup(writer, indirectInterface);

    writeIndirectFields(writer, indirectInterface);

    writeMemoryMapReference(writer, indirectInterface);

    writeTransparentBridges(writer, indirectInterface);

    writeBitsInLau(writer, indirectInterface);

    writeEndianness(writer, indirectInterface);

    writeParameters(writer, indirectInterface->getParameters());

    writeVendorExtensions(writer, indirectInterface);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeNameGroup(QXmlStreamWriter& writer, 
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    NameGroupWriter nameWriter;
    nameWriter.writeNameGroup(writer, indirectInterface);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeIndirectFields()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeIndirectFields(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    writer.writeStartElement(QStringLiteral("ipxact:indirectAddressRef"));
    writer.writeCharacters(indirectInterface->getIndirectAddressRef());
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("ipxact:indirectDataRef"));
    writer.writeCharacters(indirectInterface->getIndirectDataRef());
    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeMemoryMapReference()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeMemoryMapReference(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    if (!indirectInterface->getMemoryMapRef().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:memoryMapRef"));
        writer.writeCharacters(indirectInterface->getMemoryMapRef());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeTransparentBridges()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeTransparentBridges(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface) const
{    
    foreach (QSharedPointer<TransparentBridge> bridge, *indirectInterface->getTransparentBridges())
    {
        writer.writeStartElement(QStringLiteral("ipxact:transparentBridge"));
        writer.writeAttribute(QStringLiteral("masterRef"), bridge->getMasterRef());

        writeIsPresent(writer, bridge->getIsPresent());

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeBitsInLau()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeBitsInLau(QXmlStreamWriter& writer, 
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    if (!indirectInterface->getBitsInLau().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:bitsInLau"));
        writer.writeCharacters(indirectInterface->getBitsInLau());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceWriter::writeEndianness()
//-----------------------------------------------------------------------------
void IndirectInterfaceWriter::writeEndianness(QXmlStreamWriter& writer,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    if (!indirectInterface->getEndianness().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:endianness"));
        writer.writeCharacters(indirectInterface->getEndianness());
        writer.writeEndElement();
    }
}
