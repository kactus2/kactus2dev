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

#include <IPXACTmodels/common/ClockUnit.h>

//-----------------------------------------------------------------------------
// Function: OtherClockDriverReader::OtherClockDriverReader()
//-----------------------------------------------------------------------------
OtherClockDriverReader::OtherClockDriverReader(QObject* parent /* = 0 */) :
QObject(parent)
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
	QSharedPointer<OtherClockDriver> newOtherClockDriver (new OtherClockDriver());

    parseClockDriverAttributes(otherClockDriverNode, newOtherClockDriver);

    newOtherClockDriver->setClockPeriod(parseClockUnit(otherClockDriverNode, QStringLiteral("ipxact:clockPeriod")));

    newOtherClockDriver->setClockPulseOffset(parseClockUnit(otherClockDriverNode, QStringLiteral("ipxact:clockPulseOffset")));

    parseClockPulseValue(otherClockDriverNode, newOtherClockDriver);

    newOtherClockDriver->setClockPulseDuration(parseClockUnit(otherClockDriverNode, QStringLiteral("ipxact:clockPulseDuration")));

    return newOtherClockDriver;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverReader::parseClockDriverAttributes()
//-----------------------------------------------------------------------------
void OtherClockDriverReader::parseClockDriverAttributes(QDomNode const& otherClockDriverNode,
    QSharedPointer<OtherClockDriver> newOtherClockDriver) const
{
    QString name = otherClockDriverNode.toElement().attribute(QStringLiteral("clockName"));
    newOtherClockDriver->setClockName(name);

    if (otherClockDriverNode.toElement().hasAttribute(QStringLiteral("clockSource")))
    {
        QString clockSource = otherClockDriverNode.toElement().attribute(QStringLiteral("clockSource"));
        newOtherClockDriver->setClockSource(clockSource);
    }
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverReader::parseClockUnit()
//-----------------------------------------------------------------------------
QSharedPointer<ClockUnit> OtherClockDriverReader::parseClockUnit(QDomNode const& otherClockDriverNode,
    QString const& elementName) const
{
    QDomElement childElement = otherClockDriverNode.firstChildElement(elementName);

    QString value = childElement.firstChild().nodeValue();
    QSharedPointer<ClockUnit> newClockUnit (new ClockUnit(value));

    if (childElement.hasAttribute(QStringLiteral("units")))
    {
        QString timeUnit = childElement.attribute(QStringLiteral("units"));
        newClockUnit->setTimeUnit(timeUnit);
    }

    return newClockUnit;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverReader::parseClockDriverAttributes()
//-----------------------------------------------------------------------------
void OtherClockDriverReader::parseClockPulseValue(QDomNode const& otherClockDriverNode,
    QSharedPointer<OtherClockDriver> newOtherClockDriver) const
{
    QString pulseValue = otherClockDriverNode.firstChildElement(QStringLiteral("ipxact:clockPulseValue")).firstChild().nodeValue();
    newOtherClockDriver->setClockPulseValue(pulseValue);
}
