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
validator_(validator),
accessPolicyValidator_(expressionParser),
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
int FieldInterface::getItemIndex(std::string const& itemName) const
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
std::string FieldInterface::getIndexedItemName(int itemIndex) const
{
    std::string fieldName = "";
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
std::vector<std::string> FieldInterface::getItemNames() const
{
    std::vector<std::string> names;
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
bool FieldInterface::setName(std::string const& currentName, std::string const& newName)
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
std::string FieldInterface::getOffsetValue(std::string const& fieldName, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return parseExpressionToBaseNumber(field->getBitOffset(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getOffsetFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getOffsetFormattedExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return formattedValueFor(field->getBitOffset()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getOffsetExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getOffsetExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getBitOffset().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setOffset()
//-----------------------------------------------------------------------------
bool FieldInterface::setOffset(std::string const& fieldName, std::string const& newOffset)
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
std::string FieldInterface::getWidthValue(std::string const& fieldName, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return parseExpressionToBaseNumber(field->getBitWidth(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWidthFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWidthFormattedExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return formattedValueFor(field->getBitWidth()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWidthExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWidthExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getBitWidth().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setWidth()
//-----------------------------------------------------------------------------
bool FieldInterface::setWidth(std::string const& fieldName, std::string const& newWidth)
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
std::string FieldInterface::getResets(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return std::string();
    }

    QSharedPointer<QList<QSharedPointer<FieldReset> > > indexedResets = field->getResets();
    if (indexedResets->isEmpty())
    {
        return std::string();
    }
    else if (indexedResets->count() == 1)
    {
        return indexedResets->first()->getResetValue().toStdString();
    }
    else
    {
        return std::string("[multiple]");
    }
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getResetsToolTip()
//-----------------------------------------------------------------------------
std::string FieldInterface::getResetsToolTip(std::string const& fieldName) const
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

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getVolatile()
//-----------------------------------------------------------------------------
std::string FieldInterface::getVolatile(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getVolatile().toString().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setVolatile()
//-----------------------------------------------------------------------------
bool FieldInterface::setVolatile(std::string const& fieldName, std::string const& newVolatile)
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
std::string FieldInterface::getAccessString(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return AccessTypes::access2Str(field->getAccess()).toStdString();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return AccessTypes::access2Str(accessPolicies->at(accessPolicyIndex)->getAccess()).toStdString();
            }   
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getModeRefs()
//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, unsigned int> > FieldInterface::getModeRefs(std::string const& fieldName, 
    int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        auto fieldAccessPolicies = field->getFieldAccessPolicies();

        if (accessPolicyIndex <= fieldAccessPolicies->size() - 1)
        {
            auto modeRefs = fieldAccessPolicies->at(accessPolicyIndex)->getModeReferences();
            std::vector<std::pair<std::string, unsigned int> > modeRefList;
            std::transform(modeRefs->cbegin(), modeRefs->cend(), std::back_inserter(modeRefList), 
                [](QSharedPointer<ModeReference> const& modeRef)
                {
                    return std::make_pair<std::string, unsigned int >(modeRef->getReference().toStdString(), 
                        modeRef->getPriority());
                });

            return modeRefList;
        }
    }

    return std::vector<std::pair<std::string, unsigned int> >();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAccessPolicyCount()
//-----------------------------------------------------------------------------
int FieldInterface::getAccessPolicyCount(std::string const& fieldName) const
{
    if (auto field = getField(fieldName); field)
    {
        return field->getFieldAccessPolicies()->size();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReadResponse()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReadResponseExpression(std::string const& fieldName, int accessPolicyIndex) const
{
    auto field = getField(fieldName);

    if (!field)
    {
        return std::string();
    }

    if (auto accessPolicies = field->getFieldAccessPolicies();
        accessPolicyIndex <= accessPolicies->size() - 1)
    {
        return accessPolicies->at(accessPolicyIndex)->getReadResponse().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReadResponseFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReadResponseFormattedExpression(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return formattedValueFor(accessPolicies->at(accessPolicyIndex)->getReadResponse()).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReadResponseValue()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReadResponseValue(std::string const& fieldName, int accessPolicyIndex, int baseNumber /*= 0*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return parseExpressionToBaseNumber(
                accessPolicies->at(accessPolicyIndex)->getReadResponse(), baseNumber).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setReadResponse()
//-----------------------------------------------------------------------------
bool FieldInterface::setReadResponse(std::string const& fieldName, int accessPolicyIndex,
    std::string const& newReadResponse) const
{
    auto field = getField(fieldName);

    if (!field)
    {
        return false;
    }

    if (auto accessPolicies = field->getFieldAccessPolicies();
        accessPolicyIndex <= accessPolicies->size() - 1)
    {
        accessPolicies->at(accessPolicyIndex)->setReadResponse(QString::fromStdString(newReadResponse));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAllReferencesToIdInFieldAccessPolicy()
//-----------------------------------------------------------------------------
int FieldInterface::getAllReferencesToIdInFieldAccessPolicy(std::string const& fieldName, int accessPolicyIndex, 
    std::string const& valueID) const
{
    auto field = getField(fieldName);
    if (!field)
    {
        return 0;
    }

    if (auto accessPolicies = field->getFieldAccessPolicies();
        accessPolicyIndex <= accessPolicies->size() - 1)
    {
        QString id(QString::fromStdString(valueID));

        auto accessPolicy = accessPolicies->at(accessPolicyIndex);

        int referencesInReadResponse = accessPolicy->getReadResponse().count(id);
        int referencesInReserved = accessPolicy->getReserved().count(id);

        int referencesInWriteConstraint = 0;

        if (auto writeConstraint = accessPolicy->getWriteValueConstraint();
            writeConstraint)
        {
            if (writeConstraint->getType() == WriteValueConstraint::MIN_MAX)
            {
                referencesInWriteConstraint += accessPolicy->getWriteValueConstraint()->getMaximum().count(id);
                referencesInWriteConstraint += accessPolicy->getWriteValueConstraint()->getMinimum().count(id);
            }
        }

        return referencesInReadResponse + referencesInReserved + referencesInWriteConstraint;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasUniqueModeRefs()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidAccessPolicyModeRefs(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return validator_->singleFieldAccessPolicyHasValidModeRefs(field, accessPolicyIndex);
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::copyFieldAccessPolicies()
//-----------------------------------------------------------------------------
void FieldInterface::copyFieldAccessPolicies(std::string const& fieldName,
    std::vector<int> const& selectedAccessPolicies)
{
    if (auto const& field = getField(fieldName))
    {
        QList<QSharedPointer<FieldAccessPolicy> > copiedFieldAccessPolicies;
        for (auto index : selectedAccessPolicies)
        {
            auto accessPolicy = field->getFieldAccessPolicies()->at(index);
            copiedFieldAccessPolicies.append(accessPolicy);
        }

        QVariant accessPolicyVariant;
        accessPolicyVariant.setValue(copiedFieldAccessPolicies);

        QMimeData* newMimeData = new QMimeData();
        newMimeData->setData("text/xml/ipxact:fieldAccessPolicy", QByteArray());
        newMimeData->setImageData(accessPolicyVariant);

        QApplication::clipboard()->setMimeData(newMimeData);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::pasteFieldaccessPolicies()
//-----------------------------------------------------------------------------
int FieldInterface::pasteFieldaccessPolicies(std::string const& fieldName)
{
    auto field = getField(fieldName);
    if (!field)
    {
        return 0;
    }

    const QMimeData* pasteData = QApplication::clipboard()->mimeData();
    
    int numPasted = 0;

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<FieldAccessPolicy> > >())
        {
            auto copiedFieldAccessPolicies = pasteVariant.value<QList<QSharedPointer<FieldAccessPolicy> > >();

            for (auto copiedFieldAccessPolicy : copiedFieldAccessPolicies)
            {
                QSharedPointer<FieldAccessPolicy> newFieldAccessPolicy(new FieldAccessPolicy(*copiedFieldAccessPolicy));
                field->getFieldAccessPolicies()->append(newFieldAccessPolicy);

                numPasted++;
            }
        }
    }

    return numPasted;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setModeReferenceInterface()
//-----------------------------------------------------------------------------
void FieldInterface::setModeReferenceInterface(ModeReferenceInterface* modeRefInterface)
{
    modeReferenceInterface_ = modeRefInterface;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getModeReferenceInterface()
//-----------------------------------------------------------------------------
ModeReferenceInterface* FieldInterface::getModeReferenceInterface() const
{
    return modeReferenceInterface_;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getAccessType()
//-----------------------------------------------------------------------------
AccessTypes::Access FieldInterface::getAccessType(std::string const& fieldName, int accessPolicyIndex /*= -1*/)
    const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return field->getAccess();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return accessPolicies->at(accessPolicyIndex)->getAccess();
            }
        }
    }

    return AccessTypes::ACCESS_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setAccess()
//-----------------------------------------------------------------------------
bool FieldInterface::setAccess(std::string const& fieldName, std::string const& accessType,
    int accessPolicyIndex /*= -1*/) const
{
    
    if (auto field = getField(fieldName); field)
    {
        auto newAccess = AccessTypes::str2Access(QString::fromStdString(accessType), AccessTypes::ACCESS_COUNT);

        if (accessPolicyIndex == -1)
        {
            field->setAccess(newAccess);
            return true;
        }
        
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1 && accessPolicyIndex >= 0)
        {
            accessPolicies->at(accessPolicyIndex)->setAccess(newAccess);
            return true;
        }
    }

    return false;
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
// Function: FieldInterface::getModifiedWriteString()
//-----------------------------------------------------------------------------
std::string FieldInterface::getModifiedWriteString(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return General::modifiedWrite2Str(field->getModifiedWrite()).toStdString();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return General::modifiedWrite2Str(accessPolicies->at(accessPolicyIndex)->getModifiedWrite()).toStdString();
            }
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setModifiedWrite()
//-----------------------------------------------------------------------------
bool FieldInterface::setModifiedWrite(std::string const& fieldName, std::string const& newModifiedWrite,
    int accessPolicyIndex /*= -1*/)
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            field->setModifiedWrite(General::str2ModifiedWrite(QString::fromStdString(newModifiedWrite)));
            return true;
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                General::ModifiedWrite modifiedWrite = General::str2ModifiedWrite(QString::fromStdString(newModifiedWrite));
                accessPolicies->at(accessPolicyIndex)->setModifiedWrite(modifiedWrite);
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReadActionString()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReadActionString(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    auto readAction = getReadAction(fieldName, accessPolicyIndex);
    return General::readAction2Str(readAction).toStdString();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReadAction()
//-----------------------------------------------------------------------------
General::ReadAction FieldInterface::getReadAction(std::string const& fieldName,int accessPolicyIndex /*= -1*/)
    const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return field->getReadAction();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return accessPolicies->at(accessPolicyIndex)->getReadAction();
            }
        }
    }

    return General::READ_ACTION_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setReadAction()
//-----------------------------------------------------------------------------
bool FieldInterface::setReadAction(std::string const& fieldName, std::string const& newReadAction,
    int accessPolicyIndex /*= -1*/)
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            field->setReadAction(General::str2ReadAction(QString::fromStdString(newReadAction)));
            return true;
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                accessPolicies->at(accessPolicyIndex)->setReadAction(General::str2ReadAction(
                    QString::fromStdString(newReadAction)));
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestable()
//-----------------------------------------------------------------------------
std::string FieldInterface::getTestableValue(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return field->getTestable().toString().toStdString();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return accessPolicies->at(accessPolicyIndex)->getTestable().toString().toStdString();
            }
        }
    }
    
    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestableBool()
//-----------------------------------------------------------------------------
bool FieldInterface::getTestableBool(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return field->getTestable().toBool();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return accessPolicies->at(accessPolicyIndex)->getTestable().toBool();
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setTestable()
//-----------------------------------------------------------------------------
bool FieldInterface::setTestable(std::string const& fieldName, std::string const& newTestable,
    int accessPolicyIndex /*= -1*/) const
{
    auto field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    QString newTestableQT = QString::fromStdString(newTestable);

    if (accessPolicyIndex == -1)
    {
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
    else if (accessPolicyIndex >= 0)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            auto accessPolicy = accessPolicies->at(accessPolicyIndex);
            
            if (newTestableQT == TRUE_STRING)
            {
                accessPolicy->setTestable(true);
            }
            else if (newTestableQT == FALSE_STRING)
            {
                accessPolicy->setTestable(false);
                accessPolicy->setTestConstraint(General::TESTCONSTRAINT_COUNT);
            }
            else
            {
                accessPolicy->clearTestable();
            }

            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestConstraintString()
//-----------------------------------------------------------------------------
std::string FieldInterface::getTestConstraintString(std::string const& fieldName,
    int accessPolicyIndex /*= -1*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return General::testConstraint2Str(field->getTestConstraint()).toStdString();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return General::testConstraint2Str(
                    accessPolicies->at(accessPolicyIndex)->getTestConstraint()).toStdString();
            }
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getTestConstraint()
//-----------------------------------------------------------------------------
General::TestConstraint FieldInterface::getTestConstraint(std::string const& fieldName,
    int accessPolicyIndex /*= -1*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return field->getTestConstraint();
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return accessPolicies->at(accessPolicyIndex)->getTestConstraint();
            }
        }
    }

    return General::TESTCONSTRAINT_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setTestConstraint()
//-----------------------------------------------------------------------------
bool FieldInterface::setTestConstraint(std::string const& fieldName, std::string const& newTestConstraint,
    int accessPolicyIndex /*= -1*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            field->setTestConstraint(General::str2TestConstraint(QString::fromStdString(newTestConstraint)));
            return true;
        }
        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                accessPolicies->at(accessPolicyIndex)->setTestConstraint(
                    General::str2TestConstraint(QString::fromStdString(newTestConstraint)));
            }
            return true;
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getIsPresentValue()
//-----------------------------------------------------------------------------
std::string FieldInterface::getIsPresentValue(std::string const& fieldName, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return parseExpressionToBaseNumber(field->getIsPresent(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getIsPresentFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getIsPresentFormattedExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return formattedValueFor(field->getIsPresent()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getIsPresentExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getIsPresentExpression(std::string const& fieldName) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (field)
    {
        return field->getIsPresent().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setIsPresent()
//-----------------------------------------------------------------------------
bool FieldInterface::setIsPresent(std::string const& fieldName, std::string const& newIsPresent)
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
int FieldInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
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
bool FieldInterface::itemHasValidName(std::string const& itemName) const
{
    return validator_->hasValidName(getField(itemName));
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidOffset()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidOffset(std::string const& fieldName) const
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
bool FieldInterface::hasValidWidth(std::string const& fieldName) const
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
bool FieldInterface::hasValidResets(std::string const& fieldName) const
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
bool FieldInterface::hasValidIsPresent(std::string const& fieldName) const
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
bool FieldInterface::hasValidAccess(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    if (auto field = getField(fieldName); field)
    {
        if (accessPolicyIndex == -1)
        {
            return validator_->hasValidAccess(field);
        }

        else if (accessPolicyIndex >= 0)
        {
            if (auto accessPolicies = field->getFieldAccessPolicies();
                accessPolicyIndex <= accessPolicies->size() - 1)
            {
                return accessPolicyValidator_.hasValidAccess(accessPolicies->at(accessPolicyIndex));
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidReadResponse()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidReadResponse(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return accessPolicyValidator_.hasValidReadResponse(accessPolicies->at(accessPolicyIndex));
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidReserved()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidReserved(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return accessPolicyValidator_.hasValidReserved(accessPolicies->at(accessPolicyIndex));
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::hasValidMinimumWriteValueConstraint()
//-----------------------------------------------------------------------------
bool FieldInterface::hasValidWriteValueConstraint(std::string const& fieldName, int accessPolicyIndex) const
{
    if (auto field = getField(fieldName); field)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return accessPolicyValidator_.hasValidWriteValueConstraint(
                field->getFieldAccessPolicies()->at(accessPolicyIndex));
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getField()
//-----------------------------------------------------------------------------
QSharedPointer<Field> FieldInterface::getField(std::string const& fieldName) const
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
void FieldInterface::addField(int const& row, std::string const& newFieldName /*= std::string("")*/)
{
    QString fieldName = getUniqueName(newFieldName, FIELD_TYPE);

    QSharedPointer<Field> newField(new Field());
    newField->setName(fieldName);

    fields_->insert(row, newField);
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::addField()
//-----------------------------------------------------------------------------
void FieldInterface::addField(std::string const& newFieldName /*= std::string("")*/)
{
    addField(fields_->size(), newFieldName);
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::removeField()
//-----------------------------------------------------------------------------
bool FieldInterface::removeField(std::string const& fieldName)
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
void FieldInterface::copyRows(std::vector<int> const& selectedRows)
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

            for (auto const& field : newFields)
            {
                QSharedPointer<Field> newField(new Field(*field));
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
std::vector<std::string> FieldInterface::getExpressionsInSelectedFields(std::vector<std::string> const& fieldNames) const
{
    std::vector<std::string> expressionList;

    FieldExpressionsGatherer gatherer;

    for (auto const& name : fieldNames)
    {
        QSharedPointer<Field> field = getField(name);
        QStringList fieldExpressions = gatherer.getExpressions(field);
        for (auto const& expression : fieldExpressions)
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
std::string FieldInterface::getWriteConstraint(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    std::string writeConstraintText =
        WriteValueConversions::typeToString(WriteValueConstraint::TYPE_COUNT).toStdString();

    if (auto fieldWriteConstraint = getWriteValueConstraint(fieldName, accessPolicyIndex))
    {
        writeConstraintText = WriteValueConversions::typeToString(fieldWriteConstraint->getType()).toStdString();
    }

    return writeConstraintText;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteValueConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<WriteValueConstraint> FieldInterface::getWriteValueConstraint(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return QSharedPointer<WriteValueConstraint>();
    }
    
    if (accessPolicyIndex == -1)
    {
        QSharedPointer<WriteValueConstraint> fieldWriteConstraint = field->getWriteConstraint();
        if (fieldWriteConstraint)
        {
            return fieldWriteConstraint;
        }
    }

    else if (accessPolicyIndex >= 0)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return accessPolicies->at(accessPolicyIndex)->getWriteValueConstraint();
        }
    }

    return QSharedPointer<WriteValueConstraint>();
}


//-----------------------------------------------------------------------------
// Function: FieldInterface::setWriteConstraint()
//-----------------------------------------------------------------------------
bool FieldInterface::setWriteConstraint(std::string const& fieldName, std::string const& newConstraintText, int accessPolicyIndex /*= -1*/)
{
    QSharedPointer<Field> selectedField = getField(fieldName);
    if (!selectedField)
    {
        return false;
    }

    // Returns non-null write constraint if it exists either as std14 or std22. No need to check 
    // the access policy index later.
    auto writeConstraint = getWriteValueConstraint(fieldName, accessPolicyIndex);

    bool isStd14 = accessPolicyIndex == -1;

    if (writeConstraint)
    {
        WriteValueConstraint::Type constraintType =
            WriteValueConversions::stringToType(QString::fromStdString(newConstraintText));

        if (constraintType == WriteValueConstraint::TYPE_COUNT)
        {
            if (isStd14)
            {
                selectedField->setWriteConstraint(nullptr);
            }
            else
            {
                selectedField->getFieldAccessPolicies()->at(accessPolicyIndex)->setWriteValueConstraint(nullptr);
            }
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
            
            if (isStd14)
            {
                selectedField->setWriteConstraint(newConstraint);
            }
            else
            {
                selectedField->getFieldAccessPolicies()->at(accessPolicyIndex)->setWriteValueConstraint(newConstraint);
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMinimumValue()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMinimumValue(std::string const& fieldName, int accessPolicyIndex /*= -1*/, int const& baseNumber /*= 0*/) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
    if (constraint)
    {
        return parseExpressionToBaseNumber(constraint->getMinimum(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMinimumFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMinimumFormattedExpression(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
    if (constraint)
    {
        return formattedValueFor(constraint->getMinimum()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMinimumExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMinimumExpression(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
    if (constraint)
    {
        return constraint->getMinimum().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setWriteConstraintMinimum()
//-----------------------------------------------------------------------------
bool FieldInterface::setWriteConstraintMinimum(std::string const& fieldName,
    std::string const& newWriteConstraintMinimum,
    int accessPolicyIndex /*= -1*/)
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
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
std::string FieldInterface::getWriteConstraintMaximumValue(std::string const& fieldName, int accessPolicyIndex /*= -1*/, int const& baseNumber /*= 0*/) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
    if (constraint)
    {
        return parseExpressionToBaseNumber(constraint->getMaximum(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMaximumFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMaximumFormattedExpression(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
    if (constraint)
    {
        return formattedValueFor(constraint->getMaximum()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getWriteConstraintMaximumExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getWriteConstraintMaximumExpression(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
    if (constraint)
    {
        return constraint->getMaximum().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setWriteConstraintMaximum()
//-----------------------------------------------------------------------------
bool FieldInterface::setWriteConstraintMaximum(std::string const& fieldName,
    std::string const& newWriteConstraintMaximum,
    int accessPolicyIndex /*= -1*/)
{
    QSharedPointer<WriteValueConstraint> constraint = getWriteValueConstraint(fieldName, accessPolicyIndex);
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
std::string FieldInterface::getReservedValue(std::string const& fieldName, int accessPolicyIndex, int const& baseNumber) const
{
    QSharedPointer<Field> field = getField(fieldName);
    
    if (!field)
    {
        return std::string();
    }
    
    // Std14
    if (accessPolicyIndex == -1)
    {
        return parseExpressionToBaseNumber(field->getReserved(), baseNumber).toStdString();
    }
    // Std22
    else if (accessPolicyIndex >= 0)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return parseExpressionToBaseNumber(
                accessPolicies->at(accessPolicyIndex)->getReserved(), baseNumber).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReservedFormattedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReservedFormattedExpression(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return std::string();
    }
    
    if (accessPolicyIndex == -1)
    {
        return formattedValueFor(field->getReserved()).toStdString();
    }

    else if (accessPolicyIndex >= 0)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return formattedValueFor(accessPolicies->at(accessPolicyIndex)->getReserved()).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getReservedExpression()
//-----------------------------------------------------------------------------
std::string FieldInterface::getReservedExpression(std::string const& fieldName, int accessPolicyIndex /*= -1*/) const
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return std::string();
    }

    if (accessPolicyIndex == -1)
    {
        return field->getReserved().toStdString();
    }

    else if (accessPolicyIndex >= 0)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            return accessPolicies->at(accessPolicyIndex)->getReserved().toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setReserved()
//-----------------------------------------------------------------------------
bool FieldInterface::setReserved(std::string const& fieldName, std::string const& newReserved, int accessPolicyIndex /*= -1*/)
{
    QSharedPointer<Field> field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    if (accessPolicyIndex == -1)
    {
        field->setReserved(QString::fromStdString(newReserved));
        return true;
    }

    else if (accessPolicyIndex >= 0)
    {
        if (auto accessPolicies = field->getFieldAccessPolicies();
            accessPolicyIndex <= accessPolicies->size() - 1)
        {
            accessPolicies->at(accessPolicyIndex)->setReserved(QString::fromStdString(newReserved));
            return true;
        }
    }

    return false;
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

    return std::string();
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
// Function: FieldInterface::addFieldAccessPolicy()
//-----------------------------------------------------------------------------
bool FieldInterface::addFieldAccessPolicy(std::string const& fieldName)
{
    auto field = getField(fieldName);
    if (!field)
    {
        return false;
    }

    QSharedPointer<FieldAccessPolicy> newAccessPolicy(new FieldAccessPolicy());
    field->getFieldAccessPolicies()->append(newAccessPolicy);

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::removeFieldAccessPolicy()
//-----------------------------------------------------------------------------
bool FieldInterface::removeFieldAccessPolicy(std::string const& fieldName, int accessPolicyIndex)
{
    auto field = getField(fieldName);
    if (!field || accessPolicyIndex > field->getFieldAccessPolicies()->size() - 1)
    {
        return false;
    }

    field->getFieldAccessPolicies()->removeAt(accessPolicyIndex);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getModeReferencesInUse()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > FieldInterface::getModeReferencesInUse(std::string const& fieldName, int accessPolicyIndex) const
{
    std::vector<std::pair<unsigned int, std::string> > modeRefsInUse;

    auto field = getField(fieldName);
    
    if (!field)
    {
        return modeRefsInUse;
    }

    auto fieldAccessPolicies = field->getFieldAccessPolicies();

    int currentIndex = 0;
    for (auto const& accessPolicy : *fieldAccessPolicies)
    {
        // Get all mode references, excluding mode references in given field access policy.
        if (currentIndex != accessPolicyIndex)
        {
            auto modeReferences = accessPolicy->getModeReferences();
            std::transform(modeReferences->cbegin(), modeReferences->cend(), std::back_inserter(modeRefsInUse),
                [](auto modeReference)
                {
                    return std::make_pair(modeReference->getPriority(), modeReference->getReference().toStdString());
                });
        }
        ++currentIndex;
    }

    return modeRefsInUse;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getModeReferences()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > FieldInterface::getModeReferences(
    std::string const& fieldName, int accessPolicyIndex) const
{
    std::vector<std::pair<unsigned int, std::string> > modeRefs;
    if (auto field = getField(fieldName))
    {
        auto accessPolicies = field->getFieldAccessPolicies();
        if (accessPolicyIndex >= 0 && accessPolicyIndex < accessPolicies->size())
        {
            auto modeReferences = accessPolicies->at(accessPolicyIndex)->getModeReferences();

            std::transform(modeReferences->cbegin(), modeReferences->cend(), std::back_inserter(modeRefs),
                [](auto modeReference)
                {
                    return std::make_pair(modeReference->getPriority(), modeReference->getReference().toStdString());
                });
        }
    }

    return modeRefs;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::setModeReferences()
//-----------------------------------------------------------------------------
bool FieldInterface::setModeReferences(std::string const& fieldName, int accessPolicyIndex, 
    std::vector<std::pair<unsigned int, std::string> > const& newModeRefs)
{
    if (auto field = getField(fieldName))
    {
        auto fieldAccessPolicies = field->getFieldAccessPolicies();

        if (accessPolicyIndex >= 0 && accessPolicyIndex < fieldAccessPolicies->size())
        {
            QSharedPointer<QList<QSharedPointer<ModeReference> > > createdModeRefs(
                new QList<QSharedPointer<ModeReference> >());

            for (auto const& [priority, reference] : newModeRefs)
            {
                QSharedPointer<ModeReference> createdModeRef(new ModeReference());
                createdModeRef->setReference(QString::fromStdString(reference));
                createdModeRef->setPriority(priority);
                createdModeRefs->append(createdModeRef);
            }

            fieldAccessPolicies->at(accessPolicyIndex)->setModeReferences(createdModeRefs);
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> FieldInterface::getItem(std::string const& fieldName) const
{
    return getField(fieldName);
}
