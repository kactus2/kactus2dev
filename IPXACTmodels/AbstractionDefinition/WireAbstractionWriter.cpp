//-----------------------------------------------------------------------------
// File: WireAbstractionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Writer for IP-XACT Wire element within abstraction definition.
//-----------------------------------------------------------------------------

#include "WireAbstractionWriter.h"
#include "WireAbstraction.h"
#include "WirePort.h"

#include <IPXACTmodels/common/TimingConstraint.h>
#include <IPXACTmodels/common/CellSpecification.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/QualifierWriter.h>

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeWire()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeWire(QXmlStreamWriter& writer, QSharedPointer<WireAbstraction> wire,
    Document::Revision revision)
{
    writer.writeStartElement(QStringLiteral("ipxact:wire"));

    QualifierWriter::writeQualifier(writer, wire->getQualifier());

    QString initiatorElementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onInitiator")
        : QStringLiteral("ipxact:onMaster");

    QString targetElementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onTarget")
        : QStringLiteral("ipxact:onSlave");

    Details::writeSystem(writer, wire);
    Details::writeInitiator(writer, wire, initiatorElementName);
    Details::writeTarget(writer, wire, targetElementName);
    
    if (!wire->getDefaultValue().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:defaultValue"), wire->getDefaultValue());
    }
    else if (wire->requiresDriver())
    {
        writer.writeStartElement(QStringLiteral("ipxact:requiresDriver"));
        writer.writeAttribute(QStringLiteral("driverType"), General::driverType2Str(wire->getDriverType()));       
        writer.writeCharacters(QStringLiteral("true"));
        writer.writeEndElement();
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeSystem()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeSystem(QXmlStreamWriter& writer,
    QSharedPointer<WireAbstraction> wire)
{
    for (auto systemPort : *wire->getSystemPorts())
    {
        writer.writeStartElement(QStringLiteral("ipxact:onSystem"));
        writer.writeTextElement(QStringLiteral("ipxact:group"), systemPort->getSystemGroup());
        writeWirePort(writer, systemPort);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeWirePort()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeWirePort(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort)
{
    writePresence(writer, wirePort);

    writeWidth(writer, wirePort);

    writeDirection(writer, wirePort);

    writeModeConstraints(writer, wirePort);

    writeMirroredModeConstraints(writer, wirePort);
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writePresence()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writePresence(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort)
{
    if (wirePort->getPresence() == PresenceTypes::REQUIRED)
    {
        writer.writeTextElement(QStringLiteral("ipxact:presence"), QStringLiteral("required"));
    }
    else if (wirePort->getPresence() == PresenceTypes::OPTIONAL)
    {
        writer.writeTextElement(QStringLiteral("ipxact:presence"), QStringLiteral("optional"));
    }
    else if (wirePort->getPresence() == PresenceTypes::ILLEGAL)
    {
        writer.writeTextElement(QStringLiteral("ipxact:presence"), QStringLiteral("illegal"));
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeWidth()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeWidth(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort)
{
    if (!wirePort->getWidth().isEmpty())
    {

        writer.writeStartElement(QStringLiteral("ipxact:width"));

        if (wirePort->getAllBits())
        {
            writer.writeAttribute(QStringLiteral("allBits"), QStringLiteral("true"));
        }

        writer.writeCharacters(wirePort->getWidth());

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeDirection()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeDirection(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort)
{
    if (wirePort->getDirection() != DirectionTypes::DIRECTION_INVALID)
    {
        writer.writeTextElement(QStringLiteral("ipxact:direction"),
            DirectionTypes::direction2Str(wirePort->getDirection()));
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeModeConstraints()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeModeConstraints(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort)
{
    QSharedPointer<TimingConstraint> timing = wirePort->getTimingConstraint();
    QSharedPointer<CellSpecification> driveConstraint = wirePort->getDriveConstraint();
    QSharedPointer<CellSpecification> loadConstraint = wirePort->getLoadConstraint();

    if (!timing.isNull() || !driveConstraint.isNull() || !loadConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:modeConstraints"));

        writeTimingConstraint(writer, timing);
        writeDriveConstraint(writer, driveConstraint);
        writeLoadConstraint(writer, loadConstraint);

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeTimingConstraint()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeTimingConstraint(QXmlStreamWriter& writer, QSharedPointer<TimingConstraint> timing)
{
    if (!timing.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:timingConstraint"));

        if (timing->getClockEdge() == TimingConstraint::RISE)
        {
            writer.writeAttribute(QStringLiteral("clockEdge"), QStringLiteral("rise"));
        }
        else if (timing->getClockEdge() == TimingConstraint::FALL)
        {
            writer.writeAttribute(QStringLiteral("clockEdge"), QStringLiteral("fall"));
        }

        if (timing->getDelayType() == TimingConstraint::MINIMUM_DELAY)
        {
            writer.writeAttribute(QStringLiteral("delayType"), QStringLiteral("min"));
        }
        else if (timing->getDelayType() == TimingConstraint::MAXIMUM_DELAY)
        {
            writer.writeAttribute(QStringLiteral("delayType"), QStringLiteral("max"));
        }

        writer.writeAttribute(QStringLiteral("clockName"), timing->getClockName());

        writer.writeCharacters(QString::number(timing->getValue()));

        writer.writeEndElement();
    }
}
//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeLoadConstraint()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeLoadConstraint(QXmlStreamWriter& writer,
    QSharedPointer<CellSpecification> loadConstraint)
{
    if (!loadConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:loadConstraint"));
        writeCellSpecification(writer, loadConstraint);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeCellSpecification()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeCellSpecification(QXmlStreamWriter& writer,
    QSharedPointer<CellSpecification> specification)
{
    writer.writeStartElement(QStringLiteral("ipxact:cellSpecification"));

    if (specification->getCellStrength() == CellSpecification::LOW)
    {
        writer.writeAttribute(QStringLiteral("cellStrength"), QStringLiteral("low"));
    }
    else if (specification->getCellStrength() == CellSpecification::HIGH)
    {
        writer.writeAttribute(QStringLiteral("cellStrength"), QStringLiteral("high"));
    }
    else if (specification->getCellStrength() == CellSpecification::MEDIAN)
    {
        writer.writeAttribute(QStringLiteral("cellStrength"), QStringLiteral("median"));
    }

    if (specification->getCellClass() == CellSpecification::SEQUENTIAL)
    {
        writer.writeTextElement(QStringLiteral("ipxact:cellClass"), QStringLiteral("sequential"));
    }
    else if (specification->getCellClass() == CellSpecification::COMBINATORIAL)
    {
        writer.writeTextElement(QStringLiteral("ipxact:cellClass"), QStringLiteral("combinatorial"));
    }

    QString cellFunction = specification->getCellFunction();
    if (!cellFunction.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:cellFunction"));
        
        QStringList standardFunctions;
        standardFunctions << QStringLiteral("nd2") << QStringLiteral("buf") << QStringLiteral("inv") <<
            QStringLiteral("mux21") << QStringLiteral("dff") << QStringLiteral("latch") << QStringLiteral("xor2");
        if (standardFunctions.contains(cellFunction))
        {
            writer.writeCharacters(specification->getCellFunction());
        }
        else
        {
            writer.writeAttribute(QStringLiteral("other"), cellFunction);
            writer.writeCharacters(QStringLiteral("other"));
        }

        writer.writeEndElement();
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeDriveConstraint()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeDriveConstraint(QXmlStreamWriter& writer, QSharedPointer<CellSpecification> driveConstraint)
{
    if (!driveConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:driveConstraint"));
        writeCellSpecification(writer, driveConstraint);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeMirroredModeConstraints()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeMirroredModeConstraints(QXmlStreamWriter& writer, 
    QSharedPointer<WirePort> wirePort)
{
    QSharedPointer<TimingConstraint> timing = wirePort->getMirroredTimingConstraint();
    QSharedPointer<CellSpecification> driveConstraint = wirePort->getMirroredDriveConstraint();
    QSharedPointer<CellSpecification> loadConstraint = wirePort->getMirroredLoadConstraint();

    if (!timing.isNull() || !driveConstraint.isNull() || !loadConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:mirroredModeConstraints"));

        writeTimingConstraint(writer, timing);
        writeDriveConstraint(writer, driveConstraint);
        writeLoadConstraint(writer, loadConstraint);

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeInitiator()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeInitiator(QXmlStreamWriter& writer,
    QSharedPointer<WireAbstraction> wirePort, QString const& elementName)
{
    if (wirePort->hasMasterPort())
    {
        writer.writeStartElement(elementName);
        writeWirePort(writer, wirePort->getMasterPort());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::Details::writeTarget()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::Details::writeTarget(QXmlStreamWriter& writer,
    QSharedPointer<WireAbstraction> wirePort, QString const& elementName)
{
    if (wirePort->hasSlavePort())
    {
        writer.writeStartElement(elementName);
        writeWirePort(writer, wirePort->getSlavePort());
        writer.writeEndElement();
    }
}
