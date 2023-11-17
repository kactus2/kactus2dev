//-----------------------------------------------------------------------------
// File: WireAbstractionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.08.2015
//
// Description:
// Reader for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------

#include "WireAbstractionReader.h"

#include <IPXACTmodels/common/CellSpecification.h>
#include <IPXACTmodels/common/QualifierReader.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <IPXACTmodels/common/TimingConstraint.h>
#include "WireAbstraction.h"
#include "WirePort.h"

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::createWireAbstractionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<WireAbstraction> WireAbstractionReader::createWireAbstractionFrom(QDomNode const& wireNode,
    Document::Revision revision)
{
    QSharedPointer<WireAbstraction> wire(new WireAbstraction());

    Details::parseQualifier(wireNode, wire, revision);

    Details::parseSystems(wireNode, wire, revision);

    Details::parseInitiator(wireNode, wire, revision);

    Details::parseTarget(wireNode, wire, revision);

    wire->setDefaultValue(wireNode.firstChildElement(QStringLiteral("ipxact:defaultValue")).firstChild().nodeValue());

    QDomElement driverNode = wireNode.firstChildElement(QStringLiteral("ipxact:requiresDriver"));
    if (!driverNode.isNull())
    {
        bool requiresDriver = driverNode.firstChild().nodeValue() == QStringLiteral("true");
        if (requiresDriver)
        {
            wire->setDriverType(General::str2DriverType(driverNode.attribute(QStringLiteral("driverType")),
                General::ANY));
            wire->setRequiresDriver(true);
        }
    }

    return wire;
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseQualifier()
//-----------------------------------------------------------------------------
void WireAbstractionReader::Details::parseQualifier(QDomNode const& wireNode,
    QSharedPointer<WireAbstraction> wire, Document::Revision revision)
{
    QDomNode qualifierNode = wireNode.firstChildElement(QStringLiteral("ipxact:qualifier"));

    auto qualifier = wire->getQualifier();

    QualifierReader::parseQualifier(qualifierNode, qualifier, revision);
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseSystems()
//-----------------------------------------------------------------------------
void WireAbstractionReader::Details::parseSystems(QDomNode const& wireNode, 
    QSharedPointer<WireAbstraction> wire, Document::Revision revision)
{
    QDomNodeList systemNodes = wireNode.toElement().elementsByTagName(QStringLiteral("ipxact:onSystem"));

    for (int i = 0; i < systemNodes.count(); i++)
    {
        QDomNode systemNode = systemNodes.at(i);

        QSharedPointer<WirePort> system = parseWirePort(systemNode, revision);
        system->setSystemGroup(systemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue());
        wire->addSystemPort(system);
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseWirePort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstractionReader::Details::parseWirePort(QDomNode const& wirePortNode,
    Document::Revision revision)
{
    QSharedPointer<WirePort> wirePort(new WirePort());

    QDomNode presenceNode = wirePortNode.firstChildElement(QStringLiteral("ipxact:presence"));
    wirePort->setPresence(PresenceTypes::str2Presence(presenceNode.firstChild().nodeValue(),
        PresenceTypes::UNKNOWN));

    auto widthElement = wirePortNode.firstChildElement(QStringLiteral("ipxact:width")).firstChild();

    wirePort->setWidth(widthElement.nodeValue());

    if (revision == Document::Revision::Std22)
    {
        wirePort->setAllBits(
            widthElement.attributes().namedItem(QStringLiteral("allBits")).nodeValue() == QStringLiteral("true"));
    }

    QDomNode directionNode = wirePortNode.firstChildElement(QStringLiteral("ipxact:direction"));
    wirePort->setDirection(DirectionTypes::str2Direction(directionNode.firstChild().nodeValue(),
        DirectionTypes::DIRECTION_INVALID));

    parseConstraints(wirePortNode, wirePort);

    parseMirroredConstraints(wirePortNode, wirePort);

    return wirePort;
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseConstraints()
//-----------------------------------------------------------------------------
void WireAbstractionReader::Details::parseConstraints(QDomNode const& wirePortNode, QSharedPointer<WirePort> wirePort)
{
    QDomNode constraintsNode = wirePortNode.firstChildElement(QStringLiteral("ipxact:modeConstraints"));
    if (!constraintsNode.isNull())
    {
        QDomElement timingNode = constraintsNode.firstChildElement(QStringLiteral("ipxact:timingConstraint"));
        if (!timingNode.isNull())
        {
            wirePort->setTimingConstraint(parseTimingConstraint(timingNode));
        }

        QDomNode loadNode = constraintsNode.firstChildElement(QStringLiteral("ipxact:loadConstraint"));
        if (!loadNode.isNull())
        {
            wirePort->setLoadConstraint(parseCellSpecification(loadNode.firstChildElement()));
        }

        QDomNode driveNode = constraintsNode.firstChildElement(QStringLiteral("ipxact:driveConstraint"));
        if (!driveNode.isNull())
        {
            wirePort->setDriveConstraint(parseCellSpecification(driveNode.firstChildElement()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseTimingConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<TimingConstraint> WireAbstractionReader::Details::parseTimingConstraint(QDomElement const& timingNode)
{
    QSharedPointer<TimingConstraint> timingConstraint(new TimingConstraint());

    QString clockEdge = timingNode.attribute(QStringLiteral("clockEdge"));
    if (clockEdge == QLatin1String("fall"))
    {
        timingConstraint->setClockEdge(TimingConstraint::FALL);
    }
    else if (clockEdge == QStringLiteral("rise"))
    {
        timingConstraint->setClockEdge(TimingConstraint::RISE);
    }

    QString delay = timingNode.attribute(QStringLiteral("delayType"));
    if (delay == QLatin1String("min"))
    {
        timingConstraint->setDelayType(TimingConstraint::MINIMUM_DELAY);
    }
    else if (delay == QLatin1String("max"))
    {
        timingConstraint->setDelayType(TimingConstraint::MAXIMUM_DELAY);
    }

    timingConstraint->setClockName(timingNode.attribute(QStringLiteral("clockName")));

    timingConstraint->setValue(timingNode.firstChild().nodeValue().toDouble());

    return timingConstraint;
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseCellSpecification()
//-----------------------------------------------------------------------------
QSharedPointer<CellSpecification> WireAbstractionReader::Details::parseCellSpecification(QDomElement const& cellNode)
{
    QSharedPointer<CellSpecification> cellSpecification(new CellSpecification());

    QString cellStrength = cellNode.attribute(QStringLiteral("cellStrength"));
    if (cellStrength == QLatin1String("low"))
    {
        cellSpecification->setCellStrength(CellSpecification::LOW);
    }
    else if (cellStrength == QLatin1String("median"))
    {
        cellSpecification->setCellStrength(CellSpecification::MEDIAN);
    }
    else if (cellStrength == QLatin1String("high"))
    {
        cellSpecification->setCellStrength(CellSpecification::HIGH);
    }

    QString cellFunction = cellNode.firstChildElement(QStringLiteral("ipxact:cellFunction")).firstChild().nodeValue();
    if (cellFunction == QLatin1String("other"))
    {
        cellSpecification->setCellFunction(
            cellNode.firstChildElement(QStringLiteral("ipxact:cellFunction")).attribute(QStringLiteral("other")));
    }
    else
    {
        cellSpecification->setCellFunction(cellFunction);
    }

    QString cellClass = cellNode.firstChildElement(QStringLiteral("ipxact:cellClass")).firstChild().nodeValue();
    if (cellClass == QLatin1String("sequential"))
    {
        cellSpecification->setCellClass(CellSpecification::SEQUENTIAL);
    }
    else if (cellClass == QLatin1String("combinatorial"))
    {
        cellSpecification->setCellClass(CellSpecification::COMBINATORIAL);
    }

    return cellSpecification;
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseMirroredConstraints()
//-----------------------------------------------------------------------------
void WireAbstractionReader::Details::parseMirroredConstraints(QDomNode const& wirePortNode,
    QSharedPointer<WirePort> wirePort)
{
    QDomNode constraintsNode = wirePortNode.firstChildElement(QStringLiteral("ipxact:mirroredModeConstraints"));
    if (!constraintsNode.isNull())
    {
        QDomElement timingNode = constraintsNode.firstChildElement(QStringLiteral("ipxact:timingConstraint"));
        if (!timingNode.isNull())
        {
            wirePort->setMirroredTimingConstraint(parseTimingConstraint(timingNode));
        }

        QDomNode loadNode = constraintsNode.firstChildElement(QStringLiteral("ipxact:loadConstraint"));
        if (!loadNode.isNull())
        {
            wirePort->setMirroredLoadConstraint(parseCellSpecification(loadNode.firstChildElement()));
        }

        QDomNode driveNode = constraintsNode.firstChildElement(QStringLiteral("ipxact:driveConstraint"));
        if (!driveNode.isNull())
        {
            wirePort->setMirroredDriveConstraint(parseCellSpecification(driveNode.firstChildElement()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseInitiator()
//-----------------------------------------------------------------------------
void WireAbstractionReader::Details::parseInitiator(QDomNode const& wireNode, 
    QSharedPointer<WireAbstraction> wire, Document::Revision revision)
{
    QString elementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onInitiator")
        : QStringLiteral("ipxact:onMaster");

    QDomNode initiatorNode = wireNode.firstChildElement(elementName);
    if (!initiatorNode.isNull())
    {
        QSharedPointer<WirePort> initiator = parseWirePort(initiatorNode, revision);
        wire->setInitiatorPort(initiator);
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseTarget()
//-----------------------------------------------------------------------------
void WireAbstractionReader::Details::parseTarget(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire,
    Document::Revision revision)
{

    QString elementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onTarget")
        : QStringLiteral("ipxact:onSlave");

    QDomNode targetNode = wireNode.firstChildElement(elementName);
    if (!targetNode.isNull())
    {
        QSharedPointer<WirePort> target = parseWirePort(targetNode, revision);
        wire->setTargetPort(target);
    }
}
