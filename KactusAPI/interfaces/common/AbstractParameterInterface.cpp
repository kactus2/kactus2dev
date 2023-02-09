//-----------------------------------------------------------------------------
// File: AbstractParametersInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.09.2020
//
// Description:
// Interface for editing parameters.
//-----------------------------------------------------------------------------

#include "AbstractParameterInterface.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/Choice.h>

namespace
{
    QString const ARRAY_LEFT_ATTRIBTE = QLatin1String("kactus2:arrayLeft");
    QString const ARRAY_RIGHT_ATTRIBUTE = QLatin1String("kactus2:arrayRight");
};

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::AbstractParameterInterface()
//-----------------------------------------------------------------------------
AbstractParameterInterface::AbstractParameterInterface(QSharedPointer<ParameterValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
parameterValidator_(validator),
choices_()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setChoices()
//-----------------------------------------------------------------------------
void AbstractParameterInterface::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices)
{
    choices_ = newChoices;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setName()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<Parameter> editedParameter = getParameter(currentName);
    if (editedParameter && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueName(newName, "parameter");

        editedParameter->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getDescription()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getDescription(std::string const& itemName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(itemName);
    if (editedParameter)
    {
        return editedParameter->description().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setDescription()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<Parameter> editedParameter = getParameter(itemName);
    if (!editedParameter)
    {
        return false;
    }

    editedParameter->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getDisplayName()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getDisplayName(std::string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->displayName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setDisplayName()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setDisplayName(std::string const& parameterName, std::string const& newDisplayName)
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (!editedParameter)
    {
        return false;
    }

    editedParameter->setDisplayName(QString::fromStdString(newDisplayName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getType()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getType(std::string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->getType().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setType()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setType(std::string const& parameterName, std::string const& newType)
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (!editedParameter)
    {
        return false;
    }

    editedParameter->setType(QString::fromStdString(newType));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getBitWidthLeftValue()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getBitWidthLeftValue(std::string const& parameterName,
    int const& baseNumber) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return parseExpressionToBaseNumber(editedParameter->getVectorLeft(), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getBitWidthLeftFormattedExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getBitWidthLeftFormattedExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return formattedValueFor(editedParameter->getVectorLeft()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getBitWidthLeftExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getBitWidthLeftExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->getVectorLeft().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setBitWidthLeft()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setBitWidthLeft(std::string const& parameterName,
    std::string const& newBitWidthLeft)
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (!editedParameter)
    {
        return false;
    }

    editedParameter->setVectorLeft(QString::fromStdString(newBitWidthLeft));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getBitWidthRightValue()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getBitWidthRightValue(std::string const& parameterName,
    int const& baseNumber) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return parseExpressionToBaseNumber(editedParameter->getVectorRight(), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getBitWidthRightFormattedExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getBitWidthRightFormattedExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return formattedValueFor(editedParameter->getVectorRight()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getBitWidthRightExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getBitWidthRightExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->getVectorRight().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setBitWidthRight()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setBitWidthRight(std::string const& parameterName,
    std::string const& newBitWidthRight)
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (!editedParameter)
    {
        return false;
    }

    editedParameter->setVectorRight(QString::fromStdString(newBitWidthRight));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getMinimum()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getMinimum(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getMinimumValue().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setMinimum()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setMinimum(std::string const& parameterName, std::string const& newMinimum)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setMinimumValue(QString::fromStdString(newMinimum));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getMaximum()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getMaximum(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getMaximumValue().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setMaximum()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setMaximum(std::string const& parameterName, std::string const& newMaximum)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setMaximumValue(QString::fromStdString(newMaximum));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getChoice()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getChoice(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getChoiceRef().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setChoice()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setChoice(std::string const& parameterName, std::string const& newChoice)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setChoiceRef(QString::fromStdString(newChoice));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getValue()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getValue(std::string const& parameterName, int const& baseNumber) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        if (parameter->getType() == QStringLiteral("string"))
        {
            return parameter->getValue().toStdString();
        }
        else
        {
            return parseExpressionToBaseNumber(parameter->getValue(), baseNumber).toStdString();
        }
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getValueFormattedExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getValueFormattedExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return evaluateValue(parameter).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::evaluateValue()
//-----------------------------------------------------------------------------
QString AbstractParameterInterface::evaluateValue(QSharedPointer<Parameter> parameter) const
{
    if (!parameter->getChoiceRef().isEmpty())
    {
        QSharedPointer<Choice> choice = findChoice(parameter->getChoiceRef());

        if (parameter->getValue().contains('{') && parameter->getValue().contains('}'))
        {
            return matchArrayValuesToSelectedChoice(choice, parameter->getValue());
        }

        else
        {
            return findDisplayValueForEnumeration(choice, parameter->getValue());
        }
    }
    else
    {
        return formattedValueFor(parameter->getValue());
    }
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> AbstractParameterInterface::findChoice(QString const& choiceName) const
{
    if (choices_)
    {
        foreach(QSharedPointer<Choice> choice, *choices_)
        {
            if (choice->name() == choiceName)
            {
                return choice;
            }
        }
    }

    return QSharedPointer<Choice>(new Choice());
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::matchArrayValuesToSelectedChoice()
//-----------------------------------------------------------------------------
QString AbstractParameterInterface::matchArrayValuesToSelectedChoice(QSharedPointer<Choice> choice,
    QString const& arrayValue) const
{
    QStringList parameterArray = arrayValue.split(',');
    parameterArray.first().remove('{');
    parameterArray.last().remove('}');

    QStringList resultingArray;

    foreach(QString value, parameterArray)
    {
        resultingArray.append(findDisplayValueForEnumeration(choice, value));
    }
    QString choiceValue = resultingArray.join(',');
    choiceValue.prepend('{');
    choiceValue.append('}');

    return choiceValue;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString AbstractParameterInterface::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
    QString const& enumerationValue) const
{
    QString formattedValue = formattedValueFor(enumerationValue);
    foreach(QSharedPointer<Enumeration> enumeration, *choice->enumerations())
    {
        QString enumerationFormattedValue = formattedValueFor(enumeration->getValue());
        if (enumerationFormattedValue == formattedValue && !enumeration->getText().isEmpty())
        {
            return enumeration->getText();
        }
    }

    QString value = formattedValueFor(enumerationValue);

    foreach(QSharedPointer<Enumeration> enumeration, *choice->enumerations())
    {
        if (enumeration->getValue() == value && !enumeration->getText().isEmpty())
        {
            return enumeration->getText();
        }
    }

    return value;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getValueExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getValueExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getValue().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setValue()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setValue(std::string const& parameterName, std::string const& newValue)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setValue(QString::fromStdString(newValue));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getResolve()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getResolve(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getValueResolve().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setResolve()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setResolve(std::string const& parameterName, std::string const& newResolve)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setValueResolve(QString::fromStdString(newResolve));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getArrayLeftValue()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getArrayLeftValue(std::string const& parameterName, int const& baseNumber)
const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parseExpressionToBaseNumber(parameter->getAttribute(ARRAY_LEFT_ATTRIBTE), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getArrayLeftFormattedExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getArrayLeftFormattedExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return formattedValueFor(parameter->getAttribute(ARRAY_LEFT_ATTRIBTE)).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getArrayLeftExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getArrayLeftExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getAttribute(ARRAY_LEFT_ATTRIBTE).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setArrayLeft()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setArrayLeft(std::string const& parameterName, std::string const& newArrayLeft)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }
    
    parameter->setAttribute(ARRAY_LEFT_ATTRIBTE, QString::fromStdString(newArrayLeft));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getArrayRightValue()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getArrayRightValue(std::string const& parameterName, int const& baseNumber)
const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parseExpressionToBaseNumber(
            parameter->getAttribute(ARRAY_RIGHT_ATTRIBUTE), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getArrayRightFormattedExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getArrayRightFormattedExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return formattedValueFor(parameter->getAttribute(ARRAY_RIGHT_ATTRIBUTE)).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getArrayRightExpression()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getArrayRightExpression(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getAttribute(ARRAY_RIGHT_ATTRIBUTE).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setArrayRight()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setArrayRight(std::string const& parameterName, std::string const& newArrayRight)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setAttribute(ARRAY_RIGHT_ATTRIBUTE, QString::fromStdString(newArrayRight));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getID()
//-----------------------------------------------------------------------------
std::string AbstractParameterInterface::getID(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getValueId().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setID()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setID(std::string const& parameterName, std::string const& newID)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setValueId(QString::fromStdString(newID));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getUsageCount()
//-----------------------------------------------------------------------------
int AbstractParameterInterface::getUsageCount(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getUsageCount();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setUsageCount()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::setUsageCount(std::string const& parameterName, int const& newUsageCount)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->setUsageCount(newUsageCount);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::setUsageCount()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::increaseUsageCount(std::string const& parameterName)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->increaseUsageCount();
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::reduceUsageCount()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::reduceUsageCount(std::string const& parameterName)
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (!parameter)
    {
        return false;
    }

    parameter->decreaseUsageCount();
    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int AbstractParameterInterface::getAllReferencesToIdInItem(const std::string& itemName,
    std::string const&  valueID) const
{
    QSharedPointer<Parameter> parameter = getParameter(itemName);

    QString idString = QString::fromStdString(valueID);

    int referencesInValue = parameter->getValue().count(idString);
    int referencesInBitWidthLeft = parameter->getVectorLeft().count(idString);
    int referencesInBitWidthRight = parameter->getVectorRight().count(idString);
    int referencesInArrayLeft = parameter->getAttribute("kactus2:arrayLeft").count(idString);
    int referencesinArrayRight = parameter->getAttribute("kactus2:arrayRight").count(idString);

    int totalReferencesToParameter = referencesInValue + referencesInBitWidthLeft + referencesInBitWidthRight +
        referencesInArrayLeft + referencesinArrayRight;

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::getExpressionsInSelectedItems()
//-----------------------------------------------------------------------------
std::vector<std::string> AbstractParameterInterface::getExpressionsInSelectedItems(
    std::vector<std::string> parameterNames) const
{
    std::vector<std::string> expressions;

    for (auto name : parameterNames)
    {
        QSharedPointer<Parameter> parameter = getParameter(name);
        if (parameter)
        {
            expressions.push_back(parameter->getValue().toStdString());
            expressions.push_back(parameter->getVectorLeft().toStdString());
            expressions.push_back(parameter->getVectorRight().toStdString());
            expressions.push_back(parameter->getAttribute("kactus2:arrayLeft").toStdString());
            expressions.push_back(parameter->getAttribute("kactus2:arrayRight").toStdString());
        }
    }

    return expressions;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::validateItems()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::validateItems() const
{
//     for (auto parameter : *parameters_)
    for (auto parameterName : getItemNames())
    {
        QSharedPointer<Parameter> parameter = getParameter(parameterName);
        if (parameter && !parameterValidator_->validate(parameter))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::itemHasValidName(std::string const& itemName) const
{
    return parameterValidator_->hasValidName(getParameter(itemName));
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidType()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidType(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidValueForType(parameter->getValue(), parameter->getType());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidBitWidth()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidBitWidth(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidVector(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidMinimum()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidMinimum(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidMinimumValue(parameter) &&
            !parameterValidator_->valueIsLessThanMinimum(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidMaximum()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidMaximum(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidMaximumValue(parameter) &&
            !parameterValidator_->valueIsGreaterThanMaximum(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidChoice()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidChoice(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidChoice(parameter) &&
            parameterValidator_->hasValidValueForChoice(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidValue()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidValue(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidValue(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidResolve()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidResolve(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidResolve(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterInterface::hasValidArrayValues()
//-----------------------------------------------------------------------------
bool AbstractParameterInterface::hasValidArrayValues(std::string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->validateArrayValues(parameter->getAttribute("kactus2:arrayLeft"),
            parameter->getAttribute("kactus2:arrayRight"));
    }

    return false;
}
