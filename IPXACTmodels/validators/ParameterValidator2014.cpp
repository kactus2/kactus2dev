//-----------------------------------------------------------------------------
// File: ParameterValidator2014.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ParameterValidator2014.h"

#include <IPXACTmodels/parameter.h>

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
ParameterValidator2014::ParameterValidator2014(QSharedPointer<ExpressionParser> expressionParser) : 
ParameterValidator(), expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
ParameterValidator2014::~ParameterValidator2014()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::hasValidValueForType()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueForType(QString const& value, QString const& type) const
{
    if (type.isEmpty())
    {
        return true;
    }

    if (!expressionParser_->isValidExpression(value))
    {
        return false;
    }

    QString solvedValue = expressionParser_->parseExpression(value);
    
    if (type == "bit")
    {
        return solvedValue == "0" || solvedValue == "1";
    }
    else if (type == "byte")
    {
        return -128 <= solvedValue.toInt() && solvedValue.toInt() <= 127;
    }
    else if (type == "shortint")
    {
        bool canConvert = false;
        solvedValue.toShort(&canConvert);
        return canConvert;
    }
    else if (type == "int")
    {
        bool canConvert = false;
        solvedValue.toInt(&canConvert);
        return canConvert;
    }
    else if (type == "longint")
    {
        bool canConvert = false;
        solvedValue.toLongLong(&canConvert);
        return canConvert;
    }
    else if (type == "string")
    {
        QRegExp stringExpression("^\\s*\".*\"\\s*$");
        return stringExpression.indexIn(value) == 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidValueForFormat()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueForType(Parameter const* parameter) const
{
    return hasValidValueForType(parameter->getValue(), parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValue(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return !parameter->getValue().isEmpty() &&
        hasValidValueForType(parameter) &&
        !valueIsLessThanMinimum(parameter) &&
        !valueIsGreaterThanMaximum(parameter) &&
        hasValidValueForChoice(parameter, availableChoices);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidMinimumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidMinimumValue(Parameter const* parameter) const
{
    QString minimumValue = parameter->getMinimumValue();
    return minimumValue.isEmpty() || hasValidValueForType(minimumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidMaximumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidMaximumValue(Parameter const* parameter) const
{
    QString maximumValue = parameter->getMaximumValue();
    return maximumValue.isEmpty() || hasValidValueForType(maximumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::valueIsLessThanMinimum(Parameter const* parameter) const
{
    QString minimum = parameter->getMinimumValue();
    QString type = parameter->getType();
    QString value = parameter->getValue();

    return shouldCompareValueAndBoundary(minimum, type) && valueOf(value, type) < valueOf(minimum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueIsGreaterThanMaximum()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::valueIsGreaterThanMaximum(Parameter const* parameter) const
{
    QString maximum = parameter->getMaximumValue();
    QString type = parameter->getType();
    QString value = parameter->getValue();

    return shouldCompareValueAndBoundary(maximum, type) && valueOf(value, type) > valueOf(maximum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::shouldCompareValueAndBoundary()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::shouldCompareValueAndBoundary(QString const& boundaryValue, QString const& type) const
{
     return expressionParser_->isValidExpression(boundaryValue) && 
         (type != "bit" && type != "string" && !type.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueOf()
//-----------------------------------------------------------------------------
qreal ParameterValidator2014::valueOf(QString const& value, QString const& format) const
{
    return expressionParser_->parseExpression(value).toLongLong();
}
