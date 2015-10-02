//-----------------------------------------------------------------------------
// File: OtherClockDriverReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.10.2015
//
// Description:
// Reader class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------

#include "OtherClockDriverReader.h"
#include <IPXACTmodels/common/NameGroupReader.h>
#include "../XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: OtherClockDriverReader::OtherClockDriverReader()
//-----------------------------------------------------------------------------
OtherClockDriverReader::OtherClockDriverReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverReader::~OtherClockDriverReader()
//-----------------------------------------------------------------------------
OtherClockDriverReader::~OtherClockDriverReader()
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<OtherClockDriver> OtherClockDriverReader::createOtherClockDriverFrom
	(QDomNode const& otherClockDriverNode) const
{
	// Create the new CPU.
	QSharedPointer<OtherClockDriver> newOtherClockDriver (new OtherClockDriver());

	QDomNamedNodeMap attributeMap = otherClockDriverNode.attributes();

	QString clockName = attributeMap.namedItem(QString("clockName")).nodeValue();
	clockName = XmlUtils::removeWhiteSpace(clockName);
	QString clockSource = attributeMap.namedItem(QString("clockSource")).nodeValue();

	newOtherClockDriver->setClockName(clockName);
	newOtherClockDriver->setClockSource(clockSource);

	for (int i = 0; i < otherClockDriverNode.childNodes().count(); ++i)
	{
		QDomNode tempNode = otherClockDriverNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("ipxact:clockPeriod"))
		{
			General::ClockStruct* newClockStruct = readClockStruct(tempNode);

			newOtherClockDriver->setClockPeriod(newClockStruct);
		}

		else if (tempNode.nodeName() == QString("ipxact:clockPulseOffset"))
		{
			General::ClockStruct* newClockStruct = readClockStruct(tempNode);

			newOtherClockDriver->setClockPulseOffset(newClockStruct);
		}

		else if (tempNode.nodeName() == QString("ipxact:clockPulseValue"))
		{
			General::ClockPulseValue* newClockPulseValue = new General::ClockPulseValue();

			QString value = tempNode.childNodes().at(0).nodeValue();
			bool ok = false;
			newClockPulseValue->value_ = value.toInt(&ok);
			newOtherClockDriver->setClockPulseValue(newClockPulseValue);
		}

		else if (tempNode.nodeName() == QString("ipxact:clockPulseDuration"))
		{
			General::ClockStruct* newClockStruct = readClockStruct(tempNode);

			newOtherClockDriver->setClockPulseDuration(newClockStruct);
		}
	}

    return newOtherClockDriver;
}

General::ClockStruct* OtherClockDriverReader::readClockStruct(QDomNode &clockNode) const
{
	General::ClockStruct* newClockStruct = new General::ClockStruct();

	QString value = clockNode.childNodes().at(0).nodeValue();
	value = XmlUtils::removeWhiteSpace(value);
	bool ok = false;
	newClockStruct->value_ = value.toDouble(&ok);

	// Get the attributes.
	QDomNamedNodeMap attributeMap = clockNode.attributes();

	for (int i = 0; i < attributeMap.size(); ++i)
	{
		QDomNode tempNode = attributeMap.item(i);

		// Get attribute value
		if (tempNode.nodeName() == QString("units"))
		{
			newClockStruct->timeUnit_ = General::str2TimeUnit(tempNode.childNodes().at(i).
				nodeValue(),General::NS);
		}

		// Other attributes go to QMap.
		else
		{
			QString name = tempNode.nodeName();
			QString value = tempNode.nodeValue();
			newClockStruct->attributes_[name] = value;
		}
	}

	return newClockStruct;
}
