//-----------------------------------------------------------------------------
// File: FieldSliceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.09.2023
//
// Description:
// Validator for ipxact:fieldSlice.
//-----------------------------------------------------------------------------

#include "FieldSliceValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/FieldSlice.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>


#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::FieldSliceValidator()
//-----------------------------------------------------------------------------
FieldSliceValidator::FieldSliceValidator(QSharedPointer<Component> component, 
	QSharedPointer<ExpressionParser> expressionParser):
	component_(component),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::~FieldSliceValidator()
//-----------------------------------------------------------------------------
FieldSliceValidator::~FieldSliceValidator()
{

}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::validate()
//-----------------------------------------------------------------------------
bool FieldSliceValidator::validate(QSharedPointer<FieldSlice> fieldSlice) const
{
    return hasValidName(fieldSlice->name()) &&
        hasValidFieldReference(fieldSlice)  &&
        hasValidLeftRange(fieldSlice) &&
        hasValidRightRange(fieldSlice);
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::hasValidLeftRange()
//-----------------------------------------------------------------------------
bool FieldSliceValidator::hasValidLeftRange(QSharedPointer<FieldSlice> fieldSlice) const
{
    if (fieldSlice->getLeft().isEmpty() == false && fieldSlice->getRight().isEmpty())
    {
        return false;
    }

    if (fieldSlice->getLeft().isEmpty())
    {
        return true;
    }

    auto field = findField(fieldSlice);
    if (field.isNull())
    {
        return false;
    }


    int lsb = expressionParser_->parseExpression(field->getBitOffset()).toInt();
    int msb = lsb + expressionParser_->parseExpression(field->getBitWidth()).toInt() - 1;

    return isInValidRange(fieldSlice->getLeft(), msb, lsb);
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::hasValidRightRange()
//-----------------------------------------------------------------------------
bool FieldSliceValidator::hasValidRightRange(QSharedPointer<FieldSlice> fieldSlice) const
{
    if (fieldSlice->getRight().isEmpty() == false && fieldSlice->getLeft().isEmpty())
    {
        return false;
    }

    if (fieldSlice->getRight().isEmpty())
    {
        return true;
    }

    auto field = findField(fieldSlice);
    if (field.isNull())
    {
        return false;
    }

    int lsb = expressionParser_->parseExpression(field->getBitOffset()).toInt();
    int msb = lsb + expressionParser_->parseExpression(field->getBitWidth()).toInt() - 1;

    return isInValidRange(fieldSlice->getRight(), msb, lsb);
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::hasValidFieldReference()
//-----------------------------------------------------------------------------
bool FieldSliceValidator::hasValidFieldReference(QSharedPointer<FieldSlice> fieldSlice) const
{
    auto field = findField(fieldSlice);

    return field.isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool FieldSliceValidator::hasValidName(QString const& name) const
{
	return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FieldSliceValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<FieldSlice> fieldSlice,
    QString const& context) const
{
    if (hasValidName(fieldSlice->name()) == false)
    {
        errors.append(QObject::tr("Invalid name '%1' set for field condition within %2.").arg(
            fieldSlice->name(), context));
    }

    if (hasValidFieldReference(fieldSlice) == false)
    {
        errors.append(QObject::tr("Field reference in condition '%1' is not valid in %2.").arg(
            fieldSlice->name(), context));
    }

    if (hasValidLeftRange(fieldSlice) == false)
    {
        errors.append(QObject::tr("Left range in field condition '%1' is not valid in %2.").arg(
            fieldSlice->name(), context));
    }

    if (hasValidRightRange(fieldSlice) == false)
    {
        errors.append(QObject::tr("Right range in field condition '%1' is not valid in %2.").arg(
            fieldSlice->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::componentChange()
//-----------------------------------------------------------------------------
void FieldSliceValidator::componentChange(QSharedPointer<Component> newComponent)
{
	component_ = newComponent;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::isInValidRange()
//-----------------------------------------------------------------------------
bool FieldSliceValidator::isInValidRange(QString const& value, int leftBound, int rightBound) const
{
    bool validExpression = false;
    auto leftRange = expressionParser_->parseExpression(value, &validExpression).toInt();

    return validExpression && (rightBound <= leftRange && leftRange <= leftBound);
}

//-----------------------------------------------------------------------------
// Function: FieldSliceValidator::findField()
//-----------------------------------------------------------------------------
QSharedPointer<Field> FieldSliceValidator::findField(QSharedPointer<FieldSlice> fieldSlice) const
{
    auto addressSpace = findItem(fieldSlice->getReference(FieldReference::ADDRESS_SPACE),
        component_->getAddressSpaces());

    QSharedPointer<MemoryMapBase> memoryMap = nullptr;
    if (addressSpace)
    {
        memoryMap = addressSpace->getLocalMemoryMap();
    }
    else
    {
        memoryMap = findItem(fieldSlice->getReference(FieldReference::MEMORY_MAP), component_->getMemoryMaps());
    }

    if (memoryMap.isNull())
    {
        return nullptr;
    }

    auto addressBlock = findItem(fieldSlice->getReference(FieldReference::ADDRESS_BLOCK),
        memoryMap->getMemoryBlocks());
    if (addressBlock.isNull())
    {
        return nullptr;
    }

    auto reg = findItem(fieldSlice->getReference(FieldReference::REGISTER),
        addressBlock.dynamicCast<AddressBlock>()->getRegisterData());
    if (reg.isNull())
    {
        return nullptr;
    }

    auto field = findItem(fieldSlice->getReference(FieldReference::FIELD),
        reg.dynamicCast<Register>()->getFields());

    return field;
}
