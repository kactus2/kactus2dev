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
#include <IPXACTmodels/Component/validators/FieldValidator.h>

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/FieldExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/ResetInterface.h>

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
FieldInterface::FieldInterface() :
ParameterizableInterface(),
fields_(),
validator_(),
resetInterfaces_()
{

}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setParameters()
//-----------------------------------------------------------------------------
void FieldInterface::setFields(QSharedPointer<QList<QSharedPointer<Field> > > newFields)
{
    fields_ = newFields;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setValidator()
//-----------------------------------------------------------------------------
void FieldInterface::setValidator(QSharedPointer<FieldValidator> validator)
{
    validator_ = validator;
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
    if (!editedField)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newName, FIELD_TYPE);

    editedField->setName(uniqueNewName);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getDescription()
//-----------------------------------------------------------------------------
string FieldInterface::getDescription(string const& itemName) const
{
    QSharedPointer<Field> editedField = getField(itemName);
    if (editedField)
    {
        return editedField->description().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setDescription()
//-----------------------------------------------------------------------------
bool FieldInterface::setDescription(string const& itemName, string const& newDescription)
{
    QSharedPointer<Field> editedField = getField(itemName);
    if (!editedField)
    {
        return false;
    }

    editedField->setDescription(QString::fromStdString(newDescription));
    return true;
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
// Function: FieldInterface::getAccess()
//-----------------------------------------------------------------------------
string FieldInterface::getAccess(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return AccessTypes::access2Str(field->getAccess()).toStdString();
    }

    return string("");
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
// Function: FieldInterface::getModifiedWrite()
//-----------------------------------------------------------------------------
string FieldInterface::getModifiedWrite(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return General::modifiedWrite2Str(field->getModifiedWrite()).toStdString();
    }

    return string("");
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
// Function: FieldInterface::getReadAction()
//-----------------------------------------------------------------------------
string FieldInterface::getReadAction(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return General::readAction2Str(field->getReadAction()).toStdString();
    }

    return string("");
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
// Function: FieldInterface::getTestConstraint()
//-----------------------------------------------------------------------------
string FieldInterface::getTestConstraint(string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return General::testConstraint2Str(field->getTestConstraint()).toStdString();
    }

    return string("");
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
// Function: FieldInterface::addResetInterface()
//-----------------------------------------------------------------------------
void FieldInterface::addResetInterface(ResetInterface* newResetInterface)
{
    resetInterfaces_.push_back(newResetInterface);
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::removeResetInterface()
//-----------------------------------------------------------------------------
bool FieldInterface::removeResetInterface(ResetInterface* removedInterface)
{
    int index = -1;
    for (int i = 0; i < resetInterfaces_.size(); ++i)
    {
        if (resetInterfaces_.at(i) == removedInterface)
        {
            index = i;
            break;
        }
    }

    if (index < 0)
    {
        return false;
    }
    else
    {
        resetInterfaces_.erase(resetInterfaces_.begin() + index);
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getResetInterfaces()
//-----------------------------------------------------------------------------
std::vector<ResetInterface*> FieldInterface::getResetInterfaces() const
{
    return resetInterfaces_;
}
