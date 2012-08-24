/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapmodel.cpp
 *		Project: Kactus 2
 */

#include "memorymapmodel.h"
#include "memorymapdelegate.h"
#include <models/addressblock.h>
#include <models/generaldeclarations.h>

MemoryMapModel::MemoryMapModel(QSharedPointer<MemoryMap> memoryMap,
							   QObject *parent):
QAbstractTableModel(parent),
memoryMap_(memoryMap),
items_(memoryMap->getItems()) {

	Q_ASSERT(memoryMap_);
}

MemoryMapModel::~MemoryMapModel() {
}

int MemoryMapModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return items_.size();
}

int MemoryMapModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return MemoryMapDelegate::COLUMN_COUNT;
}

Qt::ItemFlags MemoryMapModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

QVariant MemoryMapModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case MemoryMapDelegate::USAGE_COLUMN: {
				return tr("Usage");
											}
			case MemoryMapDelegate::NAME_COLUMN: {
				return tr("Name");
													}
			case MemoryMapDelegate::BASE_COLUMN: {
				return tr("Base address");
												 }
			case MemoryMapDelegate::RANGE_COLUMN: {
				return tr("Range");
												   }
			case MemoryMapDelegate::WIDTH_COLUMN: {
				return tr("Width");
												  }
			case MemoryMapDelegate::DESCRIPTION_COLUMN: {
				return tr("Description");
												  }
			case MemoryMapDelegate::ACCESS_COLUMN: {
				return tr("Access");
												  }
			case MemoryMapDelegate::VOLATILE_COLUMN: {
				return tr("Volatile");
												  }
			default: {
				return QVariant();
					 }
		}
	}
	else {
		return QVariant();
	}
}

QVariant MemoryMapModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case MemoryMapDelegate::USAGE_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					return General::usage2Str(addrBlock->getUsage());
				}
				else {
					return QVariant();
				}
												  }
			case MemoryMapDelegate::NAME_COLUMN: {
				return items_.at(index.row())->getName();
													}
			case MemoryMapDelegate::BASE_COLUMN: {
				return items_.at(index.row())->getBaseAddress();
												 }
			case MemoryMapDelegate::RANGE_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					return addrBlock->getRange();
				}
				else {
					return QVariant();
				}
												  }
			case MemoryMapDelegate::WIDTH_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					return addrBlock->getWidth();
				}
				else {
					return QVariant();
				}
												  }
			case MemoryMapDelegate::DESCRIPTION_COLUMN: {
				return items_.at(index.row())->getDescription();
												  }
			case MemoryMapDelegate::ACCESS_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					return General::access2Str(addrBlock->getAccess());
				}
				else {
					return QVariant();
				}
												   }
			case MemoryMapDelegate::VOLATILE_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					return General::BooleanValue2Bool(addrBlock->getVolatile(), false);
				}
				else {
					return QVariant();
				}
												  }
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		if (items_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case MemoryMapDelegate::USAGE_COLUMN:
			case MemoryMapDelegate::NAME_COLUMN:
			case MemoryMapDelegate::BASE_COLUMN:
			case MemoryMapDelegate::RANGE_COLUMN:
			case MemoryMapDelegate::WIDTH_COLUMN: {
				return QColor("LemonChiffon");
												 }
			default:
				return QColor("white");
		}
	}
	else {
		return QVariant();
	}
}

bool MemoryMapModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case MemoryMapDelegate::USAGE_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					addrBlock->setUsage(General::str2Usage(value.toString(), General::USAGE_COUNT));
					break;
				}
				// if the item is not address block then usage can not be saved
				else {
					return false;
				}
												  }
			case MemoryMapDelegate::NAME_COLUMN: {
				items_.at(index.row())->setName(value.toString());
				break;
													}
			case MemoryMapDelegate::BASE_COLUMN: {
				items_.at(index.row())->setBaseAddress(value.toString());
				break;
												 }
			case MemoryMapDelegate::RANGE_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					addrBlock->setRange(value.toString());
					break;
				}
				// if the item is not address block then range can not be saved
				else {
					return false;
				}
												  }
			case MemoryMapDelegate::WIDTH_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					addrBlock->setWidth(value.toInt());
					break;
				}
				else {
					return false;
				}
												  }
			case MemoryMapDelegate::DESCRIPTION_COLUMN: {
				items_.at(index.row())->setDescription(value.toString());
				break;
												  }
			case MemoryMapDelegate::ACCESS_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					addrBlock->setAccess(General::str2Access(value.toString(), General::ACCESS_COUNT));
					break;
				}
				else {
					return false;
				}
												   }
			case MemoryMapDelegate::VOLATILE_COLUMN: {
				QSharedPointer<AddressBlock> addrBlock = items_.at(index.row()).dynamicCast<AddressBlock>();
				if (addrBlock) {
					addrBlock->setVolatile(General::bool2BooleanValue(value.toBool()));
					break;
				}
				else {
					return false;
				}
												  }
			default: {
				return false;
					 }
		}

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else {
		return false;
	}
}

bool MemoryMapModel::isValid() const {
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
		if (!memItem->isValid()) {
			return false;
		}
	}
	return true;
}

void MemoryMapModel::onAddItem( const QModelIndex& index ) {
	int row = items_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	items_.insert(row, QSharedPointer<AddressBlock>(new AddressBlock()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit itemAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void MemoryMapModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= items_.size()) {
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
