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

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/Port.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: PortMapValidator::PortMapValidator()
//-----------------------------------------------------------------------------
PortMapValidator::PortMapValidator(QSharedPointer<ExpressionParser> parser,
    QSharedPointer<QList<QSharedPointer<Port> > > ports, LibraryInterface* libraryHandler):
expressionParser_(parser),
availablePorts_(ports),
abstractionReference_(),
abstractionDefinition_(),
interfaceMode_(General::INTERFACE_MODE_COUNT),
libraryHandler_(libraryHandler)
{
    if (abstractionReference_)
    {
        QSharedPointer<Document const> abstractionDocument = libraryHandler_->getModelReadOnly(*abstractionReference_.data());

        abstractionDefinition_ = abstractionDocument.dynamicCast<AbstractionDefinition const>();
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::~PortMapValidator()
//-----------------------------------------------------------------------------
PortMapValidator::~PortMapValidator()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::busInterfaceChanged()
//-----------------------------------------------------------------------------
void PortMapValidator::busInterfaceChanged(QSharedPointer<ConfigurableVLNVReference> newAbstractionReference,
    General::InterfaceMode newInterfaceMode)
{
    if (newAbstractionReference)
    {
        abstractionReference_ = newAbstractionReference;

        QSharedPointer<Document const> abstractionDocument =
            libraryHandler_->getModelReadOnly(*abstractionReference_.data());
        abstractionDefinition_ = abstractionDocument.dynamicCast<AbstractionDefinition const>();
    }

    interfaceMode_ = newInterfaceMode;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::busInterfaceChanged()
//-----------------------------------------------------------------------------
void PortMapValidator::abstractionDefinitionChanged(QSharedPointer<AbstractionDefinition> newDefinition,
    General::InterfaceMode newInterfaceMode)
{
    if (newDefinition && abstractionDefinition_ != newDefinition)
    {
        abstractionDefinition_ = newDefinition;

        QSharedPointer<ConfigurableVLNVReference> newAbstractionReference
            (new ConfigurableVLNVReference(newDefinition->getVlnv()));
        abstractionReference_ = newAbstractionReference;
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
bool PortMapValidator::validate(QSharedPointer<PortMap> portMap) const
{
    QSharedPointer<PortAbstraction> logicalPort;
    if (portMap->getLogicalPort())
    {
        logicalPort = findLogicalPort(portMap->getLogicalPort()->name_);
    }
    QSharedPointer<Port> physicalPort;
    if (portMap->getPhysicalPort())
    {
        physicalPort = findPhysicalPort(portMap->getPhysicalPort()->name_);
    }

	return hasValidIsPresent(portMap) && hasValidLogicalPort(portMap) && 
		hasValidPhysicalPort(portMap, physicalPort) && hasValidTieOff(portMap) &&
		connectedPortsHaveValidDirections(logicalPort, physicalPort) && 
		connectedPortsHaveValidPortTypes(logicalPort, physicalPort) &&
        connectedPortsHaveSameRange(portMap);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidIsPresent(QSharedPointer<PortMap> portMap) const
{
    if (!portMap->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(portMap->getIsPresent());

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
bool PortMapValidator::hasValidLogicalPort(QSharedPointer<PortMap> portMap) const
{
    if (portMap->getLogicalPort() && !portMap->getLogicalPort()->name_.isEmpty())
    {
        if (abstractionReference_)
        {
            QSharedPointer<PortAbstraction> referencedPort = findLogicalPort(portMap->getLogicalPort()->name_);
            if (referencedPort)
            {
                return logicalPortHasValidRange(portMap->getLogicalPort()) &&
                    logicalPortRangeIsWithinAbstractionWidth(portMap->getLogicalPort(), referencedPort);
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::logicalPortHasValidRange()
//-----------------------------------------------------------------------------
bool PortMapValidator::logicalPortHasValidRange(QSharedPointer<PortMap::LogicalPort> logicalPort) const
{
    if (logicalPort->range_ &&
        (!logicalPort->range_->getLeft().isEmpty() || !logicalPort->range_->getRight().isEmpty()))
    {
        bool leftOk = true;
        quint64 rangeLeft =
            expressionParser_->parseExpression(logicalPort->range_->getLeft()).toULongLong(&leftOk);

        bool rightOk = true;
        quint64 rangeRight =
            expressionParser_->parseExpression(logicalPort->range_->getRight()).toULongLong(&rightOk);

        return leftOk && rangeLeft >= 0 && rightOk && rangeRight >= 0;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::logicalPortRangeIsWithinAbstractionWidth()
//-----------------------------------------------------------------------------
bool PortMapValidator::logicalPortRangeIsWithinAbstractionWidth(QSharedPointer<PortMap::LogicalPort> logicalPort,
    QSharedPointer<PortAbstraction> referencedPort) const
{
    if (referencedPort)
    {
        if (logicalPort->range_ && referencedPort->getWire() && !referencedPort->getWire()->getWidth(interfaceMode_).isEmpty())
        {
            quint64 rangeLeft = expressionParser_->parseExpression(logicalPort->range_->getLeft()).toULongLong();
            quint64 rangeRight = expressionParser_->parseExpression(logicalPort->range_->getRight()).toULongLong();

            quint64 abstractionWidth = expressionParser_->parseExpression(
                referencedPort->getWire()->getWidth(interfaceMode_)).toULongLong();

            return 0 <= rangeLeft && rangeLeft <= abstractionWidth - 1 && 
                0 <= rangeRight && rangeRight <= abstractionWidth - 1;
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidPhysicalPort(QSharedPointer<PortMap> portMap,
	QSharedPointer<Port> physicalPort) const
{
    if (portMap->getPhysicalPort() && !portMap->getPhysicalPort()->name_.isEmpty() &&
        portMap->getLogicalTieOff().isEmpty() && availablePorts_ && physicalPort)
    {
		return physicalPortHasValidPartSelect(portMap->getPhysicalPort()) &&
			physicalPortRangeIsWithinReferencedPort(physicalPort, portMap->getPhysicalPort());
    }
    else if (!portMap->getLogicalTieOff().isEmpty() && !portMap->getPhysicalPort())
    {
        bool changeOk = true;
        int logicalTieOff = expressionParser_->parseExpression(portMap->getLogicalTieOff()).toInt(&changeOk);

        return changeOk && logicalTieOff > 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidTieOff()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidTieOff(QSharedPointer<PortMap> portMap) const
{
    if (portMap && !portMap->getLogicalTieOff().isEmpty() && !portMap->getPhysicalPort())
    {
        bool changeOk = true;
        int logicalTieOff = expressionParser_->parseExpression(portMap->getLogicalTieOff()).toInt(&changeOk);

        return changeOk && logicalTieOff > 0;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::physicalPortHasValidPartSelect()
//-----------------------------------------------------------------------------
bool PortMapValidator::physicalPortHasValidPartSelect(QSharedPointer<PortMap::PhysicalPort> physicalPort) const
{
    if (physicalPort->partSelect_ && (!physicalPort->partSelect_->getLeftRange().isEmpty() ||
        !physicalPort->partSelect_->getRightRange().isEmpty()))
    {
        bool rangeLeftOk = true;
        quint64 rangeLeft = expressionParser_->parseExpression(
            physicalPort->partSelect_->getLeftRange()).toULongLong(&rangeLeftOk);

        bool rangeRightOk = true;
        quint64 rangeRight = expressionParser_->parseExpression(
            physicalPort->partSelect_->getRightRange()).toULongLong(&rangeRightOk);

        return rangeLeftOk && rangeLeft >= 0 && rangeRightOk && rangeRight >= 0;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::physicalPortRangeIsWithinReferencedPort()
//-----------------------------------------------------------------------------
bool PortMapValidator::physicalPortRangeIsWithinReferencedPort(QSharedPointer<Port> referencedPort,
    QSharedPointer<PortMap::PhysicalPort> physicalPort) const
{
    if (referencedPort && physicalPort->partSelect_ && (!physicalPort->partSelect_->getLeftRange().isEmpty() ||
        !physicalPort->partSelect_->getRightRange().isEmpty()))
    {
        quint64 rangeLeft =
            expressionParser_->parseExpression(physicalPort->partSelect_->getLeftRange()).toULongLong();
        quint64 rangeRight =
            expressionParser_->parseExpression(physicalPort->partSelect_->getRightRange()).toULongLong();

        quint64 portLeft = 0;
        quint64 portRight = 0;

        portLeft = expressionParser_->parseExpression(referencedPort->getLeftBound()).toULongLong();
        portRight = expressionParser_->parseExpression(referencedPort->getRightBound()).toULongLong();

        if (portLeft > portRight)
        {
            quint64 temporary = portLeft;
            portLeft = portRight;
            portRight = temporary;
        }

        return rangeLeft >= portLeft && rangeLeft <= portRight && rangeRight >= portLeft&& rangeRight <= portRight;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveValidDirections()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveValidDirections(QSharedPointer<PortAbstraction> logicalPort,
	QSharedPointer<Port> physicalPort) const
{
    if (logicalPort && physicalPort)
    {
        if (logicalPort->getWire() && physicalPort->getWire() &&
            !physicalPort->getWire()->getAllLogicalDirectionsAllowed())
        {
            DirectionTypes::Direction logicalDirection = logicalPort->getWire()->getDirection(interfaceMode_);
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

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveValidPortTypes()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveValidPortTypes(QSharedPointer<PortAbstraction> logicalPort, 
	QSharedPointer<Port> physicalPort) const
{
    if (logicalPort && physicalPort)
    {
        return (logicalPort->getWire() && physicalPort->getWire()) ||
            (logicalPort->getTransactional() && physicalPort->getTransactional());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveSameRange(QSharedPointer<PortMap> portMap) const
{
    if (portMap->getLogicalPort() && portMap->getPhysicalPort())
    {
        if (portMap->getLogicalPort()->range_ && portMap->getPhysicalPort()->partSelect_)
        {
            int logicalLeft =
                expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
            int logicalRight =
                expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
            int logicalSize = abs(logicalLeft - logicalRight) + 1;

            int physicalLeft = expressionParser_->parseExpression(
                portMap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
            int physicalRight = expressionParser_->parseExpression(
                portMap->getPhysicalPort()->partSelect_->getRightRange()).toInt();
            int physicalSize = abs(physicalLeft - physicalRight) + 1;

            return logicalSize == physicalSize;
        }
        else if (!portMap->getLogicalPort()->range_ && !portMap->getPhysicalPort()->partSelect_)
        {
            return true;
        }
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
    if (portMap->getLogicalPort())
    {
        logicalPort = findLogicalPort(portMap->getLogicalPort()->name_);
    }

    QSharedPointer<Port> physicalPort;
    if (portMap->getPhysicalPort())
    {
        physicalPort = findPhysicalPort(portMap->getPhysicalPort()->name_);
    }

    findErrorsInIsPresent(errors, portMap, context);
    findErrorsInLogicalPort(errors, portMap->getLogicalPort(), logicalPort, context);
    findErrorsInPhysicalPort(errors, portMap, physicalPort, context);
	findErrorsInPortConnection(errors, portMap, logicalPort, physicalPort, context);
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
	QSharedPointer<PortMap::LogicalPort> logicalPort, 
	QSharedPointer<PortAbstraction> referencedPort, QString const& context) const
{
    if (logicalPort && !logicalPort->name_.isEmpty())
    {
        QSharedPointer<PortAbstraction> referencedPort = findLogicalPort(logicalPort->name_);

        if (abstractionReference_)
        {
            if (!referencedPort)
            {
                errors.append(QObject::tr("Could not locate logical port %1 mapped within %2")
                    .arg(logicalPort->name_).arg(context));
            }
        }

        if (logicalPort && !logicalPortHasValidRange(logicalPort))
        {
            errors.append(QObject::tr("Invalid range given for logical port %1 mapped within %2")
                .arg(logicalPort->name_).arg(context));
        }

        if (logicalPort && !logicalPortRangeIsWithinAbstractionWidth(logicalPort, referencedPort))
        {
            errors.append(QObject::tr("Logical port %1 range is not within referenced abstraction definition port "
                "width within %2").arg(logicalPort->name_).arg(context));
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
        portMap->getLogicalTieOff().isEmpty() && availablePorts_)
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
                DirectionTypes::direction2Str(logicalPort->getWire()->getDirection(interfaceMode_));
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
// Function: PortMapValidator::findLogicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> PortMapValidator::findLogicalPort(QString const& portName) const
{
    if (abstractionReference_ && abstractionDefinition_)
    {
        foreach (QSharedPointer<PortAbstraction> definitionPort, *abstractionDefinition_->getLogicalPorts())
        {
            if (portName == definitionPort->getLogicalName())
            {
                return definitionPort;
            }
        }
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findPhysicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortMapValidator::findPhysicalPort(QString const& portName) const
{
    foreach (QSharedPointer<Port> componentPort, *availablePorts_)
    {
        if (portName == componentPort->name())
        {
            return componentPort;
        }
    }

    return QSharedPointer<Port> ();
}