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

	if ( !hasValidName( port->name() ) )
	{
		return false;
	}

	if ( !port->getIsPresent().isEmpty() && !parser->isValidExpression( port->getIsPresent() ) )
	{
		return false;
	}

	foreach ( QSharedPointer<Array> array, *port->getArrays() )
	{
		if ( !parser->isValidExpression( array->getLeft() ) ||
			!parser->isValidExpression( array->getRight() ) )
		{
			return false;
		}
	}

	if ( port->getWire() )
	{
		QSharedPointer<Wire> wire = port->getWire();

		if ( ( !wire->getVectorLeftBound().isEmpty() && !parser->isValidExpression( wire->getVectorLeftBound() ) ) ||
			( !wire->getVectorRightBound().isEmpty() && !parser->isValidExpression( wire->getVectorRightBound() ) ) )
		{
			return false;
		}

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

		if ( !TransactionalTypes::isIpXactInitiativeType( trans->getInitiative() ) )
		{
			return false;
		}

		if ( !trans->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType( trans->getKind() ) )
		{
			return false;
		}

		if ( !trans->getBusWidth().isEmpty() && !parser->isValidExpression( trans->getBusWidth() ) )
		{
			return false;
		}

		if ( !trans->getMaxConnections().isEmpty() && !parser->isValidExpression( trans->getMaxConnections() ) )
		{
			return false;
		}

		if ( !trans->getMinConnections().isEmpty() && !parser->isValidExpression( trans->getMinConnections() ) )
		{
			return false;
		}

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

		if ( protocol )
		{
			if ( protocol->getProtocolType() != "tlm" && protocol->getProtocolType() != "custom" )
			{
				return false;
			}

			if ( !protocol->getPayloadName().isEmpty() && !hasValidName( protocol->getPayloadName() ) )
			{
				return false;
			}

			if ( protocol->getPayloadType() != "generic" && protocol->getPayloadType() != "specific" )
			{
				return false;
			}

			if ( !protocol->getPayloadExtension().isEmpty() && protocol->getPayloadExtension() != "mandatory" &&
				protocol->getPayloadExtension() != "optional" )
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

	if ( !hasValidName( port->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(port->name()));
	}

	if ( !port->getIsPresent().isEmpty() && !parser->isValidExpression( port->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(port->getIsPresent()));
	}

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

		if ( !wire->getVectorLeftBound().isEmpty() && !parser->isValidExpression( wire->getVectorLeftBound() ) )
		{
			errors.append(QObject::tr("The left of vector is invalid: %1").arg(wire->getVectorLeftBound()));
		}

		if ( !wire->getVectorRightBound().isEmpty() && !parser->isValidExpression( wire->getVectorRightBound() ) )
		{
			errors.append(QObject::tr("The right of vector is invalid: %1").arg(wire->getVectorRightBound()));
		}

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

		if ( !TransactionalTypes::isIpXactInitiativeType( trans->getInitiative() ) )
		{
			errors.append(QObject::tr("The transactional initiative is invalid: %1").arg(trans->getInitiative()));
		}

		if ( !trans->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType( trans->getKind() ) )
		{
			errors.append(QObject::tr("The transactional kind is invalid: %1").arg(trans->getInitiative()));
		}

		if ( !trans->getBusWidth().isEmpty() && !parser->isValidExpression( trans->getBusWidth() ) )
		{
			errors.append(QObject::tr("The transactional bus width is invalid: %1").arg(trans->getBusWidth()));
		}

		if ( !trans->getMaxConnections().isEmpty() && !parser->isValidExpression( trans->getMaxConnections() ) )
		{
			errors.append(QObject::tr("The transactional max connections is invalid: %1").arg(trans->getMaxConnections()));
		}

		if ( !trans->getMinConnections().isEmpty() && !parser->isValidExpression( trans->getMinConnections() ) )
		{
			errors.append(QObject::tr("The transactional min connections is invalid: %1").arg(trans->getMinConnections()));
		}

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

		if ( protocol )
		{
			if ( protocol->getProtocolType() != "tlm" && protocol->getProtocolType() != "custom" )
			{
				errors.append(QObject::tr("The transactional protocol type is invalid: %1").arg(protocol->getProtocolType()));
			}

			if ( !protocol->getPayloadName().isEmpty() && !hasValidName( protocol->getPayloadName() ) )
			{
				errors.append(QObject::tr("The payload name is invalid: %1").arg(protocol->getPayloadName()));
			}

			if ( protocol->getPayloadType() != "generic" && protocol->getPayloadType() != "specific" )
			{
				errors.append(QObject::tr("The payload type is invalid: %1").arg(protocol->getPayloadType()));
			}

			if ( !protocol->getPayloadExtension().isEmpty() && protocol->getPayloadExtension() != "mandatory"
				&& protocol->getPayloadExtension() != "optional" )
			{
				errors.append(QObject::tr("The payload extension is invalid: %1").arg(protocol->getPayloadExtension()));
			}
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
