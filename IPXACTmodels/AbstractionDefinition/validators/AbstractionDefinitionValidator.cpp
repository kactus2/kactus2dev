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

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>
#include <IPXACTmodels/common/ProtocolValidator.h>

#include <QRegularExpression>
#include <QStringList>
#include "../TransactionalAbstraction.h"
#include "../PortAbstraction.h"
#include "../WireAbstraction.h"
#include "../../common/DirectionTypes.h"
#include "../../common/TimingConstraintValidator.h"
#include "../../common/CellSpecificationValidator.h"

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::AbstractionDefinitionValidator()
//-----------------------------------------------------------------------------
AbstractionDefinitionValidator::AbstractionDefinitionValidator()
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
bool AbstractionDefinitionValidator::validate(QSharedPointer<AbstractionDefinition> abstractionDefinition,
	QSharedPointer<LibraryInterface> library) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	ParameterValidator2014 paraVali(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	// Must have a valid VLNV.
	if ( !abstractionDefinition->getVlnv().isValid() )
	{
		return false;
	}

	// The defined bus must exist in the library as a component.
	if ( !library->contains( abstractionDefinition->getBusType() ) )
	{
		return false;
	}

	// If this is an extension to another abstraction definition, it must exist.
	if ( !abstractionDefinition->getExtends().isEmpty() &&
		!library->contains( abstractionDefinition->getExtends() ) )
	{
		return false;
	}

	// Any parameters must be valid.
	foreach ( QSharedPointer<Parameter> currentPara, *abstractionDefinition->getParameters() )
	{
		if ( !paraVali.hasValidValue( currentPara.data() ) )
		{
			return false;
		}
	}

	// Must have at least one logical port.
	if ( abstractionDefinition->getLogicalPorts()->size() < 1 )
	{
		return false;
	}

	// Collection of names of logical ports.
	QSet<QString> logicalNames;

	// Validate each logical port.
	foreach ( QSharedPointer<PortAbstraction> port, *abstractionDefinition->getLogicalPorts() )
	{
		if ( !isValidPortAbstraction(port, parser, logicalNames) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<AbstractionDefinition> abstractionDefinition, QString const& context,
	QSharedPointer<LibraryInterface> library) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	ParameterValidator2014 paraVali(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	// Must have a valid VLNV.
	if ( !abstractionDefinition->getVlnv().isValid() )
	{
		errors.append(QObject::tr("The VLNV is invalid or in-existing: %1")
			.arg(abstractionDefinition->getVlnv().toString()));
	}

	// The defined bus must exist in the library as a component.
	if ( !library->contains( abstractionDefinition->getBusType() ) )
	{
		errors.append(QObject::tr("The referenced bus definition does not exist: %1")
			.arg(abstractionDefinition->getBusType().toString()));
	}

	// If this is an extension to another abstraction definition, it must exist.
	if ( !abstractionDefinition->getExtends().isEmpty() &&
		!library->contains( abstractionDefinition->getExtends() ) )
	{
		errors.append(QObject::tr("The defined extension does not exist: %1")
			.arg(abstractionDefinition->getExtends().toString()));
	}

	// Any parameters must be valid.
	foreach ( QSharedPointer<Parameter> currentPara, *abstractionDefinition->getParameters() )
	{
		paraVali.findErrorsIn( errors, currentPara, context );
	}

	// Must have at least one logical port.
	if ( abstractionDefinition->getLogicalPorts()->size() < 1 )
	{
		errors.append(QObject::tr("Must have at least one port.") );
	}

	// Collection of names of logical ports.
	QSet<QString> logicalNames;

	// Validate each logical port.
	foreach ( QSharedPointer<PortAbstraction> port, *abstractionDefinition->getLogicalPorts() )
	{
		findErrorsInPortAbstraction(errors, context, port,parser,logicalNames);
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::isValidPortAbstraction()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidPortAbstraction(QSharedPointer<PortAbstraction> port,
	QSharedPointer<ExpressionParser> parser, QSet<QString>& logicalNames) const
{
	// The name must be non-empty.
	if ( !hasValidName( port->getLogicalName() ) )
	{
		return false;
	}

	// Logical name must unique within the abstraction definition.
	if ( logicalNames.contains( port->getLogicalName() ) )
	{
		return false;
	}

	// Logical name is valid: Mark it as used one!
	logicalNames.insert( port->getLogicalName() );

	// Presence must be valid expression if defined.
	if ( !port->isPresent().isEmpty() && !parser->isValidExpression( port->isPresent() ) )
	{
		return false;
	}

	QSharedPointer<WireAbstraction> wire = port->getWire();
	QSharedPointer<TransactionalAbstraction> trans = port->getTransactional();

	// Must be wire XOR trans.
	if ( ( !wire && !trans ) || ( wire && trans ) )
	{
		return false;
	}

	if ( wire )
	{
		// Default value must be valid expression if defined.
		if ( !wire->getDefaultValue().isEmpty() && !parser->isValidExpression( wire->getDefaultValue() ) )
		{
			return false;
		}

		// Master and slave ports must be valid if exist.
		if ( !isValidWirePort(wire->getMasterPort(), parser, logicalNames) || 
			!isValidWirePort(wire->getSlavePort(), parser, logicalNames) )
		{
			return false;
		}

		// System ports must be valid if exist.
		foreach( QSharedPointer<WirePort> wirePort, *wire->getSystemPorts() )
		{
			if ( !isValidWirePort(wirePort, parser, logicalNames) )
			{
				return false;
			}
		}
	}

	if ( trans )
	{
		// Master and slave ports must be valid if exist.
		if ( !isValidTransactionalPort(trans->getMasterPort(), parser) ||
			!isValidTransactionalPort(trans->getSlavePort(), parser) )
		{
			return false;
		}

		// System ports must be valid if exist.
		foreach( QSharedPointer<TransactionalPort> transPort, *trans->getSystemPorts() )
		{
			if ( !isValidTransactionalPort(transPort, parser) )
			{
				return false;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInPortAbstraction()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInPortAbstraction(QVector<QString>& errors,
	QString const& context, QSharedPointer<PortAbstraction> port, QSharedPointer<ExpressionParser> parser,
	QSet<QString>& logicalNames) const
{
	// The name must be non-empty.
	if ( !hasValidName( port->getLogicalName() ) )
	{
		errors.append(QObject::tr("A port name is invalid or in-existing: %1").arg(port->getLogicalName()));
	}

	// Logical name must unique within the abstraction definition.
	if ( logicalNames.contains( port->getLogicalName() ) )
	{
		errors.append(QObject::tr("A port name occurs more than once within the definition: %1").arg(port->getLogicalName()));
	}

	// Logical name is valid: Mark it as used one!
	logicalNames.insert( port->getLogicalName() );

	// Presence must be valid expression if defined.
	if ( !port->isPresent().isEmpty() && !parser->isValidExpression( port->isPresent() ) )
	{
		errors.append(QObject::tr("The presence of port %1 is invalid: %2")
			.arg(port->getLogicalName()).arg(port->isPresent()));
	}

	QSharedPointer<WireAbstraction> wire = port->getWire();
	QSharedPointer<TransactionalAbstraction> trans = port->getTransactional();

	// Must be wire XOR trans.
	if ( !wire && !trans )
	{
		errors.append(QObject::tr("Port %1 as neither wire nor transactional.").arg(port->getLogicalName()));
	}

	if ( wire && trans )
	{
		errors.append(QObject::tr("Port %1 has both wire and transactional.").arg(port->getLogicalName()));
	}

	if ( wire )
	{
		// Default value must be valid expression if defined.
		if ( !wire->getDefaultValue().isEmpty() && !parser->isValidExpression( wire->getDefaultValue() ) )
		{
			errors.append(QObject::tr("Port %1 has invalid default value: %1").arg(wire->getDefaultValue()));
		}

		// Master and slave ports must be valid if exist.
		findErrorsInWirePort(errors, wire->getMasterPort(), context, parser, logicalNames);
		findErrorsInWirePort(errors, wire->getSlavePort(), context, parser, logicalNames);

		// System ports must be valid if exist.
		foreach( QSharedPointer<WirePort> wirePort, *wire->getSystemPorts() )
		{
			findErrorsInWirePort(errors, wirePort, context, parser, logicalNames);
		}
	}

	if ( trans )
	{
		// Master and slave ports must be valid if exist.
		findErrorsInTransactionalPort(errors, context, trans->getMasterPort(), parser);
		findErrorsInTransactionalPort(errors, context, trans->getSlavePort(), parser);

		// System ports must be valid if exist.
		foreach( QSharedPointer<TransactionalPort> transPort, *trans->getSystemPorts() )
		{
			findErrorsInTransactionalPort(errors, context, transPort, parser);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::isValidTransactionalPort()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidTransactionalPort
	(QSharedPointer<TransactionalPort> transPort, QSharedPointer<ExpressionParser> parser) const
{
	// Since this is an optional element, it may be excluded.
	if ( !transPort )
	{
		return true;
	}

	PresenceTypes::Presence pre = transPort->getPresence();

	// These are the valid values for presence.
	if ( pre != PresenceTypes::ILLEGAL && pre != PresenceTypes::OPTIONAL
		&& pre != PresenceTypes::REQUIRED )
	{
		return false;
	}

	// Initiative must be an existing one.
	if ( !TransactionalTypes::isIpXactInitiativeType( transPort->getInitiative() ) )
	{
		return false;
	}

	// Kind must be an existing one.
	if ( !TransactionalTypes::isIpXactKindType( transPort->getKind() ) )
	{
		return false;
	}

	// Bus width must be valid expression if defined.
	if ( !transPort->getBusWidth().isEmpty() && !parser->isValidExpression( transPort->getBusWidth() ) )
	{
		return false;
	}

	QSharedPointer<Protocol> protocol = transPort->getProtocol();

	// Protocol must be valid if it exists.
	if ( protocol )
	{
		ProtocolValidator protoVal;

		if ( !protoVal.validate(protocol) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInTransactionalPort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInTransactionalPort(QVector<QString>& errors,
	QString const& context, QSharedPointer<TransactionalPort> transPort,
	QSharedPointer<ExpressionParser> parser) const
{
	// Since this is an optional element, it may be excluded.
	if ( !transPort )
	{
		return;
	}

	PresenceTypes::Presence pre = transPort->getPresence();

	// These are the valid values for presence.
	if ( pre != PresenceTypes::ILLEGAL && pre != PresenceTypes::OPTIONAL
		&& pre != PresenceTypes::REQUIRED )
	{
		errors.append(QObject::tr("Presence of transactional port is invalid: %1").arg(PresenceTypes::presence2Str(pre)));
	}

	// Initiative must be an existing one.
	if ( !TransactionalTypes::isIpXactInitiativeType( transPort->getInitiative() ) )
	{
		errors.append(QObject::tr("The transactional port initiative is invalid: %1")
			.arg(transPort->getInitiative()));
	}

	// Kind must be an existing one.
	if ( !TransactionalTypes::isIpXactKindType( transPort->getKind() ) )
	{
		errors.append(QObject::tr("The transactional port kind is invalid: %1")
			.arg(transPort->getInitiative()));
	}

	// Bus width must be valid expression if defined.
	if ( !transPort->getBusWidth().isEmpty() && !parser->isValidExpression( transPort->getBusWidth() ) )
	{
		errors.append(QObject::tr("The transactional port bus width is invalid: %1").arg(transPort->getBusWidth()));
	}

	QSharedPointer<Protocol> protocol = transPort->getProtocol();

	// Protocol must be valid if it exists.
	if ( protocol )
	{
		ProtocolValidator protoVal;

		protoVal.findErrorsIn(errors,protocol,context);
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::isValidWirePort()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::isValidWirePort(QSharedPointer<WirePort> wirePort,
	QSharedPointer<ExpressionParser> parser, QSet<QString>& logicalNames) const
{
	// Since this is an optional element, it may be excluded.
	if ( !wirePort )
	{
		return true;
	}

	PresenceTypes::Presence pre = wirePort->getPresence();

	// These are the valid values for presence.
	if ( pre != PresenceTypes::ILLEGAL && pre != PresenceTypes::OPTIONAL
		&& pre != PresenceTypes::REQUIRED )
	{
		return false;
	}

	// Width must be valid expression if defined.
	if ( !wirePort->getWidth().isEmpty() && !parser->isValidExpression( wirePort->getWidth() ) )
	{
		return false;
	}

	DirectionTypes::Direction dir = wirePort->getDirection();

	// These are the valid values for direction.
	if ( dir != DirectionTypes::IN && dir != DirectionTypes::OUT && dir != DirectionTypes::INOUT )
	{
		return false;
	}

	// Any existing constraints must be valid.
	if ( !validateConstraints(wirePort, logicalNames) )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInWirePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInWirePort(QVector<QString>& errors,
	QSharedPointer<WirePort> wirePort, QString const& context, QSharedPointer<ExpressionParser> parser,
	QSet<QString>& logicalNames) const
{
	// Since this is an optional element, it may be excluded.
	if ( !wirePort )
	{
		return;
	}

	PresenceTypes::Presence pre = wirePort->getPresence();

	// These are the valid values for presence.
	if ( pre != PresenceTypes::ILLEGAL && pre != PresenceTypes::OPTIONAL && pre != PresenceTypes::REQUIRED )
	{
		errors.append(QObject::tr("Presence of wire port is invalid: %1").arg(PresenceTypes::presence2Str(pre)));
	}

	// Width must be valid expression if defined.
	if ( !wirePort->getWidth().isEmpty() && !parser->isValidExpression( wirePort->getWidth() ) )
	{
		errors.append(QObject::tr("Width of wire port is invalid: %1").arg(wirePort->getWidth()));
	}

	DirectionTypes::Direction dir = wirePort->getDirection();

	// These are the valid values for direction.
	if ( dir != DirectionTypes::IN && dir != DirectionTypes::OUT && dir != DirectionTypes::INOUT )
	{
		errors.append(QObject::tr("Direction of wire port is invalid: %1").arg(DirectionTypes::direction2Str(dir)));
	}

	// Any existing constraints must be valid.
	findErrorsInConstraints(errors,wirePort,context,logicalNames);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::validateConstraints()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::validateConstraints(QSharedPointer<WirePort> wirePort, QSet<QString>& logicalNames) const
{
	// Validate timing constraints.
	TimingConstraintValidator timeConstVal;

	QSharedPointer<TimingConstraint> timing = wirePort->getTimingConstraint();

	if ( timing )
	{
		if ( !timeConstVal.validate(timing,logicalNames) )
		{
			return false;
		}
	}

	QSharedPointer<TimingConstraint> timingMirrored = wirePort->getMirroredTimingConstraint();

	if ( timingMirrored )
	{
		if ( !timeConstVal.validate(timingMirrored,logicalNames) )
		{
			return false;
		}
	}

	// Validate driver constraints.
	CellSpecificationValidator cellConstVal;

	QSharedPointer<CellSpecification> drive = wirePort->getDriveConstraint();

	if ( drive )
	{
		if ( !cellConstVal.validate(drive) )
		{
			return false;
		}
	}

	QSharedPointer<CellSpecification> driveMirrored = wirePort->getMirroredDriveConstraint();

	if ( driveMirrored )
	{
		if ( !cellConstVal.validate(driveMirrored) )
		{
			return false;
		}
	}

	// Validate load constraints.
	QSharedPointer<CellSpecification> load = wirePort->getLoadConstraint();

	if ( load )
	{
		if ( !cellConstVal.validate(load) )
		{
			return false;
		}
	}

	QSharedPointer<CellSpecification> loadMirrored = wirePort->getMirroredLoadConstraint();

	if ( loadMirrored )
	{
		if ( !cellConstVal.validate(loadMirrored) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInConstraints()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInConstraints(QVector<QString>& errors,
	QSharedPointer<WirePort> wirePort, QString const& context, QSet<QString>& logicalNames) const
{
	// Validate timing constraints.
	TimingConstraintValidator timeConstVal;

	QSharedPointer<TimingConstraint> timing = wirePort->getTimingConstraint();

	if ( timing )
	{
		timeConstVal.findErrorsIn(errors,timing,context,logicalNames);
	}

	QSharedPointer<TimingConstraint> timingMirrored = wirePort->getMirroredTimingConstraint();

	if ( timingMirrored )
	{
		timeConstVal.findErrorsIn(errors,timingMirrored,context,logicalNames);
	}

	// Validate driver constraints.
	CellSpecificationValidator cellConstVal;

	QSharedPointer<CellSpecification> drive = wirePort->getDriveConstraint();

	if ( drive )
	{
		cellConstVal.findErrorsIn(errors,drive,context);
	}

	QSharedPointer<CellSpecification> driveMirrored = wirePort->getMirroredDriveConstraint();

	if ( driveMirrored )
	{
		cellConstVal.findErrorsIn(errors,driveMirrored,context);
	}

	// Validate load constraints.
	QSharedPointer<CellSpecification> load = wirePort->getLoadConstraint();

	if ( load )
	{
		cellConstVal.findErrorsIn(errors,load,context);
	}

	QSharedPointer<CellSpecification> loadMirrored = wirePort->getMirroredLoadConstraint();

	if ( loadMirrored )
	{
		cellConstVal.findErrorsIn(errors,loadMirrored,context);
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression;
	whiteSpaceExpression.setPattern("^\\s*$");
	QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

	if (name.isEmpty() || whiteSpaceMatch.hasMatch())
	{
		return false;
	}

	return true;
}