//-----------------------------------------------------------------------------
// File: ParameterValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ParameterValidator.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/Enumeration.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/StringPromtAtt.h>

#include <QRegExp>
#include <qmath.h>

//-----------------------------------------------------------------------------
// Function: ParameterValidator::ParameterValidator()
//-----------------------------------------------------------------------------
ParameterValidator::ParameterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::~ParameterValidator()
//-----------------------------------------------------------------------------
ParameterValidator::~ParameterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::validate()
//-----------------------------------------------------------------------------
bool ParameterValidator::validate(Parameter const* parameter, QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
   return hasValidName(parameter) &&
       hasValidValue(parameter, availableChoices) &&
       hasValidFormat(parameter) &&
       hasValidBitStringLength(parameter)&&
       hasValidMinimumValue(parameter) &&
       hasValidMaximumValue(parameter) &&
       hasValidChoice(parameter, availableChoices) &&
       hasValidResolve(parameter) &&
       hasValidValueId(parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::isValidName()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidName(Parameter const* parameter) const
{
    return !parameter->getName().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidValue()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValue(Parameter const* parameter,
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return !parameter->getValue().isEmpty() &&
        hasValidValueForFormat(parameter) &&
        !valueIsLessThanMinimum(parameter) &&
        !valueIsGreaterThanMaximum(parameter) &&
        hasValidValueForChoice(parameter, availableChoices);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidFormat()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidFormat(Parameter const* parameter) const
{
    QString format = parameter->getValueFormat();

    return format.isEmpty() || format == "bool" || format == "bitString" ||
        format == "long" || format == "float" || format == "string";
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::isValidValueForFormat()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueForFormat(QString const& value, QString const& format) const
{
    if (format.isEmpty() || format == "string")
    {
        return true;
    }

    QRegExp validatingExp;
    if (format == "bool")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_BOOL_VALUE);
    }
    else if (format == "bitString")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_BITSTRING_VALUE);
    }
    else if (format == "long")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_LONG_VALUE);
    }
    else if (format == "float")
    {
        validatingExp.setPattern(StringPromptAtt::VALID_FLOAT_VALUE);
    }

    return validatingExp.exactMatch(value);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::isValidValueForFormat()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueForFormat(Parameter const* parameter) const
{
    return hasValidValueForFormat(parameter->getValue(), parameter->getValueFormat());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidBitStringLength()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidBitStringLength(Parameter const* parameter) const
{
   return (parameter->getValueFormat() == "bitString" && !parameter->getBitStringLength().isEmpty()) ||
       (parameter->getValueFormat() != "bitString" && parameter->getBitStringLength().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidMinimumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidMinimumValue(Parameter const* parameter) const
{
    if (!shouldCompareValueAndBoundary(parameter->getValueAttribute("spirit:minimum"), parameter->getValueFormat()))
    {
        return true;
    }

    return hasValidValueForFormat(parameter->getValueAttribute("spirit:minimum"), parameter->getValueFormat());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidMaximumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidMaximumValue(Parameter const* parameter) const
{
    if (!shouldCompareValueAndBoundary(parameter->getValueAttribute("spirit:maximum"), parameter->getValueFormat()))
    {
        return true;
    }

    return hasValidValueForFormat(parameter->getValueAttribute("spirit:maximum"), parameter->getValueFormat());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool ParameterValidator::valueIsLessThanMinimum(Parameter const* parameter) const
{
    QString minimum = parameter->getValueAttribute("spirit:minimum");
    QString format = parameter->getValueFormat();
    QString value = parameter->getValue();

    return shouldCompareValueAndBoundary(minimum, format) && valueOf(value, format) < valueOf(minimum, format);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool ParameterValidator::valueIsGreaterThanMaximum(Parameter const* parameter) const
{
    QString maximum = parameter->getValueAttribute("spirit:maximum");
    QString format = parameter->getValueFormat();
    QString value = parameter->getValue();

    return shouldCompareValueAndBoundary(maximum, format) && valueOf(value, format) > valueOf(maximum, format);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidChoice()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidChoice(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return parameter->getChoiceRef().isEmpty() || findChoiceByName(parameter->getChoiceRef(), availableChoices);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidChoiceReference()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueForChoice(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    if (parameter->getChoiceRef().isEmpty())
    {
        return true;
    }
    else
    {
        QSharedPointer<Choice> referencedChoice = findChoiceByName(parameter->getChoiceRef(), availableChoices);
        return !referencedChoice.isNull() && referencedChoice->hasEnumeration(parameter->getValue());
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidResolve()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidResolve(Parameter const* parameter) const
{
    QString resolve = parameter->getValueResolve();

    return resolve.isEmpty() || resolve == "immediate" || resolve == "user" || 
        resolve == "dependent" || resolve == "generated";
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidValueId()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueId(Parameter const* parameter) const
{
    QString resolve = parameter->getValueResolve();

    if (resolve == "user" || resolve == "generated")
    {
        return !parameter->getValueId().isEmpty();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsIn()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsIn(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList errors;
    errors.append(findErrorsInName(parameter, context));
    errors.append(findErrorsInValue(parameter, context, availableChoices));
    errors.append(findErrorsInFormat(parameter, context));
    errors.append(findErrorsInBitStringLength(parameter, context));
    errors.append(findErrorsInMinimumValue(parameter, context));
    errors.append(findErrorsInMaximumValue(parameter, context));
    errors.append(findErrorsInChoice(parameter, context, availableChoices));
    errors.append(findErrorsInResolve(parameter, context));
    errors.append(findErrorsInId(parameter, context));

    return errors;
}

//-----------------------------------------------------------------------------
// Function: Parameter::shouldCompareBoundary()
//-----------------------------------------------------------------------------
bool ParameterValidator::shouldCompareValueAndBoundary(QString const& boundaryValue, QString const& format) const
{
    return !boundaryValue.isEmpty() && (format == "long" || format == "float" || format == "bitString");
}

//-----------------------------------------------------------------------------
// Function: Parameter::toLong()
//-----------------------------------------------------------------------------
qreal ParameterValidator::valueOf(QString const& value, QString const& format) const
{
    if (format == "long")
    {
        return longValueOf(value);
    }
    else if (format == "float")
    {
        return floatValueOf(value);
    }
    else
    {
        return value.toDouble();
    }	
}

//-----------------------------------------------------------------------------
// Function: Parameter::longValueOf()
//-----------------------------------------------------------------------------
qreal ParameterValidator::longValueOf(QString const& value) const
{
    if (value.startsWith("0x", Qt::CaseInsensitive) || value.startsWith('#'))
    {
        QString hexValue = value;
        return hexValue.remove('#').toLong(0, 16);
    }
    else if (value.endsWith('k', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024;
    }
    else if (value.endsWith('M', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024;
    }
    else if (value.endsWith('G', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024*1024;
    }
    else if (value.endsWith('T', Qt::CaseInsensitive))
    {
        QString coefficient = value;
        coefficient.chop(1);

        return coefficient.toLong()*1024*1024*1024*1024;
    }
    else
    {
        return value.toLong();
    }
}

//-----------------------------------------------------------------------------
// Function: Parameter::floatValueOf()
//-----------------------------------------------------------------------------
qreal ParameterValidator::floatValueOf(QString const& value) const
{
    if (value.contains('e'))
    {
        qreal coefficient = value.left(value.indexOf('e')).toFloat();
        qreal exponent = value.mid(value.indexOf('e') + 1).toFloat();
        qreal result = coefficient * qPow(10, exponent);
        return result;
    }
    else
    {
        return value.toFloat();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInName()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInName(Parameter const* parameter, QString const& context) const
{
    QStringList nameErrors;
    if (parameter->getName().isEmpty())
    {
        nameErrors.append(QObject::tr("No name specified for %1 within %2").arg(
            parameter->elementName(), context));
    }

    return nameErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInValue()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInValue(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList valueErrors;

    if (parameter->getValue().isEmpty())
    {
        valueErrors.append(QObject::tr("No value specified for %1 %2 within %3").arg(
            parameter->elementName(), parameter->getName(), context));
    }
    else
    {
        QString format = parameter->getValueFormat();

        if (!hasValidValueForFormat(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 violates format %2 in %3 %4 within %5").arg(
                parameter->getValue(), format, parameter->elementName(), 
                parameter->getName(), context));
        }

        if (valueIsLessThanMinimum(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 violates minimum value %2 in %3 %4 within %5"
                ).arg(parameter->getValue(), parameter->getValueAttribute("spirit:minimum"), 
                parameter->elementName(), parameter->getName(), context));
        }

        if (valueIsGreaterThanMaximum(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 violates maximum value %2 in %3 %4 within %5"
                ).arg(parameter->getValue(), parameter->getValueAttribute("spirit:maximum"), 
                parameter->elementName(), parameter->getName(), context));
        }

        if (!hasValidValueForChoice(parameter, availableChoices))
        {           
            valueErrors.append(QObject::tr("Value %1 references unknown enumeration for choice "
                "%2 in %3 %4 within %5").arg(parameter->getValue(), parameter->getChoiceRef(), 
                parameter->elementName(), parameter->getName(), context));
        }
    }
    
    return valueErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInFormat()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInFormat(Parameter const* parameter, QString const& context) const
{
    QStringList formatErrors;

    if (!hasValidFormat(parameter))
    {
        formatErrors.append(QObject::tr("Invalid format %1 specified for %2 %3 within %4").arg(
            parameter->getValueFormat(), parameter->elementName(), parameter->getName(), context));
    }

    return formatErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInBitStringLength()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInBitStringLength(Parameter const* parameter, 
    QString const& context) const
{
    QStringList bitStringLengthErrors;

    if (parameter->getValueFormat() == "bitString" && parameter->getBitStringLength().isEmpty())
    {
        bitStringLengthErrors.append(QObject::tr("No bit string length specified for %1 %2 within %3").arg(
            parameter->elementName(), parameter->getName(), context));
    }
    else if (parameter->getValueFormat() != "bitString" && !parameter->getBitStringLength().isEmpty())
    {   
        bitStringLengthErrors.append(QObject::tr("Bit string length specified for format other than bitString "
            "for %1 %2 within %3").arg(
            parameter->elementName(), parameter->getName(), context));
    }

    return bitStringLengthErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInMinimumValue()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInMinimumValue(Parameter const* parameter, QString const& context) const
{
    QStringList minimumErrors;

    if (shouldCompareValueAndBoundary(parameter->getValueAttribute("spirit:minimum"), parameter->getValueFormat()) 
        && !hasValidValueForFormat(parameter->getValueAttribute("spirit:minimum"), parameter->getValueFormat()))
    {
        minimumErrors.append(QObject::tr("Minimum value %1 is not valid for format %2 in %3 %4 within %5").arg(
            parameter->getValueAttribute("spirit:minimum"), parameter->getValueFormat(),parameter->elementName(),
            parameter->getName(), context));
    }

    return minimumErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInMinimumValue()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInMaximumValue(Parameter const* parameter, QString const& context) const
{
    QStringList maximumErrors;

    if (shouldCompareValueAndBoundary(parameter->getValueAttribute("spirit:maximum"), parameter->getValueFormat()) 
        && !hasValidValueForFormat(parameter->getValueAttribute("spirit:maximum"), parameter->getValueFormat()))
    {
        maximumErrors.append(QObject::tr("Maximum value %1 is not valid for format %2 in %3 %4 within %5").arg(
            parameter->getValueAttribute("spirit:maximum"), parameter->getValueFormat(),parameter->elementName(),
            parameter->getName(), context));
    }

    return maximumErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInChoice()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInChoice(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList choiceErrors;

    if (!hasValidChoice(parameter, availableChoices))
    { 
        choiceErrors.append(QObject::tr("Choice %1 referenced in %2 %3 is not specified within %4").arg(
            parameter->getChoiceRef(), parameter->elementName(), parameter->getName(), context));
    }

    return choiceErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInResolve()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInResolve(Parameter const* parameter, QString const& context) const
{
    QStringList resolveErrors;

    if (!hasValidResolve(parameter))
    { 
        resolveErrors.append(QObject::tr("Invalid resolve %1 specified for %2 %3 within %4").arg(
            parameter->getValueResolve(), parameter->elementName(), parameter->getName(), context));
    }

    return resolveErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInId()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::findErrorsInId(Parameter const* parameter, 
    QString const& context) const
{
    QStringList idErrors;

    if (!hasValidValueId(parameter))
    {
        idErrors.append(QObject::tr("No id specified for %1 %2 with resolve %3 within %4").arg(
            parameter->elementName(), parameter->getName(), parameter->getValueResolve(), context));
    }

    return idErrors;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::findChoiceByName()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ParameterValidator::findChoiceByName(QString const& choiceName, 
    QSharedPointer<QList<QSharedPointer<Choice> > > choices) const
{
    foreach (QSharedPointer<Choice> choice, *choices)
    {
        if (choice->getName() == choiceName)
        {
            return choice;
        }
    }	

    return QSharedPointer<Choice>();
}
