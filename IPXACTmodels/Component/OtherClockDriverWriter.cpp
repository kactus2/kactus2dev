//-----------------------------------------------------------------------------
// File: OtherClockDriverWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.10.2015
//
// Description:
// Writer class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------

#include "OtherClockDriverWriter.h"

//-----------------------------------------------------------------------------
// Function: OtherClockDriverWriter::OtherClockDriverWriter()
//-----------------------------------------------------------------------------
OtherClockDriverWriter::OtherClockDriverWriter(QObject* parent /* = 0 */) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverWriter::~OtherClockDriverWriter()
//-----------------------------------------------------------------------------
OtherClockDriverWriter::~OtherClockDriverWriter()
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverWriter::writeOtherClockDriver()
//-----------------------------------------------------------------------------
void OtherClockDriverWriter::writeOtherClockDriver(QXmlStreamWriter& writer, QSharedPointer<OtherClockDriver> otherClockDriver) const
{
	writer.writeStartElement("ipxact:otherClockDriver");

    writeClockDriverAttributes(writer, otherClockDriver);

    writeClockUnit(writer, otherClockDriver->getClockPeriod(), "ipxact:clockPeriod");

    writeClockUnit(writer, otherClockDriver->getClockPulseOffset(), "ipxact:clockPulseOffset");

    writer.writeTextElement("ipxact:clockPulseValue", otherClockDriver->getClockPulseValue());

    writeClockUnit(writer, otherClockDriver->getClockPulseDuration(), "ipxact:clockPulseDuration");

	writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverWriter::writeClockDriverAttributes()
//-----------------------------------------------------------------------------
void OtherClockDriverWriter::writeClockDriverAttributes(QXmlStreamWriter& writer,
    QSharedPointer<OtherClockDriver> otherClockDriver) const
{
    writer.writeAttribute("clockName", otherClockDriver->getClockName());

    if (!otherClockDriver->getClockSource().isEmpty())
    {
        writer.writeAttribute("clockSource", otherClockDriver->getClockSource());
    }
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverWriter::writeClockUnit()
//-----------------------------------------------------------------------------
void OtherClockDriverWriter::writeClockUnit(QXmlStreamWriter& writer, QSharedPointer<ClockUnit> clockUnit,
    QString const& elementName) const
{
    writer.writeStartElement(elementName);

    if (clockUnit->getTimeUnit() != ClockUnit::TIMEUNIT_UNSPECIFIED)
    {
        writer.writeAttribute("units", clockUnit->timeUnitToString());
    }

    writer.writeCharacters(clockUnit->getValue());

    writer.writeEndElement(); // elementName
}
