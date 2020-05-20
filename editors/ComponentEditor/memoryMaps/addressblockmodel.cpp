//-----------------------------------------------------------------------------
// File: addressblockmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The model to manage the registers of a single address block or register file.
//-----------------------------------------------------------------------------

#include "addressblockmodel.h"

#include "AddressBlockColumns.h"
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/RegisterExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

#include <common/KactusColors.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::AddressBlockModel()
//-----------------------------------------------------------------------------
AddressBlockModel::AddressBlockModel(QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    QObject *parent) :
    ReferencingTableModel(parameterFinder, parent),
    ParameterizableTable(parameterFinder),
    registerData_(registerData),
    items_(),
    addressUnitBits_(0),
    parameterFinder_(parameterFinder),
    expressionFormatter_(expressionFormatter),
    registerFileValidator_(registerFileValidator),
    registerValidator_(registerFileValidator->getRegisterValidator())
{
    for (auto item : *registerData_)
    {
        QSharedPointer<Register> registerItem = item.dynamicCast<Register>();
        if (registerItem)
        {
            items_.append(registerItem);
        }
    }

    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::rowCount()
//-----------------------------------------------------------------------------
int AddressBlockModel::rowCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return items_.size();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::columnCount()
//-----------------------------------------------------------------------------
int AddressBlockModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return AddressBlockColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AddressBlockModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::headerData()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::headerData( int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        return items_.at(section)->name();
    }

    else if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == AddressBlockColumns::NAME)
        {
            return tr("Register\nname");
        }
        else if (section == AddressBlockColumns::REGISTER_OFFSET)
        {
            return tr("Offset\n[AUB]") + getExpressionSymbol();
        }
        else if (section == AddressBlockColumns::REGISTER_SIZE)
        {
            return tr("Size\n[bits]") + getExpressionSymbol();
        }
        else if (section == AddressBlockColumns::REGISTER_DIMENSION)
        {
            return tr("Dimension") + getExpressionSymbol();
        }
        else if (section == AddressBlockColumns::DESCRIPTION)
        {
            return tr("Description");
        }
        else if (section == AddressBlockColumns::VOLATILE)
        {
            return tr("Volatile");
        }
        else if (section == AddressBlockColumns::REGISTER_ACCESS)
        {
            return tr("Access");
        }
        else if (section == AddressBlockColumns::IS_PRESENT)
        {
            return tr("Is present") + getExpressionSymbol();
        }
        else
        {
            return QVariant();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::data()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$",
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return valueForIndex(index);
        }
    }

    else if (role == Qt::EditRole)
    {
        if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            return valueForIndex(index);
        }

        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index).toString();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (validateIndex(index))
        {
            QSharedPointer<Register> reg = items_.at(index.row());

            if (index.column() != AddressBlockColumns::IS_PRESENT && reg &&
                    (!reg->getIsPresent().isEmpty() && parseExpressionToDecimal(reg->getIsPresent()).toInt() != 1))
            {
                return KactusColors::DISABLED_TEXT;
            }
            else
            {
                return KactusColors::REGULAR_TEXT;
            }
        }
        else
        {
            return KactusColors::ERROR;
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == AddressBlockColumns::NAME ||
            index.column() == AddressBlockColumns::REGISTER_OFFSET ||
            index.column() == AddressBlockColumns::REGISTER_SIZE)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Register> const& reg = items_.at(index.row());

    if (index.column() == AddressBlockColumns::NAME)
    {
        return reg->name();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return reg->getAddressOffset();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return reg->getDimension();
    }
    else if (index.column() == AddressBlockColumns::DESCRIPTION)
    {
        return reg->description();
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return reg->getIsPresent();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return reg->getSize();
    }
    else if (index.column() == AddressBlockColumns::VOLATILE)
    {
        return reg->getVolatile();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        return AccessTypes::access2Str(reg->getAccess());
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<Register> const& reg = items_.at(index.row());

    if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return reg->getDimension();
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return reg->getAddressOffset();
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return reg->getIsPresent();
    }
    else if(index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return reg->getSize();
    }

    return data(index, Qt::DisplayRole);
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::setData()
//-----------------------------------------------------------------------------
bool AddressBlockModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        bool changed = true;

        QSharedPointer<Register> reg = items_.at(index.row());
        if (index.column() == AddressBlockColumns::NAME)
        {
            reg->setName(value.toString());
            emit headerDataChanged(Qt::Vertical, index.row(), index.row());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getAddressOffset());
            }

            reg->setAddressOffset(value.toString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getDimension());
            }

            reg->setDimension(value.toString());
        }
        else if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            reg->setDescription(value.toString());
        }
        else if (index.column() == AddressBlockColumns::IS_PRESENT)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getIsPresent());
            }

            reg->setIsPresent(value.toString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(reg->getSize());
            }

            reg->setSize(value.toString());
        }
        else if (index.column() == AddressBlockColumns::VOLATILE)
        {
            if (value.toString() == QLatin1String("true"))
            {
                reg->setVolatile(true);
            }
            else if (value.toString() == QLatin1String("false"))
            {
                reg->setVolatile(false);
            }
            else
            {
                reg->clearVolatile();
            }
        }
        else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
        {
            reg->setAccess(AccessTypes::str2Access(value.toString(), AccessTypes::ACCESS_COUNT));
        }
        else
        {
            changed = false;
        }

        if (changed)
        {
            if (index.column() == AddressBlockColumns::NAME ||
                index.column() == AddressBlockColumns::REGISTER_OFFSET ||
                index.column() == AddressBlockColumns::REGISTER_SIZE ||
                index.column() == AddressBlockColumns::REGISTER_DIMENSION ||
                index.column() == AddressBlockColumns::IS_PRESENT)
            {
                emit graphicsChanged();
                if (index.column() != AddressBlockColumns::NAME)
                {
                    emit childAddressInfoChanged(registerData_->indexOf(reg));
                }
            }

            emit dataChanged(index, index);
            emit contentChanged();
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList AddressBlockModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:register";

    return types;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool AddressBlockModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == AddressBlockColumns::REGISTER_DIMENSION ||
        index.column() == AddressBlockColumns::REGISTER_SIZE ||
        index.column() == AddressBlockColumns::REGISTER_OFFSET ||
        index.column() == AddressBlockColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::validateIndex()
//-----------------------------------------------------------------------------
bool AddressBlockModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<Register> const& reg = items_.at(index.row());

    if (index.column() == AddressBlockColumns::NAME)
    {
        QStringList registerNames;
        for (QSharedPointer<RegisterBase> registerItem : *registerData_)
        {
            registerNames.append(registerItem->name());
        }

        return registerNames.count(reg->name()) == 1 && registerValidator_->hasValidName(reg);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return registerValidator_->hasValidDimension(reg);
    }

    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return registerValidator_->hasValidAddressOffset(reg);
    }

    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return registerValidator_->hasValidIsPresent(reg);
    }

    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return registerValidator_->hasValidSize(reg);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int AddressBlockModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    QSharedPointer<Register> const& reg = items_.at(row);

    int count = 0;
    count += reg->getAddressOffset().count(valueID);
    count += reg->getDimension().count(valueID);
    count += reg->getIsPresent().count(valueID);
    count += reg->getSize().count(valueID);

    return count;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onAddItem()
