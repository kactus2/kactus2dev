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
#include <IPXACTmodels/common/QualifierWriter.h>

//-----------------------------------------------------------------------------
// Function: PortWriter::writePort()
//-----------------------------------------------------------------------------
void PortWriter::writePort(QXmlStreamWriter& writer, QSharedPointer<Port> port, Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:port"));

    NameGroupWriter::writeNameGroup(writer, port, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, port->getIsPresent());
    }

    if (port->getWire())
    {
        writeWire(writer, port->getWire(), docRevision);
    }
    else if (port->getTransactional())
    {
        writeTransactional(writer, port->getTransactional(), docRevision);
    }
    else if (port->getStructural())
    {
        writeStructural(writer, port->getStructural());
    }

    writeArrays(writer, port->getArrays());

    CommonItemsWriter::writeVendorExtensions(writer, port);

    writer.writeEndElement(); // ipxact:port
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeWire()
//-----------------------------------------------------------------------------
void PortWriter::writeWire(QXmlStreamWriter& writer, QSharedPointer<Wire> wire, Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:wire"));

    if (wire->getAllLogicalDirectionsAllowed())
    {
        writer.writeAttribute(QStringLiteral("allLogicalDirectionsAllowed"), QStringLiteral("true"));
    }

    writer.writeTextElement(QStringLiteral("ipxact:direction"), DirectionTypes::direction2Str(wire->getDirection()));

    if (docRevision == Document::Revision::Std22)
    {
        QualifierWriter::writeQualifier(writer, wire->getQualifier());
    }

        writeVector(writer, wire->getVectors(), docRevision);
    

    writeWireTypeDefinitions(writer, wire->getWireTypeDefs());

    writeWireDriver(writer, wire->getDriver());

    writer.writeEndElement(); // ipxact:wire
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeVector()
//-----------------------------------------------------------------------------
void PortWriter::writeVector(QXmlStreamWriter& writer, QSharedPointer<QList<Vector> > vectors,
    Document::Revision docRevision) const
{
    if (vectors->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:vectors"));

    for (auto const& vector : *vectors)
    {
        writer.writeStartElement(QStringLiteral("ipxact:vector"));

        if (docRevision == Document::Revision::Std22 && vector.getId().isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("vectorId"), vector.getId());
        }

        writer.writeTextElement(QStringLiteral("ipxact:left"), vector.getLeft());
        writer.writeTextElement(QStringLiteral("ipxact:right"), vector.getRight());

        writer.writeEndElement(); // ipxact:vector
    }

    writer.writeEndElement(); // ipxact:vectors
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTypeDefinitions()
//-----------------------------------------------------------------------------
void PortWriter::writeWireTypeDefinitions(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions) const
{
    if (typeDefinitions && hasNonEmptyTypeDefinitions(typeDefinitions))
    {
        writer.writeStartElement(QStringLiteral("ipxact:wireTypeDefs"));

        for (QSharedPointer<WireTypeDef> singleTypeDefinition : *typeDefinitions)
        {
            if (singleTypeDefinition->isEmpty() == false)
            {
                writer.writeStartElement(QStringLiteral("ipxact:wireTypeDef"));

                writeSingleTypeDefinition(writer, singleTypeDefinition, QStringLiteral("constrained"));

                writer.writeEndElement(); // ipxact:wireTypeDef
            }
        }

        writer.writeEndElement(); // ipxact:wireTypeDefs
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::hasNonEmptyTypeDefinitions()
//-----------------------------------------------------------------------------
bool PortWriter::hasNonEmptyTypeDefinitions(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions)
    const
{
    return std::any_of(typeDefinitions->cbegin(), typeDefinitions->cend(), [](auto const& singleTypeDefinition)
        {
            return singleTypeDefinition->isEmpty() == false;
        });
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

    for (QString const& definition : *typeDefinition->getTypeDefinitions())
    {
        if (!definition.isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:typeDefinition"), definition);
        }
    }

    for (QString const& viewReference : *typeDefinition->getViewRefs())
    {
        if (!viewReference.isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:viewRef"), viewReference);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeWireDriver()
//-----------------------------------------------------------------------------
void PortWriter::writeWireDriver(QXmlStreamWriter& writer, QSharedPointer<Driver> driver) const
{
    if (driver)
    {
        writer.writeStartElement(QStringLiteral("ipxact:drivers"));
        writer.writeStartElement(QStringLiteral("ipxact:driver"));

        CommonItemsWriter::writeRange(writer, driver->getRange());

        writer.writeTextElement(QStringLiteral("ipxact:defaultValue"), driver->getDefaultValue());

        writer.writeEndElement(); // ipxact:driver
        writer.writeEndElement(); // ipxact:drivers
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeTransactional()
//-----------------------------------------------------------------------------
void PortWriter::writeTransactional(QXmlStreamWriter& writer, QSharedPointer<Transactional> transactional,
    Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:transactional"));

    if (transactional->getAllLogicalInitiativesAllowed())
    {
        writer.writeAttribute(QStringLiteral("allLogicalInitiativesAllowed"), QStringLiteral("true"));
    }

    writer.writeTextElement(QStringLiteral("ipxact:initiative"), transactional->getInitiative());

    writeTransactionalKind(writer, transactional->getKind());

    writeTransactionalBusWidth(writer, transactional->getBusWidth());

    if (docRevision == Document::Revision::Std22)
    {
        QualifierWriter::writeQualifier(writer, transactional->getQualifier());
    }

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

        for (QSharedPointer<WireTypeDef> transactionalTypeDefinition : *transactional->getTransTypeDef())
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
// Function: PortWriter::writeStructural()
//-----------------------------------------------------------------------------
void PortWriter::writeStructural(QXmlStreamWriter& writer, QSharedPointer<Structural> structural) const
{
    writer.writeStartElement(QStringLiteral("ipxact:structural"));

    writer.writeStartElement(Structural::toString(structural->getType()));
    if (structural->getType() == Structural::Type::Struct || structural->getType() == Structural::Type::Union)
    {
        writer.writeAttribute(QStringLiteral("direction"), DirectionTypes::direction2Str(structural->getDirection()));
    }
    else if (structural->getType() == Structural::Type::Interface &&
        structural->getDirection() == DirectionTypes::DIRECTION_PHANTOM)
    {
        writer.writeAttribute(QStringLiteral("phantom"), QStringLiteral("true"));
    }

    writer.writeEndElement(); // struct/union/interface

    writer.writeEndElement(); // ipxact:structural
}

//-----------------------------------------------------------------------------
// Function: PortWriter::writeArrays()
//-----------------------------------------------------------------------------
void PortWriter::writeArrays(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Array> > > arrays) const
{
    if (arrays->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:arrays"));

    for (auto const& singleArray : *arrays)
    {
        writer.writeStartElement(QStringLiteral("ipxact:array"));

        writer.writeTextElement(QStringLiteral("ipxact:left"), singleArray->getLeft());
        writer.writeTextElement(QStringLiteral("ipxact:right"), singleArray->getRight());

        writer.writeEndElement(); // ipxact:array
    }

    writer.writeEndElement(); // ipxact:arrays
}
