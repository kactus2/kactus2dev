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

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include "../XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: OtherClockDriverWriter::OtherClockDriverWriter()
//-----------------------------------------------------------------------------
OtherClockDriverWriter::OtherClockDriverWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
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

	if (!otherClockDriver->getClockName().isEmpty())
	{
		writer.writeAttribute("clockName", otherClockDriver->getClockName());
	}

	// If optional attribute clockSource exists.
	if (!otherClockDriver->getClockSource().isEmpty())
	{
		writer.writeAttribute("clockSource", otherClockDriver->getClockSource());
	}

	if (otherClockDriver->getClockPeriod())
	{
		// Start the ipxact:clockPeriod tag.
		writer.writeStartElement("ipxact:clockPeriod");

		// Write the attributes for the element.
		writer.writeAttribute("units",
			General::timeUnit2Str(otherClockDriver->getClockPeriod()->timeUnit_));
		XmlUtils::writeAttributes(writer, otherClockDriver->getClockPeriod()->attributes_);

		// Write the value of the element and close the tag.
		writer.writeCharacters(QString::number(otherClockDriver->getClockPeriod()->value_));
		writer.writeEndElement();
	}

	if (otherClockDriver->getClockPulseOffset())
	{
		// Start the ipxact:clockPulseOffset tag.
		writer.writeStartElement("ipxact:clockPulseOffset");

		// Write the attributes for the element.
		writer.writeAttribute("units",
			General::timeUnit2Str(otherClockDriver->getClockPulseOffset()->timeUnit_));
		XmlUtils::writeAttributes(writer, otherClockDriver->getClockPulseOffset()->attributes_);

		// Write the value of the element and close the tag.
		writer.writeCharacters(QString::number(otherClockDriver->getClockPulseOffset()->value_));
		writer.writeEndElement();
	}

	if (otherClockDriver->getClockPulseValue())
	{
		// Start the ipxact:clockPulseValue tag.
		writer.writeStartElement("ipxact:clockPulseValue");

		// Write the attributes for the element.
		XmlUtils::writeAttributes(writer, otherClockDriver->getClockPulseValue()->attributes_);

		// Write the value of the element and close the tag.
		writer.writeCharacters(QString::number(otherClockDriver->getClockPulseValue()->value_));
		writer.writeEndElement();
	}

	if (otherClockDriver->getClockPulseDuration())
	{
		// Start the ipxact:clockPulseDuration tag.
		writer.writeStartElement("ipxact:clockPulseDuration");

		// Write the attributes for the element.
		writer.writeAttribute("units",
			General::timeUnit2Str(otherClockDriver->getClockPulseDuration()->timeUnit_));
		XmlUtils::writeAttributes(writer, otherClockDriver->getClockPulseDuration()->attributes_);

		// Write the value of the element and close the tag.
		writer.writeCharacters(QString::number(otherClockDriver->getClockPulseDuration()->value_));
		writer.writeEndElement();
	}

	writer.writeEndElement();
}