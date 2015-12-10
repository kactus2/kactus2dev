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

    wire->setDefaultValue(wireNode.firstChildElement("ipxact:defaultValue").firstChild().nodeValue());

    QDomElement driverNode = wireNode.firstChildElement("ipxact:requiresDriver");
    if (!driverNode.isNull())
    {
        bool requiresDriver = driverNode.firstChild().nodeValue() == "true";
        if (requiresDriver)
        {
            wire->setDriverType(General::str2DriverType(driverNode.attribute("driverType"), General::ANY));
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
    QDomNode qualifierNode = wireNode.firstChildElement("ipxact:qualifier");

    if (!qualifierNode.isNull())
    {
        Qualifier readQualifier;
        bool isData = qualifierNode.firstChildElement("ipxact:isData").firstChild().nodeValue() == "true";
        bool isAddress = qualifierNode.firstChildElement("ipxact:isAddress").firstChild().nodeValue() == "true";
        
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

        if (qualifierNode.firstChildElement("ipxact:isClock").firstChild().nodeValue() == "true")
        {
            wire->setQualifier(Qualifier::Clock);
        }
        else if (qualifierNode.firstChildElement("ipxact:isReset").firstChild().nodeValue() == "true")
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
    QDomNodeList systemNodes = wireNode.toElement().elementsByTagName("ipxact:onSystem");

    for (int i = 0; i < systemNodes.count(); i++)
    {
        QDomNode systemNode = systemNodes.at(i);

        QSharedPointer<WirePort> system = parseWirePort(systemNode);
        system->setSystemGroup(systemNode.firstChildElement("ipxact:group").firstChild().nodeValue());
        wire->addSystemPort(system);
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseWirePort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstractionReader::parseWirePort(QDomNode const& wirePortNode) const
{
    QSharedPointer<WirePort> wirePort(new WirePort());

    QDomNode presenceNode = wirePortNode.firstChildElement("ipxact:presence");
    wirePort->setPresence(PresenceTypes::str2Presence(presenceNode.firstChild().nodeValue(),
        PresenceTypes::UNKNOWN));

    wirePort->setWidth(wirePortNode.firstChildElement("ipxact:width").firstChild().nodeValue());

    QDomNode directionNode = wirePortNode.firstChildElement("ipxact:direction");
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
    QDomNode constraintsNode = wirePortNode.firstChildElement("ipxact:modeConstraints");
    if (!constraintsNode.isNull())
    {
        QDomElement timingNode = constraintsNode.firstChildElement("ipxact:timingConstraint");
        if (!timingNode.isNull())
        {
            wirePort->setTimingConstraint(parseTimingConstraint(timingNode));
        }

        QDomNode loadNode = constraintsNode.firstChildElement("ipxact:loadConstraint");
        if (!loadNode.isNull())
        {
            wirePort->setLoadConstraint(parseCellSpecification(loadNode.firstChildElement()));
        }

        QDomNode driveNode = constraintsNode.firstChildElement("ipxact:driveConstraint");
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

    QString clockEdge = timingNode.attribute("clockEdge");
    if (clockEdge == "fall")
    {
        timingConstraint->setClockEdge(TimingConstraint::FALL);
    }
    else if (clockEdge == "rise")
    {
        timingConstraint->setClockEdge(TimingConstraint::RISE);
    }

    QString delay = timingNode.attribute("delayType");
    if (delay == "min")
    {
        timingConstraint->setDelayType(TimingConstraint::MINIMUM_DELAY);
    }
    else if (delay == "max")
    {
        timingConstraint->setDelayType(TimingConstraint::MAXIMUM_DELAY);
    }

    timingConstraint->setClockName(timingNode.attribute("clockName"));

    timingConstraint->setValue(timingNode.firstChild().nodeValue().toDouble());

    return timingConstraint;
}

//-----------------------------------------------------------------------------
// Function: WireAbstractionReader::parseCellSpecification()
//-----------------------------------------------------------------------------
QSharedPointer<CellSpecification> WireAbstractionReader::parseCellSpecification(QDomElement const& cellNode) const
{
    QSharedPointer<CellSpecification> cellSpecification(new CellSpecification());

    QString cellStrength = cellNode.attribute("cellStrength");
    if (cellStrength == "low")
    {
        cellSpecification->setCellStrength(CellSpecification::LOW);
    }
    else if (cellStrength == "median")
    {
        cellSpecification->setCellStrength(CellSpecification::MEDIAN);
    }
    else if (cellStrength == "high")
    {
        cellSpecification->setCellStrength(CellSpecification::HIGH);
    }

    QString cellFunction = cellNode.firstChildElement("ipxact:cellFunction").firstChild().nodeValue();
    if (cellFunction == "other")
    {
        cellSpecification->setCellFunction(cellNode.firstChildElement("ipxact:cellFunction").attribute("other"));
    }
    else
    {
        cellSpecification->setCellFunction(cellFunction);
    }

    QString cellClass = cellNode.firstChildElement("ipxact:cellClass").firstChild().nodeValue();
    if (cellClass == "sequential")
    {
        cellSpecification->setCellClass(CellSpecification::SEQUENTIAL);
    }
    else if (cellClass == "combinatorial")
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
    QDomNode constraintsNode = wirePortNode.firstChildElement("ipxact:mirroredModeConstraints");
    if (!constraintsNode.isNull())
    {
        QDomElement timingNode = constraintsNode.firstChildElement("ipxact:timingConstraint");
        if (!timingNode.isNull())
        {
            wirePort->setMirroredTimingConstraint(parseTimingConstraint(timingNode));
        }

        QDomNode loadNode = constraintsNode.firstChildElement("ipxact:loadConstraint");
        if (!loadNode.isNull())
        {
            wirePort->setMirroredLoadConstraint(parseCellSpecification(loadNode.firstChildElement()));
        }

        QDomNode driveNode = constraintsNode.firstChildElement("ipxact:driveConstraint");
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
    QDomNode masterNode = wireNode.firstChildElement("ipxact:onMaster");

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
    QDomNode slaveNode = wireNode.firstChildElement("ipxact:onSlave");

    if (!slaveNode.isNull())
    {
        QSharedPointer<WirePort> slave = parseWirePort(slaveNode);
        wire->setSlavePort(slave);
    }
}
