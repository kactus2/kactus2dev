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

#include <KactusAPI/include/ExpressionParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>

#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/utilities/Search.h>

//-----------------------------------------------------------------------------
// Function: PortMapValidator::PortMapValidator()
//-----------------------------------------------------------------------------
PortMapValidator::PortMapValidator(QSharedPointer<ExpressionParser> parser,
    QSharedPointer<QList<QSharedPointer<Port> > > ports, LibraryInterface* libraryHandler) :
expressionParser_(parser),
availablePorts_(ports),
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
void PortMapValidator::abstractionDefinitionChanged(QSharedPointer<AbstractionDefinition const> newDefinition,
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
    QSharedPointer<PortMap::LogicalPort> const& logical = portMap->getLogicalPort();
    QSharedPointer<PortMap::PhysicalPort> const& physical = portMap->getPhysicalPort();
    
    if (hasValidLogicalPort(logical) && hasValidPhysicalMapping(portMap) &&
        hasValidIsPresent(portMap))
    {
        if (hasValidPhysicalPort(physical))
        {
            QSharedPointer<PortAbstraction> logicalPort = findLogicalPort(logical->name_);
            QSharedPointer<Port> physicalPort = findPhysicalPort(physical->name_);

            return connectedPortsHaveValidPortTypes(logicalPort, physicalPort) &&
                connectedPortsHaveValidDirections(logicalPort, physicalPort) &&
                connectedPortsHaveValidInitiatives(logicalPort, physicalPort) &&
                connectedPortsHaveSameRange(logical->range_, physical->partSelect_);
        }

        return hasValidTieOff(portMap->getLogicalTieOff());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidIsPresent(QSharedPointer<PortMap> const& portMap) const
{
    return CommonItemsValidator::hasValidIsPresent(portMap->getIsPresent(), expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidLogicalPort()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidLogicalPort(QSharedPointer<PortMap::LogicalPort> const& logical) const
{
    if (logical.isNull())
    {
        return false;
    }

    QSharedPointer<PortAbstraction> referencedPort = findLogicalPort(logical->name_);
    if (referencedPort.isNull() || logical->name_.isEmpty())
    {
        return false;
    }

    return logicalPortHasValidRange(logical->range_, referencedPort);
}


//-----------------------------------------------------------------------------
// Function: PortMapValidator::logicalPortHasValidRange()
//-----------------------------------------------------------------------------
bool PortMapValidator::logicalPortHasValidRange(QSharedPointer<Range> const & logicalRange,
    QSharedPointer<PortAbstraction> const & referencedPort) const
{
    if (logicalRange && (!logicalRange->getLeft().isEmpty() || !logicalRange->getRight().isEmpty()))
    {
        if (!bothRangeValuesExist(logicalRange->getLeft(), logicalRange->getRight()))
        {
            return false;
        }

        bool leftToIntOk = true;
        qint64 leftValue = 0;
        std::tie(leftToIntOk, leftValue) = checkAndParseExpression(logicalRange->getLeft());
        if (!leftToIntOk)
        {
            return false;
        }

        bool rightToIntOk = true;
        qint64 rightValue = 0;
        std::tie(rightToIntOk, rightValue) = checkAndParseExpression(logicalRange->getRight());
        if (!rightToIntOk)
        {
            return false;
        }

        return rangeIsWithinWidth(leftValue, rightValue, getLogicalPortWidth(referencedPort));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::bothRangeValuesExist()
//-----------------------------------------------------------------------------
bool PortMapValidator::bothRangeValuesExist(QString const& leftRange, QString const& rightRange) const
{
    return leftRange.isEmpty() == rightRange.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::getLogicalPortWidth()
//-----------------------------------------------------------------------------
quint64 PortMapValidator::getLogicalPortWidth(QSharedPointer<PortAbstraction> const& logicalPort) const
{
    if (logicalPort->hasWire())
    {
        return logicalPort->getWire()->getWidth(interfaceMode_, systemGroup_).toULongLong();
    }
    else if (logicalPort->hasTransactional())
    {
        return logicalPort->getTransactional()->getWidth(interfaceMode_, systemGroup_).toULongLong();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::rangeIsWithinWidth()
//-----------------------------------------------------------------------------
bool PortMapValidator::rangeIsWithinWidth(qint64 const& rangeLeft, qint64 const& rangeRight,
    quint64 const& width) const
{
    return (0 <= rangeLeft && rangeLeft <= width-1) && (0 <= rangeRight && rangeRight <= width - 1);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidPhysicalMapping()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidPhysicalMapping(QSharedPointer<PortMap> const& portMap) const
{
    QSharedPointer<PortMap::PhysicalPort> physical = portMap->getPhysicalPort();
    QString const& tieOff = portMap->getLogicalTieOff();

    return !physical.isNull() || !tieOff.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidPhysicalPort()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidPhysicalPort(QSharedPointer<PortMap::PhysicalPort> const& physical) const
{
    if (physical.isNull())
    {
        return false;
    }

    QSharedPointer<Port> portReference = findPhysicalPort(physical->name_);
    if (portReference.isNull())
    {
        return false;
    }

    return physicalPortHasValidPartSelect(physical->partSelect_, portReference);
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::hasValidTieOff()
//-----------------------------------------------------------------------------
bool PortMapValidator::hasValidTieOff(QString const& tieOff) const
{
    if (tieOff.isEmpty())
    {
        return false;
    }

    bool changeOk = true;
    quint64 tieOffValue = 0;

    std::tie(changeOk, tieOffValue) = checkAndParseExpression(tieOff);
    return changeOk;
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
        
        DirectionTypes::Direction logicalDirection =
            logicalPort->getWire()->getDirection(interfaceMode_, systemGroup_);
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
// Function: PortMapValidator::connectedPortsHaveValidInitiatives()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveValidInitiatives(QSharedPointer<PortAbstraction> const& logicalPort,
    QSharedPointer<Port> const& physicalPort) const
{
    if (logicalPort->getTransactional() && physicalPort->getTransactional())
    {
        if (physicalPort->getTransactional()->getAllLogicalInitiativesAllowed())
        {
            return true;
        }

        TransactionalTypes::Initiative logicalInitiative = TransactionalTypes::strToInitiative(
            logicalPort->getTransactional()->getInitiative(interfaceMode_, systemGroup_));
        TransactionalTypes::Initiative physicalInitiative =
            TransactionalTypes::strToInitiative(physicalPort->getTransactional()->getInitiative());

        if (physicalInitiative == TransactionalTypes::Initiative::INITIATIVE_INVALID ||
            ((logicalInitiative == TransactionalTypes::Initiative::REQUIRES &&
                physicalInitiative == TransactionalTypes::Initiative::PROVIDES) ||
                (logicalInitiative == TransactionalTypes::Initiative::PROVIDES &&
                    physicalInitiative == TransactionalTypes::Initiative::REQUIRES) ||
                (logicalInitiative == TransactionalTypes::Initiative::BOTH &&
                    (physicalInitiative == TransactionalTypes::Initiative::REQUIRES ||
                    physicalInitiative == TransactionalTypes::Initiative::PROVIDES))))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::physicalPortHasValidPartSelect()
//-----------------------------------------------------------------------------
bool PortMapValidator::physicalPortHasValidPartSelect(QSharedPointer<PartSelect> const& physicalPart,
    QSharedPointer<Port> const& referencedPort) const
{
    if (physicalPart.isNull())
    {
        return true;
    }

    if (physicalPart->getLeftRange().isEmpty() && physicalPart->getRightRange().isEmpty())
    {
        return true;
    }

    if (!bothRangeValuesExist(physicalPart->getLeftRange(), physicalPart->getRightRange()))
    {
        return false;
    }

    bool leftOK = true;
    qint64 leftValue = 0;
    std::tie(leftOK, leftValue) = checkAndParseExpression(physicalPart->getLeftRange());
    if (!leftOK)
    {
        return false;
    }

    bool rightOK = true;
    qint64 rightValue = 0;
    std::tie(rightOK, rightValue) = checkAndParseExpression(physicalPart->getRightRange());
    if (!rightOK)
    {
        return false;
    }

    if (referencedPort->getWire())
    {
        return rangeIsValidInWire(leftValue, rightValue,
            expressionParser_->parseExpression(referencedPort->getLeftBound()).toULongLong(),
            expressionParser_->parseExpression(referencedPort->getRightBound()).toULongLong());
    }
    else if (referencedPort->getTransactional())
    {
        return rangeIsWithinWidth(leftValue, rightValue, expressionParser_->parseExpression(
            referencedPort->getTransactional()->getBusWidth()).toULongLong());
    }
  
     return false;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::rangeIsValidInWire()
//-----------------------------------------------------------------------------
bool PortMapValidator::rangeIsValidInWire(quint64 const& rangeLeft, quint64 const& rangeRight,
    quint64 const& wireLeft, quint64 const& wireRight) const
{
    if (wireLeft > wireRight)
    {
        return rangeLeft >= wireRight && rangeLeft <= wireLeft &&
            rangeRight >= wireRight && rangeRight <= wireLeft;
    }
    else
    {
        return rangeLeft >= wireLeft && rangeLeft <= wireRight &&
            rangeRight >= wireLeft && rangeRight <= wireRight;
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::connectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
bool PortMapValidator::connectedPortsHaveSameRange(QSharedPointer<Range> const& logicalRange,
    QSharedPointer<PartSelect> const& physicalPart) const
{
    quint64 physicalLeft = 0;
    quint64 physicalRight = 0;

    if (physicalPart && (!physicalPart->getLeftRange().isEmpty() || !physicalPart->getRightRange().isEmpty()))
    {
        bool leftValid = false;
        std::tie(leftValid, physicalLeft) = checkAndParseExpression(physicalPart->getLeftRange());
        if (leftValid == false)
        {
            return false;
        }

        bool rightValid = false;
        std::tie(rightValid, physicalRight) = checkAndParseExpression(physicalPart->getRightRange());
        if (rightValid == false)
        {
            return false;
        }
    }

    quint64 logicalLeft = 0;
    quint64 logicalRight = 0;
    if (logicalRange && (!logicalRange->getLeft().isEmpty() || !logicalRange->getRight().isEmpty()))
    {
        bool leftValid = false;
        std::tie(leftValid, logicalLeft) = checkAndParseExpression(logicalRange->getLeft());
        if (leftValid == false)
        {
            return false;
        }

        bool rightValid = false;
        std::tie(rightValid, logicalRight) = checkAndParseExpression(logicalRange->getRight());
        if (rightValid == false)
        {
            return false;
        }
    }

    quint64 physicalMax = qMax(physicalLeft, physicalRight);
    quint64 physicalMin = qMin(physicalLeft, physicalRight);
    quint64 logicalMax = qMax(logicalLeft, logicalRight);
    quint64 logicalMin = qMin(logicalLeft, logicalRight);

    quint64 logicalSize = logicalMax - logicalMin + 1;
    quint64 physicalSize = physicalMax - physicalMin + 1;

    return logicalSize == physicalSize;
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<PortMap> portMap,
    QString const& context) const
{
    QSharedPointer<PortMap::LogicalPort> logicalPort = portMap->getLogicalPort();
    QSharedPointer<PortMap::PhysicalPort> physicalPort = portMap->getPhysicalPort();

    QSharedPointer<PortAbstraction> referencedLogicalPort;
    if (logicalPort)
    {
        referencedLogicalPort = findLogicalPort(logicalPort->name_);
    }
    QSharedPointer<Port> referencedPhysicalPort;
    if (physicalPort)
    {
        referencedPhysicalPort = findPhysicalPort(physicalPort->name_);
    }

    findErrorsInIsPresent(errors, portMap, context);
    findErrorsInLogicalPort(errors, logicalPort, referencedLogicalPort, context);

    if (physicalPort && !portMap->getLogicalTieOff().isEmpty())
    {
        errors.append(QObject::tr("Invalid port map: contains both a physical port and a tie off value within %1")
            .arg(context));
    }
    else if (!portMap->getLogicalTieOff().isEmpty())
    {
        findErrorsInTieOff(errors, portMap->getLogicalTieOff(), context);
    }
    else if (physicalPort.isNull() == false && !physicalPort->name_.isEmpty())
    {
        findErrorsInPhysicalPort(errors, physicalPort, referencedPhysicalPort, context);

        if (hasValidLogicalPort(logicalPort) && hasValidPhysicalPort(physicalPort))
        {
            findErrorsInPortConnection(
                errors, logicalPort, physicalPort, referencedLogicalPort, referencedPhysicalPort, context);
        }
    }
    else
    {
        errors.append(QObject::tr("Port map does not contain a physical port or a logical tie off within %1")
            .arg(context));
    }
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
    QSharedPointer<PortMap::LogicalPort> logicalPort, QSharedPointer<PortAbstraction> referencedPort,
    QString const& context) const
{
    if (logicalPort == nullptr || logicalPort->name_.isEmpty())
    {
        errors.append(QObject::tr("Port map does not contain a logical port within %1").arg(context));
        return;
    }

    if (referencedPort == nullptr)
    {
        errors.append(QObject::tr("Could not locate logical port %1 mapped within %2")
            .arg(logicalPort->name_).arg(context));
        return;
    }

    if (logicalPort->range_)
    {
        QSharedPointer<Range> logicalRange = logicalPort->range_;
        if (!bothRangeValuesExist(logicalRange->getLeft(), logicalRange->getRight()))
        {
            if (logicalRange->getLeft().isEmpty())
            {
                errors.append(QObject::tr("Empty left value in logical port %1 mapped within %2")
                    .arg(logicalPort->name_).arg(context));
            }
            if (logicalRange->getRight().isEmpty())
            {
                errors.append(QObject::tr("Empty right value in logical port %1 mapped within %2")
                    .arg(logicalPort->name_).arg(context));
            }
        }
        else
        {
            bool leftValid = false;
            bool rightValid = false;
            qint64 rangeLeft = 0;
            qint64 rangeRight = 0;

            std::tie(leftValid, rangeLeft) = checkAndParseExpression(logicalRange->getLeft());
            std::tie(rightValid, rangeRight) = checkAndParseExpression(logicalRange->getRight());

            if (!logicalRange->getLeft().isEmpty() && !leftValid)
            {
                errors.append(QObject::tr(
                    "Invalid value given for logical left in logical port %1 mapped within %2")
                    .arg(logicalPort->name_).arg(context));
            }
            if (!logicalRange->getRight().isEmpty() && !rightValid)
            {
                errors.append(QObject::tr(
                    "Invalid value given for logical right in logical port %1 mapped within %2")
                    .arg(logicalPort->name_).arg(context));
            }

            if (leftValid && rightValid &&
                !rangeIsWithinWidth(rangeLeft, rangeRight, getLogicalPortWidth(referencedPort)))
            {
                errors.append(QObject::tr(
                    "Range is not within the referenced port width in mapped logical port %1 in %2")
                    .arg(logicalPort->name_).arg(context));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsInTieOff()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsInTieOff(QVector<QString>& errors, QString const& tieOff, QString const& context)
    const
{
    if (!hasValidTieOff(tieOff))
    {
        errors.append(QObject::tr("Invalid port map logical tie off set within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsInPhysicalPort()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsInPhysicalPort(QVector<QString>& errors,
    QSharedPointer<PortMap::PhysicalPort> physicalPort, QSharedPointer<Port> referencedPort,
    QString const& context) const
{
    if (physicalPort == nullptr)
    {
        return;
    }

    if (physicalPort->name_.isEmpty())
    {
        errors.append(QObject::tr("Port map does not contain a physical port within %1").arg(context));
        return;
    }
    
    if (!referencedPort)
    {
        errors.append(QObject::tr("Could not locate physical port %1 mapped within %2")
            .arg(physicalPort->name_).arg(context));
        return;
    }

    QSharedPointer<PartSelect> physicalPart = physicalPort->partSelect_;
    if (!physicalPart.isNull())
    {
        if (!bothRangeValuesExist(physicalPart->getLeftRange(), physicalPart->getRightRange()))
        {
            if (physicalPart->getLeftRange().isEmpty())
            {
                errors.append(QObject::tr("Empty left value in physical port %1 mapped within %2")
                    .arg(physicalPort->name_).arg(context));
            }
            if (physicalPart->getRightRange().isEmpty())
            {
                errors.append(QObject::tr("Empty right value in physical port %1 mapped within %2")
                    .arg(physicalPort->name_).arg(context));
            }
        }
        else
        {
            bool leftValid = false;
            bool rightValid = false;
            qint64 rangeLeft = 0;
            qint64 rangeRight = 0;

            std::tie(leftValid, rangeLeft) = checkAndParseExpression(physicalPart->getLeftRange());
            std::tie(rightValid, rangeRight) = checkAndParseExpression(physicalPart->getRightRange());

            if (!leftValid)
            {
                errors.append(QObject::tr(
                    "Invalid value given for physical left in physical port %1 mapped within %2")
                    .arg(physicalPort->name_).arg(context));
            }
            if (!rightValid)
            {
                errors.append(QObject::tr(
                    "Invalid value given for physical right in physical port %1 mapped within %2")
                    .arg(physicalPort->name_).arg(context));
            }
            if (leftValid && rightValid)
            {
                if ((referencedPort->getWire() && !rangeIsValidInWire(rangeLeft, rangeRight,
                    expressionParser_->parseExpression(referencedPort->getLeftBound()).toULongLong(),
                    expressionParser_->parseExpression(referencedPort->getRightBound()).toULongLong())) ||
                    ((referencedPort->getTransactional() && !rangeIsWithinWidth(rangeLeft, rangeRight,
                        expressionParser_->parseExpression(
                            referencedPort->getTransactional()->getBusWidth()).toULongLong()))))
                {
                    errors.append(QObject::tr(
                        "Range is not within the referenced port width in mapped physical port %1 in %2")
                        .arg(physicalPort->name_).arg(context));
                }
            }
        }
    }


}

//-----------------------------------------------------------------------------
// Function: PortMapValidator::findErrorsInPortConnection()
//-----------------------------------------------------------------------------
void PortMapValidator::findErrorsInPortConnection(QVector<QString>& errors,
    QSharedPointer<PortMap::LogicalPort> logical, QSharedPointer<PortMap::PhysicalPort> physical,
    QSharedPointer<PortAbstraction> logicalPort, QSharedPointer<Port> physicalPort, QString const& context) const
{
    if (!connectedPortsHaveValidPortTypes(logicalPort, physicalPort))
    {
        errors.append(QObject::tr("Connected logical port %1 and physical port %3 do not have the same port "
            "type within %5").arg(logicalPort->name(), physicalPort->name(), context));
    }
    else
    {
        if (!connectedPortsHaveValidDirections(logicalPort, physicalPort))
        {
            QString logicalDirection =
                DirectionTypes::direction2Str(logicalPort->getWire()->getDirection(interfaceMode_, systemGroup_));
            QString physicalDirection = DirectionTypes::direction2Str(physicalPort->getWire()->getDirection());

            errors.append(QObject::tr("Invalid connection made between logical port '%1' with direction '%2' and "
                "physical port '%3' with direction '%4' within %5")
                .arg(logicalPort->name()).arg(logicalDirection).arg(physicalPort->name()).arg(physicalDirection)
                .arg(context));
        }

        if (!connectedPortsHaveValidInitiatives(logicalPort, physicalPort))
        {
            QString logicalInitiative =
                logicalPort->getTransactional()->getInitiative(interfaceMode_, systemGroup_);
            QString physicalInitiative = physicalPort->getTransactional()->getInitiative();

            errors.append(QObject::tr("Invalid connection made between logical port '%1' with initiative '%2' and "
                "physical port '%3' with initiative '%4' within %5")
                .arg(logicalPort->name()).arg(logicalInitiative).arg(physicalPort->name()).arg(physicalInitiative)
                .arg(context));
        }

        if (!connectedPortsHaveSameRange(logical->range_, physical->partSelect_))
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
