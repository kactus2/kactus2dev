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
ResetInterface::ResetInterface():
ParameterizableInterface(),
CommonInterface(),
containingField_(),
resets_()
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
// Function: ResetInterface::setValidator()
//-----------------------------------------------------------------------------
void ResetInterface::setValidator(QSharedPointer<FieldValidator> validator)
{
    validator_ = validator;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getReset()
//-----------------------------------------------------------------------------
QSharedPointer<FieldReset> ResetInterface::getReset(string const& resetType) const
{
    if (resets_)
    {
        for (auto fieldReset : *resets_)
        {
            if (fieldReset->getResetTypeReference().toStdString() == resetType)
            {
                return fieldReset;
            }
        }
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
bool ResetInterface::setResetTypeReference(string const& currentResetType, string const& newResetType)
{
    QSharedPointer<FieldReset> editedReset = getReset(currentResetType);
    if (!editedReset)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newResetType, DEFAULT_TYPE);

    editedReset->setResetTypeReference(uniqueNewName);
    return true;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetValue()
//-----------------------------------------------------------------------------
string ResetInterface::getResetValue(string const& resetType, int const& baseNumber) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return parseExpressionToBaseNumber(reset->getResetValue(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetValueFormattedExpression()
//-----------------------------------------------------------------------------
string ResetInterface::getResetValueFormattedExpression(string const& resetType) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return formattedValueFor(reset->getResetValue()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetValueExpression()
//-----------------------------------------------------------------------------
string ResetInterface::getResetValueExpression(string const& resetType) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return reset->getResetValue().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::setResetValue()
//-----------------------------------------------------------------------------
bool ResetInterface::setResetValue(string const& resetType, string const& newResetValue)
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
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
string ResetInterface::getResetMaskValue(string const& resetType, int const& baseNumber) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return parseExpressionToBaseNumber(reset->getResetMask(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetMaskFormattedExpression()
//-----------------------------------------------------------------------------
string ResetInterface::getResetMaskFormattedExpression(string const& resetType) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return formattedValueFor(reset->getResetMask()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::getResetMaskExpression()
//-----------------------------------------------------------------------------
string ResetInterface::getResetMaskExpression(string const& resetType) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return reset->getResetMask().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::setResetMask()
//-----------------------------------------------------------------------------
bool ResetInterface::setResetMask(string const& resetType, string const& newResetValue)
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (!reset)
    {
        return false;
    }

    reset->setResetMask(QString::fromStdString(newResetValue));
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
bool ResetInterface::hasValidResetType(string const& resetType) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return validator_->hasValidResetTypeReference(reset);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::hasValidResetValue()
//-----------------------------------------------------------------------------
bool ResetInterface::hasValidResetValue(string const& resetType) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return validator_->hasValidResetValue(reset);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::hasValidResetMask()
//-----------------------------------------------------------------------------
bool ResetInterface::hasValidResetMask(std::string const& resetType) const
{
    QSharedPointer<FieldReset> reset = getReset(resetType);
    if (reset)
    {
        return validator_->hasValidResetMask(reset);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::addReset()
//-----------------------------------------------------------------------------
void ResetInterface::addReset(int const& row, std::string const& newResetName)
{
    QString resetName = getUniqueName(newResetName, DEFAULT_TYPE);

    QSharedPointer<FieldReset> newReset(new FieldReset());
    newReset->setResetTypeReference(resetName);

    resets_->insert(row, newReset);
}

//-----------------------------------------------------------------------------
// Function: ResetInterface::removeReset()
//-----------------------------------------------------------------------------
bool ResetInterface::removeReset(std::string const& resetName)
{
    QSharedPointer<FieldReset> removedReset = getReset(resetName);
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
    QSharedPointer<FieldReset> reset = getReset(itemName);

    QString idString = QString::fromStdString(valueID);

    int referencesInResetValue = reset->getResetValue().count(idString);
    int referencesInResetMask = reset->getResetMask().count(idString);

    int totalReferencesToParameter = referencesInResetValue + referencesInResetMask;

    return totalReferencesToParameter;
}
