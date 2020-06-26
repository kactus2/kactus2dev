//-----------------------------------------------------------------------------
// File: memorymapmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The model to manage the address blocks of a single memory map.
//-----------------------------------------------------------------------------

#include "memorymapmodel.h"

#include "MemoryMapColumns.h"

#include <editors/ComponentEditor/memoryMaps/interfaces/AddressBlockInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/AddressBlockExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <common/KactusColors.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::MemoryMapModel()
//-----------------------------------------------------------------------------
MemoryMapModel::MemoryMapModel(AddressBlockInterface* blockInterface,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
blockInterface_(blockInterface)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::~MemoryMapModel()
//-----------------------------------------------------------------------------
MemoryMapModel::~MemoryMapModel()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::rowCount()
//-----------------------------------------------------------------------------
int MemoryMapModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return blockInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::columnCount()
//-----------------------------------------------------------------------------
int MemoryMapModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return MemoryMapColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags MemoryMapModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::headerData()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == MemoryMapColumns::USAGE_COLUMN)
        {
            return tr("Usage");
        }
        else if (section == MemoryMapColumns::NAME_COLUMN)
        {
            return tr("Address block\nname");
        }
        else if (section == MemoryMapColumns::BASE_COLUMN)
        {
            QString baseAddress = tr("Base address\n [AUB]") + getExpressionSymbol();
            return baseAddress;
        }
        else if (section == MemoryMapColumns::RANGE_COLUMN)
        {
            QString range = tr("Range\n [AUB]") + getExpressionSymbol();
            return range;
        }
        else if (section == MemoryMapColumns::WIDTH_COLUMN)
        {
            QString widthHeader = tr("Width\n [bits]") + getExpressionSymbol();
            return widthHeader;
        }

        else if (section == MemoryMapColumns::ACCESS_COLUMN)
        {
            return tr("Access");
        }
        else if (section == MemoryMapColumns::VOLATILE_COLUMN)
        {
            return tr("Volatile");
        }
        else if (section == MemoryMapColumns::IS_PRESENT)
        {
            return tr("Is present") + getExpressionSymbol();
        }
        else if (section == MemoryMapColumns::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
	}
	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::data()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= blockInterface_->itemCount())
    {
		return QVariant();
	}

    std::string blockName = blockInterface_->getIndexedItemName(index.row());

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index) && role == Qt::DisplayRole)
        {
            return formattedExpressionForIndex(index);
        }
        else if (role == Qt::DisplayRole && index.column() == MemoryMapColumns::DESCRIPTION_COLUMN)
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }

        return valueForIndex(index);
    }

    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }

	else if (role == Qt::ForegroundRole)
    {
        std::string blockPresence = blockInterface_->getIsPresentExpression(blockName);
        qint64 blockPresenceValue =
            QString::fromStdString(blockInterface_->getIsPresentValue(blockName, 10)).toLongLong();

        if (index.column() != MemoryMapColumns::IS_PRESENT && (!blockPresence.empty() && blockPresenceValue != 1))
        {
            return KactusColors::DISABLED_TEXT;
        }

        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == MemoryMapColumns::NAME_COLUMN || index.column() == MemoryMapColumns::BASE_COLUMN ||
            index.column() == MemoryMapColumns::RANGE_COLUMN || index.column() == MemoryMapColumns::WIDTH_COLUMN) 
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else 
        {
            return KactusColors::REGULAR_FIELD;
        }
    }
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::setData()
//-----------------------------------------------------------------------------
bool MemoryMapModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= blockInterface_->itemCount())
    {
		return false;
	}

    if (Qt::EditRole == role)
    {
        std::string blockName = blockInterface_->getIndexedItemName(index.row());

        if (index.column() == MemoryMapColumns::USAGE_COLUMN)
        {
            if (!blockInterface_->setUsage(blockName, value.toString().toStdString()))
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::NAME_COLUMN)
        {
            blockInterface_->setName(blockName, value.toString().toStdString());
        }									
        else if (index.column() == MemoryMapColumns::BASE_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(blockInterface_->getBaseAddressExpression(blockName)));
            }

            blockInterface_->setBaseAddress(blockName, value.toString().toStdString());
        }
        else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            std::string oldValue = blockInterface_->getRangeExpression(blockName);
            if (blockInterface_->setRange(blockName, value.toString().toStdString()))
            {
                removeReferencesFromSingleExpression(QString::fromStdString(oldValue));
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
        {
            std::string oldValue = blockInterface_->getWidthExpression(blockName);
            if (blockInterface_->setWidth(blockName, value.toString().toStdString()))
            {
                removeReferencesFromSingleExpression(QString::fromStdString(oldValue));
            }
            else
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::IS_PRESENT)
        {
            blockInterface_->setIsPresent(blockName, value.toString().toStdString());
        }
        else if (index.column() == MemoryMapColumns::DESCRIPTION_COLUMN)
        {
            blockInterface_->setDescription(blockName, value.toString().toStdString());
        }
        else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
        {
            if (!blockInterface_->setAccess(blockName, value.toString().toStdString()))
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
        {
            if (!blockInterface_->setVolatile(blockName, value.toBool()))
            {
                return false;
            }
        }
        else 
        {
            return false;
        }

        if (index.column() == MemoryMapColumns::NAME_COLUMN || index.column() == MemoryMapColumns::BASE_COLUMN ||
            index.column() == MemoryMapColumns::RANGE_COLUMN || index.column() == MemoryMapColumns::WIDTH_COLUMN ||
            index.column() == MemoryMapColumns::IS_PRESENT)
        {
            emit graphicsChanged();
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else 
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::addressUnitBitsUpdated()
//-----------------------------------------------------------------------------
void MemoryMapModel::addressUnitBitsUpdated(QString const& newAddressUnitBits)
{
    blockInterface_->setAddressUnitBits(newAddressUnitBits.toStdString());
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onAddItem()
//-----------------------------------------------------------------------------
void MemoryMapModel::onAddItem( QModelIndex const& index )
{
    int row = blockInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
    blockInterface_->addBlock(row);
	endInsertRows();

	// inform navigation tree that file set is added
	emit itemAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onRemoveItem()
//-----------------------------------------------------------------------------
void MemoryMapModel::onRemoveItem( QModelIndex const& index )
{
	// don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= blockInterface_->itemCount())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    removeReferencesInItemOnRow(index.row());
    blockInterface_->removeBlock(blockInterface_->getIndexedItemName(index.row()));
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit itemRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool MemoryMapModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == MemoryMapColumns::BASE_COLUMN || index.column() == MemoryMapColumns::RANGE_COLUMN ||
        index.column() == MemoryMapColumns::WIDTH_COLUMN || index.column() == MemoryMapColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::expressionOrValueForIndex(QModelIndex const& index) const
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
// Function: memorymapmodel::validateIndex()
//-----------------------------------------------------------------------------
bool MemoryMapModel::validateIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (blockName.empty())
    {
        return false;
    }

    if (index.column() == MemoryMapColumns::NAME_COLUMN)
    {
        return blockInterface_->itemHasValidName(blockName);
    }
    else if (index.column() == MemoryMapColumns::BASE_COLUMN)
    {
        blockInterface_->hasValidBaseAddress(blockName);
    }
    else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return blockInterface_->hasValidRange(blockName);
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return blockInterface_->hasValidWidth(blockName);
    }
    else if (index.column() == MemoryMapColumns::IS_PRESENT)
    {
        return blockInterface_->hasValidIsPresent(blockName);
    }
    else if (index.column() == MemoryMapColumns::USAGE_COLUMN)
    {
        return blockInterface_->hasValidUsage(blockName);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int MemoryMapModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return blockInterface_->getAllReferencesToIdInItem(
        blockInterface_->getIndexedItemName(row), valueID.toStdString());
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (index.column() == MemoryMapColumns::BASE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getBaseAddressFormattedExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getRangeFormattedExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getWidthFormattedExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::IS_PRESENT)
    {
        return QString::fromStdString(blockInterface_->getIsPresentFormattedExpression(blockName));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::expressionForIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (index.column() == MemoryMapColumns::BASE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getBaseAddressExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getRangeExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getWidthExpression(blockName));
    }
    else if (index.column() == MemoryMapColumns::IS_PRESENT)
    {
        return QString::fromStdString(blockInterface_->getIsPresentExpression(blockName));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::valueForIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (index.column() == MemoryMapColumns::NAME_COLUMN)
    {
        return QString::fromStdString(blockName);
    }
    else if (index.column() == MemoryMapColumns::DESCRIPTION_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getDescription(blockName));
    }
    else if (index.column() == MemoryMapColumns::BASE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getBaseAddressValue(blockName));
    }
    else if (index.column() == MemoryMapColumns::IS_PRESENT)
    {
        return QString::fromStdString(blockInterface_->getIsPresentValue(blockName));
    }
    else if (index.column() == MemoryMapColumns::USAGE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getUsageString(blockName));
    }
    else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getRangeValue(blockName));
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getWidthValue(blockName));
    }
    else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getAccessString(blockName));
    }
    else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
    {
        return QString::fromStdString(blockInterface_->getVolatile(blockName));
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onCopyRows()
//-----------------------------------------------------------------------------
void MemoryMapModel::onCopyRows(QModelIndexList indexList)
{
    std::vector<int> registerIndexes;
    for (auto index : indexList)
    {
        registerIndexes.push_back(index.row());
    }

    blockInterface_->copyRows(registerIndexes);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onPasteRows()
//-----------------------------------------------------------------------------
void MemoryMapModel::onPasteRows()
{
    int rowBegin = blockInterface_->itemCount();

    std::vector<std::string> pastedBlockNames = blockInterface_->pasteRows();
    int pasteCount = static_cast<int>(pastedBlockNames.size());
    if (pasteCount == 0)
    {
        return;
    }

    int rowEnd = rowBegin + pasteCount - 1;

    beginInsertRows(QModelIndex(), rowBegin, rowEnd);

    for (int i = rowBegin; i <= rowEnd; ++i)
    {
        emit itemAdded(i);
    }

    endInsertRows();

    QStringList pastedExpressions;
    for (auto expression : blockInterface_->getExpressionsInSelectedItems(pastedBlockNames))
    {
        pastedExpressions.append(QString::fromStdString(expression));
    }
    increaseReferencesInPastedExpressions(pastedExpressions);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::increaseReferencesInPastedField()
//-----------------------------------------------------------------------------
void MemoryMapModel::increaseReferencesInPastedExpressions(QStringList const& expressionList)
{
    ReferenceCalculator referenceCalculator(getParameterFinder());

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressionList);

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

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList MemoryMapModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:addressBlock";

    return types;
}
