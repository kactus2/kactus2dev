//-----------------------------------------------------------------------------
// File: ParameterValidator2014.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// Validator for ipxact:parameter.
//-----------------------------------------------------------------------------

#include "ParameterValidator2014.h"

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
ParameterValidator2014::ParameterValidator2014(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder) : 
expressionParser_(expressionParser),
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
ParameterValidator2014::~ParameterValidator2014()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::validate()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::validate(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return hasValidName(parameter) &&
        hasValidValue(parameter, availableChoices) &&
        hasValidMinimumValue(parameter) &&
        hasValidMaximumValue(parameter) &&
        hasValidChoice(parameter, availableChoices) &&
        hasValidResolve(parameter) &&
        hasValidValueId(parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidName()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidName(Parameter const* parameter) const
{
    return !parameter->name().isEmpty();
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
// Function: ParameterValidator2014::hasValidType()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidType(Parameter const* parameter) const
{
    QString type = parameter->getType();

    return type.isEmpty() || type == "bit" || type == "byte" || type == "shortint" ||
        type == "int" || type == "longint" || type == "shortreal" || type == "real" || 
        type == "string";
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::hasValidValueForType()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueForType(QString const& value, QString const& type) const
{
    if (type.isEmpty())
    {
        return expressionParser_->isValidExpression(value) && expressionParser_->parseExpression(value) != "x";
    }

    if (!expressionParser_->isValidExpression(value))
    {
        return false;
    }

    bool canConvert = false;
    QString solvedValue = expressionParser_->parseExpression(value);

    if (expressionParser_->isArrayExpression(solvedValue))
    {
        if (type == "bit")
        {
            if (parameterFinder_->hasId(value))
            {
                return hasValidValueForType(parameterFinder_->valueForId(value), type);
            }
            else
            {
                return isArrayValidForType(value, type);
            }
        }
        else
        {
            return isArrayValidForType(solvedValue, type);
        }
    }

    else if (type == "bit")
    {
        QRegularExpression bitExpression("^([01]|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$");
        return bitExpression.match(value).hasMatch() || bitExpression.match(solvedValue).hasMatch();
    }
    else if (type == "byte")
    {
        solvedValue.toShort(&canConvert);
        return canConvert && -128 <= solvedValue.toShort() && solvedValue.toShort() <= 127;
    }
    else if (type == "shortint")
    {
        solvedValue.toShort(&canConvert);
    }
    else if (type == "int")
    {
        solvedValue.toInt(&canConvert);
    }
    else if (type == "longint")
    {
        if (solvedValue.startsWith("-"))
        {
            solvedValue.toLongLong(&canConvert);
        }
        else
        {
            solvedValue.toULongLong(&canConvert);
        }
    }
    else if (type == "shortreal")
    {
        solvedValue.toFloat(&canConvert);
    }
    else if (type == "real")
    {
        solvedValue.toDouble(&canConvert);
    }
    else if (type == "string")
    {
        QRegularExpression stringExpression("^\\s*\".*\"\\s*$");
        return stringExpression.match(solvedValue).hasMatch();
    }

    return canConvert;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::isArrayValidForType()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::isArrayValidForType(QString const& arrayExpression, QString const& type) const
{
    QStringList subValues = splitArrayToList(arrayExpression);

    if (arrayValuesAreSameSize(subValues, type))
    {
        foreach (QString innerValue, subValues)
        {
            innerValue = innerValue.remove(" ");
            if (!hasValidValueForType(innerValue, type))
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidValueForFormat()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueForType(Parameter const* parameter) const
{
    return hasValidValueForType(parameter->getValue(), parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidMinimumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidMinimumValue(Parameter const* parameter) const
{
    QString minimumValue = parameter->getMinimumValue();

    if (!shouldCompareValueAndBoundary(minimumValue, parameter->getType()))
    {
        return true;
    }

    return hasValidValueForType(minimumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidMaximumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidMaximumValue(Parameter const* parameter) const
{
    QString maximumValue = parameter->getMaximumValue();

    if (!shouldCompareValueAndBoundary(maximumValue, parameter->getType()))
    {
        return true;
    }

    return hasValidValueForType(maximumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidChoice()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidChoice(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return parameter->getChoiceRef().isEmpty() || findChoiceByName(parameter->getChoiceRef(), availableChoices);
}

bool ParameterValidator2014::hasValidValueForChoice(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    if (parameter->getChoiceRef().isEmpty())
    {
        return true;
    }
    else
    {
        QSharedPointer<Choice> referencedChoice = findChoiceByName(parameter->getChoiceRef(), availableChoices);

        if (!referencedChoice.isNull() && parameter->getValue().contains('{') &&
            parameter->getValue().contains('}'))
        {
            QStringList valueArray = parameter->getValue().split(',');
            valueArray.first().remove('{');
            valueArray.last().remove('}');

            foreach (QString parameterValue, valueArray)
            {
                if (!referencedChoice->hasEnumeration(parameterValue))
                {
                    return false;
                }
            }
            return true;
        }

        return !referencedChoice.isNull() && referencedChoice->hasEnumeration(parameter->getValue());
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidResolve()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidResolve(Parameter const* parameter) const
{
    QString resolve = parameter->getValueResolve();

    return resolve.isEmpty() || resolve == "immediate" || resolve == "user" || 
        resolve == "dependent" || resolve == "generated";
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidValueId()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueId(Parameter const* parameter) const
{
    QString resolve = parameter->getValueResolve();

    if (resolve == "user" || resolve == "generated")
    {
        return !parameter->getValueId().isEmpty();
    }

    return true;
}


//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::valueIsLessThanMinimum(Parameter const* parameter) const
{
    QString minimum = parameter->getMinimumValue();
    QString type = parameter->getType();
    QString value = expressionParser_->parseExpression(parameter->getValue());

    if (expressionParser_->isArrayExpression(value) && type != "bit" && type != "string" && !type.isEmpty())
    {
        QStringList subValues = splitArrayToList(value);

        foreach (QString innerValue, subValues)
        {
            if (shouldCompareValueAndBoundary(minimum, type) && valueOf(innerValue, type) < valueOf(minimum, type))
            {
                return true;
            }
        }

        return false;
    }

    return shouldCompareValueAndBoundary(minimum, type) && valueOf(value, type) < valueOf(minimum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueIsGreaterThanMaximum()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::valueIsGreaterThanMaximum(Parameter const* parameter) const
{
    QString maximum = parameter->getMaximumValue();
    QString type = parameter->getType();
    QString value = expressionParser_->parseExpression(parameter->getValue());

    if (expressionParser_->isArrayExpression(value) && type != "bit" && type != "string" && !type.isEmpty())
    {
        QStringList subValues = splitArrayToList(value);

        foreach (QString innerValue, subValues)
        {
            if (shouldCompareValueAndBoundary(maximum, type) && valueOf(innerValue, type) > valueOf(maximum, type))
            {
                return true;
            }
        }

        return false;
    }

    return shouldCompareValueAndBoundary(maximum, type) && valueOf(value, type) > valueOf(maximum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsIn()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsIn(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList errors;
    errors.append(findErrorsInName(parameter, context));
    errors.append(findErrorsInValue(parameter, context, availableChoices));
    errors.append(findErrorsInType(parameter, context));
    errors.append(findErrorsInMinimumValue(parameter, context));
    errors.append(findErrorsInMaximumValue(parameter, context));
    errors.append(findErrorsInChoice(parameter, context, availableChoices));
    errors.append(findErrorsInResolve(parameter, context));

    return errors;
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
qreal ParameterValidator2014::valueOf(QString const& value, QString const& type) const
{
    if (type == "real" || type == "shortreal")
    {
        return expressionParser_->parseExpression(value).toDouble();
    }
    else
    {
        return expressionParser_->parseExpression(value).toLongLong();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsInName()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsInName(Parameter const* parameter, QString const& context) const
{
    QStringList nameErrors;
    if (parameter->name().isEmpty())
    {
        nameErrors.append(QObject::tr("No name specified for %1 within %2").arg(
            parameter->elementName(), context));
    }

    return nameErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsInType()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsInType(Parameter const* parameter, QString const& context) const
{
    QStringList typeErrors;

    if (!hasValidType(parameter))
    {
        typeErrors.append(QObject::tr("Invalid type %1 specified for %2 %3 within %4").arg(
            parameter->getType(), parameter->elementName(), parameter->name(), context));
    }

    return typeErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsInValue()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsInValue(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList valueErrors;

    if (parameter->getValue().isEmpty())
    {
        valueErrors.append(QObject::tr("No value specified for %1 %2 within %3").arg(
            parameter->elementName(), parameter->name(), context));
    }
    else
    {
        if (!hasValidValueForType(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 is not valid for type %2 in %3 %4 within %5").arg(
                parameter->getValue(), parameter->getType(), parameter->elementName(), 
                parameter->name(), context));
        }

        if (valueIsLessThanMinimum(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 violates minimum value %2 in %3 %4 within %5"
                ).arg(parameter->getValue(), parameter->getMinimumValue(), 
                parameter->elementName(), parameter->name(), context));
        }

        if (valueIsGreaterThanMaximum(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 violates maximum value %2 in %3 %4 within %5"
                ).arg(parameter->getValue(), parameter->getMaximumValue(), 
                parameter->elementName(), parameter->name(), context));
        }

        if (!hasValidValueForChoice(parameter, availableChoices))
        {           
            valueErrors.append(QObject::tr("Value %1 references unknown enumeration for choice "
                "%2 in %3 %4 within %5").arg(parameter->getValue(), parameter->getChoiceRef(), 
                parameter->elementName(), parameter->name(), context));
        }
    }

    return valueErrors;
}

QStringList ParameterValidator2014::findErrorsInMinimumValue(Parameter const* parameter, QString const& context) const
{
    QStringList minimumErrors;

    if (shouldCompareValueAndBoundary(parameter->getValueAttribute("ipxact:minimum"), parameter->getValueAttribute("ipxact:format"))
        && !hasValidValueForFormat(parameter->getValueAttribute("ipxact:minimum"), parameter->getValueAttribute("ipxact:format")))
    {
        minimumErrors.append(QObject::tr("Minimum value %1 is not valid for format %2 in %3 %4 within %5").arg(
            parameter->getValueAttribute("ipxact:minimum"), parameter->getValueAttribute("ipxact:format"),parameter->elementName(),
            parameter->name(), context));
    }

    return minimumErrors;
}

QStringList ParameterValidator2014::findErrorsInMaximumValue(Parameter const* parameter, QString const& context) const
{
    QStringList maximumErrors;

    if (shouldCompareValueAndBoundary(parameter->getValueAttribute("ipxact:maximum"), parameter->getValueAttribute("ipxact:format"))
        && !hasValidValueForFormat(parameter->getValueAttribute("ipxact:maximum"), parameter->getValueAttribute("ipxact:format")))
    {
        maximumErrors.append(QObject::tr("Maximum value %1 is not valid for format %2 in %3 %4 within %5").arg(
            parameter->getValueAttribute("ipxact:maximum"), parameter->getValueAttribute("ipxact:format"),parameter->elementName(),
            parameter->name(), context));
    }

    return maximumErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::isValidValueForFormat()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueForFormat(QString const& value, QString const& format) const
{
    return expressionParser_->isValidExpression(value);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsInChoice()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsInChoice(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList choiceErrors;

    if (!hasValidChoice(parameter, availableChoices))
    { 
        choiceErrors.append(QObject::tr("Choice %1 referenced in %2 %3 is not specified within %4").arg(
            parameter->getChoiceRef(), parameter->elementName(), parameter->name(), context));
    }

    return choiceErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsInResolve()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsInResolve(Parameter const* parameter, QString const& context) const
{
    QStringList resolveErrors;

    if (!hasValidResolve(parameter))
    { 
        resolveErrors.append(QObject::tr("Invalid resolve %1 specified for %2 %3 within %4").arg(
            parameter->getValueResolve(), parameter->elementName(), parameter->name(), context));
    }

    return resolveErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::splitArrayToList()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::splitArrayToList(QString const& arrayValue) const
{
    QStringList subValues = arrayValue.split(',');
    QRegularExpression startExpression ("^'?{");

    subValues.first().remove(startExpression);
    subValues.last().chop(1);

    return subValues;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::arrayValuesAreSameSize()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::arrayValuesAreSameSize(QStringList const& bitArray, QString type) const
{
    if (type == "bit")
    {
        QString firstValue = bitArray.first();
        firstValue = firstValue.remove(" ");
        QRegularExpression sizeValueSeparator ("'[bB]");
        int bitSize = 1;

        if (firstValue.contains(sizeValueSeparator))
        {
            QStringList comparisonValues = firstValue.split(sizeValueSeparator);

            if (!comparisonValues.at(0).isEmpty())
            {
                bitSize = comparisonValues.at(0).toInt();
            }

            foreach (QString arrayValue, bitArray)
            {
                arrayValue = arrayValue.remove(" ");
                comparisonValues = arrayValue.split(sizeValueSeparator);

                if (comparisonValues.at(0).isEmpty())
                {
                    comparisonValues[0] = "1";
                }

                comparisonValues[1] = comparisonValues[1].remove("_");
                if (comparisonValues.at(0).toInt() != bitSize || comparisonValues.at(1).size() != bitSize)
                {
                    return false;
                }
            }
        }

        else
        {
            foreach (QString arrayValue, bitArray)
            {
                if (arrayValue.size() != bitSize)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidArrayValues()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::validateArrayValues(QString const& arrayLeft, QString const& arrayRight) const
{
    if (arrayLeft.isEmpty() && arrayRight.isEmpty())
    {
        return true;
    }

    else
    {
        bool arrayLeftIsOk = true;
        bool arrayRightIsOk = true;

        QString leftValue = expressionParser_->parseExpression(arrayLeft);
        QString rightValue = expressionParser_->parseExpression(arrayRight);
        leftValue.toInt(&arrayLeftIsOk);
        rightValue.toInt(&arrayRightIsOk);

        return arrayLeftIsOk && arrayRightIsOk;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::findChoiceByName()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ParameterValidator2014::findChoiceByName(QString const& choiceName, 
    QSharedPointer<QList<QSharedPointer<Choice> > > choices) const
{
    foreach (QSharedPointer<Choice> choice, *choices)
    {
        if (choice->name() == choiceName)
        {
            return choice;
        }
    }	

    return QSharedPointer<Choice>();
}