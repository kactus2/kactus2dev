//-----------------------------------------------------------------------------
// File: ParametersInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.03.2020
//
// Description:
// Interface for editing parameters.
//-----------------------------------------------------------------------------

#include "ParametersInterface.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/Choice.h>

using namespace std;

namespace
{
    QString const ARRAY_LEFT_ATTRIBTE = QLatin1String("kactus2:arrayLeft");
    QString const ARRAY_RIGHT_ATTRIBUTE = QLatin1String("kactus2:arrayRight");
};

//-----------------------------------------------------------------------------
// Function: ParametersInterface::ParametersInterface()
//-----------------------------------------------------------------------------
ParametersInterface::ParametersInterface(QSharedPointer<ParameterValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
parameters_(),
moduleParameters_(),
parameterValidator_(validator),
choices_()
{

}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setParameters()
//-----------------------------------------------------------------------------
void ParametersInterface::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setModuleParameters()
//-----------------------------------------------------------------------------
void ParametersInterface::setModuleParameters(QSharedPointer<QList<QSharedPointer<ModuleParameter>>> newParameters)
{
    moduleParameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setChoices()
//-----------------------------------------------------------------------------
void ParametersInterface::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices)
{
    choices_ = newChoices;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getItemIndex()
//-----------------------------------------------------------------------------
int ParametersInterface::getItemIndex(string const& itemName) const
{
    if (parameters_)
    {
        for (int i = 0; i < parameters_->size(); ++i)
        {
            if (parameters_->at(i)->name().toStdString() == itemName)
            {
                return i;
            }
        }
    }
    if (moduleParameters_)
    {
        for (int i = 0; i < moduleParameters_->size(); ++i)
        {
            if (moduleParameters_->at(i)->name().toStdString() == itemName)
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
string ParametersInterface::getIndexedItemName(int const& itemIndex) const
{
    string parameterName = "";
    if (parameters_ && (itemIndex >= 0 && itemIndex < parameters_->size()))
    {
        parameterName = parameters_->at(itemIndex)->name().toStdString();
    }
    else if (moduleParameters_ && (itemIndex >= 0 && itemIndex < moduleParameters_->size()))
    {
        parameterName = moduleParameters_->at(itemIndex)->name().toStdString();
    }

    return parameterName;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::itemCount()
//-----------------------------------------------------------------------------
int ParametersInterface::itemCount() const
{
    if (parameters_)
    {
        return parameters_->count();
    }
    if (moduleParameters_)
    {
        return moduleParameters_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> ParametersInterface::getItemNames() const
{
    vector<string> names;
    if (parameters_)
    {
        for (auto parameter : *parameters_)
        {
            names.push_back(parameter->name().toStdString());
        }
    }
    if (moduleParameters_)
    {
        for (auto parameter : *moduleParameters_)
        {
            names.push_back(parameter->name().toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setName()
//-----------------------------------------------------------------------------
bool ParametersInterface::setName(string const& currentName, string const& newName)
{
    QSharedPointer<Parameter> editedParameter = getParameter(currentName);
    if (!editedParameter)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newName, "parameter");

    editedParameter->setName(uniqueNewName);
    return true;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getDescription()
//-----------------------------------------------------------------------------
string ParametersInterface::getDescription(string const& itemName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(itemName);
    if (editedParameter)
    {
        return editedParameter->description().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setDescription()
//-----------------------------------------------------------------------------
bool ParametersInterface::setDescription(string const& itemName, string const& newDescription)
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
// Function: ParametersInterface::getDisplayName()
//-----------------------------------------------------------------------------
string ParametersInterface::getDisplayName(string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->displayName().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setDisplayName()
//-----------------------------------------------------------------------------
bool ParametersInterface::setDisplayName(string const& parameterName, string const& newDisplayName)
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
// Function: ParametersInterface::getType()
//-----------------------------------------------------------------------------
string ParametersInterface::getType(string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->getType().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setType()
//-----------------------------------------------------------------------------
bool ParametersInterface::setType(string const& parameterName, string const& newType)
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
// Function: ParametersInterface::getBitWidthLeftValue()
//-----------------------------------------------------------------------------
string ParametersInterface::getBitWidthLeftValue(string const& parameterName, int const& baseNumber) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return parseExpressionToBaseNumber(editedParameter->getVectorLeft(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getBitWidthLeftFormattedExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getBitWidthLeftFormattedExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return formattedValueFor(editedParameter->getVectorLeft()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getBitWidthLeftExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getBitWidthLeftExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->getVectorLeft().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setBitWidthLeft()
//-----------------------------------------------------------------------------
bool ParametersInterface::setBitWidthLeft(string const& parameterName, string const& newBitWidthLeft)
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
// Function: ParametersInterface::getBitWidthRightValue()
//-----------------------------------------------------------------------------
string ParametersInterface::getBitWidthRightValue(string const& parameterName, int const& baseNumber) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return parseExpressionToBaseNumber(editedParameter->getVectorRight(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getBitWidthRightFormattedExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getBitWidthRightFormattedExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return formattedValueFor(editedParameter->getVectorRight()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getBitWidthRightExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getBitWidthRightExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> editedParameter = getParameter(parameterName);
    if (editedParameter)
    {
        return editedParameter->getVectorRight().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setBitWidthRight()
//-----------------------------------------------------------------------------
bool ParametersInterface::setBitWidthRight(string const& parameterName, string const& newBitWidthRight)
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
// Function: ParametersInterface::getMinimum()
//-----------------------------------------------------------------------------
string ParametersInterface::getMinimum(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getMinimumValue().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setMinimum()
//-----------------------------------------------------------------------------
bool ParametersInterface::setMinimum(string const& parameterName, string const& newMinimum)
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
// Function: ParametersInterface::getMaximum()
//-----------------------------------------------------------------------------
string ParametersInterface::getMaximum(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getMaximumValue().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setMaximum()
//-----------------------------------------------------------------------------
bool ParametersInterface::setMaximum(string const& parameterName, string const& newMaximum)
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
// Function: ParametersInterface::getChoice()
//-----------------------------------------------------------------------------
string ParametersInterface::getChoice(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getChoiceRef().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setChoice()
//-----------------------------------------------------------------------------
bool ParametersInterface::setChoice(string const& parameterName, string const& newChoice)
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
// Function: ParametersInterface::getValue()
//-----------------------------------------------------------------------------
string ParametersInterface::getValue(string const& parameterName, int const& baseNumber) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parseExpressionToBaseNumber(parameter->getValue(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getValueFormattedExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getValueFormattedExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return evaluateValue(parameter).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::evaluateValue()
//-----------------------------------------------------------------------------
QString ParametersInterface::evaluateValue(QSharedPointer<Parameter> parameter) const
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
QSharedPointer<Choice> ParametersInterface::findChoice(QString const& choiceName) const
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
// Function: ParametersInterface::matchArrayValuesToSelectedChoice()
//-----------------------------------------------------------------------------
QString ParametersInterface::matchArrayValuesToSelectedChoice(QSharedPointer<Choice> choice,
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
QString ParametersInterface::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
    QString const& enumerationValue) const
{
    QString value = parseExpressionToDecimal(enumerationValue);

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
// Function: ParametersInterface::getValueExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getValueExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getValue().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setValue()
//-----------------------------------------------------------------------------
bool ParametersInterface::setValue(string const& parameterName, string const& newValue)
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
string ParametersInterface::getResolve(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getValueResolve().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setResolve()
//-----------------------------------------------------------------------------
bool ParametersInterface::setResolve(string const& parameterName, string const& newResolve)
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
// Function: ParametersInterface::getArrayLeftValue()
//-----------------------------------------------------------------------------
string ParametersInterface::getArrayLeftValue(string const& parameterName, int const& baseNumber) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parseExpressionToBaseNumber(parameter->getAttribute(ARRAY_LEFT_ATTRIBTE), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getArrayLeftFormattedExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getArrayLeftFormattedExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return formattedValueFor(parameter->getAttribute(ARRAY_LEFT_ATTRIBTE)).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getArrayLeftExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getArrayLeftExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getAttribute(ARRAY_LEFT_ATTRIBTE).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setArrayLeft()
//-----------------------------------------------------------------------------
bool ParametersInterface::setArrayLeft(string const& parameterName, string const& newArrayLeft)
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
// Function: ParametersInterface::getArrayRightValue()
//-----------------------------------------------------------------------------
string ParametersInterface::getArrayRightValue(string const& parameterName, int const& baseNumber) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parseExpressionToBaseNumber(
            parameter->getAttribute(ARRAY_RIGHT_ATTRIBUTE), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getArrayRightFormattedExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getArrayRightFormattedExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return formattedValueFor(parameter->getAttribute(ARRAY_RIGHT_ATTRIBUTE)).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getArrayRightExpression()
//-----------------------------------------------------------------------------
string ParametersInterface::getArrayRightExpression(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getAttribute(ARRAY_RIGHT_ATTRIBUTE).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setArrayRight()
//-----------------------------------------------------------------------------
bool ParametersInterface::setArrayRight(string const& parameterName, string const& newArrayRight)
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
// Function: ParametersInterface::getID()
//-----------------------------------------------------------------------------
string ParametersInterface::getID(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getValueId().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setID()
//-----------------------------------------------------------------------------
bool ParametersInterface::setID(string const& parameterName, string const& newID)
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
// Function: ParametersInterface::getUsageCount()
//-----------------------------------------------------------------------------
int ParametersInterface::getUsageCount(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameter->getUsageCount();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setUsageCount()
//-----------------------------------------------------------------------------
bool ParametersInterface::setUsageCount(string const& parameterName, int const& newUsageCount)
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
// Function: ParametersInterface::setUsageCount()
//-----------------------------------------------------------------------------
bool ParametersInterface::increaseUsageCount(string const& parameterName)
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
// Function: ParametersInterface::reduceUsageCount()
//-----------------------------------------------------------------------------
bool ParametersInterface::reduceUsageCount(string const& parameterName)
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
// Function: ParametersInterface::addParameter()
//-----------------------------------------------------------------------------
void ParametersInterface::addParameter(int const& row, string const& newParameterName)
{
    QString parameterName = getUniqueName(newParameterName, "parameter");

    QSharedPointer<Parameter> newParameter(new Parameter());
    newParameter->setName(parameterName);

    parameters_->insert(row, newParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::addModuleParameter()
//-----------------------------------------------------------------------------
void ParametersInterface::addModuleParameter(int const& row, string const& newParameterName)
{
    QString parameterName = getUniqueName(newParameterName, "parameter");

    QSharedPointer<ModuleParameter> newParameter(new ModuleParameter());
    newParameter->setName(parameterName);

    moduleParameters_->insert(row, newParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::removeParameter()
//-----------------------------------------------------------------------------
bool ParametersInterface::removeParameter(string const& parameterName)
{
    QSharedPointer<Parameter> removedParameter = getParameter(parameterName);
    if (!removedParameter)
    {
        return false;
    }

    return parameters_->removeOne(removedParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::removeModuleParameter()
//-----------------------------------------------------------------------------
bool ParametersInterface::removeModuleParameter(string const& parameterName)
{
    QSharedPointer<ModuleParameter> removedParameter = getModuleParameter(parameterName);
    if (!removedParameter)
    {
        return false;
    }

    return moduleParameters_->removeOne(removedParameter);
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int ParametersInterface::getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const
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
// Function: ParametersInterface::validateItems()
//-----------------------------------------------------------------------------
bool ParametersInterface::validateItems() const
{
    for (auto parameter : *parameters_)
    {
        if (!parameterValidator_->validate(parameter))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool ParametersInterface::itemHasValidName(string const& itemName) const
{
    return parameterValidator_->hasValidName(getParameter(itemName));
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ParametersInterface::getParameter(string const& parameterName) const
{
    if (parameters_)
    {
        for (auto parameter : *parameters_)
        {
            if (parameter->name().toStdString() == parameterName)
            {
                return parameter;
            }
        }
    }
    else
    {
        return getModuleParameter(parameterName);
    }

    return QSharedPointer<Parameter>();
}


//-----------------------------------------------------------------------------
// Function: ParametersInterface::isModuleParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> ParametersInterface::getModuleParameter(string const& parameterName) const
{
    if (moduleParameters_)
    {
        for (auto parameter : *moduleParameters_)
        {
            if (parameter->name().toStdString() == parameterName)
            {
                return parameter;
            }
        }
    }

    return QSharedPointer<ModuleParameter>();
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::hasValidType()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidType(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidValueForType(parameter->getValue(), parameter->getType());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::hasValidBitWidth()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidBitWidth(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidVector(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::hasValidMinimum()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidMinimum(string const& parameterName) const
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
// Function: ParametersInterface::hasValidMaximum()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidMaximum(string const& parameterName) const
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
// Function: ParametersInterface::hasValidChoice()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidChoice(string const& parameterName) const
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
// Function: ParametersInterface::hasValidValue()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidValue(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidValue(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::hasValidResolve()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidResolve(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->hasValidResolve(parameter);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::hasValidArrayValues()
//-----------------------------------------------------------------------------
bool ParametersInterface::hasValidArrayValues(string const& parameterName) const
{
    QSharedPointer<Parameter> parameter = getParameter(parameterName);
    if (parameter)
    {
        return parameterValidator_->validateArrayValues(parameter->getAttribute("kactus2:arrayLeft"),
            parameter->getAttribute("kactus2:arrayRight"));
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getDataType()
//-----------------------------------------------------------------------------
string ParametersInterface::getDataType(string const& parameterName) const
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (moduleParameter)
    {
        return moduleParameter->getDataType().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setDataType()
//-----------------------------------------------------------------------------
bool ParametersInterface::setDataType(string const& parameterName, string const& newDataType)
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (!moduleParameter)
    {
        return false;
    }

    moduleParameter->setDataType(QString::fromStdString(newDataType));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::getUsageType()
//-----------------------------------------------------------------------------
string ParametersInterface::getUsageType(string const& parameterName) const
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (moduleParameter)
    {
        return moduleParameter->getUsageType().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ParametersInterface::setUsageType()
//-----------------------------------------------------------------------------
bool ParametersInterface::setUsageType(string const& parameterName, string const& newUsageType)
{
    QSharedPointer<ModuleParameter> moduleParameter = getModuleParameter(parameterName);
    if (!moduleParameter)
    {
        return false;
    }

    moduleParameter->setUsageType(QString::fromStdString(newUsageType));
    return true;
}
