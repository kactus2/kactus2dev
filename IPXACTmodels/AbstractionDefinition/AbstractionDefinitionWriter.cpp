//-----------------------------------------------------------------------------
// File: AbstractionDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Writer class for IP-XACT AbstractionDefinition element.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionWriter.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstractionWriter.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstractionWriter.h>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::AbstractionDefinitionWriter()
//-----------------------------------------------------------------------------
AbstractionDefinitionWriter::AbstractionDefinitionWriter(QObject *parent): DocumentWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::~AbstractionDefinitionWriter()
//-----------------------------------------------------------------------------
AbstractionDefinitionWriter::~AbstractionDefinitionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writeAbstractionDefinition()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writeAbstractionDefinition(QXmlStreamWriter& writer, 
    QSharedPointer<AbstractionDefinition> abstractionDefinition) const
{
    writer.writeStartDocument();
    
    writeTopComments(writer, abstractionDefinition);

    writeXmlProcessingInstructions(writer, abstractionDefinition);

    writer.writeStartElement("ipxact:abstractionDefinition");
    writeNamespaceDeclarations(writer);

    writeVLNVElements(writer, abstractionDefinition->getVlnv());

    writeBusType(writer, abstractionDefinition);

    writeExtends(writer, abstractionDefinition);

    writePorts(writer, abstractionDefinition);

    writeDescription(writer, abstractionDefinition);

    writeParameters(writer, abstractionDefinition);

    writeAssertions(writer, abstractionDefinition);

    writeVendorExtensions(writer, abstractionDefinition);

    writer.writeEndElement(); // "ipxact:abstractionDefinition"
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writeBusType()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writeBusType(QXmlStreamWriter& writer,
    QSharedPointer<AbstractionDefinition> abstractionDefinition) const
{
        writer.writeStartElement("ipxact:busType");
        writeVLNVAttributes(writer, abstractionDefinition->getBusType());
        writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writeExtends()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writeExtends(QXmlStreamWriter& writer,
    QSharedPointer<AbstractionDefinition> abstractionDefinition) const
{
    if (!abstractionDefinition->getExtends().isEmpty())
    {
        writer.writeStartElement("ipxact:extends");
        writeVLNVAttributes(writer, abstractionDefinition->getExtends());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writePorts()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writePorts(QXmlStreamWriter& writer, 
    QSharedPointer<AbstractionDefinition> abstractionDefinition) const
{
    QList<QSharedPointer<PortAbstraction> > logicalPorts = *abstractionDefinition->getLogicalPorts(); 
    if (!logicalPorts.isEmpty())
    {
        writer.writeStartElement("ipxact:ports");
        foreach (QSharedPointer<PortAbstraction> logicalPort, logicalPorts)
        {
            writePort(writer, logicalPort);
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writePort(QXmlStreamWriter& writer,
    QSharedPointer<PortAbstraction> logicalPort) const
{
    writer.writeStartElement("ipxact:port");

    if (!logicalPort->isPresent().isEmpty())
    {
        writer.writeTextElement("ipxact:isPresent", logicalPort->isPresent());
    }

    writer.writeTextElement("ipxact:logicalName", logicalPort->name());

    if (!logicalPort->displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", logicalPort->displayName());
    }

    if (!logicalPort->description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", logicalPort->description());
    }

    writeWire(writer, logicalPort);

    writeTransactional(writer, logicalPort);

    writeVendorExtensions(writer, logicalPort);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writeWire()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writeWire(QXmlStreamWriter& writer, 
    QSharedPointer<PortAbstraction> logicalPort) const
{
    if (logicalPort->hasWire())
    {
        WireAbstractionWriter wireWriter;
        wireWriter.writeWire(writer, logicalPort->getWire());
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionWriter::writeTransactional()
//-----------------------------------------------------------------------------
void AbstractionDefinitionWriter::writeTransactional(QXmlStreamWriter& writer, QSharedPointer<PortAbstraction> logicalPort) const
{
    if (logicalPort->hasTransactional())
    {
        TransactionalAbstractionWriter transactionalWriter;
        transactionalWriter.writeTransactional(writer, logicalPort->getTransactional());
    }
}
