/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapmodel.cpp
 *		Project: Kactus 2
 */

#include "memorymapmodel.h"

#include "MemoryMapColumns.h"

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QColor>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::MemoryMapModel()
//-----------------------------------------------------------------------------
MemoryMapModel::MemoryMapModel(QSharedPointer<MemoryMap> memoryMap,
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
							   QObject *parent):
QAbstractTableModel(parent),
memoryMap_(memoryMap),
componentChoices_(componentChoices),
items_(memoryMap->getItems()) 
{
	Q_ASSERT(memoryMap_);
}

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
	return items_.size();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::columnCount()
//-----------------------------------------------------------------------------
int MemoryMapModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid()) {
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
            return tr("Base address\n [AUB]");
        }
        else if (section == MemoryMapColumns::RANGE_COLUMN)
        {
            return tr("Range\n [AUB]");
        }
        else if (section == MemoryMapColumns::WIDTH_COLUMN)
        {
            return tr("Width\n [bits]");
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
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
		return QVariant();
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == MemoryMapColumns::USAGE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock) {
                return General::usage2Str(addrBlock->getUsage());
            }
            else {
                return QVariant();
            }
        }
        else if (index.column() == MemoryMapColumns::NAME_COLUMN)
        {
            return items_.at(index.row())->getName();
        }
        else if (index.column() == MemoryMapColumns::BASE_COLUMN)
        {
            return items_.at(index.row())->getBaseAddress();
        }
        else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock) {
                return addrBlock->getRange();
            }
            else {
                return QVariant();
            }
        }
        else if (index.column() == MemoryMapColumns::WIDTH_COLUMN) {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock) {
                return addrBlock->getWidth();
            }
            else {
                return QVariant();
            }
        }
        else if (index.column() == MemoryMapColumns::DESCRIPTION_COLUMN) {
            return items_.at(index.row())->getDescription();
        }
        else if (index.column() == MemoryMapColumns::ACCESS_COLUMN) {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock) {
                return General::access2Str(addrBlock->getAccess());
            }
            else {
                return QVariant();
            }
        }
        else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock) {
                return General::BooleanValue2Bool(addrBlock->getVolatile(), false);
            }
            else {
                return QVariant();
            }
        }
        else {
            return QVariant();
        }
    }

	else if (role == Qt::ForegroundRole)
    {
		if (items_.at(index.row())->isValid(componentChoices_))
        {
			return QColor("black");
		}
		else
        {
			return QColor("red");
		}
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == MemoryMapColumns::USAGE_COLUMN ||
            index.column() == MemoryMapColumns::NAME_COLUMN ||
            index.column() == MemoryMapColumns::BASE_COLUMN ||
            index.column() == MemoryMapColumns::RANGE_COLUMN ||
            index.column() == MemoryMapColumns::WIDTH_COLUMN) 
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
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
		return false;
	}

    if (Qt::EditRole == role)
    {
        if (index.column() == MemoryMapColumns::USAGE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
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
            items_.at(index.row())->setName(value.toString());
        }									
        else if (index.column() == MemoryMapColumns::BASE_COLUMN)
        {
            items_.at(index.row())->setBaseAddress(value.toString());
        }
        else if (index.column() == MemoryMapColumns::RANGE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
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
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock) {
                addrBlock->setWidth(value.toInt());
            }
            else 
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::DESCRIPTION_COLUMN)
        {
            items_.at(index.row())->setDescription(value.toString());
        }
        else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                addrBlock->setAccess(General::str2Access(value.toString(), General::ACCESS_COUNT));
            }
            else 
            {
                return false;
            }
        }
        else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
        {
            QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                addrBlock->setVolatile(General::bool2BooleanValue(value.toBool()));
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
// Function: MemoryMapModel::isValid()
//-----------------------------------------------------------------------------
bool MemoryMapModel::isValid() const
{
	foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
		if (!memItem->isValid(componentChoices_)) 
        {
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::onAddItem()
//-----------------------------------------------------------------------------
void MemoryMapModel::onAddItem( QModelIndex const& index )
{
	int row = items_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	// the address where the current memory map ends
	quint64 previousEnd = memoryMap_->getLastAddress();
	// if this is the first item to add then do not increase address
	if (previousEnd != 0)
    {
		++previousEnd;
	}

	// convert the address to hexadecimal form
	QString newBase = QString::number(previousEnd, 16);
	newBase = newBase.toUpper();
	newBase.prepend("0x");

	beginInsertRows(QModelIndex(), row, row);
	QSharedPointer<AddressBlock> addrBlock(new AddressBlock());
	addrBlock->setWidth(32);
	addrBlock->setRange("4");
	addrBlock->setBaseAddress(newBase);
	items_.insert(row, addrBlock);
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
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	items_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit itemRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
