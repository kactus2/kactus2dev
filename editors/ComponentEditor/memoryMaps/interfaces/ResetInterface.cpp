//-----------------------------------------------------------------------------
// File: ResetInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.04.2020
//
// Description:
// Interface for editing resets.
//-----------------------------------------------------------------------------

#include "ResetInterface.h"

#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/FieldReset.h>

#include <IPXACTmodels/Component/validators/FieldValidator.h>

using namespace std;

namespace
{
    string const DEFAULT_TYPE = "";
};

//-----------------------------------------------------------------------------
// Function: ResetInterface::ResetInterface()
//-----------------------------------------------------------------------------
ResetInterface::ResetInterface(QSharedPointer<FieldValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
CommonInterface(),
containingField_(),
resets_(),
validator_(validator)
{
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::setResets()
//-----------------------------------------------------------------------------
void ResetInterface::setResets(QSharedPointer<Field> containingField)
{
    containingField_ = containingField;
    resets_ = containingField->getResets();
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getReset()
//-----------------------------------------------------------------------------
QSharedPointer<FieldReset> ResetInterface::getReset(int const& resetIndex) const
{
    if (resetIndex >= 0 && resetIndex < resets_->size())
    {
        return resets_->at(resetIndex);
    }

    return QSharedPointer<FieldReset>();
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> ResetInterface::getItemNames() const
{
    vector<string> resetNames;
    for (auto reset : *resets_)
    {
        resetNames.push_back(reset->getResetTypeReference().toStdString());
    }

    return resetNames;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::itemCount()
//-----------------------------------------------------------------------------
int ResetInterface::itemCount() const
{
    if (resets_)
    {
        return resets_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetTypeReference()
//-----------------------------------------------------------------------------
string ResetInterface::getResetTypeReference(int const& itemIndex) const
{
    string resetType = "";
    if (itemIndex >= 0 && itemIndex < resets_->size())
    {
        resetType = resets_->at(itemIndex)->getResetTypeReference().toStdString();
    }

    return resetType;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::setResetTypeReference()
//-----------------------------------------------------------------------------
bool ResetInterface::setResetTypeReference(int const& resetIndex, std::string const& newResetType)
{
    QSharedPointer<FieldReset> editedReset = getReset(resetIndex);
    if (!editedReset)
    {
        return false;
    }

    editedReset->setResetTypeReference(QString::fromStdString(newResetType));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetValue()
//-----------------------------------------------------------------------------
std::string ResetInterface::getResetValue(int const& resetIndex, int const& baseNumber) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return parseExpressionToBaseNumber(reset->getResetValue(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetValueFormattedExpression()
//-----------------------------------------------------------------------------
std::string ResetInterface::getResetValueFormattedExpression(int const& resetIndex) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return formattedValueFor(reset->getResetValue()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetValueExpression()
//-----------------------------------------------------------------------------
std::string ResetInterface::getResetValueExpression(int const& resetIndex) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return reset->getResetValue().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::setResetValue()
//-----------------------------------------------------------------------------
bool ResetInterface::setResetValue(int const& resetIndex, std::string const& newResetValue)
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (!reset)
    {
        return false;
    }

    reset->setResetValue(QString::fromStdString(newResetValue));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetMaskValue()
//-----------------------------------------------------------------------------
std::string ResetInterface::getResetMaskValue(int const& resetIndex, int const& baseNumber /* = 0 */) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return parseExpressionToBaseNumber(reset->getResetMask(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetMaskFormattedExpression()
//-----------------------------------------------------------------------------
std::string ResetInterface::getResetMaskFormattedExpression(int const& resetIndex) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return formattedValueFor(reset->getResetMask()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetMaskExpression()
//-----------------------------------------------------------------------------
std::string ResetInterface::getResetMaskExpression(int const& resetIndex) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return reset->getResetMask().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::setResetMask()
//-----------------------------------------------------------------------------
bool ResetInterface::setResetMask(int const& resetIndex, std::string const& newResetMask)
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (!reset)
    {
        return false;
    }

    reset->setResetMask(QString::fromStdString(newResetMask));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::validateItems()
//-----------------------------------------------------------------------------
bool ResetInterface::validateItems() const
{
    return validator_->hasValidResets(containingField_);
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::hasValidResetType()
//-----------------------------------------------------------------------------
bool ResetInterface::hasValidResetType(int const& resetIndex) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return validator_->hasValidResetTypeReference(reset);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::hasValidResetValue()
//-----------------------------------------------------------------------------
bool ResetInterface::hasValidResetValue(int const& resetIndex) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return validator_->hasValidResetValue(reset);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::hasValidResetMask()
//-----------------------------------------------------------------------------
bool ResetInterface::hasValidResetMask(int const& resetIndex) const
{
    QSharedPointer<FieldReset> reset = getReset(resetIndex);
    if (reset)
    {
        return validator_->hasValidResetMask(reset);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::addReset()
//-----------------------------------------------------------------------------
void ResetInterface::addReset(int const& row)
{
    QSharedPointer<FieldReset> newReset(new FieldReset());
    resets_->insert(row, newReset);
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::removeReset()
//-----------------------------------------------------------------------------
bool ResetInterface::removeReset(int const& resetIndex)
{
    QSharedPointer<FieldReset> removedReset = getReset(resetIndex);
    if (!removedReset)
    {
        return false;
    }

    return resets_->removeOne(removedReset);
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int ResetInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
{
    for (int i = 0; i < itemCount(); ++i)
    {
        if (getResetTypeReference(i) == itemName)
        {
            return getAllReferencesToIdInIndex(i, valueID);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getAllReferencesToIdInIndex()
//-----------------------------------------------------------------------------
int ResetInterface::getAllReferencesToIdInIndex(int const& itemIndex, std::string const& valueID) const
{
    int totalReferencesToParameter = 0;
    QSharedPointer<FieldReset> reset = getReset(itemIndex);
    if (reset)
    {
        QString idString = QString::fromStdString(valueID);

        int referencesInResetValue = reset->getResetValue().count(idString);
        int referencesInResetMask = reset->getResetMask().count(idString);

        totalReferencesToParameter = referencesInResetValue + referencesInResetMask;
    }

    return totalReferencesToParameter;
}
