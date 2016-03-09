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

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/Choice.h>

#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/common/validators/CellSpecificationValidator.h>
#include <IPXACTmodels/common/validators/TimingConstraintValidator.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::AbstractionDefinitionValidator()
//-----------------------------------------------------------------------------
AbstractionDefinitionValidator::AbstractionDefinitionValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> expressionParser) : 
library_(library), 
    expressionParser_(expressionParser),
    parameterValidator_(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > > ()),
    protocolValidator_()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::~AbstractionDefinitionValidator()
//-----------------------------------------------------------------------------
AbstractionDefinitionValidator::~AbstractionDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::validate(QSharedPointer<AbstractionDefinition> abstractionDefinition) const
{
	// Must have a valid VLNV.
	if (!abstractionDefinition->getVlnv().isValid())
	{
		return false;
	}

	// The defined bus must exist in the library as a component.
	if (!library_->contains(abstractionDefinition->getBusType()))
	{
		return false;
	}

	// If this is an extension to another abstraction definition, it must exist.
	if (!abstractionDefinition->getExtends().isEmpty() && !library_->contains(abstractionDefinition->getExtends()))
	{
		return false;
	}

	// Any parameters must be valid.
	foreach (QSharedPointer<Parameter> parameter, *abstractionDefinition->getParameters())
	{
		if (!parameterValidator_.hasValidValue(parameter))
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
	foreach (QSharedPointer<PortAbstraction> portAbstraction, *abstractionDefinition->getLogicalPorts())
	{
        // Logical name must unique within the abstraction definition.
        if (logicalNames.contains(portAbstraction->getLogicalName()))
        {
            return false;
        }

		if (!isValidPortAbstraction(portAbstraction, abstractionDefinition->getLogicalPorts()))
		{
			return false;
		}

        // Logical name is valid: Mark it as used one!
        logicalNames.append(portAbstraction->getLogicalName());
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<AbstractionDefinition> abstractionDefinition) const
{
	// Must have a valid VLNV.
	if (!abstractionDefinition->getVlnv().isValid())
	{
		errors.append(QObject::tr("The VLNV %1 is invalid.").arg(abstractionDefinition->getVlnv().toString()));
	}

	// The defined bus must exist in the library as a component.
	if (!library_->contains(abstractionDefinition->getBusType()))
	{
		errors.append(QObject::tr("The bus definition %1 referenced in %2 is not found in the library.")
			.arg(abstractionDefinition->getBusType().toString(), abstractionDefinition->getVlnv().toString()));
	}

	// If this is an extension to another abstraction definition, it must exist.
	if (!abstractionDefinition->getExtends().isEmpty() && !library_->contains(abstractionDefinition->getExtends()))
	{
		errors.append(QObject::tr("The bus definition %1 extended in %2 is not found in the library").arg(
            abstractionDefinition->getExtends().toString(), abstractionDefinition->getVlnv().toString()));
	}

    QString context = QObject::tr("abstraction definition %1").arg(abstractionDefinition->getVlnv().toString());
	// Any parameters must be valid.
	foreach (QSharedPointer<Parameter> currentPara, *abstractionDefinition->getParameters())
	{
		parameterValidator_.findErrorsIn(errors, currentPara, context);
	}

	// Must have at least one logical port.
	if (abstractionDefinition->getLogicalPorts()->isEmpty())
	{
		errors.append(QObject::tr("Abstraction definition must define at least one port."));
	}

	// Collection of names of logical ports.
	QVector<QString> logicalNames;

	// Validate each logical port.
	foreach (QSharedPointer<PortAbstraction> port, *abstractionDefinition->getLogicalPorts())
    {	
        // Logical name must unique within the abstraction definition.
        if (logicalNames.contains(port->getLogicalName()))
        {
            errors.append(QObject::tr("Port name %1 is not unique within %2.").arg(port->getLogicalName(),
               context));
        }

		findErrorsInPortAbstraction(errors, port, abstractionDefinition->getLogicalPorts());

        logicalNames.append(port->getLogicalName());
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::isValidPortAbstraction()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidPortAbstraction(QSharedPointer<PortAbstraction> port,
	QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
	// The name must be non-empty.
	if (!hasValidName(port->getLogicalName()))
	{
		return false;
	}

	// Presence must be valid expression if defined.
	if (!port->isPresent().isEmpty() && !expressionParser_->isValidExpression(port->isPresent()))
	{
		return false;
	}

	QSharedPointer<WireAbstraction> wire = port->getWire();
	QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();

	// Must be wire XOR trans.
	if ((!wire && !transactional) || (wire && transactional))
	{
		return false;
	}

	if (wire)
	{
		// Default value must be valid expression if defined.
		if (!wire->getDefaultValue().isEmpty() && !expressionParser_->isValidExpression(wire->getDefaultValue()))
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
            foreach(QSharedPointer<WirePort> systemWirePort, *wire->getSystemPorts())
            {
                if (!isValidWirePort(systemWirePort, ports))
                {
                    return false;
                }
            }
        }
	}

	if (transactional)
	{
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
            foreach(QSharedPointer<TransactionalPort> transPort, *transactional->getSystemPorts())
            {
                if (!isValidTransactionalPort(transPort))
                {
                    return false;
                }
            }
        }
	}

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInPortAbstraction()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInPortAbstraction(QVector<QString>& errors, 
    QSharedPointer<PortAbstraction> port,
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
	// The name must be non-empty.
	if (!hasValidName(port->getLogicalName()))
	{
		errors.append(QObject::tr("Port name %1 is invalid.").arg(port->getLogicalName()));
	}

	// Presence must be valid expression if defined.
	if (!port->isPresent().isEmpty() && !expressionParser_->isValidExpression(port->isPresent()))
	{
		errors.append(QObject::tr("The presence '%1' of port %2 is invalid.")
			.arg(port->isPresent(), port->getLogicalName()));
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

	if (wire)
	{
		// Default value must be valid expression if defined.
		if (!wire->getDefaultValue().isEmpty() && !expressionParser_->isValidExpression(wire->getDefaultValue()))
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
            foreach(QSharedPointer<WirePort> wirePort, *wire->getSystemPorts())
            {
                findErrorsInWirePort(errors, wirePort, context, ports);
            }
        }
	}

	if (transactional)
	{
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
            foreach(QSharedPointer<TransactionalPort> transPort, *transactional->getSystemPorts())
            {
                findErrorsInTransactionalPort(errors, context, transPort);
            }
        }
	}
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
	if (!transPort->getBusWidth().isEmpty() && !expressionParser_->isValidExpression(transPort->getBusWidth()))
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
	if (!transPort->getBusWidth().isEmpty() && !expressionParser_->isValidExpression(transPort->getBusWidth()))
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
	if (!wirePort->getWidth().isEmpty() && !expressionParser_->isValidExpression(wirePort->getWidth()))
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
	if (!wirePort->getWidth().isEmpty() && !expressionParser_->isValidExpression(wirePort->getWidth()))
	{
		errors.append(QObject::tr("Wire port width '%1' is invalid in %2.").arg(wirePort->getWidth(), context));
	}

	// Any existing constraints must be valid.
	findErrorsInConstraints(errors, wirePort, context, ports);
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
	QRegularExpression whiteSpaceExpression("^\\s*$");

	if (name.isEmpty() || whiteSpaceExpression.match(name).hasMatch())
	{
		return false;
	}

	return true;
}
