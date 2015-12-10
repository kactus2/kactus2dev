//-----------------------------------------------------------------------------
// Port: PortValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 25.11.2015
//
// Description:
// Validator for ipxact:Port.
//-----------------------------------------------------------------------------

#include "PortValidator.h"

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/ProtocolValidator.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: PortValidator::PortValidator()
//-----------------------------------------------------------------------------
PortValidator::PortValidator()
{

}

//-----------------------------------------------------------------------------
// Function: PortValidator::~PortValidator()
//-----------------------------------------------------------------------------
PortValidator::~PortValidator()
{

}

//-----------------------------------------------------------------------------
// Function: PortValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool PortValidator::validate(QSharedPointer<Port> port,
	QSharedPointer<QList<QSharedPointer<View> > > views) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	// Name must be valid.
	if ( !hasValidName( port->name() ) )
	{
		return false;
	}

	// Presence must be valid if defined.
	if ( !port->getIsPresent().isEmpty() && !parser->isValidExpression( port->getIsPresent() ) )
	{
		return false;
	}

	// Any arrays must have valid left and right.
	foreach ( QSharedPointer<Array> array, *port->getArrays() )
	{
		if ( !parser->isValidExpression( array->getLeft() ) || !parser->isValidExpression( array->getRight() ) )
		{
			return false;
		}
	}

	if ( port->getWire() )
	{
		QSharedPointer<Wire> wire = port->getWire();

		// Bounds must be valid if defined.
		if ( ( !wire->getVectorLeftBound().isEmpty() && !parser->isValidExpression( wire->getVectorLeftBound() ) ) ||
			( !wire->getVectorRightBound().isEmpty() && !parser->isValidExpression( wire->getVectorRightBound() ) ) )
		{
			return false;
		}

		// Any view reference must point to an existing view.
		foreach ( QSharedPointer<WireTypeDef> typeDef, *wire->getWireTypeDefs() )
		{
			foreach ( QString viewRef, typeDef->getViewRefs() )
			{
				if ( !referencedViewExists(views, viewRef) )
				{
					return false;
				}
			}
		}
	}

	if ( port->getTransactional() )
	{
		QSharedPointer<Transactional> trans = port->getTransactional();

		// There must a known initiative type.
		if ( !TransactionalTypes::isIpXactInitiativeType( trans->getInitiative() ) )
		{
			return false;
		}

		// If defined, kind must a known one.
		if ( !trans->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType( trans->getKind() ) )
		{
			return false;
		}

		// If defined, bus width must be a valid expression.
		if ( !trans->getBusWidth().isEmpty() && !parser->isValidExpression( trans->getBusWidth() ) )
		{
			return false;
		}

		// If defined, max connections width must be a valid expression.
		if ( !trans->getMaxConnections().isEmpty() && !parser->isValidExpression( trans->getMaxConnections() ) )
		{
			return false;
		}

		// If defined, min connections width must be a valid expression.
		if ( !trans->getMinConnections().isEmpty() && !parser->isValidExpression( trans->getMinConnections() ) )
		{
			return false;
		}

		// Any view reference must point to an existing view.
		foreach ( QSharedPointer<WireTypeDef> typeDef, *trans->getTransTypeDef() )
		{
			foreach ( QString viewRef, typeDef->getViewRefs() )
			{
				if ( !referencedViewExists(views, viewRef) )
				{
					return false;
				}
			}
		}

		QSharedPointer<Protocol> protocol = trans->getProtocol();

	// Protocol must be valid if it exists.
		if ( protocol )
		{
			ProtocolValidator protoVal;

			if ( !protoVal.validate(protocol) )
			{
				return false;
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PortValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<Port> port, QString const& contex,
	QSharedPointer<QList<QSharedPointer<View> > > views) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	// Name must be valid.
	if ( !hasValidName( port->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(port->name()));
	}

	// Presence must be valid if defined.
	if ( !port->getIsPresent().isEmpty() && !parser->isValidExpression( port->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(port->getIsPresent()));
	}

	// Any arrays must have valid left and right.
	foreach ( QSharedPointer<Array> array, *port->getArrays() )
	{
		if ( !parser->isValidExpression( array->getLeft() ) )
		{
			errors.append(QObject::tr("The left of array is invalid: %1").arg(array->getLeft()));
		}

		if ( !parser->isValidExpression( array->getRight() ) )
		{
			errors.append(QObject::tr("The right of array is invalid: %1").arg(array->getRight()));
		}
	}

	if ( port->getWire() )
	{
		QSharedPointer<Wire> wire = port->getWire();

		// Bounds must be valid if defined.
		if ( !wire->getVectorLeftBound().isEmpty() && !parser->isValidExpression( wire->getVectorLeftBound() ) )
		{
			errors.append(QObject::tr("The left of vector is invalid: %1").arg(wire->getVectorLeftBound()));
		}

		if ( !wire->getVectorRightBound().isEmpty() && !parser->isValidExpression( wire->getVectorRightBound() ) )
		{
			errors.append(QObject::tr("The right of vector is invalid: %1").arg(wire->getVectorRightBound()));
		}

		// Any view reference must point to an existing view.
		foreach ( QSharedPointer<WireTypeDef> typeDef, *wire->getWireTypeDefs() )
		{
			foreach ( QString viewRef, typeDef->getViewRefs() )
			{
				if ( !referencedViewExists(views, viewRef) )
				{
					errors.append(QObject::tr("A view reference does not exist: %1").arg(viewRef));
				}
			}
		}
	}

	if ( port->getTransactional() )
	{
		QSharedPointer<Transactional> trans = port->getTransactional();

		// There must a known initiative type.
		if ( !TransactionalTypes::isIpXactInitiativeType( trans->getInitiative() ) )
		{
			errors.append(QObject::tr("The transactional initiative is invalid: %1").arg(trans->getInitiative()));
		}

		// If defined, kind must a known one.
		if ( !trans->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType( trans->getKind() ) )
		{
			errors.append(QObject::tr("The transactional kind is invalid: %1").arg(trans->getInitiative()));
		}

		// If defined, bus width must be a valid expression.
		if ( !trans->getBusWidth().isEmpty() && !parser->isValidExpression( trans->getBusWidth() ) )
		{
			errors.append(QObject::tr("The transactional bus width is invalid: %1").arg(trans->getBusWidth()));
		}

		// If defined, max connections width must be a valid expression.
		if ( !trans->getMaxConnections().isEmpty() && !parser->isValidExpression( trans->getMaxConnections() ) )
		{
			errors.append(QObject::tr("The transactional max connections is invalid: %1").arg(trans->getMaxConnections()));
		}

		// If defined, min connections width must be a valid expression.
		if ( !trans->getMinConnections().isEmpty() && !parser->isValidExpression( trans->getMinConnections() ) )
		{
			errors.append(QObject::tr("The transactional min connections is invalid: %1").arg(trans->getMinConnections()));
		}

		// Any view reference must point to an existing view.
		foreach ( QSharedPointer<WireTypeDef> typeDef, *trans->getTransTypeDef() )
		{
			foreach ( QString viewRef, typeDef->getViewRefs() )
			{
				if ( !referencedViewExists(views, viewRef) )
				{
					errors.append(QObject::tr("A referenced view does not exist: %1").arg(viewRef));
				}
			}
		}

		QSharedPointer<Protocol> protocol = trans->getProtocol();

		// Protocol must be valid if it exists.
		if ( protocol )
		{
			ProtocolValidator protoVal;
			protoVal.findErrorsIn(errors,protocol,contex);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: PortValidator::referencedViewExists()
//-----------------------------------------------------------------------------
bool PortValidator::referencedViewExists(QSharedPointer<QList<QSharedPointer<View> > > views, QString viewRef) const
{
	foreach ( QSharedPointer<View> view, *views )
	{
		if ( view->name() == viewRef )
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidName()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidName(QString const& name) const
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
