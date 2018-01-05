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

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: PortValidator::PortValidator()
//-----------------------------------------------------------------------------
PortValidator::PortValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<View> > > views):
expressionParser_(expressionParser),
availableViews_(views)
{

}

//-----------------------------------------------------------------------------
// Function: PortValidator::~PortValidator()
//-----------------------------------------------------------------------------
PortValidator::~PortValidator()
{

}

//-----------------------------------------------------------------------------
// Function: PortValidator::componentChange()
//-----------------------------------------------------------------------------
void PortValidator::componentChange(QSharedPointer<QList<QSharedPointer<View> > > newViews)
{
    availableViews_ = newViews;
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
        bool valueIsOk = expressionParser_->isValidExpression(arrayValue);
        if (valueIsOk)
        {
            int valueInt = expressionParser_->parseExpression(arrayValue).toInt();
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

        // Any view reference must point to an existing view.
        foreach (QSharedPointer<WireTypeDef> typeDef, *wire->getWireTypeDefs())
        {
            foreach (QString const& viewRef, *typeDef->getViewRefs())
            {
                if (!referencedViewExists(viewRef))
                {
                    return false;
                }
            }
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
        if (expressionParser_->isValidExpression(portBound))
        {
            bool canConvertToInt = true;
            int valueInt = expressionParser_->parseExpression(portBound).toInt(&canConvertToInt);
            return canConvertToInt && valueInt >= 0;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::referencedViewExists()
//-----------------------------------------------------------------------------
bool PortValidator::referencedViewExists(QString const& viewRef) const
{
    foreach (QSharedPointer<View> view, *availableViews_)
    {
        if (view->name() == viewRef)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortValidator::hasValidTransactionalPort()
//-----------------------------------------------------------------------------
bool PortValidator::hasValidTransactionalPort(QSharedPointer<Port> port) const
{
    if (port->getTransactional())
    {
        QSharedPointer<Transactional> trans = port->getTransactional();

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
        if (!trans->getBusWidth().isEmpty() && !expressionParser_->isValidExpression(trans->getBusWidth()))
        {
            return false;
        }

        // If defined, max connections width must be a valid expression.
        if (!trans->getMaxConnections().isEmpty() && !expressionParser_->isValidExpression(trans->getMaxConnections()))
        {
            return false;
        }

        // If defined, min connections width must be a valid expression.
        if (!trans->getMinConnections().isEmpty() && !expressionParser_->isValidExpression(trans->getMinConnections()))
        {
            return false;
        }

        // Any view reference must point to an existing view.
        foreach (QSharedPointer<WireTypeDef> typeDef, *trans->getTransTypeDef())
        {
            foreach (QString viewRef, *typeDef->getViewRefs())
            {
                if (!referencedViewExists(viewRef))
                {
                    return false;
                }
            }
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

    // Any view reference must point to an existing view.
    foreach (QSharedPointer<WireTypeDef> typeDef, *wire->getWireTypeDefs())
    {
        foreach (QString viewRef, *typeDef->getViewRefs())
        {
            if (!referencedViewExists(viewRef))
            {
                errors.append(QObject::tr("A view reference does not exist: %1 in port %2").arg(viewRef,
                    port->name()));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortValidator::findErrorsInTransactional()
//-----------------------------------------------------------------------------
void PortValidator::findErrorsInTransactional(QVector<QString> &errors, QSharedPointer<Port> port, 
    QString const& context) const
{
    QSharedPointer<Transactional> trans = port->getTransactional();

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
    if ( !trans->getBusWidth().isEmpty() && !expressionParser_->isValidExpression( trans->getBusWidth() ) )
    {
        errors.append(QObject::tr("The transactional bus width is invalid: %1 in port %2")
            .arg(trans->getBusWidth()).arg(port->name()));
    }

    // If defined, max connections width must be a valid expression.
    if ( !trans->getMaxConnections().isEmpty() && !expressionParser_->isValidExpression( trans->getMaxConnections() ) )
    {
        errors.append(QObject::tr("The transactional max connections is invalid: %1 in port %2")
            .arg(trans->getMaxConnections()).arg(port->name()));
    }

    // If defined, min connections width must be a valid expression.
    if ( !trans->getMinConnections().isEmpty() && !expressionParser_->isValidExpression( trans->getMinConnections() ) )
    {
        errors.append(QObject::tr("The transactional min connections is invalid: %1 in port %2")
            .arg(trans->getMinConnections()).arg(port->name()));
    }

    // Any view reference must point to an existing view.
    foreach (QSharedPointer<WireTypeDef> typeDef, *trans->getTransTypeDef())
    {
        foreach (QString viewRef, *typeDef->getViewRefs())
        {
            if (!referencedViewExists(viewRef))
            {
                errors.append(QObject::tr("A referenced view does not exist: %1 in port %2").arg(viewRef,
                    port->name()));
            }
        }
    }

    QSharedPointer<Protocol> protocol = trans->getProtocol();

    // Protocol must be valid if it exists.
    if (protocol)
    {
        ProtocolValidator protocolValidator;
        protocolValidator.findErrorsIn(errors, protocol, context);
    }
}

