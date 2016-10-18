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
OtherClockDriverWriter::OtherClockDriverWriter(QObject* parent) :
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
	writer.writeStartElement(QStringLiteral("ipxact:otherClockDriver"));

    writeClockDriverAttributes(writer, otherClockDriver);

    writeClockUnit(writer, otherClockDriver->getClockPeriod(), QStringLiteral("ipxact:clockPeriod"));

    writeClockUnit(writer, otherClockDriver->getClockPulseOffset(), QStringLiteral("ipxact:clockPulseOffset"));

    writer.writeTextElement(QStringLiteral("ipxact:clockPulseValue"), otherClockDriver->getClockPulseValue());

    writeClockUnit(writer, otherClockDriver->getClockPulseDuration(), QStringLiteral("ipxact:clockPulseDuration"));

	writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverWriter::writeClockDriverAttributes()
//-----------------------------------------------------------------------------
void OtherClockDriverWriter::writeClockDriverAttributes(QXmlStreamWriter& writer,
    QSharedPointer<OtherClockDriver> otherClockDriver) const
{
    writer.writeAttribute(QStringLiteral("clockName"), otherClockDriver->getClockName());

    if (!otherClockDriver->getClockSource().isEmpty())
    {
        writer.writeAttribute(QStringLiteral("clockSource"), otherClockDriver->getClockSource());
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
        writer.writeAttribute(QStringLiteral("units"), clockUnit->timeUnitToString());
    }

    writer.writeCharacters(clockUnit->getValue());

    writer.writeEndElement(); // elementName
}
