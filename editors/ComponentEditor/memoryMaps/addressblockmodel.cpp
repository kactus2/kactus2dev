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

#include <editors/ComponentEditor/memoryMaps/AddressBlockColumns.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <common/KactusColors.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::AddressBlockModel()
//-----------------------------------------------------------------------------
AddressBlockModel::AddressBlockModel(RegisterInterface* registerInterface,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
registerInterface_(registerInterface)
{
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

    return registerInterface_->itemCount();
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
        return QString::fromStdString(registerInterface_->getIndexedItemName(section));
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
    if (!index.isValid() || index.row() < 0 || index.row() >= registerInterface_->itemCount())
    {
        return QVariant();
    }

    std::string registerName = registerInterface_->getIndexedItemName(index.row());

    if (role == Qt::DisplayRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedExpressionForIndex(index);
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
        return expressionOrValueForIndex(index);
    }

    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ForegroundRole)
    {
        if (validateIndex(index))
        {
            std::string registerPresence = registerInterface_->getIsPresentExpression(registerName);
            qint64 registerPresenceValue =
                QString::fromStdString(registerInterface_->getIsPresentValue(registerName, 10)).toLongLong();

            if (index.column() != AddressBlockColumns::IS_PRESENT && 
                (!registerPresence.empty() && registerPresenceValue == 0))
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
// Function: addressblockmodel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string registerName = registerInterface_->getIndexedItemName(index.row());
    if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return QString::fromStdString(registerInterface_->getOffsetFormattedExpression(registerName));
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return QString::fromStdString(registerInterface_->getDimensionFormattedExpression(registerName));
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return QString::fromStdString(registerInterface_->getIsPresentFormattedExpression(registerName));
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return QString::fromStdString(registerInterface_->getSizeFormattedExpression(registerName));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::expressionForIndex(QModelIndex const& index) const
{
    std::string registerName = registerInterface_->getIndexedItemName(index.row());
    if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return QString::fromStdString(registerInterface_->getOffsetExpression(registerName));
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return QString::fromStdString(registerInterface_->getDimensionExpression(registerName));
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return QString::fromStdString(registerInterface_->getIsPresentExpression(registerName));
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return QString::fromStdString(registerInterface_->getSizeExpression(registerName));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::valueForIndex(QModelIndex const& index) const
{
    std::string registerName = registerInterface_->getIndexedItemName(index.row());

    if (index.column() == AddressBlockColumns::NAME)
    {
        return QString::fromStdString(registerName);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return QString::fromStdString(registerInterface_->getOffsetValue(registerName));
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return QString::fromStdString(registerInterface_->getDimensionValue(registerName));
    }
    else if (index.column() == AddressBlockColumns::DESCRIPTION)
    {
        return QString::fromStdString(registerInterface_->getDescription(registerName));
    }
    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return QString::fromStdString(registerInterface_->getIsPresentValue(registerName));
    }
    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return QString::fromStdString(registerInterface_->getSizeValue(registerName));
    }
    else if (index.column() == AddressBlockColumns::VOLATILE)
    {
        return QString::fromStdString(registerInterface_->getVolatile(registerName));
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        return QString::fromStdString(registerInterface_->getAccessString(registerName));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant AddressBlockModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockmodel::setData()
//-----------------------------------------------------------------------------
bool AddressBlockModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= registerInterface_->itemCount())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        bool changed = true;

        std::string registerName = registerInterface_->getIndexedItemName(index.row());
        if (index.column() == AddressBlockColumns::NAME)
        {
            registerInterface_->setName(registerName, value.toString().toStdString());
            emit headerDataChanged(Qt::Vertical, index.row(), index.row());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(registerInterface_->getOffsetExpression(registerName)));
            }

            registerInterface_->setOffset(registerName, value.toString().toStdString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(registerInterface_->getDimensionExpression(registerName)));
            }

            registerInterface_->setDimension(registerName, value.toString().toStdString());
        }
        else if (index.column() == AddressBlockColumns::DESCRIPTION)
        {
            registerInterface_->setDescription(registerName, value.toString().toStdString());
        }
        else if (index.column() == AddressBlockColumns::IS_PRESENT)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(registerInterface_->getIsPresentExpression(registerName)));
            }

            registerInterface_->setIsPresent(registerName, value.toString().toStdString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(registerInterface_->getSizeExpression(registerName)));
            }

            registerInterface_->setSize(registerName, value.toString().toStdString());
        }
        else if (index.column() == AddressBlockColumns::VOLATILE)
        {
            registerInterface_->setVolatile(registerName, value.toString().toStdString());
        }
        else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
        {
            registerInterface_->setAccess(registerName, value.toString().toStdString());
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
    std::string registerName = registerInterface_->getIndexedItemName(index.row());

    if (index.column() == AddressBlockColumns::NAME)
    {
        return registerInterface_->itemHasValidName(registerName);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_DIMENSION)
    {
        return registerInterface_->hasValidDimension(registerName);
    }

    else if (index.column() == AddressBlockColumns::REGISTER_OFFSET)
    {
        return registerInterface_->hasValidOffset(registerName);
    }

    else if (index.column() == AddressBlockColumns::IS_PRESENT)
    {
        return registerInterface_->hasValidIsPresent(registerName);
    }

    else if (index.column() == AddressBlockColumns::REGISTER_SIZE)
    {
        return registerInterface_->hasValidSize(registerName);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int AddressBlockModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return registerInterface_->getAllReferencesToIdInItem(
        registerInterface_->getIndexedItemName(row), valueID.toStdString());
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onAddItem()
//-----------------------------------------------------------------------------
void AddressBlockModel::onAddItem(QModelIndex const& index)
{
    int row = registerInterface_->itemCount();
    if (index.isValid())
    {
        row = index.row();
    }

    int dataIndex = registerInterface_->indexInRegisterData(row);

    beginInsertRows(QModelIndex(), row, row);

    registerInterface_->addRegister(row, dataIndex);

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
    int dataIndex = registerInterface_->indexInRegisterData(index.row());
    if (!index.isValid() || index.row() < 0 || index.row() >= registerInterface_->itemCount() || dataIndex < 0)
    {
        return;
    }

    std::string removedRegisterName = registerInterface_->getIndexedItemName(index.row());

    // remove the specified item
    beginRemoveRows(QModelIndex(), index.row(), index.row());

    decreaseReferencesWithRemovedRegister(QString::fromStdString(removedRegisterName));
    registerInterface_->removeRegister(removedRegisterName, dataIndex);

    endRemoveRows();

    // inform navigation tree that file set has been removed
    emit itemRemoved(dataIndex);

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::decreaseReferencesWithRemovedRegister()
//-----------------------------------------------------------------------------
void AddressBlockModel::decreaseReferencesWithRemovedRegister(QString const& registerName)
{
    QMap<QString, int> referencedParameters = getReferencedParameters(registerName);

    foreach(QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::getReferencedParameters()
//-----------------------------------------------------------------------------
QMap<QString, int> AddressBlockModel::getReferencedParameters(QString const& registerName) const
{
    std::vector<std::string> registerNameList;
    registerNameList.push_back(registerName.toStdString());

    auto expressionList = registerInterface_->getExpressionsInSelectedRegisters(registerNameList);
    QStringList expressionListQT;
    for (auto expression : expressionList)
    {
        expressionListQT.append(QString::fromStdString(expression));
    }

    ReferenceCalculator refCalculator(getParameterFinder());

    return refCalculator.getReferencedParameters(expressionListQT);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::addressUnitBitsChanged()
//-----------------------------------------------------------------------------
void AddressBlockModel::addressUnitBitsChanged(int newAddressUnitbits)
{
    registerInterface_->setAddressUnitBits(newAddressUnitbits);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onCopyRows()
//-----------------------------------------------------------------------------
void AddressBlockModel::onCopyRows(QModelIndexList indexList)
{
    std::vector<int> registerIndexes;
    for (auto index : indexList)
    {
        registerIndexes.push_back(index.row());
    }

    registerInterface_->copyRows(registerIndexes);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::onPasteRows()
//-----------------------------------------------------------------------------
void AddressBlockModel::onPasteRows()
{
    int pastedRegisterCount = registerInterface_->getPasteRowCount();
    if (pastedRegisterCount == 0)
    {
        return;
    }

    int rowBegin = registerInterface_->itemCount();
    int rowEnd = rowBegin + pastedRegisterCount - 1;
    
    beginInsertRows(QModelIndex(), rowBegin, rowEnd);

    registerInterface_->pasteRows();

    for (int i = rowBegin; i <= rowEnd; ++i)
    {
        emit itemAdded(i);
        increaseReferencesInPastedRegister(QString::fromStdString(registerInterface_->getIndexedItemName(i)));
    }

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockModel::increaseReferencesInPastedRegister()
//-----------------------------------------------------------------------------
void AddressBlockModel::increaseReferencesInPastedRegister(QString const& registerName)
{
    QMap<QString, int> referencedParameters = getReferencedParameters(registerName);

    QMapIterator<QString, int> refParameterIterator(referencedParameters);
    while (refParameterIterator.hasNext())
    {
        refParameterIterator.next();
        for (int i = 0; i < refParameterIterator.value(); ++i)
        {
            emit increaseReferences(refParameterIterator.key());
        }
    }
}
