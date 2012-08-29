/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: enumeratedvaluemodel.cpp
 *		Project: Kactus 2
 */

#include "enumeratedvaluemodel.h"
#include "enumeratedvaluedelegate.h"

EnumeratedValueModel::EnumeratedValueModel(QSharedPointer<Field> field, 
										   QObject *parent):
QAbstractTableModel(parent),
field_(field),
enumValues_(field->getEnumeratedValues()) {

	Q_ASSERT(field_);
}

EnumeratedValueModel::~EnumeratedValueModel() {
}

int EnumeratedValueModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return enumValues_.size();
}

int EnumeratedValueModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return EnumeratedValueDelegate::COLUMN_COUNT;
}

Qt::ItemFlags EnumeratedValueModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant EnumeratedValueModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case EnumeratedValueDelegate::NAME_COLUMN: {
				return tr("Enumeration name");
												}
			case EnumeratedValueDelegate::VALUE_COLUMN: {
				return tr("Value");
												}
			case EnumeratedValueDelegate::USAGE_COLUMN: {
				return tr("Usage");
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

QVariant EnumeratedValueModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= enumValues_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case EnumeratedValueDelegate::NAME_COLUMN: {
				return enumValues_.at(index.row())->getName();
													   }
			case EnumeratedValueDelegate::VALUE_COLUMN: {
				return enumValues_.at(index.row())->getValue();
														}
			case EnumeratedValueDelegate::USAGE_COLUMN: {
				return EnumeratedValue::usage2Str(enumValues_.at(index.row())->getUsage());
												  }
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		if (enumValues_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case EnumeratedValueDelegate::NAME_COLUMN:
			case EnumeratedValueDelegate::VALUE_COLUMN:{
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

bool EnumeratedValueModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= enumValues_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case EnumeratedValueDelegate::NAME_COLUMN: {
				enumValues_[index.row()]->setName(value.toString());
				break;
													   }
			case EnumeratedValueDelegate::VALUE_COLUMN: {
				enumValues_[index.row()]->setValue(value.toString());
				break;
														}
			case EnumeratedValueDelegate::USAGE_COLUMN: {
				enumValues_[index.row()]->setUsage(EnumeratedValue::str2Usage(value.toString(), EnumeratedValue::READWRITE));
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

bool EnumeratedValueModel::isValid() const {
	foreach (QSharedPointer<EnumeratedValue> enumValue, enumValues_) {
		if (!enumValue->isValid()) {
			return false;
		}
	}
	return true;
}

void EnumeratedValueModel::onAddItem( const QModelIndex& index ) {
	int row = enumValues_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	enumValues_.insert(row, QSharedPointer<EnumeratedValue>(new EnumeratedValue()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit enumAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void EnumeratedValueModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= enumValues_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	enumValues_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit enumRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
