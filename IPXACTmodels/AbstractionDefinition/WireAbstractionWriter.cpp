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
    writer.writeStartElement("ipxact:wire");

    writeQualifier(writer, wire);

    writeSystem(writer, wire);
    writerMaster(writer, wire);
    writerSlave(writer, wire);

    if (!wire->getDefaultValue().isEmpty())
    {
        writer.writeTextElement("ipxact:defaultValue", wire->getDefaultValue());
    }
    else if (wire->requiresDriver())
    {
        writer.writeStartElement("ipxact:requiresDriver");
        writer.writeAttribute("driverType", General::driverType2Str(wire->getDriverType()));       
        writer.writeCharacters("true");
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
        writer.writeStartElement("ipxact:qualifier");
        if (qualifier.isData())
        {
            writer.writeTextElement("ipxact:isData", "true");
        }
        if (qualifier.isAddress())
        {
            writer.writeTextElement("ipxact:isAddress", "true");
        }
        if (qualifier.isClock())
        {
            writer.writeTextElement("ipxact:isClock", "true");
        }
        else if (qualifier.isReset())
        {
            writer.writeTextElement("ipxact:isReset", "true");
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
        writer.writeStartElement("ipxact:onSystem");
        writer.writeTextElement("ipxact:group", systemPort->getSystemGroup());
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
        writer.writeTextElement("ipxact:presence", "required");
    }
    else if (wirePort->getPresence() == PresenceTypes::OPTIONAL)
    {
        writer.writeTextElement("ipxact:presence", "optional");
    }
    else if (wirePort->getPresence() == PresenceTypes::ILLEGAL)
    {
        writer.writeTextElement("ipxact:presence", "illegal");
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeWidth()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeWidth(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort) const
{
    if (!wirePort->getWidth().isEmpty())
    {
        writer.writeTextElement("ipxact:width", wirePort->getWidth());
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionWriter::writeDirection()
//-----------------------------------------------------------------------------
void WireAbstractionWriter::writeDirection(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort) const
{
    if (wirePort->getDirection() != DirectionTypes::DIRECTION_INVALID)
    {
        writer.writeTextElement("ipxact:direction", DirectionTypes::direction2Str(wirePort->getDirection()));
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
        writer.writeStartElement("ipxact:modeConstraints");

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
        writer.writeStartElement("ipxact:timingConstraint");

        if (timing->getClockEdge() == TimingConstraint::RISE)
        {
            writer.writeAttribute("clockEdge", "rise");
        }
        else if (timing->getClockEdge() == TimingConstraint::FALL)
        {
            writer.writeAttribute("clockEdge", "fall");
        }

        if (timing->getDelayType() == TimingConstraint::MINIMUM_DELAY)
        {
            writer.writeAttribute("delayType", "min");
        }
        else if (timing->getDelayType() == TimingConstraint::MAXIMUM_DELAY)
        {
            writer.writeAttribute("delayType", "max");
        }

        writer.writeAttribute("clockName", timing->getClockName());

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
        writer.writeStartElement("ipxact:loadConstraint");
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
    writer.writeStartElement("ipxact:cellSpecification");

    if (specification->getCellStrength() == CellSpecification::LOW)
    {
        writer.writeAttribute("cellStrength", "low");
    }
    else if (specification->getCellStrength() == CellSpecification::HIGH)
    {
        writer.writeAttribute("cellStrength", "high");
    }
    else if (specification->getCellStrength() == CellSpecification::MEDIAN)
    {
        writer.writeAttribute("cellStrength", "median");
    }

    if (specification->getCellClass() == CellSpecification::SEQUENTIAL)
    {
        writer.writeTextElement("ipxact:cellClass", "sequential");
    }
    else if (specification->getCellClass() == CellSpecification::COMBINATORIAL)
    {
        writer.writeTextElement("ipxact:cellClass", "combinatorial");
    }

    QString cellFunction = specification->getCellFunction();
    if (!cellFunction.isEmpty())
    {
        writer.writeStartElement("ipxact:cellFunction");
        
        QStringList standardFunctions;
        standardFunctions << "nd2" << "buf" << "inv" << "mux21" << "dff" << "latch" << "xor2";
        if (standardFunctions.contains(cellFunction))
        {
            writer.writeCharacters(specification->getCellFunction());
        }
        else
        {
            writer.writeAttribute("other", cellFunction);
            writer.writeCharacters("other");
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
        writer.writeStartElement("ipxact:driveConstraint");
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
        writer.writeStartElement("ipxact:mirroredModeConstraints");

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
        writer.writeStartElement("ipxact:onMaster");
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
        writer.writeStartElement("ipxact:onSlave");
        writeWirePort(writer, wirePort->getSlavePort());
        writer.writeEndElement();
    }
}
