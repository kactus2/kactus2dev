//-----------------------------------------------------------------------------
// File: PortAbstractionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 03.09.2024
//
// Description:
// Validator for port abstractions.
//-----------------------------------------------------------------------------

#include "PortAbstractionValidator.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>

#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/validators/QualifierValidator.h>
#include <IPXACTmodels/common/validators/TimingConstraintValidator.h>
#include <IPXACTmodels/common/validators/CellSpecificationValidator.h>

#include <KactusAPI/include/ExpressionParser.h>

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::PortAbstractionValidator()
//-----------------------------------------------------------------------------
PortAbstractionValidator::PortAbstractionValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> expressionParser) :
expressionParser_(expressionParser),
library_(library),
packetValidator_(expressionParser_)
{

}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::validate()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::validate(QSharedPointer<PortAbstraction> port,
    QSharedPointer<AbstractionDefinition > abstractionDefinition,
    QSharedPointer<const BusDefinition> busDefinition) const
{
    if (!hasValidName(port))
    {
        return false;
    }

    if (abstractionDefinition->getRevision() != Document::Revision::Std22)
    {
        if (port->getMatch())
        {
            return false;
        }

        // Presence must be valid expression if defined.
        bool isValidPresence = false;
        expressionParser_->parseExpression(port->isPresent(), &isValidPresence);
        if (!isValidPresence)
        {
            return false;
        }
    }
    else
    {
        if (!port->isPresent().isEmpty())
        {
            return false;
        }
    }

    QSharedPointer<WireAbstraction> wire = port->getWire();
    QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();

    // Must be wire XOR trans.
    if ((!wire && !transactional) || (wire && transactional))
    {
        return false;
    }

    QStringList systemGroups = BusDefinitionUtils::getSystemGroups(busDefinition, library_);

    auto ports = abstractionDefinition->getLogicalPorts();
    if (wire)
    {
        if (!isValidPortQualifier(wire->getQualifier(), abstractionDefinition->getRevision()))
        {
            return false;
        }

        // Default value must be valid expression if defined.
        bool isValidDefaultValue = false;
        expressionParser_->parseExpression(wire->getDefaultValue(), &isValidDefaultValue);
        if (isValidDefaultValue == false)
        {
            return false;
        }

        // Master and slave ports must be valid if exist.
        if (wire->getMasterPort() && !isValidWirePort(wire->getMasterPort(), ports))
        {
            return false;
        }

        if (wire->getSlavePort() && !isValidWirePort(wire->getSlavePort(), ports))
        {
            return false;
        }

        // System ports must be valid if exist.
        if (wire->getSystemPorts())
        {
            for (auto const& systemWirePort : *wire->getSystemPorts())
            {
                if (!isValidWirePort(systemWirePort, ports) ||
                    !systemGroups.contains(systemWirePort->getSystemGroup()))
                {
                    return false;
                }
            }
        }
    }

    if (transactional)
    {
        if (!isValidPortQualifier(transactional->getQualifier(), abstractionDefinition->getRevision()))
        {
            return false;
        }

        // Master and slave ports must be valid if exist.
        if (transactional->getMasterPort() && !isValidTransactionalPort(transactional->getMasterPort()))
        {
            return false;
        }

        if (transactional->getSlavePort() && !isValidTransactionalPort(transactional->getSlavePort()))
        {
            return false;
        }

        // System ports must be valid if exist.
        if (transactional->getSystemPorts())
        {
            for (auto const& transPort : *transactional->getSystemPorts())
            {
                if (!isValidTransactionalPort(transPort) || !systemGroups.contains(transPort->getSystemGroup()))
                {
                    return false;
                }
            }
        }
    }

    if (!port->getPackets()->isEmpty() && (abstractionDefinition->getRevision() == Document::Revision::Std14 ||
        !hasValidPortPackets(port)))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PortAbstractionValidator::findErrorsIn(QStringList& errors, QSharedPointer<PortAbstraction> port,
    QSharedPointer<AbstractionDefinition> abstractionDefinition, 
    QSharedPointer<const BusDefinition> busDefinition) const
{
    // The name must be non-empty.
    if (!hasValidName(port))
    {
        errors.append(QObject::tr("Port name %1 is invalid.").arg(port->getLogicalName()));
    }

    // Presence must be valid expression if defined.
    if (!isValidExpression(port->isPresent()))
    {
        errors.append(QObject::tr("The presence '%1' of port %2 is invalid.")
            .arg(port->isPresent(), port->getLogicalName()));
    }

    if (abstractionDefinition->getRevision() != Document::Revision::Std22 && port->getMatch())
    {
        errors.append(QObject::tr("Port match is set for port %1 in abstraction definition not using IP-XACT"
            " standard revision 2022.").arg(port->getLogicalName()));
    }

    QSharedPointer<WireAbstraction> wire = port->getWire();
    QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();

    // Must be wire XOR trans.
    if (!wire && !transactional)
    {
        errors.append(QObject::tr("Port %1 has neither wire nor transactional defined.").arg(
            port->getLogicalName()));
    }

    if (wire && transactional)
    {
        errors.append(QObject::tr("Port %1 has both wire and transactional.").arg(port->getLogicalName()));
    }

    QString context = QObject::tr("port %1").arg(port->getLogicalName());

    QStringList systemGroupNames;
    QString busDefinitionIdentifier = QStringLiteral(":::");
    if (busDefinition)
    {
        systemGroupNames = BusDefinitionUtils::getSystemGroups(busDefinition, library_);
        busDefinitionIdentifier = busDefinition->getVlnv().toString();
    }

    auto ports = abstractionDefinition->getLogicalPorts();
    if (wire)
    {
        if (auto qualifier = wire->getQualifier(); qualifier->isSet())
        {
            findErrorsInPortQualifier(errors, qualifier, context, abstractionDefinition);
        }

        // Default value must be valid expression if defined.
        bool isValidDefaultValue = false;
        expressionParser_->parseExpression(wire->getDefaultValue(), &isValidDefaultValue);
        if (isValidDefaultValue == false)
        {
            errors.append(QObject::tr("Default value for port %1 is invalid.").arg(port->getLogicalName()));
        }

        // Master and slave ports must be valid if exist.
        if (wire->getMasterPort())
        {
            findErrorsInWirePort(errors, wire->getMasterPort(), context, ports);
        }

        if (wire->getSlavePort())
        {
            findErrorsInWirePort(errors, wire->getSlavePort(), context, ports);
        }

        // System ports must be valid if exist.
        if (wire->getSystemPorts())
        {
            for (auto const& wirePort : *wire->getSystemPorts())
            {
                findErrorsInWirePort(errors, wirePort, context, ports);
                findErrorsInSystemGroup(
                    errors, wirePort->getSystemGroup(), context, systemGroupNames, busDefinitionIdentifier);
            }
        }
    }

    if (transactional)
    {
        if (auto qualifier = transactional->getQualifier(); qualifier->isSet())
        {
            findErrorsInPortQualifier(errors, qualifier, context, abstractionDefinition);
        }

        // Master and slave ports must be valid if exist.
        if (transactional->getMasterPort())
        {
            findErrorsInTransactionalPort(errors, context, transactional->getMasterPort());
        }

        if (transactional->getSlavePort())
        {
            findErrorsInTransactionalPort(errors, context, transactional->getSlavePort());
        }

        if (transactional->getSystemPorts())
        {
            // System ports must be valid if exist.
            for (auto const& transPort : *transactional->getSystemPorts())
            {
                findErrorsInTransactionalPort(errors, context, transPort);
                findErrorsInSystemGroup(
                    errors, transPort->getSystemGroup(), context, systemGroupNames, busDefinitionIdentifier);
            }
        }
    }

    findErrorsInPortPackets(errors, context, port, abstractionDefinition->getRevision());
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::hasValidName(QSharedPointer<PortAbstraction> portAbstraction) const
{
    return CommonItemsValidator::hasValidName(portAbstraction->getLogicalName());
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::wireHasValidDefaultValue()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::wireHasValidDefaultValue(QSharedPointer<WireAbstraction> wireAbstraction) const
{
    return isValidExpression(wireAbstraction->getDefaultValue());
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::isValidWirePort()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::isValidWirePort(QSharedPointer<WirePort> wirePort, QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
    // Width must be valid expression if defined.
    // Any existing constraints must be valid.
    if (!wireHasValidWidth(wirePort) || !validateConstraints(wirePort, ports))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::wireHasValidWidth()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::wireHasValidWidth(QSharedPointer<WirePort> wirePort) const
{
    return isValidExpression(wirePort->getWidth());
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::isValidTransactionalPort()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::isValidTransactionalPort(QSharedPointer<TransactionalPort> transPort) const
{
    // Initiative must be an existing one.
    if (!transPort->getInitiative().isEmpty() &&
        !TransactionalTypes::isIpXactInitiativeType(transPort->getInitiative()))
    {
        return false;
    }

    // Kind must be an existing one.
    if (!transPort->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType(transPort->getKind()))
    {
        return false;
    }

    // Bus width must be valid expression if defined.
    if (!transactionalHasValidBusWidth(transPort))
    {
        return false;
    }

    QSharedPointer<Protocol> protocol = transPort->getProtocol();

    // Protocol must be valid if it exists.
    if (protocol && !protocolValidator_.validate(protocol))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::transactionalHasValidBusWidth()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::transactionalHasValidBusWidth(QSharedPointer<TransactionalPort> transPort) const
{
    return isValidExpression(transPort->getBusWidth());
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::isValidPortQualifier()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::isValidPortQualifier(QSharedPointer<Qualifier> qualifier, Document::Revision documentRevision) const
{
    if (!QualifierValidator::validate(qualifier, documentRevision))
    {
        return false;
    }

    if (qualifier->hasType(Qualifier::Type::PowerEnable) &&
        !qualifier->getAttribute(Qualifier::Attribute::PowerDomainReference).isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::validateConstraints()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::validateConstraints(QSharedPointer<WirePort> wirePort, QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
    // Validate timing constraints.
    TimingConstraintValidator timeConstraintValidator;

    if (auto timing = wirePort->getTimingConstraint();
        timing && !timeConstraintValidator.validate(timing, ports))
    {
        return false;
    }

    if (auto timingMirrored = wirePort->getMirroredTimingConstraint();
        timingMirrored && !timeConstraintValidator.validate(timingMirrored, ports))
    {
        return false;
    }

    // Validate driver constraints.
    CellSpecificationValidator cellConstraintValidator;

    if (auto drive = wirePort->getDriveConstraint();
        drive && !cellConstraintValidator.validate(drive))
    {
        return false;
    }

    if (auto driveMirrored = wirePort->getMirroredDriveConstraint();
        driveMirrored && !cellConstraintValidator.validate(driveMirrored))
    {
        return false;
    }

    // Validate load constraints.
    if (auto load = wirePort->getLoadConstraint();
        load && !cellConstraintValidator.validate(load))
    {
        return false;
    }

    if (auto loadMirrored = wirePort->getMirroredLoadConstraint();
        loadMirrored && !cellConstraintValidator.validate(loadMirrored))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::hasValidPortPackets()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::hasValidPortPackets(QSharedPointer<PortAbstraction> port) const
{
    auto packets = port->getPackets();

    if (packets->isEmpty())
    {
        return true;
    }

    PacketValidator packetValidator(expressionParser_);
    for (auto const& packet : *packets)
    {
        if (!packetValidator.validate(packet))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::findErrorsInConstraints()
//-----------------------------------------------------------------------------
void PortAbstractionValidator::findErrorsInConstraints(QStringList& errors, QSharedPointer<WirePort> wirePort, QString const& context, QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
    // Validate timing constraints.
    TimingConstraintValidator timeConstraintValidator;

    if (auto timing = wirePort->getTimingConstraint())
    {
        timeConstraintValidator.findErrorsIn(errors, timing, context, ports);
    }

    
    if (auto timingMirrored = wirePort->getMirroredTimingConstraint())
    {
        timeConstraintValidator.findErrorsIn(errors, timingMirrored, context, ports);
    }

    // Validate driver constraints.
    CellSpecificationValidator cellConstVal;

    if (auto drive = wirePort->getDriveConstraint())
    {
        cellConstVal.findErrorsIn(errors, drive, context);
    }

    if (auto driveMirrored = wirePort->getMirroredDriveConstraint())
    {
        cellConstVal.findErrorsIn(errors, driveMirrored, context);
    }

    // Validate load constraints.
    if (auto load = wirePort->getLoadConstraint())
    {
        cellConstVal.findErrorsIn(errors, load, context);
    }

    if (auto loadMirrored = wirePort->getMirroredLoadConstraint())
    {
        cellConstVal.findErrorsIn(errors, loadMirrored, context);
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::findErrorsInWirePort()
//-----------------------------------------------------------------------------
void PortAbstractionValidator::findErrorsInWirePort(QStringList& errors, QSharedPointer<WirePort> wirePort, QString const& context, QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
    // Width must be valid expression if defined.
    if (!wireHasValidWidth(wirePort))
    {
        errors.append(QObject::tr("Wire port width is invalid in %1.").arg(context));
    }

    // Any existing constraints must be valid.
    findErrorsInConstraints(errors, wirePort, context, ports);
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::findErrorsInTransactionalPort()
//-----------------------------------------------------------------------------
void PortAbstractionValidator::findErrorsInTransactionalPort(QStringList& errors, QString const& context, QSharedPointer<TransactionalPort> transPort) const
{
    // Initiative must be an existing one.
    if (!transPort->getInitiative().isEmpty() &&
        !TransactionalTypes::isIpXactInitiativeType(transPort->getInitiative()))
    {
        errors.append(QObject::tr("The transactional port initiative %1 is invalid in %2.")
            .arg(transPort->getInitiative(), context));
    }

    // Kind must be an existing one.
    if (!transPort->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType(transPort->getKind()))
    {
        errors.append(QObject::tr("The transactional port kind %1 is invalid in %2.")
            .arg(transPort->getInitiative(), context));
    }

    // Bus width must be valid expression if defined.
    bool isValidBusWidth = false;
    expressionParser_->parseExpression(transPort->getBusWidth(), &isValidBusWidth);
    if (isValidBusWidth == false)
    {
        errors.append(QObject::tr("The transactional port bus width is invalid in %1.").arg(context));
    }

    // Protocol must be valid if it exists.
    if (QSharedPointer<Protocol> protocol = transPort->getProtocol())
    {
        protocolValidator_.findErrorsIn(errors, protocol, context);
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::findErrorsInPortQualifier()
//-----------------------------------------------------------------------------
void PortAbstractionValidator::findErrorsInPortQualifier(QStringList& errors, QSharedPointer<Qualifier> qualifier, QString const& context, QSharedPointer<AbstractionDefinition> abstraction) const
{
    QualifierValidator::findErrorsIn(errors, qualifier, context, abstraction->getRevision());

    if (qualifier->hasType(Qualifier::Type::PowerEnable) && 
        !qualifier->getAttribute(Qualifier::Attribute::PowerDomainReference).isEmpty())
    {
        errors.append(QObject::tr("Illegal attribute powerDomainRef set for power enable qualifier "
            "of %1 within abstraction definition %2.").arg(context, abstraction->getVlnv().toString()));
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::findErrorsInSystemGroup()
//-----------------------------------------------------------------------------
void PortAbstractionValidator::findErrorsInSystemGroup(QVector<QString>& errors, QString const& systemGroup, QString const& context, QStringList const& availableSystemNames, QString const& busDefinitionIdentifier) const
{
    if (systemGroup.isEmpty())
    {
        errors.append(QObject::tr("System group must be assigned for %1.").arg(context));
    }
    else if (!availableSystemNames.contains(systemGroup))
    {
        errors.append(QObject::tr(
            "The system group %1 in %2 is not defined in system groups of bus definition %3.")
            .arg(systemGroup, context, busDefinitionIdentifier));
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::findErrorsInPortPackets()
//-----------------------------------------------------------------------------
void PortAbstractionValidator::findErrorsInPortPackets(QVector<QString>& errors, QString const& context, QSharedPointer<PortAbstraction> port, Document::Revision revision) const
{
    auto packets = port->getPackets();

    if (packets->isEmpty())
    {
        return;
    }

    if (revision != Document::Revision::Std22)
    {
        errors.append(QObject::tr("Packets defined in %1 not supported by IP-XACT document revision.").arg(context));
    }
    else
    {
        PacketValidator packetValidator(expressionParser_);
        for (auto const& packet : *packets)
        {
            packetValidator.findErrorsIn(errors, packet, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstractionValidator::isValidExpressionOrZero()
//-----------------------------------------------------------------------------
bool PortAbstractionValidator::isValidExpression(QString const& expression) const
{
    bool expressionOk = false;
    QString solvedValue = expressionParser_->parseExpression(expression, &expressionOk);

    if (expressionOk == false)
    {
        return false;
    }

    return true;
}
