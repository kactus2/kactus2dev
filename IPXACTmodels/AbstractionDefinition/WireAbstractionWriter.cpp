//-----------------------------------------------------------------------------
// File: WireAbstractionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Writer class for IP-XACT Wire element within abstraction definition.
//-----------------------------------------------------------------------------

#include "WireAbstractionWriter.h"

#include <IPXACTmodels/common/TimingConstraint.h>
#include "WireAbstraction.h"
#include "WirePort.h"

#include <IPXACTmodels/common/CellSpecification.h>

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::WireAbstractionWriter()
//-----------------------------------------------------------------------------
WireAbstractionWriter::WireAbstractionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::~WireAbstractionWriter()
//-----------------------------------------------------------------------------
WireAbstractionWriter::~WireAbstractionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeWire()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeWire(QXmlStreamWriter& writer, QSharedPointer<WireAbstraction> wire) const
{
    writer.writeStartElement(QStringLiteral("ipxact:wire"));

    writeQualifier(writer, wire);

    writeSystem(writer, wire);
    writerMaster(writer, wire);
    writerSlave(writer, wire);

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
// Function: WireAbstractionWriter::writeQualifier()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeQualifier(QXmlStreamWriter& writer, 
    QSharedPointer<WireAbstraction> wire) const
{
    Qualifier qualifier = wire->getQualifier();
    if (qualifier.isSet())
    {
        writer.writeStartElement(QStringLiteral("ipxact:qualifier"));
        if (qualifier.isData())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isData"), QStringLiteral("true"));
        }
        if (qualifier.isAddress())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isAddress"), QStringLiteral("true"));
        }
        if (qualifier.isClock())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isClock"), QStringLiteral("true"));
        }
        else if (qualifier.isReset())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isReset"), QStringLiteral("true"));
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeSystem()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeSystem(QXmlStreamWriter& writer,
    QSharedPointer<WireAbstraction> wire) const
{
    foreach (QSharedPointer<WirePort> systemPort, *wire->getSystemPorts())
    {
        writer.writeStartElement(QStringLiteral("ipxact:onSystem"));
        writer.writeTextElement(QStringLiteral("ipxact:group"), systemPort->getSystemGroup());
        writeWirePort(writer, systemPort);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeWirePort()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeWirePort(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort) const
{
    writePresence(writer, wirePort);

    writeWidth(writer, wirePort);

    writeDirection(writer, wirePort);

    writeModeConstraints(writer, wirePort);

    writeMirroredModeConstraints(writer, wirePort);
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writePresence()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writePresence(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort) const
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
// Function: WireAbstractionWriter::writeWidth()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeWidth(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort) const
{
    if (!wirePort->getWidth().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:width"), wirePort->getWidth());
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeDirection()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeDirection(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort) const
{
    if (wirePort->getDirection() != DirectionTypes::DIRECTION_INVALID)
    {
        writer.writeTextElement(QStringLiteral("ipxact:direction"),
            DirectionTypes::direction2Str(wirePort->getDirection()));
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeModeConstraints()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeModeConstraints(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort) const
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
// Function: WireAbstractionWriter::writeTimingConstraint()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeTimingConstraint(QXmlStreamWriter& writer, QSharedPointer<TimingConstraint> timing) const
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
// Function: WireAbstractionWriter::writeLoadConstraint()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeLoadConstraint(QXmlStreamWriter& writer,
    QSharedPointer<CellSpecification> loadConstraint) const
{
    if (!loadConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:loadConstraint"));
        writeCellSpecification(writer, loadConstraint);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeCellSpecification()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeCellSpecification(QXmlStreamWriter& writer,
    QSharedPointer<CellSpecification> specification) const
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
// Function: WireAbstractionWriter::writeDriveConstraint()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeDriveConstraint(QXmlStreamWriter& writer, QSharedPointer<CellSpecification> driveConstraint) const
{
    if (!driveConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:driveConstraint"));
        writeCellSpecification(writer, driveConstraint);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeMirroredModeConstraints()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeMirroredModeConstraints(QXmlStreamWriter& writer, 
    QSharedPointer<WirePort> wirePort) const
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
// Function: WireAbstractionWriter::writerMaster()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writerMaster(QXmlStreamWriter& writer,
    QSharedPointer<WireAbstraction> wirePort) const
{
    if (wirePort->hasMasterPort())
    {
        writer.writeStartElement(QStringLiteral("ipxact:onMaster"));
        writeWirePort(writer, wirePort->getMasterPort());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writerSlave()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writerSlave(QXmlStreamWriter& writer,
    QSharedPointer<WireAbstraction> wirePort) const
{
    if (wirePort->hasSlavePort())
    {
        writer.writeStartElement(QStringLiteral("ipxact:onSlave"));
        writeWirePort(writer, wirePort->getSlavePort());
        writer.writeEndElement();
    }
}
