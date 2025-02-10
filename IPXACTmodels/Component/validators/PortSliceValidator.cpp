//-----------------------------------------------------------------------------
// File: PortSliceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 17.08.2023
//
// Description:
// Validator for ipxact:portSlice.
//-----------------------------------------------------------------------------

#include "PortSliceValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/PortSlice.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::PortSliceValidator()
//-----------------------------------------------------------------------------
PortSliceValidator::PortSliceValidator(QSharedPointer<Component> component, 
	QSharedPointer<ExpressionParser> expressionParser):
	component_(component),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::~PortSliceValidator()
//-----------------------------------------------------------------------------
PortSliceValidator::~PortSliceValidator()
{

}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::validate()
//-----------------------------------------------------------------------------
bool PortSliceValidator::validate(QSharedPointer<PortSlice> portSlice) const
{
    if (hasValidName(portSlice->name()) == false)
    {
        return false;
    }

    if (hasValidPortReference(portSlice) == false)
    {
        return false;
    }

    auto physicalPort = component_->getPort(portSlice->getPortRef());
    auto leftBound = expressionParser_->parseExpression(physicalPort->getLeftBound()).toInt();
    auto rightBound = expressionParser_->parseExpression(physicalPort->getRightBound()).toInt();

    if (isInValidRange(portSlice->getLeftRange(), leftBound, rightBound) == false)
    {
        return false;
    }

    if (isInValidRange(portSlice->getRightRange(), leftBound, rightBound) == false)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::hasValidLeftRange()
//-----------------------------------------------------------------------------
bool PortSliceValidator::hasValidLeftRange(QSharedPointer<PortSlice> portSlice) const
{
    auto physicalPort = component_->getPort(portSlice->getPortRef());
    if (physicalPort.isNull())
    {
        return false;
    }

    auto leftBound = expressionParser_->parseExpression(physicalPort->getLeftBound()).toInt();
    auto rightBound = expressionParser_->parseExpression(physicalPort->getRightBound()).toInt();

    return isInValidRange(portSlice->getLeftRange(), leftBound, rightBound);
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::hasValidRightRange()
//-----------------------------------------------------------------------------
bool PortSliceValidator::hasValidRightRange(QSharedPointer<PortSlice> portSlice) const
{
    auto physicalPort = component_->getPort(portSlice->getPortRef());
    if (physicalPort.isNull())
    {
        return false;
    }

    auto leftBound = expressionParser_->parseExpression(physicalPort->getLeftBound()).toInt();
    auto rightBound = expressionParser_->parseExpression(physicalPort->getRightBound()).toInt();

    return isInValidRange(portSlice->getRightRange(), leftBound, rightBound);
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::hasValidPortReference()
//-----------------------------------------------------------------------------
bool PortSliceValidator::hasValidPortReference(QSharedPointer<PortSlice> portSlice) const
{
    auto physicalPort = component_->getPort(portSlice->getPortRef());
    return physicalPort.isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool PortSliceValidator::hasValidName(QString const& name) const
{
	return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PortSliceValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<PortSlice> portSlice, QString const& context) const
{
    if (hasValidName(portSlice->name()) == false)
    {
        errors.append(QObject::tr("Invalid name '%1' set for port condition within %2.").arg(
            portSlice->name(), context));
    }

    if (portSlice->getPortRef().isEmpty())
    {
        errors.append(QObject::tr("No port reference set for '%1' within %2.").arg(
            portSlice->name(), context));
    }
    else if (component_ && component_->hasPort(portSlice->getPortRef()) == false)
    {
        errors.append(QObject::tr(
            "Port '%1' in port condition '%2' in %3 could not be found in the component.").arg(
                portSlice->getPortRef(), portSlice->name(), context));
    }

    auto physicalPort = component_->getPort(portSlice->getPortRef());

    if (physicalPort)
    {
        auto leftBound = expressionParser_->parseExpression(physicalPort->getLeftBound()).toInt();
        auto rightBound = expressionParser_->parseExpression(physicalPort->getRightBound()).toInt();

        bool leftOk = false;
        bool rightOk = false;
        auto leftRange = expressionParser_->parseExpression(portSlice->getLeftRange(), &leftOk).toInt();
        auto rightRange = expressionParser_->parseExpression(portSlice->getRightRange(), &rightOk).toInt();

        if (leftOk == false)
        {
            errors.append(QObject::tr("Left range in port condition '%1' is not a valid expression in %2.").arg(
                portSlice->name(), context));
        }
        else if (!(leftBound <= leftRange && leftRange <= rightBound))
        {
            errors.append(QObject::tr("Range in port condition '%1' is outside the bounds of port '%2' in %3.").arg(
                portSlice->name(), portSlice->getPortRef(), context));
        }

        if (rightOk == false)
        {
            errors.append(QObject::tr("Right range in port condition '%1' is not a valid expression in %3.").arg(
                portSlice->name(),  context));
        }
        else if (!(leftBound <= rightRange && rightRange <= rightBound))
        {
            errors.append(QObject::tr("Range in port condition '%1' is outside the bounds of port '%2' in %3.").arg(
                portSlice->name(), portSlice->getPortRef(), context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::componentChange()
//-----------------------------------------------------------------------------
void PortSliceValidator::componentChange(QSharedPointer<Component> newComponent)
{
	component_ = newComponent;
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::setExpressionParser()
//-----------------------------------------------------------------------------
void PortSliceValidator::setExpressionParser(QSharedPointer<ExpressionParser> expressionParser)
{
    expressionParser_ = expressionParser;
}

//-----------------------------------------------------------------------------
// Function: PortSliceValidator::isInValidRange()
//-----------------------------------------------------------------------------
bool PortSliceValidator::isInValidRange(QString const& value, int leftBound, int rightBound) const
{
    bool validExpression = false;
    auto rangeValue = expressionParser_->parseExpression(value, &validExpression).toInt();

    return validExpression && (rightBound <= rangeValue && rangeValue <= leftBound);
}
