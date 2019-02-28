//-----------------------------------------------------------------------------
// File: PortMapValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.12.2015
//
// Description:
// Validator for the ipxact:portMap.
//-----------------------------------------------------------------------------

#include "PortMapValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/utilities/Search.h>

//-----------------------------------------------------------------------------
// Function: PortMapValidator::PortMapValidator()
//-----------------------------------------------------------------------------
PortMapValidator::PortMapValidator(QSharedPointer<ExpressionParser> parser,
    QSharedPointer<QList<QSharedPointer<Port> > > ports, LibraryInterface* libraryHandler) :
    expressionParser_(parser),
    availablePorts_(ports),
    abstractionDefinition_(),
    interfaceMode_(General::INTERFACE_MODE_COUNT),
    systemGroup_(),
    libraryHandler_(libraryHandler)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::busInterfaceChanged()
//-----------------------------------------------------------------------------
void PortMapValidator::busInterfaceChanged(QSharedPointer<ConfigurableVLNVReference> newAbstractionReference,
    General::InterfaceMode newInterfaceMode, QString const& newSystemGroup)
{
    if (newAbstractionReference)
    {
        abstractionDefinition_ = libraryHandler_->getModelReadOnly<AbstractionDefinition>(*newAbstractionReference);
    }
    else
    {
        abstractionDefinition_.clear();
    }

    interfaceMode_ = newInterfaceMode;
    systemGroup_ = newSystemGroup;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::busInterfaceChanged()
//-----------------------------------------------------------------------------
void PortMapValidator::abstractionDefinitionChanged(QSharedPointer<AbstractionDefinition> newDefinition,
    General::InterfaceMode newInterfaceMode)
{
    if (newDefinition)
    {
        abstractionDefinition_ = newDefinition;
    }

    interfaceMode_ = newInterfaceMode;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::componentChanged()
//-----------------------------------------------------------------------------
void PortMapValidator::componentChanged(QSharedPointer<QList<QSharedPointer<Port> > > newPorts)
{
    availablePorts_ = newPorts;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::validate()
//-----------------------------------------------------------------------------
bool PortMapValidator::validate(QSharedPointer<PortMap> const& portMap) const
{    
    QSharedPointer<PortMap::LogicalPort> logical = portMap->getLogicalPort();   
    if (logical.isNull())
    {
        return false;
    }

    QSharedPointer<PortAbstraction> logicalPort = findLogicalPort(logical->name_);
    if (logicalPort.isNull() || logical->name_.isEmpty())
    {
        return false;
    }
    
    QSharedPointer<PortMap::PhysicalPort> physical = portMap->getPhysicalPort();
    if (physical.isNull() && hasValidTieOff(portMap) == false)
    {
        return false;
    }
    
    QSharedPointer<Port> physicalPort(0);
    if (physical.isNull() == false)
    {
        physicalPort = findPhysicalPort(physical->name_);
        if (physicalPort.isNull())
        {
            return false;
        }
    }

    if (hasValidIsPresent(portMap) == false)
    {
        return false;
    }

    if (physical)
    {
        return connectedPortsHaveValidDirections(logicalPort, physicalPort) &&
            connectedPortsHaveValidPortTypes(logicalPort, physicalPort) &&
            connectedPortsHaveValidBoundaries(logicalPort, logical, physicalPort, physical);
    }
   
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidIsPresent(QSharedPointer<PortMap> const& portMap) const
{
    if (!portMap->getIsPresent().isEmpty())
    {
        QString const solvedValue = expressionParser_->parseExpression(portMap->getIsPresent());

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);
        
        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidLogicalPort()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidLogicalPort(QSharedPointer<PortMap> const& portMap) const
{
    QSharedPointer<PortMap::LogicalPort> logical = portMap->getLogicalPort();
    if (logical.isNull())
    {
        return false;
    }

    QSharedPointer<PortAbstraction> referencedPort = findLogicalPort(logical->name_);
    if (referencedPort.isNull() || logical->name_.isEmpty())
    {
        return false;
    }

    return logicalPortHasValidRange(logical, referencedPort);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidPhysicalPort(QSharedPointer<PortMap> const& portMap,
	QSharedPointer<Port>  const& physicalPort) const
{
    QSharedPointer<PortMap::PhysicalPort> physical = portMap->getPhysicalPort();

    if (physical.isNull() || physical->name_.isEmpty() || physicalPort.isNull() ||
        portMap->getLogicalTieOff().isEmpty() == false)
    {
        return false;
    }

    return physicalPortRangeIsWithinReferencedPort(physicalPort, physical);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidTieOff()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidTieOff(QSharedPointer<PortMap> const& portMap) const
{
    if (portMap->getPhysicalPort() || portMap->getLogicalTieOff().isEmpty())
    {
        return false;
    }

    bool changeOk = true;
    int logicalTieOff = expressionParser_->parseExpression(portMap->getLogicalTieOff()).toInt(&changeOk);

    return changeOk && logicalTieOff >= 0;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveValidDirections()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveValidDirections(QSharedPointer<PortAbstraction> const& logicalPort,
    QSharedPointer<Port> const& physicalPort) const
{
    if (logicalPort->getWire() && physicalPort->getWire())
    {
        if (physicalPort->getWire()->getAllLogicalDirectionsAllowed())
        {
            return true;
        }
        
        DirectionTypes::Direction logicalDirection = logicalPort->getWire()->getDirection(interfaceMode_,
            systemGroup_);
        DirectionTypes::Direction physicalDirection = physicalPort->getWire()->getDirection();

        if ((logicalDirection == DirectionTypes::IN && physicalDirection == DirectionTypes::OUT) ||
            (logicalDirection == DirectionTypes::OUT && physicalDirection == DirectionTypes::IN) ||
            (logicalDirection == DirectionTypes::INOUT &&
            (physicalDirection == DirectionTypes::IN || physicalDirection == DirectionTypes::OUT)))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveValidPortTypes()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveValidPortTypes(QSharedPointer<PortAbstraction> const& logicalPort,
    QSharedPointer<Port> const& physicalPort) const
{
    return (logicalPort->getWire() && physicalPort->getWire()) ||
        (logicalPort->getTransactional() && physicalPort->getTransactional());
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveSameRange(QSharedPointer<PortMap> const& portMap) const
{
    if (portMap->getLogicalPort() && portMap->getPhysicalPort() &&
        portMap->getLogicalPort()->range_ && portMap->getPhysicalPort()->partSelect_)
    {
        int logicalLeft =
            expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
        int logicalRight =
            expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();

        int physicalLeft = expressionParser_->parseExpression(
            portMap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
        int physicalRight = expressionParser_->parseExpression(
            portMap->getPhysicalPort()->partSelect_->getRightRange()).toInt();

        int logicalSize = abs(logicalLeft - logicalRight) + 1;
        int physicalSize = abs(physicalLeft - physicalRight) + 1;

        return logicalSize == physicalSize;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<PortMap> portMap,
    QString const& context) const
{
    QSharedPointer<PortAbstraction> logicalPort;
    QSharedPointer<PortMap::LogicalPort> logical = portMap->getLogicalPort();
    if (logical)
    {
        logicalPort = findLogicalPort(logical->name_);
    }

    QSharedPointer<Port> physicalPort;
    QSharedPointer<PortMap::PhysicalPort> physical = portMap->getPhysicalPort();
    if (physical)
    {
        physicalPort = findPhysicalPort(physical->name_);
    }

    findErrorsInIsPresent(errors, portMap, context);
    findErrorsInLogicalPort(errors, logical, context);
    findErrorsInPhysicalPort(errors, portMap, physicalPort, context);
	findErrorsInPortConnection(errors, portMap, logicalPort, physicalPort, context);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveValidBoundaries()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveValidBoundaries(QSharedPointer<PortAbstraction> logicalPort,
    QSharedPointer<PortMap::LogicalPort> logical, QSharedPointer<Port> physicalPort,
    QSharedPointer<PortMap::PhysicalPort> physical) const
{
    quint64 physicalLeft = 0;
    quint64 physicalRight = 0;

    if (physical->partSelect_ && (!physical->partSelect_->getLeftRange().isEmpty() ||
        !physical->partSelect_->getRightRange().isEmpty()))
    {
        bool leftValid = false;
        std::tie(leftValid, physicalLeft) = checkAndParseExpression(physical->partSelect_->getLeftRange());

        if (leftValid == false)
        {
            return false;
        }

        bool rightValid = false;
        std::tie(rightValid, physicalRight) = checkAndParseExpression(physical->partSelect_->getRightRange());

        if (rightValid == false)
        {
            return false;
        }

        quint64 portLeft = expressionParser_->parseExpression(physicalPort->getLeftBound()).toULongLong();
        quint64 portRight = expressionParser_->parseExpression(physicalPort->getRightBound()).toULongLong();

        if (portLeft > portRight)
        {
            quint64 temporary = portLeft;
            portLeft = portRight;
            portRight = temporary;
        }

        if (physicalLeft < portLeft || physicalLeft > portRight ||
            physicalRight < portLeft || physicalRight > portRight)
        {
            return false;
        }
    }

    quint64 logicalLeft = 0;
    quint64 logicalRight = 0;

    if (logical->range_ && (!logical->range_->getLeft().isEmpty() || !logical->range_->getRight().isEmpty()))
    {
        bool leftValid = false;
        std::tie(leftValid, logicalLeft) = checkAndParseExpression(logical->range_->getLeft());

        if (leftValid == false)
        {
            return false;
        }

        bool rightValid = false;
        std::tie(rightValid, logicalRight) = checkAndParseExpression(logical->range_->getRight());

        if (rightValid == false)
        {
            return false;
        }

        if (logicalPort->getWire())
        {
            QString const width = logicalPort->getWire()->getWidth(interfaceMode_, systemGroup_);
            if (width.isEmpty() == false)
            {
                quint64 abstractionWidth = expressionParser_->parseExpression(width).toULongLong();

                if (logicalLeft > abstractionWidth - 1 || logicalRight > abstractionWidth - 1)
                {
                    return false;
                }
            }
        }
    }

    int logicalSize = logicalLeft - logicalRight + 1;
    int physicalSize = physicalLeft - physicalRight + 1;

    return logicalSize == physicalSize;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::logicalPortHasValidRange()
//-----------------------------------------------------------------------------
bool PortMapValidator::logicalPortHasValidRange(QSharedPointer<PortMap::LogicalPort> const& logicalPort,
    QSharedPointer<PortAbstraction> const& referencedPort) const
{
    if (logicalPort->range_ &&
        (!logicalPort->range_->getLeft().isEmpty() || !logicalPort->range_->getRight().isEmpty()))
    {
        bool leftValid = false;
        quint64 logicalLeft = 0;
        std::tie(leftValid, logicalLeft) = checkAndParseExpression(logicalPort->range_->getLeft());

        if (leftValid == false)
        {
            return false;
        }

        bool rightValid = false;
        quint64 logicalRight = 0;
        std::tie(rightValid, logicalRight) = checkAndParseExpression(logicalPort->range_->getRight());

        if (rightValid == false)
        {
            return false;
        }

        if (referencedPort->getWire())
        {
            QString const width = referencedPort->getWire()->getWidth(interfaceMode_, systemGroup_);
            if (width.isEmpty() == false)
            {
                quint64 abstractionWidth = expressionParser_->parseExpression(width).toULongLong();

                if (logicalLeft > abstractionWidth - 1 || logicalRight > abstractionWidth - 1)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::physicalPortHasValidPartSelect()
//-----------------------------------------------------------------------------
bool PortMapValidator::physicalPortHasValidPartSelect(QSharedPointer<PortMap::PhysicalPort> const& physicalPort) const
{
    if (physicalPort->partSelect_ && (!physicalPort->partSelect_->getLeftRange().isEmpty() ||
        !physicalPort->partSelect_->getRightRange().isEmpty()))
    {
        auto leftPart = checkAndParseExpression(physicalPort->partSelect_->getLeftRange());
        if (leftPart.first == false)
        {
            return false;
        }

        auto rightPart = checkAndParseExpression(physicalPort->partSelect_->getRightRange());
        if (rightPart.first == false)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::physicalPortRangeIsWithinReferencedPort()
//-----------------------------------------------------------------------------
bool PortMapValidator::physicalPortRangeIsWithinReferencedPort(QSharedPointer<Port> const& referencedPort,
    QSharedPointer<PortMap::PhysicalPort> const& physicalPort) const
{
    if (physicalPort->partSelect_ && (!physicalPort->partSelect_->getLeftRange().isEmpty() ||
        !physicalPort->partSelect_->getRightRange().isEmpty()))
    {
        bool leftValid = false;
        quint64 physicalLeft = 0;
        std::tie(leftValid, physicalLeft) = checkAndParseExpression(physicalPort->partSelect_->getLeftRange());

        if (leftValid == false)
        {
            return false;
        }

        bool rightValid = false;
        quint64 physicalright = 0;
        std::tie(rightValid, physicalright) = checkAndParseExpression(physicalPort->partSelect_->getRightRange());

        if (rightValid == false)
        {
            return false;
        }

        quint64 portLeft = expressionParser_->parseExpression(referencedPort->getLeftBound()).toULongLong();
        quint64 portRight = expressionParser_->parseExpression(referencedPort->getRightBound()).toULongLong();

        if (portLeft > portRight)
        {
            quint64 temporary = portLeft;
            portLeft = portRight;
            portRight = temporary;
        }

        return physicalLeft >= portLeft && physicalLeft <= portRight &&
            physicalright >= portLeft && physicalright <= portRight;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<PortMap> portMap,
    QString const& context) const
{
    if (!hasValidIsPresent(portMap))
    {
        errors.append(QObject::tr("Invalid isPresent set for port map within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsInLogicalPort()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsInLogicalPort(QVector<QString>& errors,
    QSharedPointer<PortMap::LogicalPort> logicalPort, QString const& context) const
{
    if (logicalPort && !logicalPort->name_.isEmpty())
    {
        QSharedPointer<PortAbstraction> referencedPort = findLogicalPort(logicalPort->name_);

        if (!referencedPort)
        {
            errors.append(QObject::tr("Could not locate logical port %1 mapped within %2")
                .arg(logicalPort->name_).arg(context));
        }

        if (!logicalPortHasValidRange(logicalPort, referencedPort))
        {
            errors.append(QObject::tr("Invalid range given for logical port %1 mapped within %2")
                .arg(logicalPort->name_).arg(context));
        }
    }
    else
    {
        errors.append(QObject::tr("Port map does not contain a logical port within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsInPhysicalPort()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsInPhysicalPort(QVector<QString>& errors, QSharedPointer<PortMap> portMap,
	QSharedPointer<Port> physicalPort,
    QString const& context) const
{
    if (portMap->getPhysicalPort() && !portMap->getPhysicalPort()->name_.isEmpty() &&
        portMap->getLogicalTieOff().isEmpty())
    {
        if (!physicalPort)
        {
            errors.append(QObject::tr("Could not locate physical port %1 mapped within %2")
                .arg(portMap->getPhysicalPort()->name_).arg(context));
        }

        if (!physicalPortHasValidPartSelect(portMap->getPhysicalPort()))
        {
            errors.append(QObject::tr("Invalid part select given for physical port %1 mapped within %2")
                .arg(portMap->getPhysicalPort()->name_).arg(context));
        }

        if (!physicalPortRangeIsWithinReferencedPort(physicalPort, portMap->getPhysicalPort()))
        {
            errors.append(QObject::tr("Physical port ranges are not within the referenced port %1 within %2")
                .arg(physicalPort->name()).arg(context));
        }
    }
    else if (!portMap->getLogicalTieOff().isEmpty() && !portMap->getPhysicalPort())
    {
        bool changeOk = true;
        int logicalTieOff = expressionParser_->parseExpression(portMap->getLogicalTieOff()).toInt(&changeOk);

        if (!changeOk || logicalTieOff <= 0)
        {
            errors.append(QObject::tr("Invalid port map logical tie off set within %1").arg(context));
        }
    }
    else
    {
        errors.append(QObject::tr("Port map does not contain a physical port or a logical tie off within %1")
            .arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsInPortConnection()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsInPortConnection(QVector<QString>& errors, QSharedPointer<PortMap> portMap,
	QSharedPointer<PortAbstraction> logicalPort,
	QSharedPointer<Port> physicalPort,
    QString const& context) const
{
    if (logicalPort && physicalPort)
    {
		if (!connectedPortsHaveValidDirections(logicalPort, physicalPort))
        {
            QString logicalDirection =
                DirectionTypes::direction2Str(logicalPort->getWire()->getDirection(interfaceMode_, systemGroup_));
            QString physicalDirection = DirectionTypes::direction2Str(physicalPort->getWire()->getDirection());

            errors.append(QObject::tr("Invalid connection made between logical port %1 with direction %2 and "
                "physical port %3 with direction %4 within %5")
                .arg(logicalPort->name()).arg(logicalDirection).arg(physicalPort->name()).arg(physicalDirection)
                .arg(context));
        }

		if (!connectedPortsHaveValidPortTypes(logicalPort, physicalPort))
        {
            errors.append(QObject::tr("Connected logical port %1 and physical port %3 do not have the same port "
                "type within %5").arg(logicalPort->name(), physicalPort->name(), context));
        }

        if (!connectedPortsHaveSameRange(portMap))
        {
            errors.append(QObject::tr("Connected logical port %1 and physical port %2 do not have the same range "
                "size within %3").arg(logicalPort->name(), physicalPort->name(), context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::checkAndParseExpression()
//-----------------------------------------------------------------------------
std::pair<bool, quint64> PortMapValidator::checkAndParseExpression(QString const& expression) const
{
    bool expressionValid = false;
    bool conversionValid = false;
    quint64 value = expressionParser_->parseExpression(expression, &expressionValid).toULongLong(&conversionValid);

    return std::make_pair(expressionValid && conversionValid, value);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findLogicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> PortMapValidator::findLogicalPort(QString const& portName) const
{
    if (abstractionDefinition_)
    {
        return Search::findByName(portName, *abstractionDefinition_->getLogicalPorts());
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findPhysicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortMapValidator::findPhysicalPort(QString const& portName) const
{
    return Search::findByName(portName, *availablePorts_);
}
