//-----------------------------------------------------------------------------
// File: PortWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.09.2015
//
// Description:
// Writer class for IP-XACT port element.
//-----------------------------------------------------------------------------

#include "PortWriter.h"

#include <IPXACTmodels/common/ProtocolWriter.h>
#include <IPXACTmodels/common/Vector.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/TransactionalTypes.h>

//-----------------------------------------------------------------------------
// Function: PortWriter::PortWriter()
//-----------------------------------------------------------------------------
PortWriter::PortWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: PortWriter::~PortWriter()
//-----------------------------------------------------------------------------
PortWriter::~PortWriter()
{

}

//-----------------------------------------------------------------------------
// Function: PortWriter::writePort()
//-----------------------------------------------------------------------------
void PortWriter::writePort(QXmlStreamWriter& writer, QSharedPointer<Port> port) const
{
    writer.writeStartElement("ipxact:port");

    NameGroupWriter nameWriter;
    nameWriter.writeNameGroup(writer, port);

    writeIsPresent(writer, port->getIsPresent());

    if (port->getWire())
    {
        writeWire(writer, port->getWire());
    }
    else if (port->getTransactional())
    {
        writeTransactional(writer, port->getTransactional());
    }

    writeArrays(writer, port->getArrays());

    writeVendorExtensions(writer, port);

    writer.writeEndElement(); // ipxact:port
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeWire()
//-----------------------------------------------------------------------------
void PortWriter::writeWire(QXmlStreamWriter& writer, QSharedPointer<Wire> wire) const
{
    writer.writeStartElement("ipxact:wire");

    if (wire->getAllLogicalDirectionsAllowed())
    {
        writer.writeAttribute("allLogicalDirectionsAllowed", "true");
    }

    writer.writeTextElement("ipxact:direction", DirectionTypes::direction2Str(wire->getDirection()));

    writeVector(writer, wire->getVector());

    writeWireTypeDefinitions(writer, wire->getWireTypeDefs());

    writeWireDefaultValue(writer, wire->getDefaultDriverValue());

    writer.writeEndElement(); // ipxact:wire
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeVector()
//-----------------------------------------------------------------------------
void PortWriter::writeVector(QXmlStreamWriter& writer, QSharedPointer<Vector> vector) const
{
    if (vector)
    {
        writer.writeStartElement("ipxact:vectors");

        writer.writeStartElement("ipxact:vector");

        writer.writeTextElement("ipxact:left", vector->getLeft());
        writer.writeTextElement("ipxact:right", vector->getRight());

        writer.writeEndElement(); // ipxact:vector

        writer.writeEndElement(); // ipxact:vectors
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTypeDefinitions()
//-----------------------------------------------------------------------------
void PortWriter::writeWireTypeDefinitions(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions) const
{
    if (typeDefinitions && !typeDefinitions->isEmpty())
    {
        writer.writeStartElement("ipxact:wireTypeDefs");

        foreach(QSharedPointer<WireTypeDef> singleTypeDefinition, *typeDefinitions)
        {
            writer.writeStartElement("ipxact:wireTypeDef");

            writeSingleTypeDefinition(writer, singleTypeDefinition, "constrained");

            writer.writeEndElement(); // ipxact:wireTypeDef
        }

        writer.writeEndElement(); // ipxact:wireTypeDefs
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeSingleTypeDefinition()
//-----------------------------------------------------------------------------
void PortWriter::writeSingleTypeDefinition(QXmlStreamWriter& writer, QSharedPointer<WireTypeDef> typeDefinition,
    QString const& attributeName) const
{
    if (!typeDefinition->getTypeName().isEmpty())
    {
        writer.writeStartElement("ipxact:typeName");

        if (typeDefinition->isConstrained())
        {
            writer.writeAttribute(attributeName, "true");
        }

        writer.writeCharacters(typeDefinition->getTypeName());
        writer.writeEndElement(); // ipxact:typeName
    }

    foreach (QString definition, typeDefinition->getTypeDefinitions())
    {
        writer.writeTextElement("ipxact:typeDefinition", definition);
    }

    foreach (QString viewReference, typeDefinition->getViewRefs())
    {
        writer.writeTextElement("ipxact:viewRef", viewReference);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeWireDefaultValue()
//-----------------------------------------------------------------------------
void PortWriter::writeWireDefaultValue(QXmlStreamWriter& writer, QString const& defaultValue) const
{
    if (!defaultValue.isEmpty())
    {
        writer.writeStartElement("ipxact:drivers");
        writer.writeStartElement("ipxact:driver");

        writer.writeTextElement("ipxact:defaultValue", defaultValue);

        writer.writeEndElement(); // ipxact:driver
        writer.writeEndElement(); // ipxact:drivers
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTransactional()
//-----------------------------------------------------------------------------
void PortWriter::writeTransactional(QXmlStreamWriter& writer, QSharedPointer<Transactional> transactional) const
{
    writer.writeStartElement("ipxact:transactional");

    if (transactional->getAllLogicalInitiativesAllowed())
    {
        writer.writeAttribute("allLogicalInitiativesAllowed", "true");
    }

    writer.writeTextElement("ipxact:initiative", transactional->getInitiative());

    writeTransactionalKind(writer, transactional->getKind());

    writeTransactionalBusWidth(writer, transactional->getBusWidth());

    ProtocolWriter protocolWriter;
    protocolWriter.writeProtocol(writer, transactional->getProtocol());

    writeTransactionalTypeDefinitions(writer, transactional);

    writeTransactionalMinMaxConnections(writer, transactional);

    writer.writeEndElement(); // ipxact:transactional
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTransactionalKind()
//-----------------------------------------------------------------------------
void PortWriter::writeTransactionalKind(QXmlStreamWriter& writer, QString const& kind) const
{
    if (!kind.isEmpty())
    {
        writer.writeStartElement("ipxact:kind");
        if (TransactionalTypes::isIpXactKindType(kind))
        {
            writer.writeCharacters(kind);
        }
        else
        {
            writer.writeAttribute("custom", kind);
            writer.writeCharacters("custom");
        }

        writer.writeEndElement(); // ipxact:kind
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTransactionalBusWidth()
//-----------------------------------------------------------------------------
void PortWriter::writeTransactionalBusWidth(QXmlStreamWriter& writer, QString const& busWidth) const
{
    if (!busWidth.isEmpty())
    {
        writer.writeTextElement("ipxact:busWidth", busWidth);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTransactionalTypeDefinitions()
//-----------------------------------------------------------------------------
void PortWriter::writeTransactionalTypeDefinitions(QXmlStreamWriter& writer,
    QSharedPointer<Transactional> transactional) const
{
    if (!transactional->getTransTypeDef()->isEmpty())
    {
        writer.writeStartElement("ipxact:transTypeDefs");

        foreach (QSharedPointer<WireTypeDef> transactionalTypeDefinition, *transactional->getTransTypeDef())
        {
            writer.writeStartElement("ipxact:transTypeDef");

            writeSingleTypeDefinition(writer, transactionalTypeDefinition, "exact");

            writer.writeEndElement(); // ipxact:transTypeDef
        }

        writer.writeEndElement(); // ipxact:transTypeDefs
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTransactionalMinMaxConnections()
//-----------------------------------------------------------------------------
void PortWriter::writeTransactionalMinMaxConnections(QXmlStreamWriter& writer,
    QSharedPointer<Transactional> transactional) const
{
    if (!transactional->getMinConnections().isEmpty() || !transactional->getMaxConnections().isEmpty())
    {
        writer.writeStartElement("ipxact:connection");

        if (!transactional->getMaxConnections().isEmpty())
        {
            writer.writeTextElement("ipxact:maxConnections", transactional->getMaxConnections());
        }
        if (!transactional->getMinConnections().isEmpty())
        {
            writer.writeTextElement("ipxact:minConnections", transactional->getMinConnections());
        }
        writer.writeEndElement(); // ipxact:connection
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeArrays()
//-----------------------------------------------------------------------------
void PortWriter::writeArrays(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Array> > > arrays) const
{
    if (!arrays->isEmpty())
    {
        writer.writeStartElement("ipxact:arrays");

        foreach (QSharedPointer<Array> singleArray, *arrays)
        {
            writer.writeStartElement("ipxact:array");

            writer.writeTextElement("ipxact:left", singleArray->getLeft());
            writer.writeTextElement("ipxact:right", singleArray->getRight());

            writer.writeEndElement(); // ipxact:array
        }

        writer.writeEndElement(); // ipxact:arrays
    }
}
