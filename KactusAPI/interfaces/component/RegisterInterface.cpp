//-----------------------------------------------------------------------------
// File: RegisterInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2020
//
// Description:
// Interface for editing registers.
//-----------------------------------------------------------------------------

#include <RegisterInterface.h>

#include <IPXACTmodels/common/AccessTypes.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <FieldInterface.h>
#include <AccessPolicyInterface.h>
#include <RegisterExpressionsGatherer.h>

#include <QMimeData>
#include <QApplication>
#include <QClipboard>

using namespace std;

namespace
{
    QString const TRUE_STRING = QLatin1String("true");
    QString const FALSE_STRING = QLatin1String("false");

    std::string const REGISTER_TYPE = "register";
};

//-----------------------------------------------------------------------------
// Function: RegisterInterface::RegisterInterface()
//-----------------------------------------------------------------------------
RegisterInterface::RegisterInterface(QSharedPointer<RegisterValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    FieldInterface* subInterface, AccessPolicyInterface* accessPolicyInterface) :
ParameterizableInterface(expressionParser, expressionFormatter),
registers_(),
validator_(validator),
subInterface_(subInterface),
accessPolicyInterface_(accessPolicyInterface),
addressUnitBits_(0)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setRegisters()
//-----------------------------------------------------------------------------
void RegisterInterface::setRegisters(QSharedPointer<QList<QSharedPointer<RegisterBase>>> newRegisterData)
{
    registerData_ = newRegisterData;

    registers_.clear();
    for (auto item : *registerData_)
    {
        QSharedPointer<Register> registerItem = item.dynamicCast<Register>();
        if (registerItem)
        {
            registers_.append(registerItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getItemIndex()
//-----------------------------------------------------------------------------
int RegisterInterface::getItemIndex(string const& itemName) const
{
    for (int i = 0; i < registers_.size(); ++i)
    {
        if (registers_.at(i)->name().toStdString() == itemName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
string RegisterInterface::getIndexedItemName(int const& itemIndex) const
{
    string fieldName = "";
    if (itemIndex >= 0 && itemIndex < registers_.size())
    {
        fieldName = registers_.at(itemIndex)->name().toStdString();
    }

    return fieldName;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::itemCount()
//-----------------------------------------------------------------------------
int RegisterInterface::itemCount() const
{
    return registers_.count();
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::registerDataCount()
//-----------------------------------------------------------------------------
int RegisterInterface::registerDataCount() const
{
    return registerData_->count();
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::indexInRegisterData()
//-----------------------------------------------------------------------------
int RegisterInterface::indexInRegisterData(int const& index) const
{
    QSharedPointer<Register> indexedRegister = getRegister(getIndexedItemName(index));
    if (indexedRegister)
    {
        return registerData_->indexOf(indexedRegister);
    }

    return registerDataCount();
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> RegisterInterface::getItemNames() const
{
    vector<string> names;
    for (auto baseRegister : *registerData_)
    {
        names.push_back(baseRegister->name().toStdString());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setName()
//-----------------------------------------------------------------------------
bool RegisterInterface::setName(string const& currentName, string const& newName)
{
    QSharedPointer<Register> editedRegister = getRegister(currentName);
    if (editedRegister && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueName(newName, REGISTER_TYPE);

        editedRegister->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getOffsetValue()
//-----------------------------------------------------------------------------
string RegisterInterface::getOffsetValue(std::string const& registerName, int const& baseNumber) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return parseExpressionToBaseNumber(selectedRegister->getAddressOffset(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getOffsetFormattedExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getOffsetFormattedExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return formattedValueFor(selectedRegister->getAddressOffset()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getOffsetExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getOffsetExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return selectedRegister->getAddressOffset().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setOffset()
//-----------------------------------------------------------------------------
bool RegisterInterface::setOffset(std::string const& registerName, std::string const& newOffset)
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    selectedRegister->setAddressOffset(QString::fromStdString(newOffset));
    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getDimensionValue()
//-----------------------------------------------------------------------------
string RegisterInterface::getDimensionValue(std::string const& registerName, int const& baseNumber) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return parseExpressionToBaseNumber(selectedRegister->getDimension(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getDimensionFormattedExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getDimensionFormattedExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return formattedValueFor(selectedRegister->getDimension()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getDimensionExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getDimensionExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return selectedRegister->getDimension().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setDimension()
//-----------------------------------------------------------------------------
bool RegisterInterface::setDimension(std::string const& registerName, std::string const& newDimension)
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    selectedRegister->setDimension(QString::fromStdString(newDimension));
    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getIsPresentValue()
//-----------------------------------------------------------------------------
string RegisterInterface::getIsPresentValue(std::string const& registerName, int const& baseNumber) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return parseExpressionToBaseNumber(selectedRegister->getIsPresent(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getIsPresentFormattedExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getIsPresentFormattedExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return formattedValueFor(selectedRegister->getIsPresent()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getIsPresentExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getIsPresentExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return selectedRegister->getIsPresent().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setIsPresent()
//-----------------------------------------------------------------------------
bool RegisterInterface::setIsPresent(std::string const& registerName, std::string const& newIsPresent)
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    selectedRegister->setIsPresent(QString::fromStdString(newIsPresent));
    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getSizeValue()
//-----------------------------------------------------------------------------
string RegisterInterface::getSizeValue(std::string const& registerName, int const& baseNumber) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return parseExpressionToBaseNumber(selectedRegister->getSize(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getSizeFormattedExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getSizeFormattedExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return formattedValueFor(selectedRegister->getSize()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getSizeExpression()
//-----------------------------------------------------------------------------
string RegisterInterface::getSizeExpression(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return selectedRegister->getSize().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setSize()
//-----------------------------------------------------------------------------
bool RegisterInterface::setSize(std::string const& registerName, std::string const& newSize)
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    selectedRegister->setSize(QString::fromStdString(newSize));
    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getVolatile()
//-----------------------------------------------------------------------------
string RegisterInterface::getVolatile(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (selectedRegister)
    {
        return selectedRegister->getVolatile().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setVolatile()
//-----------------------------------------------------------------------------
bool RegisterInterface::setVolatile(std::string const& registerName, std::string const& newVolatile)
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    if (QString::fromStdString(newVolatile) == TRUE_STRING)
    {
        selectedRegister->setVolatile(true);
    }
    else if (QString::fromStdString(newVolatile) == FALSE_STRING)
    {
        selectedRegister->setVolatile(false);
    }
    else
    {
        selectedRegister->clearVolatile();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getAccessString()
//-----------------------------------------------------------------------------
std::string RegisterInterface::getAccessString(std::string const& registerName) const
{
    if (auto selectedRegister = getRegister(registerName))
    {
        return AccessTypes::access2Str(selectedRegister->getAccess()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access RegisterInterface::getAccess(std::string const& registerName) const
{
    if (auto selectedRegister = getRegister(registerName))
    {
        return selectedRegister->getAccess();
    }

    return AccessTypes::ACCESS_COUNT;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setAccess()
//-----------------------------------------------------------------------------
bool RegisterInterface::setAccess(std::string const& registerName, std::string const& newAccess)
{
    if (auto selectedRegister = getRegister(registerName))
    {
        auto newAccessType = AccessTypes::str2Access(QString::fromStdString(newAccess), AccessTypes::ACCESS_COUNT);

        selectedRegister->setAccess(newAccessType);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int RegisterInterface::getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const
{
    QSharedPointer<Register> selectedRegister = getRegister(itemName);

    QString idString = QString::fromStdString(valueID);

    int referencesInOffset = selectedRegister->getAddressOffset().count(idString);
    int referencesInDimension = selectedRegister->getDimension().count(idString);
    int referencesInIsPresent = selectedRegister->getIsPresent().count(idString);
    int referencesInSize = selectedRegister->getSize().count(idString);

    int totalReferencesToParameter =
        referencesInOffset + referencesInDimension + referencesInIsPresent + referencesInSize;

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::validateItems()
//-----------------------------------------------------------------------------
bool RegisterInterface::validateItems() const
{
    for (auto currentRegister : registers_)
    {
        if (!validator_->validate(currentRegister))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool RegisterInterface::itemHasValidName(string const& itemName) const
{
    QStringList registerNames;
    for (QSharedPointer<RegisterBase> registerItem : *registerData_)
    {
        registerNames.append(registerItem->name());
    }

    return registerNames.count(QString::fromStdString(itemName)) == 1 &&
        validator_->hasValidName(getRegister(itemName));
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::hasValidDimension()
//-----------------------------------------------------------------------------
bool RegisterInterface::hasValidDimension(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    return validator_->hasValidDimensions(selectedRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::hasValidOffset()
//-----------------------------------------------------------------------------
bool RegisterInterface::hasValidOffset(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    return validator_->hasValidAddressOffset(selectedRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool RegisterInterface::hasValidIsPresent(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    return validator_->hasValidIsPresent(selectedRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::hasValidSize()
//-----------------------------------------------------------------------------
bool RegisterInterface::hasValidSize(std::string const& registerName) const
{
    QSharedPointer<Register> selectedRegister = getRegister(registerName);
    if (!selectedRegister)
    {
        return false;
    }

    return validator_->hasValidSize(selectedRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getRegister()
//-----------------------------------------------------------------------------
QSharedPointer<Register> RegisterInterface::getRegister(std::string const& registerName) const
{
    for (auto currentRegister : registers_)
    {
        if (currentRegister->name().toStdString() == registerName)
        {
            return currentRegister;
        }
    }

    return QSharedPointer<Register>();
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::setAddressUnitBits()
//-----------------------------------------------------------------------------
void RegisterInterface::setAddressUnitBits(int const& newAddressUnitbits)
{
    addressUnitBits_ = newAddressUnitbits;
}

//-----------------------------------------------------------------------------
// Function: FieldInterface::addRegister()
//-----------------------------------------------------------------------------
void RegisterInterface::addRegister(int const& row, int const& dataIndex, std::string const& newRegisterName)
{
    QString registerName = getUniqueName(newRegisterName, REGISTER_TYPE);

    auto lastRegister = std::max_element(registerData_->cbegin(), registerData_->cend(),
        [this](QSharedPointer<RegisterBase> const& a, QSharedPointer<RegisterBase> const& b)
    {
        return parseExpressionToDecimal(a->getAddressOffset()).toInt() <
            parseExpressionToDecimal(b->getAddressOffset()).toInt();
    });

    quint64 lastRegAddress = 0;
    quint64 lastRegDimension = 0;
    quint64 lastRegRange = 0;

    if (lastRegister != registerData_->cend())
    {
        lastRegAddress = parseExpressionToDecimal((*lastRegister)->getAddressOffset()).toInt();
        lastRegDimension = parseExpressionToDecimal((*lastRegister)->getDimension()).toInt();

        QSharedPointer<Register> reg = lastRegister->dynamicCast<Register>();
        QSharedPointer<RegisterFile> regFile = lastRegister->dynamicCast<RegisterFile>();
        if (reg)
        {
            lastRegRange = parseExpressionToDecimal(reg->getSize()).toInt() / qMax(addressUnitBits_, 1u);
        }
        else if (regFile)
        {
            lastRegRange = parseExpressionToDecimal(regFile->getRange()).toInt();
        }
    }

    qreal offsetIncrease = qMax(quint64(0), lastRegRange * qMax(quint64(1), lastRegDimension));

    quint64 itemAddress = lastRegAddress + offsetIncrease;
    QString offset = QStringLiteral("'h") + QString::number(itemAddress, 16);

    QSharedPointer<Register> regItem(new Register());
    regItem->setAddressOffset(offset);
    regItem->setName(registerName);
    registers_.insert(row, regItem);
    registerData_->insert(dataIndex, regItem);
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::removeRegister()
//-----------------------------------------------------------------------------
bool RegisterInterface::removeRegister(std::string const& registerName, int const& dataIndex)
{
    QSharedPointer<Register> removedRegister = getRegister(registerName);
    if (!removedRegister || dataIndex < 0 || dataIndex >= registerDataCount())
    {
        return false;
    }

    registerData_->removeAt(dataIndex);
    return registers_.removeOne(removedRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::copyRows()
//-----------------------------------------------------------------------------
void RegisterInterface::copyRows(std::vector<int> selectedRows)
{
    QList<QSharedPointer<Register> > copiedRegisters;
    for (auto index : selectedRows)
    {
        QSharedPointer<Register> currentRegister = registers_.at(index);
        copiedRegisters.append(currentRegister);
    }

    QVariant registerVariant;
    registerVariant.setValue(copiedRegisters);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:register", QByteArray());
    newMimeData->setImageData(registerVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::pasteRows()
//-----------------------------------------------------------------------------
std::vector<std::string> RegisterInterface::pasteRows()
{
    std::vector<std::string> pastedRegisterNames;

    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Register> > >())
        {
            QList<QSharedPointer<Register> > copiedRegisterList = pasteVariant.value<QList<QSharedPointer<Register>>>();

            foreach(QSharedPointer<Register> copiedRegister, copiedRegisterList)
            {
                QSharedPointer<Register> newRegister(new Register(*copiedRegister.data()));
                newRegister->setName(getUniqueName(newRegister->name().toStdString(), REGISTER_TYPE));
                registers_.append(newRegister);
                registerData_->append(newRegister);

                pastedRegisterNames.push_back(newRegister->name().toStdString());
            }
        }
    }

    return pastedRegisterNames;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getPasteRowCount()
//-----------------------------------------------------------------------------
int RegisterInterface::getPasteRowCount() const
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();
    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Register> > >())
        {
            QList<QSharedPointer<Register> > newRegisters =
                pasteVariant.value<QList<QSharedPointer<Register> > >();
            return newRegisters.count();
        }
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getExpressionsInSelectedFields()
//-----------------------------------------------------------------------------
std::vector<std::string> RegisterInterface::
    getExpressionsInSelectedRegisters(std::vector<std::string> registerNames) const
{
    std::vector<std::string> expressionList;

    RegisterExpressionsGatherer gatherer;

    for (auto name : registerNames)
    {
        QSharedPointer<Register> currentRegister = getRegister(name);
        QStringList registerExpressions = gatherer.getExpressions(currentRegister);
        for (auto expression : registerExpressions)
        {
            expressionList.push_back(expression.toStdString());
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getSubInterface()
//-----------------------------------------------------------------------------
FieldInterface* RegisterInterface::getSubInterface() const
{
    return subInterface_;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getAccessPolicyInterface()
//-----------------------------------------------------------------------------
AccessPolicyInterface* RegisterInterface::getAccessPolicyInterface() const
{
    return accessPolicyInterface_;
}

//-----------------------------------------------------------------------------
// Function: RegisterInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> RegisterInterface::getItem(std::string const& registerName) const
{
    return getRegister(registerName);
}
