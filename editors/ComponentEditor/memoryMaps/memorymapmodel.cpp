//-----------------------------------------------------------------------------
// File: memorymapmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The model to manage the address blocks of a single memory map.
//-----------------------------------------------------------------------------

#include "memorymapmodel.h"

#include "MemoryMapColumns.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/AddressBlockExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>

#include <QColor>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::MemoryMapModel()
//-----------------------------------------------------------------------------
MemoryMapModel::MemoryMapModel(QSharedPointer<MemoryMapBase> memoryRemap,
                               QSharedPointer<ExpressionParser> expressionParser,
                               QSharedPointer<ParameterFinder> parameterFinder,
                               QSharedPointer<ExpressionFormatter> expressionFormatter,
                               QSharedPointer<AddressBlockValidator> addressBlockValidator,
                               QString const& addressUnitBits, QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
memoryRemap_(memoryRemap),
memoryBlocks_(memoryRemap->getMemoryBlocks()),
expressionFormatter_(expressionFormatter),
addressBlockValidator_(addressBlockValidator),
addressUnitBits_(addressUnitBits)
{
    setExpressionParser(expressionParser);

    Q_ASSERT(memoryRemap_);
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
	return memoryBlocks_->size();
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
        else if (section == MemoryMapColumns::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
        else if (section == MemoryMapColumns::ACCESS_COLUMN)
        {
            return tr("Access");
        }
        else if (section == MemoryMapColumns::VOLATILE_COLUMN)
        {
            return tr("Volatile");
        }
        else {
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
	if (!index.isValid() || index.row() < 0 || index.row() >= memoryBlocks_->size())
    {
		return QVariant();
	}

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            if (role == Qt::DisplayRole)
            {
                return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
            }
            else if (role == Qt::ToolTipRole)
            {
                return formattedValueFor(valueForIndex(index).toString());
            }
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
        if (index.column() == MemoryMapColumns::DESCRIPTION_COLUMN)
        {
            return valueForIndex(index);
        }

        return expressionOrValueForIndex(index);
    }

	else if (role == Qt::ForegroundRole)
    {
        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == MemoryMapColumns::NAME_COLUMN || index.column() == MemoryMapColumns::BASE_COLUMN ||
            index.column() == MemoryMapColumns::RANGE_COLUMN || index.column() == MemoryMapColumns::WIDTH_COLUMN) 
        {
            return QColor("LemonChiffon");
        }
        else 
        {
            return QColor("white");
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
	if (!index.isValid() || index.row() < 0 || index.row() >= memoryBlocks_->size())
    {
		return false;
	}

    if (Qt::EditRole == role)
    {
        if (index.column() == MemoryMapColumns::USAGE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                addrBlock->setUsage(General::str2Usage(value.toString(), General::USAGE_COUNT));
            }
            // if the item is not address block then usage can not be saved
            else 
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::NAME_COLUMN)
        {
            memoryBlocks_->at(index.row())->setName(value.toString());
        }									
        else if (index.column() == MemoryMapColumns::BASE_COLUMN)
        {
            if (!value.isValid())
            {
                removeReferencesFromSingleExpression(memoryBlocks_->at(index.row())->getBaseAddress());
            }

            memoryBlocks_->at(index.row())->setBaseAddress(value.toString());
        }
        else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(addrBlock->getRange());
                }
                addrBlock->setRange(value.toString());
            }
            // if the item is not address block then range can not be saved
            else 
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                if (!value.isValid())
                {
                    removeReferencesFromSingleExpression(addrBlock->getWidth());
                }
                addrBlock->setWidth(value.toString());
            }
            else 
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::DESCRIPTION_COLUMN)
        {
            memoryBlocks_->at(index.row())->setDescription(value.toString());
        }
        else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                addrBlock->setAccess(AccessTypes::str2Access(value.toString(), AccessTypes::ACCESS_COUNT));
            }
            else 
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                addrBlock->setVolatile(value.toBool());
            }
            else 
            {
                return false;
            }
        }
        else 
        {
            return false;
        }

        if (index.column() == MemoryMapColumns::NAME_COLUMN || index.column() == MemoryMapColumns::BASE_COLUMN ||
            index.column() == MemoryMapColumns::RANGE_COLUMN || index.column() == MemoryMapColumns::WIDTH_COLUMN)
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
    addressUnitBits_ = newAddressUnitBits;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onAddItem()
