//-----------------------------------------------------------------------------
// File: WireAbstractionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.08.2015
//
// Description:
// Reader class for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------

#include "WireAbstractionReader.h"

#include <IPXACTmodels/common/CellSpecification.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/TimingConstraint.h>
#include "WireAbstraction.h"
#include "WirePort.h"

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::WireAbstractionReader()
//-----------------------------------------------------------------------------
WireAbstractionReader::WireAbstractionReader(QObject* parent) : QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::~WireAbstractionReader()
//-----------------------------------------------------------------------------
WireAbstractionReader::~WireAbstractionReader()
{

}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::createWireAbstractionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<WireAbstraction> WireAbstractionReader::createWireAbstractionFrom(QDomNode const& wireNode) const
{
    QSharedPointer<WireAbstraction> wire(new WireAbstraction());

    parseQualifier(wireNode, wire);

    parseSystems(wireNode, wire);

    parseMaster(wireNode, wire);

    parseSlave(wireNode, wire);

    wire->setDefaultValue(wireNode.firstChildElement(QStringLiteral("ipxact:defaultValue")).firstChild().nodeValue());

    QDomElement driverNode = wireNode.firstChildElement(QStringLiteral("ipxact:requiresDriver"));
    if (!driverNode.isNull())
    {
        bool requiresDriver = driverNode.firstChild().nodeValue() == QStringLiteral("true");
        if (requiresDriver)
        {
            wire->setDriverType(General::str2DriverType(driverNode.attribute(QStringLiteral("driverType")),
                General::ANY));
        }
    }

    return wire;
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseQualifier()
//-----------------------------------------------------------------------------
void WireAbstractionReader::parseQualifier(QDomNode const& wireNode, 
    QSharedPointer<WireAbstraction> wire) const
{
    QDomNode qualifierNode = wireNode.firstChildElement(QStringLiteral("ipxact:qualifier"));

    if (!qualifierNode.isNull())
    {
        Qualifier readQualifier;
        bool isData = qualifierNode.firstChildElement(QStringLiteral("ipxact:isData")).firstChild().nodeValue() ==
            QStringLiteral("true");
        bool isAddress = qualifierNode.firstChildElement(
            QStringLiteral("ipxact:isAddress")).firstChild().nodeValue() == QStringLiteral("true");
        
        if (isData && isAddress)
        {
             wire->setQualifier(Qualifier::Data_Address);
        }
        else if (isData)
        {
            wire->setQualifier(Qualifier::Data);
        }
        else if (isAddress)
        {
            wire->setQualifier(Qualifier::Address);
        }

        if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isClock")).firstChild().nodeValue() ==
            QStringLiteral("true"))
        {
            wire->setQualifier(Qualifier::Clock);
        }
        else if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isReset")).firstChild().nodeValue() == 
            QStringLiteral("true"))
        {
            wire->setQualifier(Qualifier::Reset);
        }          
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseSystems()
//-----------------------------------------------------------------------------
void WireAbstractionReader::parseSystems(QDomNode const& wireNode, 
    QSharedPointer<WireAbstraction> wire) const
{
    QDomNodeList systemNodes = wireNode.toElement().elementsByTagName(QStringLiteral("ipxact:onSystem"));

    for (int i = 0; i < systemNodes.count(); i++)
    {
        QDomNode systemNode = systemNodes.at(i);

        QSharedPointer<WirePort> system = parseWirePort(systemNode);
        system->setSystemGroup(systemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue());
        wire->addSystemPort(system);
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseWirePort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstractionReader::parseWirePort(QDomNode const& wirePortNode) const
{
    QSharedPointer<WirePort> wirePort(new WirePort());

    QDomNode presenceNode = wirePortNode.firstChildElement(QStringLiteral("ipxact:presence"));
    wirePort->setPresence(PresenceTypes::str2Presence(presenceNode.firstChild().nodeValue(),
        PresenceTypes::UNKNOWN));

    wirePort->setWidth(wirePortNode.firstChildElement(QStringLiteral("ipxact:width")).firstChild().nodeValue());

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
void WireAbstractionReader::parseConstraints(QDomNode const& wirePortNode, QSharedPointer<WirePort> wirePort) const
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
QSharedPointer<TimingConstraint> WireAbstractionReader::parseTimingConstraint(QDomElement const& timingNode) const
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
QSharedPointer<CellSpecification> WireAbstractionReader::parseCellSpecification(QDomElement const& cellNode) const
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
void WireAbstractionReader::parseMirroredConstraints(QDomNode const& wirePortNode,
    QSharedPointer<WirePort> wirePort) const
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
// Function: WireAbstractionReader::parseMaster()
//-----------------------------------------------------------------------------
void WireAbstractionReader::parseMaster(QDomNode const& wireNode, 
    QSharedPointer<WireAbstraction> wire) const
{
    QDomNode masterNode = wireNode.firstChildElement(QStringLiteral("ipxact:onMaster"));

    if (!masterNode.isNull())
    {
        QSharedPointer<WirePort> master = parseWirePort(masterNode);
        wire->setMasterPort(master);
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseSlave()
//-----------------------------------------------------------------------------
void WireAbstractionReader::parseSlave(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire) const
{
    QDomNode slaveNode = wireNode.firstChildElement(QStringLiteral("ipxact:onSlave"));

    if (!slaveNode.isNull())
    {
        QSharedPointer<WirePort> slave = parseWirePort(slaveNode);
        wire->setSlavePort(slave);
    }
}
