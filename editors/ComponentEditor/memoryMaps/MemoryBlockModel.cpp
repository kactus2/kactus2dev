//-----------------------------------------------------------------------------
// File: MemoryBlockModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.02.2022
//
// Description:
// The parent model for editing memory blocks.
//-----------------------------------------------------------------------------

#include "MemoryBlockModel.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <KactusAPI/include/MemoryBlockInterface.h>

#include <common/KactusColors.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::MemoryBlockModel()
//-----------------------------------------------------------------------------
MemoryBlockModel::MemoryBlockModel(MemoryBlockInterface* blockInterface,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    Document::Revision docRevision, QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
docRevision_(docRevision),
blockInterface_(blockInterface)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::rowCount()
//-----------------------------------------------------------------------------
int MemoryBlockModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return blockInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags MemoryBlockModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::headerData()
//-----------------------------------------------------------------------------
QVariant MemoryBlockModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == nameColumn())
        {
            return tr("Name");
        }
        else if (section == baseAddressColumn())
        {
            QString baseAddress = tr("Base address\n [AUB]") + getExpressionSymbol();
            return baseAddress;
        }
        else if (section == isPresentColumn())
        {
            return tr("Is present") + getExpressionSymbol();
        }
        else if (section == descriptionColumn())
        {
            return tr("Description");
        }
	}
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::data()
//-----------------------------------------------------------------------------
QVariant MemoryBlockModel::data(QModelIndex const& index, int role) const
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
        else if (role == Qt::DisplayRole && index.column() == descriptionColumn())
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

        if (index.column() != isPresentColumn() && (!blockPresence.empty() && blockPresenceValue != 1))
        {
            return KactusColors::DISABLED_TEXT;
        }

        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == nameColumn() || index.column() == baseAddressColumn())
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else if (index.flags() == (Qt::ItemIsSelectable | Qt::ItemIsEnabled))
        {
            return KactusColors::DISABLED_FIELD;
        }
        else 
        {
            return KactusColors::REGULAR_FIELD;
        }
    }
    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::setData()
//-----------------------------------------------------------------------------
bool MemoryBlockModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= blockInterface_->itemCount())
    {
		return false;
	}

    if (Qt::EditRole == role)
    {
        std::string blockName = blockInterface_->getIndexedItemName(index.row());

        if (index.column() == nameColumn())
        {
            blockInterface_->setName(blockName, value.toString().toStdString());

            QString oldName = QString::fromStdString(blockName);
            QString newName = QString::fromStdString(blockInterface_->getIndexedItemName(index.row()));

            emit blockNameChanged(oldName, newName);
        }
        else if (index.column() == baseAddressColumn())
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(
                    QString::fromStdString(blockInterface_->getBaseAddressExpression(blockName)));
            }

            blockInterface_->setBaseAddress(blockName, value.toString().toStdString());
        }
        else if (index.column() == isPresentColumn())
        {
            blockInterface_->setIsPresent(blockName, value.toString().toStdString());
        }
        else if (index.column() == descriptionColumn())
        {
            blockInterface_->setDescription(blockName, value.toString().toStdString());
        }
        else 
        {
            return false;
        }

        if (index.column() == nameColumn() || index.column() == baseAddressColumn() ||
            index.column() == isPresentColumn())
        {
            emit graphicsChanged(index.row());
        }

        if (index.column() == baseAddressColumn() || index.column() == isPresentColumn())
        {
            emit childAddressingChanged(index.row());
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
// Function: MemoryBlockModel::addressUnitBitsUpdated()
//-----------------------------------------------------------------------------
void MemoryBlockModel::addressUnitBitsUpdated(QString const& newAddressUnitBits)
{
    blockInterface_->setAddressUnitBits(newAddressUnitBits.toStdString());
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::onAddItem()
//-----------------------------------------------------------------------------
void MemoryBlockModel::onAddItem( QModelIndex const& index )
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

    emit invalidateOtherFilter();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::onRemoveItem()
//-----------------------------------------------------------------------------
void MemoryBlockModel::onRemoveItem( QModelIndex const& index )
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

    emit invalidateOtherFilter();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryBlockModel::expressionOrValueForIndex(QModelIndex const& index) const
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
// Function: MemoryBlockModel::validateIndex()
//-----------------------------------------------------------------------------
bool MemoryBlockModel::validateIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (blockName.empty())
    {
        return false;
    }

    if (index.column() == nameColumn())
    {
        return blockInterface_->itemHasValidName(blockName);
    }
    else if (index.column() == baseAddressColumn())
    {
        return blockInterface_->hasValidBaseAddress(blockName);
    }
    else if (index.column() == isPresentColumn())
    {
        return blockInterface_->hasValidIsPresent(blockName);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int MemoryBlockModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return blockInterface_->getAllReferencesToIdInItem(
        blockInterface_->getIndexedItemName(row), valueID.toStdString());
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryBlockModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (index.column() == baseAddressColumn())
    {
        return QString::fromStdString(blockInterface_->getBaseAddressFormattedExpression(blockName));
    }
    else if (index.column() == isPresentColumn())
    {
        return QString::fromStdString(blockInterface_->getIsPresentFormattedExpression(blockName));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryBlockModel::expressionForIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (index.column() == baseAddressColumn())
    {
        return QString::fromStdString(blockInterface_->getBaseAddressExpression(blockName));
    }
    else if (index.column() == isPresentColumn())
    {
        return QString::fromStdString(blockInterface_->getIsPresentExpression(blockName));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryBlockModel::valueForIndex(QModelIndex const& index) const
{
    std::string blockName = blockInterface_->getIndexedItemName(index.row());
    if (index.column() == nameColumn())
    {
        return QString::fromStdString(blockName);
    }
    else if (index.column() == descriptionColumn())
    {
        return QString::fromStdString(blockInterface_->getDescription(blockName));
    }
    else if (index.column() == baseAddressColumn())
    {
        return QString::fromStdString(blockInterface_->getBaseAddressValue(blockName));
    }
    else if (index.column() == isPresentColumn())
    {
        return QString::fromStdString(blockInterface_->getIsPresentValue(blockName));
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::onCopyRows()
//-----------------------------------------------------------------------------
void MemoryBlockModel::onCopyRows(QModelIndexList indexList)
{
    std::vector<int> registerIndexes;
    for (auto index : indexList)
    {
        registerIndexes.push_back(index.row());
    }

    blockInterface_->copyRows(registerIndexes);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockModel::onPasteRows()
//-----------------------------------------------------------------------------
void MemoryBlockModel::onPasteRows()
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
// Function: MemoryBlockModel::increaseReferencesInPastedField()
//-----------------------------------------------------------------------------
void MemoryBlockModel::increaseReferencesInPastedExpressions(QStringList const& expressionList)
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
