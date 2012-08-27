/* 
 *  	Created on: 21.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsmodel.cpp
 *		Project: Kactus 2
 */

#include "memorymapsmodel.h"
#include "memorymapsdelegate.h"

#include <QColor>

MemoryMapsModel::MemoryMapsModel( QSharedPointer<Component> component,
								 QObject *parent ):
QAbstractTableModel(parent),
component_(component),
memMaps_(component->getMemoryMaps()) {

	Q_ASSERT(component_);
}

MemoryMapsModel::~MemoryMapsModel() {
}

int MemoryMapsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return memMaps_.size();
}

int MemoryMapsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return MemoryMapsDelegate::COLUMN_COUNT;
}

Qt::ItemFlags MemoryMapsModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant MemoryMapsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case MemoryMapsDelegate::NAME_COLUMN: {
				return tr("Memory map\nname");
										  }
			case MemoryMapsDelegate::AUB_COLUMN: {
				return tr("Address unit\nbits");
												 }
			case MemoryMapsDelegate::DESC_COLUMN: {
				return tr("Description");
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

QVariant MemoryMapsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= memMaps_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case MemoryMapsDelegate::NAME_COLUMN: {
				return memMaps_.at(index.row())->getName();
										  }
			case MemoryMapsDelegate::AUB_COLUMN: {
				return memMaps_.at(index.row())->getAddressUnitBits();
										  }
			case MemoryMapsDelegate::DESC_COLUMN: {
				return memMaps_.at(index.row())->getDescription();
												 }
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		if (memMaps_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case MemoryMapsDelegate::NAME_COLUMN: {
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

bool MemoryMapsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= memMaps_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case MemoryMapsDelegate::NAME_COLUMN: {
				memMaps_[index.row()]->setName(value.toString());
				break;
										  }
			case MemoryMapsDelegate::AUB_COLUMN: {
				memMaps_[index.row()]->setAddressUnitBits(value.toUInt());
				break;
										  }
			case MemoryMapsDelegate::DESC_COLUMN: {
				memMaps_[index.row()]->setDescription(value.toString());
				break;
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

bool MemoryMapsModel::isValid() const {
	foreach (QSharedPointer<MemoryMap> memMap, memMaps_) {
		if (!memMap->isValid()) {
			return false;
		}
	}
	return true;
}

void MemoryMapsModel::onAddItem( const QModelIndex& index ) {
	int row = memMaps_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	memMaps_.insert(row, QSharedPointer<MemoryMap>(new MemoryMap()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit memoryMapAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void MemoryMapsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= memMaps_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	memMaps_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit memoryMapRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