//-----------------------------------------------------------------------------
void AddressBlockModel::onAddItem(QModelIndex const& index)
{
    // Find last register/registerFile by address offset.
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

    qreal offsetIncrease = qMax(quint64(1), lastRegRange * qMax(quint64(1), lastRegDimension));

    // convert the address to hexadecimal form
    quint64 itemAddress = lastRegAddress + offsetIncrease;
    QString offset = QStringLiteral("'h") + QString::number(itemAddress, 16);

    // if the index is valid then add the item to the correct position
    int row = items_.size();
    if (index.isValid())
    {
        row = index.row();
    }

    int dataIndex = registerData_->size();
    if (row < items_.size() - 1)
    {
        dataIndex = registerData_->indexOf(items_.at(row + 1));
    }

    beginInsertRows(QModelIndex(), row, row);

    QSharedPointer<Register> regItem(new Register());    
    regItem->setAddressOffset(offset);
    regItem->setSize(QString::number(addressUnitBits_));
    items_.insert(row, regItem);
    registerData_->insert(dataIndex, regItem);
    endInsertRows();

    // inform navigation tree that file set is added
    emit itemAdded(dataIndex);

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AddressBlockModel::onRemoveItem(QModelIndex const& index )
{
    // don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
        return;
    }

    // remove the specified item
    beginRemoveRows(QModelIndex(), index.row(), index.row());

    QSharedPointer<Register> removedRegister = items_.at(index.row());
    if (removedRegister)
    {
        decreaseReferencesWithRemovedRegister(removedRegister);
    }

    int dataIndex = registerData_->indexOf(removedRegister);
    registerData_->removeAt(dataIndex);
    items_.removeAt(index.row());
    endRemoveRows();

    // inform navigation tree that file set has been removed
    emit itemRemoved(dataIndex);

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::decreaserReferencesWithRemovedRegisters()
//-----------------------------------------------------------------------------
void AddressBlockModel::decreaseReferencesWithRemovedRegister(QSharedPointer<Register> removedRegister)
{
    QStringList expressionList;

    RegisterExpressionsGatherer registerGatherer;
    expressionList << registerGatherer.getExpressions(removedRegister);

    ReferenceCalculator referenceCalculator(getParameterFinder());
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressionList);

    foreach (QString referencedID, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedID); ++i)
        {
            emit decreaseReferences(referencedID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::increaseReferencesInPastedField()
//-----------------------------------------------------------------------------
void AddressBlockModel::increaseReferencesInPastedRegister(QSharedPointer<Register> pastedRegister,
                                                           RegisterExpressionsGatherer& gatherer,
                                                           ReferenceCalculator& referenceCalculator)
{
    QStringList registerExpressions = gatherer.getExpressions(pastedRegister);

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(registerExpressions);

    QMapIterator<QString, int> refParameterIterator (referencedParameters);
    while (refParameterIterator.hasNext())
    {
        refParameterIterator.next();
        for (int i = 0; i < refParameterIterator.value(); ++i)
        {
            emit increaseReferences(refParameterIterator.key());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void AddressBlockModel::addressUnitBitsChanged(int newAddressUnitbits)
{
    addressUnitBits_ = newAddressUnitbits;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onCopyRows()
//-----------------------------------------------------------------------------
void AddressBlockModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<Register> > copiedRegisters;
    for (QModelIndex index : indexList)
    {
        copiedRegisters.append(items_.at(index.row()));
    }

    QVariant registerVariant;
    registerVariant.setValue(copiedRegisters);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:register", QByteArray());
    newMimeData->setImageData(registerVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onPasteRows()
//-----------------------------------------------------------------------------
void AddressBlockModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Register> > >())
        {
            RegisterExpressionsGatherer gatherer;
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<Register> > newRegsiters =
                    pasteVariant.value<QList<QSharedPointer<Register> > >();

            int rowBegin = items_.size();
            int rowEnd = rowBegin + newRegsiters.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            for (QSharedPointer<Register> copiedRegister : newRegsiters)
            {
                QSharedPointer<Register> newRegister(new Register(*copiedRegister));
                newRegister->setName(getUniqueName(newRegister->name(), getAllNames()));
                
                registerData_->append(newRegister);
                items_.append(newRegister);

                increaseReferencesInPastedRegister(newRegister, gatherer, referenceCalculator);

                emit itemAdded(registerData_->size() - 1);
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::getAllNames()
//-----------------------------------------------------------------------------
QStringList AddressBlockModel::getAllNames() const
{
    QStringList names;
    for (QSharedPointer<RegisterBase> registerItem : *registerData_)
    {
        names.append(registerItem->name());
    }

    return names;
}
