//-----------------------------------------------------------------------------
// File: AbstractionDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Writer for IP-XACT AbstractionDefinition element.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionWriter.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstractionWriter.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstractionWriter.h>
#include <IPXACTmodels/AbstractionDefinition/PacketWriter.h>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writeAbstractionDefinition()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writeAbstractionDefinition(QXmlStreamWriter& writer, 
    QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
    writer.writeStartDocument();
    
    DocumentWriter::writeTopComments(writer, abstractionDefinition);

    DocumentWriter::writeXmlProcessingInstructions(writer, abstractionDefinition);

    writer.writeStartElement(QStringLiteral("ipxact:abstractionDefinition"));
    DocumentWriter::writeNamespaceDeclarations(writer, abstractionDefinition);

    DocumentWriter::writeDocumentNameGroup(writer, abstractionDefinition);

    Details::writeBusType(writer, abstractionDefinition);

    Details::writeExtends(writer, abstractionDefinition);

    Details::writePorts(writer, abstractionDefinition);

    Details::writeDescription(writer, abstractionDefinition);

    Details::writeChoices(writer, abstractionDefinition);

    DocumentWriter::writeParameters(writer, abstractionDefinition);

    DocumentWriter::writeAssertions(writer, abstractionDefinition);

    CommonItemsWriter::writeVendorExtensions(writer, abstractionDefinition);

    writer.writeEndElement(); // "ipxact:abstractionDefinition"
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writeBusType()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writeBusType(QXmlStreamWriter& writer,
    QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
        writer.writeStartElement(QStringLiteral("ipxact:busType"));
        CommonItemsWriter::writeVLNVAttributes(writer, abstractionDefinition->getBusType());
        writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writeExtends()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writeExtends(QXmlStreamWriter& writer,
    QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
    if (!abstractionDefinition->getExtends().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:extends"));
        CommonItemsWriter::writeVLNVAttributes(writer, abstractionDefinition->getExtends());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writePorts()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writePorts(QXmlStreamWriter& writer, 
    QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
    QList<QSharedPointer<PortAbstraction> > logicalPorts = *abstractionDefinition->getLogicalPorts(); 
    if (!logicalPorts.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:ports"));
        for (auto logicalPort : logicalPorts)
        {
            writePort(writer, logicalPort, abstractionDefinition->getRevision());
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writePort(QXmlStreamWriter& writer,
    QSharedPointer<PortAbstraction> logicalPort, Document::Revision revision)
{
    writer.writeStartElement(QStringLiteral("ipxact:port"));

    if (!logicalPort->isPresent().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:isPresent"), logicalPort->isPresent());
    }

    writer.writeTextElement(QStringLiteral("ipxact:logicalName"), logicalPort->name());

    if (!logicalPort->displayName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:displayName"), logicalPort->displayName());
    }

    if (!logicalPort->shortDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:shortDescription"), logicalPort->shortDescription());
    }

    if (!logicalPort->description().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), logicalPort->description());
    }

    if (logicalPort->getMatch())
    {
        writer.writeTextElement(QStringLiteral("ipxact:match"), QStringLiteral("true"));
    }

    writeWire(writer, logicalPort, revision);

    writeTransactional(writer, logicalPort, revision);

    writePackets(writer, logicalPort);

    CommonItemsWriter::writeVendorExtensions(writer, logicalPort);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writeWire()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writeWire(QXmlStreamWriter& writer, 
    QSharedPointer<PortAbstraction> logicalPort, Document::Revision revision)
{
    if (logicalPort->hasWire())
    {
        WireAbstractionWriter wireWriter;
        wireWriter.writeWire(writer, logicalPort->getWire(), revision);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writeTransactional()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writeTransactional(QXmlStreamWriter& writer,
    QSharedPointer<PortAbstraction> logicalPort, Document::Revision revision)
{
    if (logicalPort->hasTransactional())
    {
        TransactionalAbstractionWriter transactionalWriter;
        transactionalWriter.writeTransactional(writer, logicalPort->getTransactional(), revision);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writePackets()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writePackets(QXmlStreamWriter& writer,
    QSharedPointer<PortAbstraction> logicalPort)
{
    auto packets = logicalPort->getPackets();

    if (packets->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:packets"));

    for (auto const& packet : *packets)
    {
        PacketWriter::writePacket(writer, packet);
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writeDescription()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writeDescription(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
    if (abstractionDefinition->getRevision() == Document::Revision::Std22)
    {
        return;
    }

    CommonItemsWriter::writeDescription(writer, abstractionDefinition->getDescription());
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::Details::writeChoices()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::Details::writeChoices(QXmlStreamWriter& writer, QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
    if (abstractionDefinition->getRevision() != Document::Revision::Std22)
    {
        return;
    }

    CommonItemsWriter::writeChoices(writer, abstractionDefinition->getChoices());
}
