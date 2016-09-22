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
PortWriter::PortWriter(QObject* parent) : CommonItemsWriter(parent)
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
    writer.writeStartElement(QStringLiteral("ipxact:port"));

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
    writer.writeStartElement(QStringLiteral("ipxact:wire"));

    if (wire->getAllLogicalDirectionsAllowed())
    {
        writer.writeAttribute(QStringLiteral("allLogicalDirectionsAllowed"), QStringLiteral("true"));
    }

    writer.writeTextElement(QStringLiteral("ipxact:direction"), DirectionTypes::direction2Str(wire->getDirection()));

    if (wire->getVector() && (!wire->getVector()->getLeft().isEmpty() || !wire->getVector()->getRight().isEmpty()))
    {
        writeVector(writer, wire->getVector());
    }

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
        writer.writeStartElement(QStringLiteral("ipxact:vectors"));

        writer.writeStartElement(QStringLiteral("ipxact:vector"));

        writer.writeTextElement(QStringLiteral("ipxact:left"), vector->getLeft());
        writer.writeTextElement(QStringLiteral("ipxact:right"), vector->getRight());

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
        writer.writeStartElement(QStringLiteral("ipxact:wireTypeDefs"));

        foreach(QSharedPointer<WireTypeDef> singleTypeDefinition, *typeDefinitions)
        {
            writer.writeStartElement(QStringLiteral("ipxact:wireTypeDef"));

            writeSingleTypeDefinition(writer, singleTypeDefinition, QStringLiteral("constrained"));

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
        writer.writeStartElement(QStringLiteral("ipxact:typeName"));

        if (typeDefinition->isConstrained())
        {
            writer.writeAttribute(attributeName, QStringLiteral("true"));
        }

        writer.writeCharacters(typeDefinition->getTypeName());
        writer.writeEndElement(); // ipxact:typeName
    }

    foreach (QString definition, typeDefinition->getTypeDefinitions())
    {
        writer.writeTextElement(QStringLiteral("ipxact:typeDefinition"), definition);
    }

    foreach (QString viewReference, typeDefinition->getViewRefs())
    {
        writer.writeTextElement(QStringLiteral("ipxact:viewRef"), viewReference);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeWireDefaultValue()
//-----------------------------------------------------------------------------
void PortWriter::writeWireDefaultValue(QXmlStreamWriter& writer, QString const& defaultValue) const
{
    if (!defaultValue.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:drivers"));
        writer.writeStartElement(QStringLiteral("ipxact:driver"));

        writer.writeTextElement(QStringLiteral("ipxact:defaultValue"), defaultValue);

        writer.writeEndElement(); // ipxact:driver
        writer.writeEndElement(); // ipxact:drivers
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTransactional()
//-----------------------------------------------------------------------------
void PortWriter::writeTransactional(QXmlStreamWriter& writer, QSharedPointer<Transactional> transactional) const
{
    writer.writeStartElement(QStringLiteral("ipxact:transactional"));

    if (transactional->getAllLogicalInitiativesAllowed())
    {
        writer.writeAttribute(QStringLiteral("allLogicalInitiativesAllowed"), QStringLiteral("true"));
    }

    writer.writeTextElement(QStringLiteral("ipxact:initiative"), transactional->getInitiative());

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
        writer.writeStartElement(QStringLiteral("ipxact:kind"));
        if (TransactionalTypes::isIpXactKindType(kind))
        {
            writer.writeCharacters(kind);
        }
        else
        {
            writer.writeAttribute(QStringLiteral("custom"), kind);
            writer.writeCharacters(QStringLiteral("custom"));
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
        writer.writeTextElement(QStringLiteral("ipxact:busWidth"), busWidth);
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
        writer.writeStartElement(QStringLiteral("ipxact:transTypeDefs"));

        foreach (QSharedPointer<WireTypeDef> transactionalTypeDefinition, *transactional->getTransTypeDef())
        {
            writer.writeStartElement(QStringLiteral("ipxact:transTypeDef"));

            writeSingleTypeDefinition(writer, transactionalTypeDefinition, QStringLiteral("exact"));

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
        writer.writeStartElement(QStringLiteral("ipxact:connection"));

        if (!transactional->getMaxConnections().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:maxConnections"), transactional->getMaxConnections());
        }
        if (!transactional->getMinConnections().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:minConnections"), transactional->getMinConnections());
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
        writer.writeStartElement(QStringLiteral("ipxact:arrays"));

        foreach (QSharedPointer<Array> singleArray, *arrays)
        {
            writer.writeStartElement(QStringLiteral("ipxact:array"));

            writer.writeTextElement(QStringLiteral("ipxact:left"), singleArray->getLeft());
            writer.writeTextElement(QStringLiteral("ipxact:right"), singleArray->getRight());

            writer.writeEndElement(); // ipxact:array
        }

        writer.writeEndElement(); // ipxact:arrays
    }
}