//-----------------------------------------------------------------------------
void MemoryMapModel::onAddItem( QModelIndex const& index )
{
	int row = memoryBlocks_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	// the address where the current memory map ends
    quint64 previousEnd = getLastAddress();
	// if this is the first item to add then do not increase address
	if (previousEnd != 0)
    {
		++previousEnd;
	}
    
    int lastBaseAddress = 0;
    int lastRange = 0;

    for (int itemIndex = 0; itemIndex < memoryBlocks_->size(); ++itemIndex)
    {
        QString calculatedExpression = parseExpressionToDecimal(memoryBlocks_->at(itemIndex)->getBaseAddress());
        int addressBlockBaseAddress = calculatedExpression.toInt();

        if (addressBlockBaseAddress > lastBaseAddress ||
            (itemIndex == memoryBlocks_->size() - 1 && lastBaseAddress == 0))
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlocks_->at(itemIndex).dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                lastBaseAddress = addressBlockBaseAddress;
                lastRange = parseExpressionToDecimal(addressBlock->getRange()).toInt();
            }
        }
    }


	// convert the address to hexadecimal form
    QString newBase = QString::number(lastBaseAddress + lastRange, 16);
	newBase = newBase.toUpper();
    newBase.prepend("'h");

	beginInsertRows(QModelIndex(), row, row);
	QSharedPointer<AddressBlock> addrBlock(new AddressBlock());
    addrBlock->setWidth("32");
	addrBlock->setRange("4");
	addrBlock->setBaseAddress(newBase);
	memoryBlocks_->insert(row, addrBlock);
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
	if (!index.isValid() || index.row() < 0 || index.row() >= memoryBlocks_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    QSharedPointer<AddressBlock> addressBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
    if (addressBlock)
    {
        decreaseReferencesWithRemovedAddressBlock(addressBlock);
    }

	memoryBlocks_->removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit itemRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::decreaseReferencesWithRemovedAddressBlock()
//-----------------------------------------------------------------------------
void MemoryMapModel::decreaseReferencesWithRemovedAddressBlock(QSharedPointer<AddressBlock> removedAddressBlock)
{
    AddressBlockExpressionGatherer addressBlockGatherer;
    QStringList expressionList = addressBlockGatherer.getExpressions(removedAddressBlock);

    ReferenceCalculator referenceCalculator(getParameterFinder());
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressionList);

    foreach (QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool MemoryMapModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == MemoryMapColumns::BASE_COLUMN || index.column() == MemoryMapColumns::RANGE_COLUMN ||
        index.column() == MemoryMapColumns::WIDTH_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    QSharedPointer<AddressBlock> addressBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();

    if (addressBlock)
    {
        if (index.column() == MemoryMapColumns::BASE_COLUMN)
        {
            return addressBlock->getBaseAddress();
        }
        else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            return addressBlock->getRange();
        }
        else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
        {
            return addressBlock->getWidth();
        }
        else
        {
            return data(index, Qt::DisplayRole);
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::validateIndex()
//-----------------------------------------------------------------------------
bool MemoryMapModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<AddressBlock> addressBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
    if (!addressBlock)
    {
        return false;
    }

    if (index.column() == MemoryMapColumns::NAME_COLUMN)
    {
        return addressBlockValidator_->hasValidName(addressBlock);
    }
    else if (index.column() == MemoryMapColumns::BASE_COLUMN)
    {
        return addressBlockValidator_->hasValidBaseAddress(addressBlock);
    }
    else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
    {
        return addressBlockValidator_->hasValidRange(addressBlock);
    }
    else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
    {
        return addressBlockValidator_->hasValidWidth(addressBlock);
    }
    else if (index.column() == MemoryMapColumns::USAGE_COLUMN)
    {
        return addressBlockValidator_->hasValidUsage(addressBlock);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int MemoryMapModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    QSharedPointer<AddressBlock> addressBlock = memoryBlocks_->at(row).dynamicCast<AddressBlock>();

    if (addressBlock)
    {
        int referencesInBaseAddress = memoryBlocks_->at(row)->getBaseAddress().count(valueID);
        int referencesInRange = addressBlock->getRange().count(valueID);
        int referencesInWidth = addressBlock->getWidth().count(valueID);

        int totalReferences = referencesInBaseAddress + referencesInRange + referencesInWidth;
        return totalReferences;
    }

    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == MemoryMapColumns::NAME_COLUMN)
    {
        return memoryBlocks_->at(index.row())->name();
    }
    else if (index.column() == MemoryMapColumns::DESCRIPTION_COLUMN)
    {
        return memoryBlocks_->at(index.row())->description();
    }
    else if (index.column() == MemoryMapColumns::BASE_COLUMN)
    {
        return memoryBlocks_->at(index.row())->getBaseAddress();
    }

    QSharedPointer<AddressBlock> addressBlock = memoryBlocks_->at(index.row()).dynamicCast<AddressBlock>();
    if (addressBlock)
    {
        if (index.column() == MemoryMapColumns::USAGE_COLUMN)
        {
            return General::usage2Str(addressBlock->getUsage());
        }

        else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            return addressBlock->getRange();
        }
        else if (index.column() == MemoryMapColumns::WIDTH_COLUMN)
        {
            return addressBlock->getWidth();
        }

        else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
        {
            return AccessTypes::access2Str(addressBlock->getAccess());
        }
        else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
        {
            return addressBlock->getVolatile();
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryMapModel::getLastAddress() const
{
    int index = -1;
    unsigned int lastBaseAddress = 0;

    for (int i = 0; i < memoryRemap_->getMemoryBlocks()->size(); ++i)
    {
        AddressBlock* block = dynamic_cast<AddressBlock*>(memoryRemap_->getMemoryBlocks()->at(i).data());
        if (block != 0)
        {
            bool ok = true;
//             unsigned int baseAddress = General::str2Uint(block->getBaseAddress());
            unsigned int baseAddress = parseExpressionToDecimal(block->getBaseAddress()).toUInt(&ok);

            if (ok)
            {
                if (baseAddress >= lastBaseAddress)
                {
                    lastBaseAddress = baseAddress;
                    index = i;
                }
            }
        }
    }

    if (index == -1)
    {
        return 0;
    }

    AddressBlock* block = dynamic_cast<AddressBlock*>(memoryRemap_->getMemoryBlocks()->at(index).data());

//     return lastBaseAddress + General::str2Uint(static_cast<AddressBlock*>(memoryRemap_->getMemoryBlocks()->at(index).data())->getRange()) - 1;
    return lastBaseAddress + parseExpressionToDecimal(block->getRange()).toInt() - 1;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onCopyRows()
//-----------------------------------------------------------------------------
void MemoryMapModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<MemoryBlockBase> > copiedBlocks;
    foreach (QModelIndex index, indexList)
    {
        QSharedPointer<MemoryBlockBase> memoryBlock = memoryBlocks_->at(index.row());
        copiedBlocks.append(memoryBlock);
    }

    QVariant memoryBlockVariant;
    memoryBlockVariant.setValue(copiedBlocks);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:addressBlock", QByteArray());
    newMimeData->setImageData(memoryBlockVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onPasteRows()
//-----------------------------------------------------------------------------
void MemoryMapModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<MemoryBlockBase> > >())
        {
            AddressBlockExpressionGatherer gatherer;
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<MemoryBlockBase> > newMemoryBlocks =
                pasteVariant.value<QList<QSharedPointer<MemoryBlockBase> > >();

            int rowBegin = memoryBlocks_->size();
            int rowEnd = rowBegin + newMemoryBlocks.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            foreach(QSharedPointer<MemoryBlockBase> copiedBlock, newMemoryBlocks)
            {
                QSharedPointer<AddressBlock> addressBlock = copiedBlock.dynamicCast<AddressBlock>();
                if (addressBlock)
                {
                    QSharedPointer<AddressBlock> newBlock (new AddressBlock(*addressBlock.data()));
                    newBlock->setName(getUniqueName(newBlock->name(), getCurrentItemNames()));

                    memoryBlocks_->append(newBlock);

                    increaseReferencesInPastedAddressBlock(newBlock, gatherer, referenceCalculator);

                    emit itemAdded(memoryBlocks_->size() - 1);
                }
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapmodel::getCurrentItemNames()
//-----------------------------------------------------------------------------
QStringList MemoryMapModel::getCurrentItemNames()
{
    QStringList names;
    foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *memoryBlocks_)
    {
        names.append(memoryBlock->name());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::increaseReferencesInPastedAddressBlock()
//-----------------------------------------------------------------------------
void MemoryMapModel::increaseReferencesInPastedAddressBlock(QSharedPointer<AddressBlock> pastedBlock,
    AddressBlockExpressionGatherer& gatherer, ReferenceCalculator& referenceCalculator)
{
    QStringList blockExpressions = gatherer.getExpressions(pastedBlock);

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(blockExpressions);

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
// Function: MemoryMapModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList MemoryMapModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:addressBlock";

    return types;
}
