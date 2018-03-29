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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/validators/ProtocolValidator.h>

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/Component/validators/PortTypeValidator.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: PortValidator::PortValidator()
//-----------------------------------------------------------------------------
PortValidator::PortValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<View> > > views):
expressionParser_(expressionParser),
typeValidator_(new PortTypeValidator(views))
{

}

//-----------------------------------------------------------------------------
// Function: PortValidator::~PortValidator()
//-----------------------------------------------------------------------------
PortValidator::~PortValidator()
{

}

//-----------------------------------------------------------------------------
// Function: PortValidator::getTypeValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PortTypeValidator> PortValidator::getTypeValidator() const
{
    return typeValidator_;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::componentChange()
//-----------------------------------------------------------------------------
void PortValidator::componentChange(QSharedPointer<QList<QSharedPointer<View> > > newViews)
{
    typeValidator_->componentChange(newViews);
}

//-----------------------------------------------------------------------------
// Function: PortValidator::validate()
//-----------------------------------------------------------------------------
bool PortValidator::validate(QSharedPointer<Port> port) const
{
    return hasValidName(port->name()) && hasValidIsPresent(port) && hasValidArrays(port) && hasValidWire(port) &&
        hasValidTransactionalPort(port) && (port->getTransactional() || port->getWire());
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidName()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidName(QString const& name) const
{
    QRegularExpression nameExpression;
    nameExpression.setPattern(QStringLiteral("^[a-zA-Z:_]+[a-zA-Z0-9:_.-]*$"));

    return nameExpression.match(name).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidIsPresent(QSharedPointer<Port> port) const
{
    if (!port->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(port->getIsPresent());

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
// Function: PortValidator::hasValidArrays()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidArrays(QSharedPointer<Port> port) const
{
    // Any arrays must have valid left and right.
    foreach ( QSharedPointer<Array> array, *port->getArrays() )
    {
        if (!arrayValueIsValid(array->getLeft()) || !arrayValueIsValid(array->getRight()))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::arrayValueIsValid()
//-----------------------------------------------------------------------------
bool PortValidator::arrayValueIsValid(QString const& arrayValue) const
{
    if (!arrayValue.isEmpty())
    {
        bool valueIsOk = false;
        int valueInt = expressionParser_->parseExpression(arrayValue, &valueIsOk).toInt();
         
        if (valueIsOk)
        {
            return valueInt >= 0;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidWire()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidWire(QSharedPointer<Port> port) const
{
    QSharedPointer<Wire> wire = port->getWire();

    if (wire)
    {
		// Bounds not defined, means they are ok.
        if (!wireHasValidDirection(wire))
        {
            return false;
        }

		if (!wire->getVectorLeftBound().isEmpty() || !wire->getVectorRightBound().isEmpty())
		{
			// Bounds must be valid if defined.
			if (!portBoundIsValid(wire->getVectorLeftBound()) || !portBoundIsValid(wire->getVectorRightBound()))
			{
				return false;
			}
		}

        if (!hasValidDefaultValue(port))
        {
            return false;
        }

        if (!hasValidTypeDefinitions(wire->getWireTypeDefs()))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidDefaultValue()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidDefaultValue(QSharedPointer<Port> port) const
{
    QSharedPointer<Wire> wire = port->getWire();

    if (!wire)
    {
        return true;
    }

    bool inDirection = wire->getDirection() == DirectionTypes::IN || wire->getDirection() == DirectionTypes::INOUT;
    return inDirection || wire->getDefaultDriverValue().isEmpty();        
}

//-----------------------------------------------------------------------------
// Function: PortValidator::wireHasValidDirection()
//-----------------------------------------------------------------------------
bool PortValidator::wireHasValidDirection(QSharedPointer<Wire> wire) const
{
    return wire->getDirection() != DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::portBoundIsValid()
//-----------------------------------------------------------------------------
bool PortValidator::portBoundIsValid(QString const& portBound) const
{
    if (!portBound.isEmpty())
    {
        bool isValidBound = false;
        bool canConvertToInt = false;
        int valueInt = expressionParser_->parseExpression(portBound, &isValidBound).toInt(&canConvertToInt);
        
        return isValidBound && canConvertToInt && valueInt >= 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidTransactionalPort()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidTransactionalPort(QSharedPointer<Port> port) const
{
    QSharedPointer<Transactional> trans = port->getTransactional();
    if (trans)
    {
        // There must a known initiative type.
        if (!TransactionalTypes::isIpXactInitiativeType(trans->getInitiative()))
        {
            return false;
        }

        // If defined, kind must a known one.
        if (!trans->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType(trans->getKind()))
        {
            return false;
        }

        // If defined, bus width must be a valid expression.
        bool isValidBusWidth = false;
        expressionParser_->parseExpression(trans->getBusWidth(), &isValidBusWidth);
        if (isValidBusWidth == false)
        {
            return false;
        }

        // If defined, max connections width must be a valid expression.
        bool isValidMaxConnections = false;
        expressionParser_->parseExpression(trans->getMaxConnections(), &isValidMaxConnections);
        if (isValidMaxConnections == false)
        {
            return false;
        }

        // If defined, min connections width must be a valid expression.
        bool isValidMinConnections = false;
        expressionParser_->parseExpression(trans->getMinConnections(), &isValidMinConnections);
        if (isValidMinConnections == false)
        {
            return false;
        }

        if (!hasValidTypeDefinitions(trans->getTransTypeDef()))
        {
            return false;
        }

        QSharedPointer<Protocol> protocol = trans->getProtocol();

        // Protocol must be valid if it exists.
        if (protocol)
        {
            ProtocolValidator protocolValidator;

            if (!protocolValidator.validate(protocol))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidTypeDefinitions()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidTypeDefinitions(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions)
    const
{
    foreach (QSharedPointer<WireTypeDef> singleTypeDefinition, *typeDefinitions)
    {
        if (!typeValidator_->validate(singleTypeDefinition, typeDefinitions))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PortValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Port> port, QString const& context) const
{
	// Name must be valid.
	if (!hasValidName(port->name()))
	{
        errors.append(QObject::tr("Invalid name set for port %1 within %2").arg(port->name()).arg(context));
	}

	// Presence must be valid if defined.
    if (!hasValidIsPresent(port))
	{
		errors.append(QObject::tr("The presence is invalid: %1 in port %2")
            .arg(port->getIsPresent()).arg(port->name()));
	}

	// Any arrays must have valid left and right.
	foreach ( QSharedPointer<Array> array, *port->getArrays() )
	{
        if (!arrayValueIsValid(array->getLeft()))
		{
			errors.append(QObject::tr("The left of array is invalid: %1 in port %2").arg(array->getLeft(), 
                port->name()));
		}
        if (!arrayValueIsValid(array->getRight()))
		{
			errors.append(QObject::tr("The right of array is invalid: %1 in port %2").arg(
                array->getRight(), port->name()));
		}
	}

	if (port->getWire())
	{
        findErrorsInWire(errors, port, context);
	}

	if (port->getTransactional())
	{
        findErrorsInTransactional(errors, port, context);
	}
}

//-----------------------------------------------------------------------------
// Function: PortValidator::findErrorsInWire()
//-----------------------------------------------------------------------------
void PortValidator::findErrorsInWire(QVector<QString>& errors, QSharedPointer<Port> port, QString const& context)
    const
{
    QSharedPointer<Wire> wire = port->getWire();

    if (wire)
    {
        if (!wire->getVectorLeftBound().isEmpty() || !wire->getVectorRightBound().isEmpty())
        {
            // Bounds must be valid.
            if (!portBoundIsValid(wire->getVectorLeftBound()))
            {
                errors.append(QObject::tr("The left of vector is invalid: %1 in port %2").arg(
                    wire->getVectorLeftBound(), port->name()));
            }

            if (!portBoundIsValid(wire->getVectorRightBound()))
            {
                errors.append(QObject::tr("The right of vector is invalid: %1 in port %2").arg(
                    wire->getVectorRightBound(), port->name()));
            }
        }

        if (!wireHasValidDirection(wire))
        {
            errors.append(QObject::tr("Invalid direction set for port %1 within %2").arg(port->name()).arg(context));
        }

        if (!hasValidDefaultValue(port))
        {
            errors.append(QObject::tr("Default value cannot be set for direction %1 for port %2 within %3.").arg(
                DirectionTypes::direction2Str(port->getDirection()), port->name(), context));
        }

        findErrorsInTypeDefinitions(errors, wire->getWireTypeDefs(), context);
    }
}

//-----------------------------------------------------------------------------
// Function: PortValidator::findErrorsInTransactional()
//-----------------------------------------------------------------------------
void PortValidator::findErrorsInTransactional(QVector<QString> &errors, QSharedPointer<Port> port, 
    QString const& context) const
{
    QSharedPointer<Transactional> trans = port->getTransactional();
    if (trans)
    {
        // There must a known initiative type.
        if (!TransactionalTypes::isIpXactInitiativeType( trans->getInitiative() ) )
        {
            errors.append(QObject::tr("The transactional initiative is invalid: %1 in port %2")
                .arg(trans->getInitiative()).arg(port->name()));
        }

        // If defined, kind must a known one.
        if ( !trans->getKind().isEmpty() && !TransactionalTypes::isIpXactKindType( trans->getKind() ) )
        {
            errors.append(QObject::tr("The transactional kind is invalid: %1 in port %2")
                .arg(trans->getInitiative()).arg(port->name()));
        }

        // If defined, bus width must be a valid expression.
        bool isValidBusWidth = false;
        expressionParser_->parseExpression(trans->getBusWidth(), &isValidBusWidth);
        if (isValidBusWidth == false)
        {
            errors.append(QObject::tr("The transactional bus width is invalid: %1 in port %2")
                .arg(trans->getBusWidth()).arg(port->name()));
        }

        // If defined, max connections width must be a valid expression.
        bool isValidMaxConnections = false;
        expressionParser_->parseExpression(trans->getMaxConnections(), &isValidMaxConnections);
        if (isValidMaxConnections == false)
        {
            errors.append(QObject::tr("The transactional max connections is invalid: %1 in port %2")
                .arg(trans->getMaxConnections()).arg(port->name()));
        }

        // If defined, min connections width must be a valid expression.
        bool isValidMinConnections = false;
        expressionParser_->parseExpression(trans->getMaxConnections(), &isValidMinConnections);
        if (isValidMinConnections == false)
        {
            errors.append(QObject::tr("The transactional min connections is invalid: %1 in port %2")
                .arg(trans->getMinConnections()).arg(port->name()));
        }

        findErrorsInTypeDefinitions(errors, trans->getTransTypeDef(), context);

        QSharedPointer<Protocol> protocol = trans->getProtocol();

        // Protocol must be valid if it exists.
        if (protocol)
        {
            ProtocolValidator protocolValidator;
            protocolValidator.findErrorsIn(errors, protocol, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortValidator::findErrorsInTypeDefinitions()
//-----------------------------------------------------------------------------
void PortValidator::findErrorsInTypeDefinitions(QVector<QString>& errors,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions, QString const& context) const
{
    foreach (QSharedPointer<WireTypeDef> singleTypeDefinition, *typeDefinitions)
    {
        typeValidator_->findErrorsIn(errors, context, singleTypeDefinition, typeDefinitions);
    }
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidTypes()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidTypes(QSharedPointer<Port> port) const
{
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions;

    if (port->getWire())
    {
        typeDefinitions = port->getWire()->getWireTypeDefs();
    }
    else if (port->getTransactional())
    {
        typeDefinitions = port->getTransactional()->getTransTypeDef();
    }

    if (typeDefinitions)
    {
        foreach (QSharedPointer<WireTypeDef> singleType, *typeDefinitions)
        {
            if (!typeValidator_->validate(singleType, typeDefinitions))
            {
                return false;
            }
        }
        
        return true;
    }

    return false;
}
