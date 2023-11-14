//-----------------------------------------------------------------------------
// AbstractionDefinition: AbstractionDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 04.12.2015
//
// Description:
// Validator for ipxact:AbstractionDefinition.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionValidator.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/Packet.h>

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/validators/ChoiceValidator.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/common/validators/CellSpecificationValidator.h>
#include <IPXACTmodels/common/validators/TimingConstraintValidator.h>
#include <IPXACTmodels/common/validators/QualifierValidator.h>
#include <IPXACTmodels/AbstractionDefinition/validators/PacketValidator.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::AbstractionDefinitionValidator()
//-----------------------------------------------------------------------------
AbstractionDefinitionValidator::AbstractionDefinitionValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> expressionParser) : 
library_(library), 
    expressionParser_(expressionParser),
    protocolValidator_()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::validate(QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
	// Must have a valid VLNV.
	if (!abstractionDefinition->getVlnv().isValid())
	{
		return false;
	}

    if (!parameterValidator_)
    {
        parameterValidator_ = QSharedPointer<ParameterValidator>(
            new ParameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >(), 
                abstractionDefinition->getRevision()));
    }

	// The defined bus must exist in the library as a component.
    QSharedPointer<const BusDefinition> busDefinition = getBusDefinition(abstractionDefinition);
    if (!busDefinition)
    {
        return false;
    }

	// If this is an extension to another abstraction definition, it must exist.
    if (!hasValidExtend(abstractionDefinition) || !hasValidExtendPorts(abstractionDefinition))
	{
		return false;
	}

	// Any parameters must be valid.
	for (auto const& parameter : *abstractionDefinition->getParameters())
	{
		if (!parameterValidator_->hasValidValue(parameter))
		{
			return false;
		}
	}

	// Must have at least one logical port.
	if (abstractionDefinition->getLogicalPorts()->isEmpty())
	{
		return false;
	}

	// Collection of names of logical ports.
	QVector<QString> logicalNames;

	// Validate each logical port.
	for (auto const& portAbstraction : *abstractionDefinition->getLogicalPorts())
	{
        // Logical name must unique within the abstraction definition.
        if (logicalNames.contains(portAbstraction->getLogicalName()))
        {
            return false;
        }

		if (!isValidPortAbstraction(portAbstraction, abstractionDefinition, busDefinition))
		{
			return false;
		}

        // Logical name is valid: Mark it as used one!
        logicalNames.append(portAbstraction->getLogicalName());
	}

    if (!hasValidChoices(abstractionDefinition))
    {
        return false;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
    if (!parameterValidator_)
    {
        parameterValidator_ = QSharedPointer<ParameterValidator>(
            new ParameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >(),
                abstractionDefinition->getRevision()));
    }

	// Must have a valid VLNV.
	if (!abstractionDefinition->getVlnv().isValid())
	{
		errors.append(QObject::tr("The VLNV %1 is invalid.").arg(abstractionDefinition->getVlnv().toString()));
	}

	// The defined bus must exist in the library as a component.
    QSharedPointer<const BusDefinition> busDefinition = getBusDefinition(abstractionDefinition);
    if (!busDefinition)
    {
        errors.append(QObject::tr("The bus definition %1 referenced in %2 is not found in the library.")
            .arg(abstractionDefinition->getBusType().toString(), abstractionDefinition->getVlnv().toString()));
    }

    QString context = QObject::tr("abstraction definition %1").arg(abstractionDefinition->getVlnv().toString());

    // If this is an extension to another abstraction definition, it must exist.
    findErrorsInExtend(errors, context, abstractionDefinition);
    
    // Any parameters must be valid.
	for (auto const& currentPara : *abstractionDefinition->getParameters())
	{
		parameterValidator_->findErrorsIn(errors, currentPara, context);
	}

	// Must have at least one logical port.
	if (abstractionDefinition->getLogicalPorts()->isEmpty())
	{
		errors.append(QObject::tr("Abstraction definition must define at least one port."));
	}

	// Collection of names of logical ports.
	QVector<QString> logicalNames;

	// Validate each logical port.
	for (auto const& port : *abstractionDefinition->getLogicalPorts())
    {	
        // Logical name must unique within the abstraction definition.
        if (logicalNames.contains(port->getLogicalName()))
        {
            errors.append(QObject::tr("Port name %1 is not unique within %2.").arg(port->getLogicalName(),
               context));
        }

		findErrorsInPortAbstraction(errors, port, abstractionDefinition, busDefinition);

        logicalNames.append(port->getLogicalName());
	}

    if (auto choices = abstractionDefinition->getChoices(); !choices->isEmpty())
    {
        findErrorsInChoices(errors, context, choices);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::isValidPortAbstraction()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidPortAbstraction(QSharedPointer<PortAbstraction> port,
    QSharedPointer<AbstractionDefinition > abstractionDefinition,
    QSharedPointer<const BusDefinition> busDefinition) const
{
	// The name must be non-empty.
	if (!hasValidName(port->getLogicalName()))
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

    if (!hasValidPortPackets(port, abstractionDefinition))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInPortAbstraction()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInPortAbstraction(QVector<QString>& errors,
    QSharedPointer<PortAbstraction> port, QSharedPointer<AbstractionDefinition> abstractionDefinition,
    QSharedPointer<const BusDefinition> busDefinition) const
{
	// The name must be non-empty.
	if (!hasValidName(port->getLogicalName()))
	{
		errors.append(QObject::tr("Port name %1 is invalid.").arg(port->getLogicalName()));
	}

	// Presence must be valid expression if defined.
    bool isValidPresence = false;
    expressionParser_->parseExpression(port->isPresent(), &isValidPresence);	
	if (isValidPresence == false)
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
			errors.append(QObject::tr("Default value '%1' for port %2 is invalid.").arg(wire->getDefaultValue(),
                port->getLogicalName()));
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
// Function: AbstractionDefinitionValidator::isValidTransactionalPort()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidTransactionalPort(QSharedPointer<TransactionalPort> transPort) const
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
    bool isValidBusWidth = false;
    expressionParser_->parseExpression(transPort->getBusWidth(), &isValidBusWidth);		
	if (isValidBusWidth == false)
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
// Function: AbstractionDefinitionValidator::findErrorsInTransactionalPort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInTransactionalPort(QVector<QString>& errors,
	QString const& context, QSharedPointer<TransactionalPort> transPort) const
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
		errors.append(QObject::tr("The transactional port bus width '%1' is invalid in %2.").arg(
            transPort->getBusWidth(), context));
	}

	QSharedPointer<Protocol> protocol = transPort->getProtocol();
	// Protocol must be valid if it exists.
	if (protocol)
	{
		protocolValidator_.findErrorsIn(errors, protocol, context);
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::isValidWirePort()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidWirePort(QSharedPointer<WirePort> wirePort, 
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
	// Width must be valid expression if defined.
    bool isValidWidth = false;
    expressionParser_->parseExpression(wirePort->getWidth(), &isValidWidth);
	if (isValidWidth == false)
	{
		return false;
	}

	// Any existing constraints must be valid.
	if (!validateConstraints(wirePort, ports))
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInWirePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInWirePort(QVector<QString>& errors,
	QSharedPointer<WirePort> wirePort, QString const& context,
	QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
	// Width must be valid expression if defined.
    bool isValidWidth = false;
    expressionParser_->parseExpression(wirePort->getWidth(), &isValidWidth);
	if (isValidWidth == false)
	{
		errors.append(QObject::tr("Wire port width '%1' is invalid in %2.").arg(wirePort->getWidth(), context));
	}

	// Any existing constraints must be valid.
	findErrorsInConstraints(errors, wirePort, context, ports);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::isValidPortQualifier()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidPortQualifier(QSharedPointer<Qualifier> qualifier,
    Document::Revision documentRevision) const
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
// Function: AbstractionDefinitionValidator::findErrorsInPortQualifier()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInPortQualifier(QVector<QString>& errors, 
    QSharedPointer<Qualifier> qualifier, QString const& context, 
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    QualifierValidator::findErrorsIn(errors, qualifier, context, abstraction->getRevision());

    if (qualifier->hasType(Qualifier::Type::PowerEnable))
    {
        if (!qualifier->getAttribute(Qualifier::Attribute::PowerDomainReference).isEmpty())
        {
            errors.append(QObject::tr("Illegal attribute powerDomainRef set for power enable qualifier "
                "of %1 within abstraction definition %2.").arg(context, abstraction->getVlnv().toString()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::validateConstraints()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::validateConstraints(QSharedPointer<WirePort> wirePort,
	QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
	// Validate timing constraints.
	TimingConstraintValidator timeConstraintValidator;

	QSharedPointer<TimingConstraint> timing = wirePort->getTimingConstraint();

	if (timing && !timeConstraintValidator.validate(timing, ports))
    {
        return false;
    }

	QSharedPointer<TimingConstraint> timingMirrored = wirePort->getMirroredTimingConstraint();

    if (timingMirrored && !timeConstraintValidator.validate(timingMirrored, ports))
    {
        return false;
    }

	// Validate driver constraints.
	CellSpecificationValidator cellConstraintValidator;

	QSharedPointer<CellSpecification> drive = wirePort->getDriveConstraint();

    if (drive && !cellConstraintValidator.validate(drive))
    {
        return false;
    }

	QSharedPointer<CellSpecification> driveMirrored = wirePort->getMirroredDriveConstraint();

	if (driveMirrored && !cellConstraintValidator.validate(driveMirrored))
    {
        return false;
    }

	// Validate load constraints.
	QSharedPointer<CellSpecification> load = wirePort->getLoadConstraint();

	if (load && !cellConstraintValidator.validate(load))
    {
        return false;
    }

	QSharedPointer<CellSpecification> loadMirrored = wirePort->getMirroredLoadConstraint();

    if (loadMirrored && !cellConstraintValidator.validate(loadMirrored))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInConstraints()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInConstraints(QVector<QString>& errors,
	QSharedPointer<WirePort> wirePort, QString const& context,
	QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{    
	// Validate timing constraints.
	TimingConstraintValidator timeConstraintValidator;

	QSharedPointer<TimingConstraint> timing = wirePort->getTimingConstraint();

	if (timing)
	{
		timeConstraintValidator.findErrorsIn(errors, timing, context, ports);
	}

	QSharedPointer<TimingConstraint> timingMirrored = wirePort->getMirroredTimingConstraint();

	if (timingMirrored)
	{
		timeConstraintValidator.findErrorsIn(errors, timingMirrored, context, ports);
	}

	// Validate driver constraints.
	CellSpecificationValidator cellConstVal;

	QSharedPointer<CellSpecification> drive = wirePort->getDriveConstraint();

	if (drive)
	{
		cellConstVal.findErrorsIn(errors, drive, context);
	}

	QSharedPointer<CellSpecification> driveMirrored = wirePort->getMirroredDriveConstraint();

	if (driveMirrored)
	{
		cellConstVal.findErrorsIn(errors, driveMirrored, context);
	}

	// Validate load constraints.
	QSharedPointer<CellSpecification> load = wirePort->getLoadConstraint();

	if (load)
	{
		cellConstVal.findErrorsIn(errors, load, context);
	}

	QSharedPointer<CellSpecification> loadMirrored = wirePort->getMirroredLoadConstraint();

	if (loadMirrored)
	{
		cellConstVal.findErrorsIn(errors, loadMirrored, context);
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression(QStringLiteral("^\\s*$"));

	if (name.isEmpty() || whiteSpaceExpression.match(name).hasMatch())
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getBusDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<const BusDefinition> AbstractionDefinitionValidator::getBusDefinition(
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<const Document> busDefinitionDocument = library_->getModelReadOnly(abstraction->getBusType());
    if (busDefinitionDocument)
    {
        QSharedPointer<const BusDefinition> busDefinition =
            busDefinitionDocument.dynamicCast<const BusDefinition>();

        return busDefinition;
    }

    return QSharedPointer<const BusDefinition>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInSystemGroup()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInSystemGroup(QVector<QString>& errors, QString const& systemGroup,
    QString const& context, QStringList const& availableSystemNames, QString const& busDefinitionIdentifier) const
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
// Function: AbstractionDefinitionValidator::hasValidExtendPorts()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidExtendPorts(QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<AbstractionDefinition> extendAbstraction = getExtendedAbstractionDefinition(abstraction);
    if (extendAbstraction)
    {
        for (auto portAbstraction : *abstraction->getLogicalPorts())
        {
            QSharedPointer<PortAbstraction> extendPort = getMatchingExtendPort(portAbstraction, extendAbstraction);
            if (extendPort &&
                (*portAbstraction->getQualifier() != *extendPort->getQualifier() ||
                portAbstraction->getMatch() != extendPort->getMatch() ||
                ((portAbstraction->hasWire() && !extendWirePortIsValid(portAbstraction, extendPort)) ||
                (portAbstraction->hasTransactional() &&
                    !extendTransactionalPortAbstractionIsValid(portAbstraction, extendPort)))))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::extendWirePortIsValid()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::extendWirePortIsValid(QSharedPointer<PortAbstraction> abstractionPort,
    QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasWire())
    {
        return false;
    }

    QSharedPointer<WireAbstraction> portWire = abstractionPort->getWire();
    QSharedPointer<WireAbstraction> extendWire = extendPort->getWire();

    if ((extendWire->getMasterPort() && (!portWire->getMasterPort() ||
        portWire->getMasterPort()->getDirection() != extendWire->getMasterPort()->getDirection())) ||
        (extendWire->getSlavePort() && (!portWire->getSlavePort() ||
            portWire->getSlavePort()->getDirection() != extendWire->getSlavePort()->getDirection())))
    {
        return false;
    }

    for (auto extendSystem : *extendWire->getSystemPorts())
    {
        QSharedPointer<WirePort> matchingSystem;
        for (auto portSystem : *portWire->getSystemPorts())
        {
            if (portSystem->getSystemGroup() == extendSystem->getSystemGroup())
            {
                matchingSystem = portSystem;
                break;
            }
        }

        if (!matchingSystem || extendSystem->getDirection() != matchingSystem->getDirection())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::extendTransactionalPortAbstractionIsValid()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::extendTransactionalPortAbstractionIsValid(
    QSharedPointer<PortAbstraction> abstractionPort, QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasTransactional())
    {
        return false;
    }

    QSharedPointer<TransactionalAbstraction> portTransactional = abstractionPort->getTransactional();
    QSharedPointer<TransactionalAbstraction> extendTransactional = extendPort->getTransactional();

    if (extendTransactional->getMasterPort() &&
        !extendTransactionalPortIsValid(portTransactional->getMasterPort(), extendTransactional->getMasterPort()))
    {
        return false;
    }

    if (extendTransactional->getSlavePort() &&
        !extendTransactionalPortIsValid(portTransactional->getSlavePort(), extendTransactional->getSlavePort()))
    {
        return false;
    }

    for (auto extendSystem : *extendTransactional->getSystemPorts())
    {
        QSharedPointer<TransactionalPort> portSystem;
        for (auto transactionalSystemPort : *portTransactional->getSystemPorts())
        {
            if (transactionalSystemPort->getSystemGroup() == extendSystem->getSystemGroup())
            {
                portSystem = transactionalSystemPort;
                break;
            }
        }

        if (!extendTransactionalPortIsValid(portSystem, extendSystem))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::extendTransactionalPortIsValid()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::extendTransactionalPortIsValid(QSharedPointer<TransactionalPort> port,
    QSharedPointer<TransactionalPort> extendPort) const
{
    if (!port)
    {
        return false;
    }

    QSharedPointer<Protocol> portProtocol = port->getProtocol();
    QSharedPointer<Protocol> extendPortProtocol = extendPort->getProtocol();

    return port->getInitiative() == extendPort->getInitiative() &&
        port->getKind() == extendPort->getKind() && port->getBusWidth() == extendPort->getBusWidth() &&
        ((!portProtocol && !extendPortProtocol) || (portProtocol && extendPortProtocol &&
        portProtocol->getProtocolType() == extendPortProtocol->getProtocolType() &&
            portProtocol->getPayloadName() == extendPortProtocol->getPayloadName() &&
            portProtocol->getPayloadType() == extendPortProtocol->getPayloadType() &&
            portProtocol->getPayloadExtension() == extendPortProtocol->getPayloadExtension()));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getMatchingExtendPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinitionValidator::getMatchingExtendPort(
    QSharedPointer<PortAbstraction> logicalPort, QSharedPointer<AbstractionDefinition> extendAbstraction) const
{
    for (auto extendPort : *extendAbstraction->getLogicalPorts())
    {
        if (extendPort->name() == logicalPort->name())
        {
            return extendPort;
        }
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidExtend()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidExtend(QSharedPointer<AbstractionDefinition> abstraction) const
{
    if (!abstraction->getExtends().isEmpty() && (!library_->contains(abstraction->getExtends()) ||
        !busTypeDefinesExtendedAbstractionBusType(abstraction)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::busTypeDefinesExtendedAbstractionBusType()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::busTypeDefinesExtendedAbstractionBusType(
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<AbstractionDefinition> extendAbstraction = getExtendedAbstractionDefinition(abstraction);
    if (!extendAbstraction)
    {
        return false;
    }

    return busTypeIsExtendedFromExtendBusType(getBusDefinition(abstraction), getBusDefinition(extendAbstraction));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getExtendedAbstractionDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionDefinition> AbstractionDefinitionValidator::getExtendedAbstractionDefinition(
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    if (abstraction->getExtends().isValid())
    {
        QSharedPointer<Document> extendDocument = library_->getModel(abstraction->getExtends());
        if (extendDocument)
        {
            QSharedPointer<AbstractionDefinition> extendAbstraction =
                extendDocument.dynamicCast<AbstractionDefinition>();
            if (extendAbstraction)
            {
                return extendAbstraction;
            }
        }
    }

    return QSharedPointer<AbstractionDefinition>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::busTypeIsExtendedFrom()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::busTypeIsExtendedFromExtendBusType(
    QSharedPointer<const BusDefinition> busType, QSharedPointer<const BusDefinition> extendBustype) const
{
    if (busType == extendBustype)
    {
        return true;
    }

    QSharedPointer<const BusDefinition> busTypeExtend = getBusTypeExtend(busType);
    if (busTypeExtend && busTypeIsExtendedFromExtendBusType(busTypeExtend, extendBustype))
    {
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getBusTypeExtend()
//-----------------------------------------------------------------------------
QSharedPointer<const BusDefinition> AbstractionDefinitionValidator::getBusTypeExtend(
    QSharedPointer<const BusDefinition> busType) const
{
    if (!busType->getExtends().isEmpty() && busType->getExtends().isValid() &&
        busType->getExtends().getType() == VLNV::BUSDEFINITION)
    {
        QSharedPointer<const Document> extendDocument = library_->getModelReadOnly(busType->getExtends());
        if (extendDocument)
        {
            QSharedPointer<const BusDefinition> extendBusType = extendDocument.dynamicCast<const BusDefinition>();
            if (extendBusType)
            {
                return extendBusType;
            }
        }
    }

    return QSharedPointer<const BusDefinition>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtend()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtend(QVector<QString>& errors, QString const& context,
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    VLNV extendVLNV = abstraction->getExtends();
    if (!extendVLNV.isEmpty())
    {
        if (!library_->contains(extendVLNV))
        {
            errors.append(QObject::tr("The bus definition %1 extended in %2 is not found in the library")
                .arg(extendVLNV.toString(), context));
        }
        else
        {
            if (!busTypeDefinesExtendedAbstractionBusType(abstraction))
            {
                errors.append(
                    QObject::tr("The bus definition %1 extended in %2 does not define extended abstraction definition"
                        " bus type").arg(abstraction->getBusType().toString(), context));
            }

            findErrorsInExtendPorts(errors, context, abstraction);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendPorts()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendPorts(QVector<QString>& errors, QString const& context,
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<AbstractionDefinition> extendAbstraction = getExtendedAbstractionDefinition(abstraction);
    if (extendAbstraction)
    {
        for (auto portAbstraction : *abstraction->getLogicalPorts())
        {
            QSharedPointer<PortAbstraction> extendPort = getMatchingExtendPort(portAbstraction, extendAbstraction);
            if (extendPort)
            {
                if (*portAbstraction->getQualifier() != *extendPort->getQualifier())
                {
                    errors.append(QObject::tr("The qualifier of extended port %1 in %2 cannot be edited")
                        .arg(extendPort->name(), context));
                }

                if (portAbstraction->getMatch() != extendPort->getMatch())
                {
                    errors.append(QObject::tr("Match of extended port %1 in %2 cannot be edited")
                        .arg(extendPort->name(), context));
                }

                if (portAbstraction->hasWire())
                {
                    findErrorsInExtendWireAbstraction(errors, context, portAbstraction, extendPort);
                }
                if (portAbstraction->hasTransactional())
                {
                    findErrorsInExtendTransactionalAbstraction(errors, context, portAbstraction, extendPort);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendWirePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendWireAbstraction(QVector<QString>& errors, QString const& context,
    QSharedPointer<PortAbstraction> port, QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasWire())
    {
        errors.append(QObject::tr("The extended port %1 in %2 is not wire").arg(extendPort->name(), context));
        return;
    }

    QSharedPointer<WireAbstraction> portWire = port->getWire();
    QSharedPointer<WireAbstraction> extendWire = extendPort->getWire();

    findErrorsInExtendWirePort(errors, context, portWire->getMasterPort(), extendWire->getMasterPort(),
        extendPort->name(), General::interfaceMode2Str(General::MASTER));
    findErrorsInExtendWirePort(errors, context, portWire->getSlavePort(), extendWire->getSlavePort(),
        extendPort->name(), General::interfaceMode2Str(General::SLAVE));

    for (auto extendSystem : *extendWire->getSystemPorts())
    {
        QSharedPointer<WirePort> matchingSystem;
        for (auto portSystem : *portWire->getSystemPorts())
        {
            if (portSystem->getSystemGroup() == extendSystem->getSystemGroup())
            {
                matchingSystem = portSystem;
                break;
            }
        }

        if (!matchingSystem)
        {
            errors.append(QObject::tr("The extended system %1 port %2 in %3 was not found")
                .arg(extendSystem->getSystemGroup(), extendPort->name(), context));
        }
        else if (matchingSystem->getDirection() != extendSystem->getDirection())
        {
            errors.append(QObject::tr("The direction of extended system %1 port %2 in %3 cannot be edited")
                .arg(extendSystem->getSystemGroup(), extendPort->name(), context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendWirePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendWirePort(QVector<QString>& errors, QString const& context,
    QSharedPointer<WirePort> port, QSharedPointer<WirePort> extendPort, QString const& portName,
    QString const& modeName) const
{
    if (extendPort)
    {
        if (!port)
        {
            errors.append(QObject::tr("The extended %1 port %2 in %3 was not found")
                .arg(modeName, portName, context));
        }
        else if (port->getDirection() != extendPort->getDirection())
        {
            errors.append(QObject::tr("The direction of extended %1 port %2 in %3 cannot be edited")
                .arg(modeName, portName, context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendTransactionalPort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendTransactionalAbstraction(QVector<QString>& errors,
    QString const& context, QSharedPointer<PortAbstraction> port, QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasTransactional())
    {
        errors.append(QObject::tr("The extended port %1 in %2 is not transactional")
            .arg(extendPort->name(), context));
        return;
    }

    QSharedPointer<TransactionalAbstraction> portTransactional = port->getTransactional();
    QSharedPointer<TransactionalAbstraction> extendTransactional = extendPort->getTransactional();

    QString masterContext = QObject::tr("extended %1 port %2 in %3")
        .arg(General::interfaceMode2Str(General::MASTER), port->name(), context);
    QString slaveContext = QObject::tr("extended %1 port %2 in %3")
        .arg(General::interfaceMode2Str(General::SLAVE), port->name(), context);

    findErrorsInExtendTransactionalPort(errors, masterContext, portTransactional->getMasterPort(),
        extendTransactional->getMasterPort());
    findErrorsInExtendTransactionalPort(errors, slaveContext, portTransactional->getSlavePort(),
        extendTransactional->getSlavePort());

    for (auto extendSystem : *extendTransactional->getSystemPorts())
    {
        QSharedPointer<TransactionalPort> portSystem;
        for (auto transactionalSystemPort : *portTransactional->getSystemPorts())
        {
            if (transactionalSystemPort->getSystemGroup() == extendSystem->getSystemGroup())
            {
                portSystem = transactionalSystemPort;
                break;
            }
        }

        QString systemContext = QObject::tr("extended %1 %2 port %3 in %4").arg(
            General::interfaceMode2Str(General::SYSTEM), portSystem->getSystemGroup(),  port->name(), context);
        findErrorsInExtendTransactionalPort(errors, systemContext, portSystem, extendSystem);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendTransactionalPort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendTransactionalPort(QVector<QString>& errors,
    QString const& context, QSharedPointer<TransactionalPort> port, QSharedPointer<TransactionalPort> extendPort)
    const
{
    if (!extendPort)
    {
        return;
    }

    if (!port)
    {
        errors.append(QObject::tr("The %1 was not found").arg(context));
    }

    if (port->getInitiative() != extendPort->getInitiative())
    {
        errors.append(QObject::tr("The initiative of %1 cannot be edited").arg(context));
    }
    if (port->getKind() != extendPort->getKind())
    {
        errors.append(QObject::tr("The kind of %1 cannot be edited").arg(context));
    }
    if (port->getBusWidth() != extendPort->getBusWidth())
    {
        errors.append(QObject::tr("The bus width of %1 cannot be edited").arg(context));
    }

    QSharedPointer<Protocol> portProtocol = port->getProtocol();
    QSharedPointer<Protocol> extendPortProtocol = extendPort->getProtocol();

    if ((portProtocol && !extendPortProtocol) || (!portProtocol && extendPortProtocol))
    {
        errors.append(QObject::tr("The %1 does not contain protocol data").arg(context));
    }
    else if (portProtocol && extendPortProtocol)
    {
        if (portProtocol->getProtocolType() != extendPortProtocol->getProtocolType() ||
            portProtocol->getProtocolType() == QLatin1String("custom") &&
            extendPortProtocol->getProtocolType() == QLatin1String("custom") &&
            portProtocol->getCustomProtocolType() != extendPortProtocol->getCustomProtocolType())
        {
            errors.append(QObject::tr("The protocol type of %1 cannot be edited").arg(context));
        }
        if (portProtocol->getPayloadName() != extendPortProtocol->getPayloadName())
        {
            errors.append(QObject::tr("The payload name of %1 cannot be edited").arg(context));
        }
        if (portProtocol->getPayloadType() != extendPortProtocol->getPayloadType())
        {
            errors.append(QObject::tr("The payload type of %1 cannot be edited").arg(context));
        }
        if (portProtocol->getPayloadExtension() != extendPortProtocol->getPayloadExtension())
        {
            errors.append(QObject::tr("The payload extension of %1 cannot be edited").arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidPortPackets()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidPortPackets(QSharedPointer<PortAbstraction> port, 
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    auto packets = port->getPackets();
    
    if (packets->isEmpty())
    {
        return true;
    }

    if (abstraction->getRevision() != Document::Revision::Std22)
    {
        return false;
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
// Function: AbstractionDefinitionValidator::findErrorsInPortPackets()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInPortPackets(QVector<QString>& errors, QString const& context,
    QSharedPointer<PortAbstraction> port, Document::Revision revision) const
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
// Function: AbstractionDefinitionValidator::hasValidChoices()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidChoices(QSharedPointer<AbstractionDefinition> abstraction) const
{
    auto choices = abstraction->getChoices();

    if (choices->isEmpty())
    {
        return true;
    }

    if (abstraction->getRevision() != Document::Revision::Std22)
    {
        return false;
    }

    ChoiceValidator choiceValidator(expressionParser_);

    for (auto const& choice : *choices)
    {
        if (!choiceValidator.validate(choice))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInChoices()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInChoices(QVector<QString>& errors, QString const& context,
    QSharedPointer<QList<QSharedPointer<Choice> > >choices) const
{
    ChoiceValidator choiceValidator(expressionParser_);

    for (auto const& choice : *choices)
    {
        choiceValidator.findErrorsIn(errors, choice, context);
    }
}
