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
#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/Component/AccessHandleWriter.h>

//-----------------------------------------------------------------------------
// Function: PortWriter::writePort()
//-----------------------------------------------------------------------------
void PortWriter::writePort(QXmlStreamWriter& writer, QSharedPointer<Port> port, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:port"));

    NameGroupWriter::writeNameGroup(writer, port, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, port->getIsPresent());
    }

    if (port->getWire())
    {
        Details::writeWire(writer, port->getWire(), docRevision);
    }
    else if (port->getTransactional())
    {
        Details::writeTransactional(writer, port->getTransactional(), docRevision);
    }
    else if (port->getStructured() && docRevision == Document::Revision::Std22)
    {
        Details::writeStructured(writer, port->getStructured());
    }

    Details::writeArrays(writer, port->getArrays());

    Details::writePortAccess(writer, port, docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, port);

    writer.writeEndElement(); // ipxact:port
}

//-----------------------------------------------------------------------------
// Function: PortWriter::Details::writeWire()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeWire(QXmlStreamWriter& writer, QSharedPointer<Wire> wire, Document::Revision docRevision)
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

    writeVectors(writer, wire->getVectors(), docRevision);
    
    writeWireTypeDefinitions(writer, wire->getWireTypeDefs());

    writeWireDriver(writer, wire->getDriver());

    writer.writeEndElement(); // ipxact:wire
}

//-----------------------------------------------------------------------------
// Function: PortWriter::Details::writeVector()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeVectors(QXmlStreamWriter& writer, QSharedPointer<QList<Vector> > vectors,
    Document::Revision docRevision)
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
// Function: PortWriter::Details::writeTypeDefinitions()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeWireTypeDefinitions(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions)
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
// Function: PortWriter::Details::hasNonEmptyTypeDefinitions()
//-----------------------------------------------------------------------------
bool PortWriter::Details::hasNonEmptyTypeDefinitions(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions)
   
{
    return std::any_of(typeDefinitions->cbegin(), typeDefinitions->cend(), [](auto const& singleTypeDefinition)
        {
            return singleTypeDefinition->isEmpty() == false;
        });
}

//-----------------------------------------------------------------------------
// Function: PortWriter::Details::writeSingleTypeDefinition()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeSingleTypeDefinition(QXmlStreamWriter& writer, QSharedPointer<WireTypeDef> typeDefinition,
    QString const& attributeName)
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
// Function: PortWriter::Details::writeWireDriver()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeWireDriver(QXmlStreamWriter& writer, QSharedPointer<Driver> driver)
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
// Function: PortWriter::Details::writeTransactional()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeTransactional(QXmlStreamWriter& writer, QSharedPointer<Transactional> transactional,
    Document::Revision docRevision)
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
// Function: PortWriter::Details::writeTransactionalKind()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeTransactionalKind(QXmlStreamWriter& writer, QString const& kind)
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
// Function: PortWriter::Details::writeTransactionalBusWidth()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeTransactionalBusWidth(QXmlStreamWriter& writer, QString const& busWidth)
{
    if (!busWidth.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:busWidth"), busWidth);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::Details::writeTransactionalTypeDefinitions()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeTransactionalTypeDefinitions(QXmlStreamWriter& writer,
    QSharedPointer<Transactional> transactional)
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
// Function: PortWriter::Details::writeTransactionalMinMaxConnections()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeTransactionalMinMaxConnections(QXmlStreamWriter& writer,
    QSharedPointer<Transactional> transactional)
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
// Function: PortWriter::Details::writeStructured()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeStructured(QXmlStreamWriter& writer, QSharedPointer<Structured> structured)
{
    writer.writeStartElement(QStringLiteral("ipxact:structured"));

    if (structured->isPacked())
    {
        writer.writeAttribute(QStringLiteral("packed"), QStringLiteral("true"));
    }
    
    writeStructuredType(writer, structured);

    writeVectors(writer, structured->getVectors(), Document::Revision::Std22);

    writeSubPorts(writer, structured);

    writer.writeEndElement(); // ipxact:structured
}

//-----------------------------------------------------------------------------
// Function: PortWriter::Details::writeStructuredType()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeStructuredType(QXmlStreamWriter& writer, QSharedPointer<Structured> structured)
{
    if (structured->getType() != Structured::Type::Undefined)
    {
		writer.writeStartElement(QStringLiteral("ipxact:") + Structured::toString(structured->getType()));
		if (structured->getType() == Structured::Type::Struct || structured->getType() == Structured::Type::Union)
		{
			writer.writeAttribute(QStringLiteral("direction"), DirectionTypes::direction2Str(structured->getDirection()));
		}
		else if (structured->getType() == Structured::Type::Interface &&
			structured->getDirection() == DirectionTypes::DIRECTION_PHANTOM)
		{
			writer.writeAttribute(QStringLiteral("phantom"), QStringLiteral("true"));
		}
		writer.writeEndElement(); // ipxact:struct/union/interface
    }
}

//-----------------------------------------------------------------------------
// Function: PortWriter::Details::writeSubPorts()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeSubPorts(QXmlStreamWriter& writer, QSharedPointer<Structured> structured)
{
    if (structured->getSubPorts()->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:subPorts"));

    for (auto const& subPort : *structured->getSubPorts())
    {
        writer.writeStartElement(QStringLiteral("ipxact:subPort"));

        if (subPort->isIO())
        {
            writer.writeAttribute(QStringLiteral("isIO"), QStringLiteral("true"));
        }

        NameGroupWriter::writeNameGroup(writer, subPort, Document::Revision::Std22);

        if (subPort->isWire())
        {
            writeWire(writer, subPort->getWire(), Document::Revision::Std22);
        }
        else if (subPort->isStructured())
        {
            writeStructured(writer, subPort->getStructured());
        }

        writer.writeEndElement(); // ipxact:subPort
    }

    writer.writeEndElement(); // ipxact:subPorts
}

//-----------------------------------------------------------------------------
// Function: PortWriter::Details::writeArrays()
//-----------------------------------------------------------------------------
void PortWriter::Details::writeArrays(QXmlStreamWriter& writer, QSharedPointer<QList<Array> > arrays)
{
    if (arrays->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:arrays"));

    for (auto const& singleArray : *arrays)
    {
        writer.writeStartElement(QStringLiteral("ipxact:array"));

        writer.writeTextElement(QStringLiteral("ipxact:left"), singleArray.getLeft());
        writer.writeTextElement(QStringLiteral("ipxact:right"), singleArray.getRight());

        writer.writeEndElement(); // ipxact:array
    }

    writer.writeEndElement(); // ipxact:arrays
}

void PortWriter::Details::writePortAccess(QXmlStreamWriter& writer, QSharedPointer<Port> port, Document::Revision docRevision)
{
    auto access = port->getAccess();
    if (access == nullptr) return;

    writer.writeStartElement(QStringLiteral("ipxact:access"));

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:portAccessType"), Port::Access::typeToString(access->type_));

    if (access->accessHandles_->isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:accessHandles"));

        for (auto const& accessHandle : *access->accessHandles_)
        {
            AccessHandleWriter::writeAccessHandle(writer, accessHandle, docRevision);
        }
        
        writer.writeEndElement(); // ipxact:accessHandles   
    }

    writer.writeEndElement(); // ipxact:access
}

