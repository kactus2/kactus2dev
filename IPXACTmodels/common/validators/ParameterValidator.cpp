//-----------------------------------------------------------------------------
// File: ParameterValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// Validator for ipxact:parameter.
//-----------------------------------------------------------------------------

#include "ParameterValidator.h"

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
ParameterValidator::ParameterValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices, Document::Revision docRevision):
expressionParser_(expressionParser),
    availableChoices_(availableChoices),
    typeValidator_(QRegularExpression(QStringLiteral("bit|byte|shortint|int|longint|shortreal|real|string|^$"))),
    docRevision_(docRevision)
{
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::componentChange()
//-----------------------------------------------------------------------------
void ParameterValidator::componentChange(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices,
    Document::Revision docRevision)
{
    availableChoices_ = newChoices;
    setStdRevision(docRevision);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::setStdRevision()
//-----------------------------------------------------------------------------
void ParameterValidator::setStdRevision(Document::Revision docRevision)
{
    docRevision_ = docRevision;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::validate()
//-----------------------------------------------------------------------------
bool ParameterValidator::validate(QSharedPointer<const Parameter> parameter) const
{
    return hasValidName(parameter) &&
        hasValidValue(parameter) &&
        hasValidMinimumValue(parameter) &&
        hasValidMaximumValue(parameter) &&
        hasValidChoice(parameter) &&
        hasValidResolve(parameter) &&
        hasValidValueId(parameter) &&
        hasValidVector(parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidName(QSharedPointer<const Parameter> parameter) const
{
    return !parameter->name().trimmed().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidValue()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValue(QSharedPointer<const Parameter> parameter) const
{
    if (parameter->getValue().isEmpty())
    {
        return false;
    }

    QString value = expressionParser_->parseExpression(parameter->getValue());
    QString type = parameter->getType();

    return hasValidValueForType(parameter) &&
        !valueIsLessThanMinimum(parameter, value, type) &&
        !valueIsGreaterThanMaximum(parameter, value, type) &&
        hasValidValueForChoice(parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidType()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidType(QSharedPointer<const Parameter> parameter) const
{
    int pos = 0;
    QString type = parameter->getType();
    return typeValidator_.validate(type, pos) == QRegularExpressionValidator::Acceptable;  
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::hasValidValueForType()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueForType(QString const& value, QString const& type) const
{
    if (type.isEmpty() || type.compare(QLatin1String("string")) == 0)
    {
        return true;
    }

    if (value.contains(QLatin1Char('{')))
    {
        return isArrayValidForType(value, type);
    }

    bool isValidValue = false;
    QString solvedValue = expressionParser_->parseExpression(value, &isValidValue);

    bool canConvert = false;
    if (type == QLatin1String("bit"))
    {
        solvedValue.toInt(&canConvert);
        if (canConvert)
        {
            ValueFormatter formatter;
            solvedValue = formatter.format(solvedValue, 2);

            QRegularExpression bitExpression(QStringLiteral("^([01]|[1-9]?[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$"));
            return bitExpression.match(value).hasMatch() || bitExpression.match(solvedValue).hasMatch();
        }
    }
    else if (type == QLatin1String("byte"))
    {
        solvedValue.toShort(&canConvert);
        return canConvert && -128 <= solvedValue.toShort() && solvedValue.toShort() <= 127;
    }
    else if (type == QLatin1String("shortint"))
    {
        solvedValue.toShort(&canConvert);
    }
    else if (type == QLatin1String("int"))
    {
        solvedValue.toInt(&canConvert);
    }
    else if (type == QLatin1String("longint"))
    {
        if (solvedValue.startsWith(QStringLiteral("-")))
        {
            solvedValue.toLongLong(&canConvert);
        }
        else
        {
            solvedValue.toULongLong(&canConvert);
        }
    }
    else if (type == QLatin1String("shortreal"))
    {
        solvedValue.toFloat(&canConvert);
    }
    else if (type == QLatin1String("real"))
    {
        solvedValue.toDouble(&canConvert);
    }
    else if (type == QLatin1String("string"))
    {
        QRegularExpression stringExpression(QStringLiteral("^\\s*\".*\"\\s*$"));
        return stringExpression.match(solvedValue).hasMatch();
    }

    return canConvert;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::isArrayValidForType()
//-----------------------------------------------------------------------------
bool ParameterValidator::isArrayValidForType(QString const& arrayExpression, QString const& type) const
{
    QStringList subValues = splitArrayToList(arrayExpression);

    if (arrayValuesAreSameSize(subValues, type))
    {
        for (QString const& innerValue : subValues)
        {
            if (!hasValidValueForType(innerValue, type))
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidVector()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidVector(QSharedPointer<const Parameter> parameter) const
{
    if ((!parameter->getVectors()->isEmpty() && parameter->getType() != QLatin1String("bit")) ||
        !validateArrayValues(parameter->getVectorLeft(), parameter->getVectorRight()))
    {
        return false;
    }

    if (docRevision_ != Document::Revision::Std22 && vectorIdExists(parameter))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::vectorIdExists()
//-----------------------------------------------------------------------------
bool ParameterValidator::vectorIdExists(QSharedPointer<const Parameter> parameter) const
{
    auto vectors = parameter->getVectors();
    return std::any_of(vectors->cbegin(), vectors->cend(), [](auto vector)
        {
            return !vector->getId().isEmpty();
        });
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidValueForType()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueForType(QSharedPointer<const Parameter> parameter) const
{
    return hasValidValueForType(parameter->getValue(), parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidMinimumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidMinimumValue(QSharedPointer<const Parameter> parameter) const
{
    QString minimumValue = parameter->getMinimumValue();

    if (!shouldCompareValueAndBoundary(minimumValue, parameter->getType()))
    {
        return true;
    }

    return hasValidValueForType(minimumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidMaximumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidMaximumValue(QSharedPointer<const Parameter> parameter) const
{
    QString maximumValue = parameter->getMaximumValue();

    if (!shouldCompareValueAndBoundary(maximumValue, parameter->getType()))
    {
        return true;
    }

    return hasValidValueForType(maximumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidChoice()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidChoice(QSharedPointer<const Parameter> parameter) const
{
    return parameter->getChoiceRef().isEmpty() || findChoiceByName(parameter->getChoiceRef());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidValueForChoice()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueForChoice(QSharedPointer<const Parameter> parameter) const
{
    if (parameter->getChoiceRef().isEmpty())
    {
        return true;
    }

    QSharedPointer<Choice> referencedChoice = findChoiceByName(parameter->getChoiceRef());

    if (!referencedChoice.isNull() && parameter->getValue().contains(QLatin1Char('{')) &&
        parameter->getValue().contains(QLatin1Char('}')))
    {
        QStringList valueArray = parameter->getValue().split(QLatin1Char(','));
        valueArray.first().remove(QLatin1Char('{'));
        valueArray.last().remove(QLatin1Char('}'));

        for (QString const& parameterValue : valueArray)
        {
            if (!referencedChoice->hasEnumeration(parameterValue))
            {
                return false;
            }
        }
        return true;
    }
    
    return !referencedChoice.isNull() && 
        referencedChoice->hasEnumeration(expressionParser_->parseExpression(parameter->getValue()));
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidResolve()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidResolve(QSharedPointer<const Parameter> parameter) const
{
    QString resolve = parameter->getValueResolve();

    return resolve.isEmpty() || resolve == QLatin1String("immediate") || resolve == QLatin1String("user") ||
        resolve == QLatin1String("generated");
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidValueId()
//-----------------------------------------------------------------------------
bool ParameterValidator::hasValidValueId(QSharedPointer<const Parameter> parameter) const
{
    QString resolve = parameter->getValueResolve();

    if (resolve == QLatin1String("user") || resolve == QLatin1String("generated"))
    {
        return !parameter->getValueId().isEmpty();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool ParameterValidator::valueIsLessThanMinimum(QSharedPointer<const Parameter> parameter) const
{
    QString type = parameter->getType();
    QString value = expressionParser_->parseExpression(parameter->getValue());

    return valueIsLessThanMinimum(parameter, value, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::valueIsGreaterThanMaximum()
//-----------------------------------------------------------------------------
bool ParameterValidator::valueIsGreaterThanMaximum(QSharedPointer<const Parameter> parameter) const
{
    QString type = parameter->getType();
    QString value = expressionParser_->parseExpression(parameter->getValue());

    return valueIsGreaterThanMaximum(parameter, value, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    findErrorsInName(errors, parameter, context);
    findErrorsInValue(errors, parameter, context);
    findErrorsInType(errors, parameter, context);
    findErrorsInMinimumValue(errors, parameter, context);
    findErrorsInMaximumValue(errors, parameter, context);
    findErrorsInChoice(errors, parameter, context);
    findErrorsInResolve(errors, parameter, context);
    findErrorsInValueId(errors, parameter, context);
    findErrorsInVector(errors, parameter, context);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::shouldCompareValueAndBoundary()
//-----------------------------------------------------------------------------
bool ParameterValidator::shouldCompareValueAndBoundary(QString const& boundaryValue, QString const& type) const
{
    if (boundaryValue.isEmpty() || type.isEmpty())
    {
        return false;
    }

    bool isValidBoundary = false;
    expressionParser_->parseExpression(boundaryValue, &isValidBoundary);
    return isValidBoundary && type != QLatin1String("bit") && type != QLatin1String("string");
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::valueOf()
//-----------------------------------------------------------------------------
qreal ParameterValidator::valueOf(QString const& value, QString const& type) const
{
    if (type == QLatin1String("real") || type == QLatin1String("shortreal"))
    {
        return value.toDouble();
    }
    else
    {
        return value.toLongLong();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (!hasValidName(parameter))
    {
        errors.append(QObject::tr("No valid name specified for %1 %2 within %3").arg(parameter->elementName(),
            parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInType()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInType(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (!hasValidType(parameter))
    {
        errors.append(QObject::tr("Invalid type %1 specified for %2 %3 within %4").arg(
            parameter->getType(), parameter->elementName(), parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInValue()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInValue(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (parameter->getValue().isEmpty())
    {
        errors.append(QObject::tr("No value specified for %1 %2 within %3").arg(parameter->elementName(),
            parameter->name(), context));
        return;
    }

    QString solvedValue = expressionParser_->parseExpression(parameter->getValue());
    QString parameterType = parameter->getType();

    if (!hasValidValueForType(solvedValue, parameterType))
    {
        errors.append(QObject::tr("Value '%1' is not valid for type %2 in %3 %4 within %5").arg(
            parameter->getValue(), parameter->getType(), parameter->elementName(), parameter->name(), context));
    }

    if (valueIsLessThanMinimum(parameter, solvedValue, parameterType))
    {
        errors.append(QObject::tr("Value '%1' violates minimum value %2 in %3 %4 within %5").arg(
            parameter->getValue(), parameter->getMinimumValue(), parameter->elementName(), parameter->name(),
            context));
    }

    if (valueIsGreaterThanMaximum(parameter, solvedValue, parameterType))
    {
        errors.append(QObject::tr("Value '%1' violates maximum value %2 in %3 %4 within %5").arg(
            parameter->getValue(), parameter->getMaximumValue(), parameter->elementName(), parameter->name(), 
            context));
    }

    if (!hasValidValueForChoice(parameter))
    {           
        errors.append(QObject::tr("Value '%1' references unknown enumeration for choice %2 in %3 %4 within %5").arg(
            parameter->getValue(), parameter->getChoiceRef(), parameter->elementName(), parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInMinimumValue()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInMinimumValue(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (shouldCompareValueAndBoundary(parameter->getValueAttribute(QStringLiteral("ipxact:minimum")),
        parameter->getType())
        && !hasValidValueForType(parameter->getValueAttribute(QStringLiteral("ipxact:minimum")), parameter->getType()))
    {
        errors.append(QObject::tr("Minimum value %1 is not valid for format %2 in %3 %4 within %5").arg(
            parameter->getValueAttribute(QStringLiteral("ipxact:minimum")), 
            parameter->getValueAttribute(QStringLiteral("ipxact:format")),
            parameter->elementName(), parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInMaximumValue()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInMaximumValue(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (shouldCompareValueAndBoundary(parameter->getValueAttribute(QStringLiteral("ipxact:maximum")),
        parameter->getType())
        && !hasValidValueForType(parameter->getValueAttribute(QStringLiteral("ipxact:maximum")), parameter->getType()))
    {
        errors.append(QObject::tr("Maximum value %1 is not valid for format %2 in %3 %4 within %5").arg(
            parameter->getValueAttribute(QStringLiteral("ipxact:maximum")),
            parameter->getValueAttribute(QStringLiteral("ipxact:format")),
            parameter->elementName(), parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInChoice()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInChoice(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (!hasValidChoice(parameter))
    { 
        errors.append(QObject::tr("Choice %1 referenced in %2 %3 is not specified within %4").arg(
            parameter->getChoiceRef(), parameter->elementName(), parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInResolve()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInResolve(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (!hasValidResolve(parameter))
    { 
        errors.append(QObject::tr("Invalid resolve %1 specified for %2 %3 within %4").arg(
            parameter->getValueResolve(), parameter->elementName(), parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInValueId()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInValueId(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if (!hasValidValueId(parameter))
    {
        errors.append(QObject::tr("No identifier specified for %1 %2 with resolve %3 within %4").arg(
            parameter->elementName(), parameter->name(), parameter->getValueResolve(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::findErrorsInVector()
//-----------------------------------------------------------------------------
void ParameterValidator::findErrorsInVector(QVector<QString>& errors, QSharedPointer<Parameter> parameter,
    QString const& context) const
{
    if ((!parameter->getVectors()->isEmpty() && parameter->getType() != QLatin1String("bit")) ||
        !validateArrayValues(parameter->getVectorLeft(), parameter->getVectorRight()))
    {
        errors.append(QObject::tr("Invalid bit vector values specified for %1 %2 within %3").arg(
            parameter->elementName(), parameter->name(), context));
    }

    if (docRevision_ != Document::Revision::Std22 && vectorIdExists(parameter))
    {
        errors.append(QObject::tr("Vector ID specified for %1 %2 within %3 not using IP-XACT standard"
            " revision 2022").arg(parameter->elementName(), parameter->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool ParameterValidator::valueIsLessThanMinimum(QSharedPointer<const Parameter> parameter,
    QString const& solvedValue, QString const& type) const
{
    QString minimum = parameter->getMinimumValue();
    if (type != QLatin1String("bit") && type != QLatin1String("string") && !type.isEmpty())
    {
        QStringList subValues = splitArrayToList(solvedValue);

        for (QString const& innerValue : subValues)
        {
            if (shouldCompareValueAndBoundary(minimum, type) && valueOf(innerValue, type) < valueOf(minimum, type))
            {
                return true;
            }
        }

        return false;
    }

    return shouldCompareValueAndBoundary(minimum, type) && valueOf(solvedValue, type) < valueOf(minimum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::valueIsGreaterThanMaximum()
//-----------------------------------------------------------------------------
bool ParameterValidator::valueIsGreaterThanMaximum(QSharedPointer<const Parameter> parameter,
    QString const& solvedValue, QString const& type) const
{
    QString maximum = parameter->getMaximumValue();

    if (type != QLatin1String("bit") && type != QLatin1String("string") && !type.isEmpty())
    {
        QStringList subValues = splitArrayToList(solvedValue);

        for (QString const& innerValue : subValues)
        {
            if (shouldCompareValueAndBoundary(maximum, type) && valueOf(innerValue, type) > valueOf(maximum, type))
            {
                return true;
            }
        }

        return false;
    }

    return shouldCompareValueAndBoundary(maximum, type) && valueOf(solvedValue, type) > valueOf(maximum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::splitArrayToList()
//-----------------------------------------------------------------------------
QStringList ParameterValidator::splitArrayToList(QString const& arrayValue) const
{
    QStringList subValues = arrayValue.split(QLatin1Char(','));
    QRegularExpression startExpression (QStringLiteral("^'?{"));
    QRegularExpression endExpression(QStringLiteral("}\\s*$"));

    subValues.first().remove(startExpression);
    subValues.last().remove(endExpression);

    return subValues;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::arrayValuesAreSameSize()
//-----------------------------------------------------------------------------
bool ParameterValidator::arrayValuesAreSameSize(QStringList const& bitArray, QString const& type) const
{
    if (type == QLatin1String("bit") && bitArray.size() > 1)
    {
        ValueFormatter formatter;
        QString formattedFirst = formatter.format(expressionParser_->parseExpression(bitArray.first()), 2);
        int bitSize = formattedFirst.size();

        for (int i = 1; i < bitArray.size(); ++i)
        {
            QString solvedValue = expressionParser_->parseExpression(bitArray.at(i));
            QString formattedValue = formatter.format(solvedValue, 2);

            if (bitSize != formattedValue.size())
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator::hasValidArrayValues()
//-----------------------------------------------------------------------------
bool ParameterValidator::validateArrayValues(QString const& arrayLeft, QString const& arrayRight) const
{
    if (arrayLeft.isEmpty() && arrayRight.isEmpty())
    {
        return true;
    }

    bool arrayLeftIsOk = true;
    bool arrayRightIsOk = true;

    QString leftValue = expressionParser_->parseExpression(arrayLeft);
    QString rightValue = expressionParser_->parseExpression(arrayRight);
    leftValue.toInt(&arrayLeftIsOk);
    rightValue.toInt(&arrayRightIsOk);

    return arrayLeftIsOk && arrayRightIsOk;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::findChoiceByName()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ParameterValidator::findChoiceByName(QString const& choiceName) const
{
    if (availableChoices_)
    {
        for (QSharedPointer<Choice> const& choice : *availableChoices_)
        {
            if (choice->name() == choiceName)
            {
                return choice;
            }
        }	
    }

    return QSharedPointer<Choice>();
}
