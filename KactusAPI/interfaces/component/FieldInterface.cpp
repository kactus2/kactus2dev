//-----------------------------------------------------------------------------
// File: FieldInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.04.2020
//
// Description:
// Interface for editing registers.
//-----------------------------------------------------------------------------

#include "FieldInterface.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>

#include <FieldExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <ResetInterface.h>

#include <QMimeData>
#include <QApplication>
#include <QClipboard>

using namespace std;

namespace
{
    QString const TRUE_STRING = QLatin1String("true");
    QString const FALSE_STRING = QLatin1String("false");

    std::string const FIELD_TYPE = "field";
};

//-----------------------------------------------------------------------------
// Function: FieldInterface::FieldInterface()
//-----------------------------------------------------------------------------
FieldInterface::FieldInterface(QSharedPointer<FieldValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    ResetInterface* subInterface):
ParameterizableInterface(expressionParser, expressionFormatter),
fields_(),
validator_(validator),
subInterface_(subInterface)
{

}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setFields()
//-----------------------------------------------------------------------------
void FieldInterface::setFields(QSharedPointer<QList<QSharedPointer<Field> > > newFields)
{
    fields_ = newFields;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getItemIndex()
//-----------------------------------------------------------------------------
int FieldInterface::getItemIndex(string const& itemName) const
{
    if (fields_)
    {
        for (int i = 0; i < fields_->size(); ++i)
        {
            if (fields_->at(i)->name().toStdString() == itemName)
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
string FieldInterface::getIndexedItemName(int const& itemIndex) const
{
    string fieldName = "";
    if (fields_&& (itemIndex >= 0 && itemIndex < fields_->size()))
    {
        fieldName = fields_->at(itemIndex)->name().toStdString();
    }

    return fieldName;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::itemCount()
//-----------------------------------------------------------------------------
int FieldInterface::itemCount() const
{
    if (fields_)
    {
        return fields_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> FieldInterface::getItemNames() const
{
    vector<string> names;
    if (fields_)
    {
        for (auto field : *fields_)
        {
            names.push_back(field->name().toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setName()
//-----------------------------------------------------------------------------
bool FieldInterface::setName(string const& currentName, string const& newName)
{
    QSharedPointer<Field> editedField = getField(currentName);
    if (editedField && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueName(newName, FIELD_TYPE);

        editedField->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getOffsetValue()
//-----------------------------------------------------------------------------
string FieldInterface::getOffsetValue(string const& fieldName, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return parseExpressionToBaseNumber(field->getBitOffset(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getOffsetFormattedExpression()
//-----------------------------------------------------------------------------
string FieldInterface::getOffsetFormattedExpression(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return formattedValueFor(field->getBitOffset()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getOffsetExpression()
//-----------------------------------------------------------------------------
string FieldInterface::getOffsetExpression(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getBitOffset().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setOffset()
//-----------------------------------------------------------------------------
bool FieldInterface::setOffset(string const& fieldName, string const& newOffset)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setBitOffset(QString::fromStdString(newOffset));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWidthValue()
//-----------------------------------------------------------------------------
string FieldInterface::getWidthValue(string const& fieldName, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return parseExpressionToBaseNumber(field->getBitWidth(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWidthFormattedExpression()
//-----------------------------------------------------------------------------
string FieldInterface::getWidthFormattedExpression(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return formattedValueFor(field->getBitWidth()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWidthExpression()
//-----------------------------------------------------------------------------
string FieldInterface::getWidthExpression(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getBitWidth().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setWidth()
//-----------------------------------------------------------------------------
bool FieldInterface::setWidth(string const& fieldName, string const& newWidth)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setBitWidth(QString::fromStdString(newWidth));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getResets()
//-----------------------------------------------------------------------------
string FieldInterface::getResets(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return string("");
    }

    QSharedPointer<QList<QSharedPointer<FieldReset> > > indexedResets = field->getResets();
    if (indexedResets->isEmpty())
    {
        return string("");
    }
    else if (indexedResets->count() == 1)
    {
        return indexedResets->first()->getResetValue().toStdString();
    }
    else
    {
        return string("[multiple]");
    }
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getResetsToolTip()
//-----------------------------------------------------------------------------
string FieldInterface::getResetsToolTip(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        QSharedPointer<QList<QSharedPointer<FieldReset> > > resets = field->getResets();
        if (resets && !resets->isEmpty())
        {
            QString tooltip = "<html><head/><body><p>";
            for (int i = 0; i < resets->size(); ++i)
            {
                if (i > 0)
                {
                    tooltip.append("<br>");
                }

                QSharedPointer<FieldReset> fieldReset = resets->at(i);
                QString resetReference = fieldReset->getResetTypeReference();
                if (resetReference.isEmpty())
                {
                    resetReference = "HARD";
                }

                QString resetValue = formattedValueFor(fieldReset->getResetValue());

                tooltip.append(resetReference + " : " + resetValue);
            }

            tooltip.append("</p></body></html>");

            return tooltip.toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getVolatile()
//-----------------------------------------------------------------------------
string FieldInterface::getVolatile(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getVolatile().toString().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setVolatile()
//-----------------------------------------------------------------------------
bool FieldInterface::setVolatile(string const& fieldName, string const& newVolatile)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    QString newVolatileQT = QString::fromStdString(newVolatile);

    if (newVolatileQT == TRUE_STRING)
    {
        field->setVolatile(true);
    }
    else if (newVolatileQT == FALSE_STRING)
    {
        field->setVolatile(false);
    }
    else
    {
        field->clearVolatile();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAccessString()
//-----------------------------------------------------------------------------
string FieldInterface::getAccessString(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return AccessTypes::access2Str(field->getAccess()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAccessString()
//-----------------------------------------------------------------------------
string FieldInterface::getAccessString(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        auto fieldAccessPolicies = field->getFieldAccessPolicies();

        if (accessPolicyIndex <= fieldAccessPolicies->size() - 1)
        {
            auto access = fieldAccessPolicies->at(accessPolicyIndex)->getAccess();
            return AccessTypes::access2Str(access).toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getModeRefs()
//-----------------------------------------------------------------------------
std::vector<std::string> FieldInterface::getModeRefs(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        auto fieldAccessPolicies = field->getFieldAccessPolicies();

        if (accessPolicyIndex <= fieldAccessPolicies->size() - 1)
        {
            auto modeRefs = fieldAccessPolicies->at(accessPolicyIndex)->getModeReferences();
            vector<string> modeRefList;
            std::transform(modeRefs->cbegin(), modeRefs->cend(), std::back_inserter(modeRefList), [](QSharedPointer<ModeReference> const& modeRef)
                {
                    return modeRef->getReference().toStdString();
                });

            // return only mode names for now
            return modeRefList;
        }
    }

    return vector<string>();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAccessPolicyCount()
//-----------------------------------------------------------------------------
int FieldInterface::getAccessPolicyCount(std::string const& fieldName) const
{
    auto field = getField(fieldName);
    if (field)
    {
        return field->getFieldAccessPolicies()->size();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAccessType()
//-----------------------------------------------------------------------------
AccessTypes::Access FieldInterface::getAccessType(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getAccess();
    }

    return AccessTypes::ACCESS_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setAccess()
//-----------------------------------------------------------------------------
bool FieldInterface::setAccess(string const& fieldName, string const& newAccess)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setAccess(AccessTypes::str2Access(QString::fromStdString(newAccess), AccessTypes::ACCESS_COUNT));

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getModifiedWriteString()
//-----------------------------------------------------------------------------
string FieldInterface::getModifiedWriteString(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return General::modifiedWrite2Str(field->getModifiedWrite()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getModifiedWriteValue()
//-----------------------------------------------------------------------------
General::ModifiedWrite FieldInterface::getModifiedWriteValue(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getModifiedWrite();
    }

    return General::MODIFIED_WRITE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setModifiedWrite()
//-----------------------------------------------------------------------------
bool FieldInterface::setModifiedWrite(string const& fieldName, string const& newModifiedWrite)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setModifiedWrite(General::str2ModifiedWrite(QString::fromStdString(newModifiedWrite)));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReadActiongetReadActionString()
//-----------------------------------------------------------------------------
string FieldInterface::getReadActionString(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return General::readAction2Str(field->getReadAction()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReadAction()
//-----------------------------------------------------------------------------
General::ReadAction FieldInterface::getReadAction(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getReadAction();
    }

    return General::READ_ACTION_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setReadAction()
//-----------------------------------------------------------------------------
bool FieldInterface::setReadAction(string const& fieldName, string const& newReadAction)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setReadAction(General::str2ReadAction(QString::fromStdString(newReadAction)));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestable()
//-----------------------------------------------------------------------------
string FieldInterface::getTestableValue(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getTestable().toString().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestableBool()
//-----------------------------------------------------------------------------
bool FieldInterface::getTestableBool(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getTestable().toBool();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setTestable()
//-----------------------------------------------------------------------------
bool FieldInterface::setTestable(string const& fieldName, string const& newTestable)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    QString newTestableQT = QString::fromStdString(newTestable);
    if (newTestableQT == TRUE_STRING)
    {
        field->setTestable(true);
    }
    else if (newTestableQT == FALSE_STRING)
    {
        field->setTestable(false);
        field->setTestConstraint(General::TESTCONSTRAINT_COUNT);
    }
    else
    {
        field->clearTestable();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestConstraintString()
//-----------------------------------------------------------------------------
string FieldInterface::getTestConstraintString(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return General::testConstraint2Str(field->getTestConstraint()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestConstraint()
//-----------------------------------------------------------------------------
General::TestConstraint FieldInterface::getTestConstraint(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getTestConstraint();
    }

    return General::TESTCONSTRAINT_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setTestConstraint()
//-----------------------------------------------------------------------------
bool FieldInterface::setTestConstraint(string const& fieldName, string const& newTestConstraint)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setTestConstraint(General::str2TestConstraint(QString::fromStdString(newTestConstraint)));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getIsPresentValue()
//-----------------------------------------------------------------------------
string FieldInterface::getIsPresentValue(string const& fieldName, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return parseExpressionToBaseNumber(field->getIsPresent(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getIsPresentFormattedExpression()
//-----------------------------------------------------------------------------
string FieldInterface::getIsPresentFormattedExpression(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return formattedValueFor(field->getIsPresent()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getIsPresentExpression()
//-----------------------------------------------------------------------------
string FieldInterface::getIsPresentExpression(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getIsPresent().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setIsPresent()
//-----------------------------------------------------------------------------
bool FieldInterface::setIsPresent(string const& fieldName, string const& newIsPresent)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setIsPresent(QString::fromStdString(newIsPresent));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int FieldInterface::getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const
{
    QSharedPointer<Field> field = getField(itemName);

    QString idString = QString::fromStdString(valueID);

    int referencesInOffset = field->getBitOffset().count(idString);
    int referencesInWidth = field->getBitWidth().count(idString);
    int refrencesInIsPresent = field->getIsPresent().count(idString);

    int totalReferencesToParameter = referencesInOffset + referencesInWidth + refrencesInIsPresent;

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::validateItems()
//-----------------------------------------------------------------------------
bool FieldInterface::validateItems() const
{
    for (auto field : *fields_)
    {
        if (!validator_->validate(field))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool FieldInterface::itemHasValidName(string const& itemName) const
{
    return validator_->hasValidName(getField(itemName));
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidOffset()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidOffset(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return validator_->hasValidBitOffset(field);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidWidth()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidWidth(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return validator_->hasValidBitWidth(field);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidResets()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidResets(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return validator_->hasValidResets(field);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidIsPresent(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return validator_->hasValidIsPresent(field);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidAccess()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidAccess(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return validator_->hasValidAccess(field);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getField()
//-----------------------------------------------------------------------------
QSharedPointer<Field> FieldInterface::getField(string const& fieldName) const
{
    if (fields_)
    {
        for (auto field : *fields_)
        {
            if (field->name().toStdString() == fieldName)
            {
                return field;
            }
        }
    }

    return QSharedPointer<Field>();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::addField()
//-----------------------------------------------------------------------------
void FieldInterface::addField(int const& row, string const& newFieldName)
{
    QString fieldName = getUniqueName(newFieldName, FIELD_TYPE);

    QSharedPointer<Field> newField(new Field());
    newField->setName(fieldName);

    fields_->insert(row, newField);
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::removeField()
//-----------------------------------------------------------------------------
bool FieldInterface::removeField(string const& fieldName)
{
    QSharedPointer<Field> removedField = getField(fieldName);
    if (!removedField)
    {
        return false;
    }

    return fields_->removeOne(removedField);
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::copyRows()
//-----------------------------------------------------------------------------
void FieldInterface::copyRows(std::vector<int> selectedRows)
{
    QList<QSharedPointer<Field> > copiedFields;
    for (auto index : selectedRows)
    {
        QSharedPointer<Field> field = fields_->at(index);
        copiedFields.append(field);
    }

    QVariant fieldVariant;
    fieldVariant.setValue(copiedFields);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:field", QByteArray());
    newMimeData->setImageData(fieldVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::pasteRows()
//-----------------------------------------------------------------------------
std::vector<std::string> FieldInterface::pasteRows()
{
    std::vector<std::string> pastedFieldNames;

    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Field> > >())
        {
            QList<QSharedPointer<Field> > newFields = pasteVariant.value<QList<QSharedPointer<Field> > >();

            foreach(QSharedPointer<Field> copiedField, newFields)
            {
                QSharedPointer<Field> newField(new Field(*copiedField.data()));
                newField->setName(getUniqueName(newField->name().toStdString(), FIELD_TYPE));
                fields_->append(newField);

                pastedFieldNames.push_back(newField->name().toStdString());
            }
        }
    }

    return pastedFieldNames;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getPasteRowCount()
//-----------------------------------------------------------------------------
int FieldInterface::getPasteRowCount() const
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();
    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Field> > >())
        {
            QList<QSharedPointer<Field> > newFields = pasteVariant.value<QList<QSharedPointer<Field> > >();
            return newFields.count();
        }
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getExpressionsInSelectedFields()
//-----------------------------------------------------------------------------
std::vector<std::string> FieldInterface::getExpressionsInSelectedFields(std::vector<std::string> fieldNames) const
{
    std::vector<std::string> expressionList;

    FieldExpressionsGatherer gatherer;

    for (auto name : fieldNames)
    {
        QSharedPointer<Field> field = getField(name);
        QStringList fieldExpressions = gatherer.getExpressions(field);
        for (auto expression : fieldExpressions)
        {
            expressionList.push_back(expression.toStdString());
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getSubInterface()
//-----------------------------------------------------------------------------
ResetInterface* FieldInterface::getSubInterface() const
{
    return subInterface_;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasWriteConstraint()
//-----------------------------------------------------------------------------
bool FieldInterface::hasWriteConstraint(std::string const& fieldName) const
{
    QSharedPointer<WriteValueConstraint> fieldWriteConstraint = getWriteValueConstraint(fieldName);
    if (fieldWriteConstraint)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraint()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraint(std::string const& fieldName) const
{
    std::string writeConstraintText =
        WriteValueConversions::typeToString(WriteValueConstraint::TYPE_COUNT).toStdString();

    QSharedPointer<WriteValueConstraint> fieldWriteConstraint = getWriteValueConstraint(fieldName);
    if (fieldWriteConstraint)
    {
        writeConstraintText = WriteValueConversions::typeToString(fieldWriteConstraint->getType()).toStdString();
    }

    return writeConstraintText;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteValueConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<WriteValueConstraint> FieldInterface::getWriteValueConstraint(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        QSharedPointer<WriteValueConstraint> fieldWriteConstraint = field->getWriteConstraint();
        if (fieldWriteConstraint)
        {
            return fieldWriteConstraint;
        }
    }

    return QSharedPointer<WriteValueConstraint>();
}


//-----------------------------------------------------------------------------
// Function: FieldInterface::setWriteConstraint()
//-----------------------------------------------------------------------------
bool FieldInterface::setWriteConstraint(std::string const& fieldName, std::string const& newConstraintText)
{
    QSharedPointer<Field> selectedField = getField(fieldName);
    if (!selectedField)
    {
        return false;
    }

    QSharedPointer<WriteValueConstraint> writeConstraint = selectedField->getWriteConstraint();
    if (writeConstraint)
    {
        WriteValueConstraint::Type constraintType =
            WriteValueConversions::stringToType(QString::fromStdString(newConstraintText));

        if (constraintType == WriteValueConstraint::TYPE_COUNT)
        {
            selectedField->setWriteConstraint(QSharedPointer<WriteValueConstraint>(0));
        }
        else
        {
            writeConstraint->setType(constraintType);
        }
    }
    else
    {
        QSharedPointer<WriteValueConstraint> newConstraint(new WriteValueConstraint());
        WriteValueConstraint::Type constraintType =
            WriteValueConversions::stringToType(QString::fromStdString(newConstraintText));

        if (constraintType != WriteValueConstraint::TYPE_COUNT)
        {
            newConstraint->setType(constraintType);
            selectedField->setWriteConstraint(newConstraint);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMinimumValue()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMinimumValue(std::string const& fieldName, int const& baseNumber)
    const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (constraint)
    {
        return parseExpressionToBaseNumber(constraint->getMinimum(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMinimumFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMinimumFormattedExpression(std::string const& fieldName) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (constraint)
    {
        return formattedValueFor(constraint->getMinimum()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMinimumExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMinimumExpression(std::string const& fieldName) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (constraint)
    {
        return constraint->getMinimum().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setWriteConstraintMinimum()
//-----------------------------------------------------------------------------
bool FieldInterface::setWriteConstraintMinimum(std::string const& fieldName,
    std::string const& newWriteConstraintMinimum)
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (!constraint)
    {
        return false;
    }

    constraint->setMinimum(QString::fromStdString(newWriteConstraintMinimum));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMaximumValue()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMaximumValue(std::string const& fieldName, int const& baseNumber)
const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (constraint)
    {
        return parseExpressionToBaseNumber(constraint->getMaximum(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMaximumFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMaximumFormattedExpression(std::string const& fieldName) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (constraint)
    {
        return formattedValueFor(constraint->getMaximum()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMaximumExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMaximumExpression(std::string const& fieldName) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (constraint)
    {
        return constraint->getMaximum().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setWriteConstraintMaximum()
//-----------------------------------------------------------------------------
bool FieldInterface::setWriteConstraintMaximum(std::string const& fieldName,
    std::string const& newWriteConstraintMaximum)
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName);
    if (!constraint)
    {
        return false;
    }

    constraint->setMaximum(QString::fromStdString(newWriteConstraintMaximum));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReservedValue()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReservedValue(std::string const& fieldName, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return parseExpressionToBaseNumber(field->getReserved(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReservedFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReservedFormattedExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return formattedValueFor(field->getReserved()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReservedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReservedExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getReserved().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setReserved()
//-----------------------------------------------------------------------------
bool FieldInterface::setReserved(std::string const& fieldName, std::string const& newReserved)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setReserved(QString::fromStdString(newReserved));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getID()
//-----------------------------------------------------------------------------
std::string FieldInterface::getID(std::string const& fieldName)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getId().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setID()
//-----------------------------------------------------------------------------
bool FieldInterface::setID(std::string const& fieldName, std::string const& newID)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    field->setId(QString::fromStdString(newID));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> FieldInterface::getItem(std::string const& fieldName) const
{
    return getField(fieldName);
}
