/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgesmodel.cpp
 *		Project: Kactus 2
 */

#include "bridgesmodel.h"
#include "bridgesdelegate.h"

#include <QColor>

BridgesModel::BridgesModel(QSharedPointer<SlaveInterface> slave, 
						   QObject *parent):
QAbstractTableModel(parent),
bridges_(&slave->getBridges()) {

}

BridgesModel::~BridgesModel() {
}

int BridgesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return bridges_->size();
}

int BridgesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return BridgesDelegate::COLUMN_COUNT;
}

Qt::ItemFlags BridgesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	
	// for opaque column checking is possible
	else if (index.column() == BridgesDelegate::OPAQUE_COLUMN) {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant BridgesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case BridgesDelegate::MASTER_COLUMN: {
				return tr("Master bus interface");
												}
			case BridgesDelegate::OPAQUE_COLUMN: {
				return tr("Opaque");
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

QVariant BridgesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= bridges_->size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case BridgesDelegate::MASTER_COLUMN: {
				return bridges_->at(index.row())->masterRef_;
												}
			case BridgesDelegate::OPAQUE_COLUMN: {
				return bridges_->at(index.row())->opaque_;
														}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {
		if (bridges_->at(index.row())->masterRef_.isEmpty()) {
			return QColor("red");
		}
		else {
			return QColor("black");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case BridgesDelegate::MASTER_COLUMN: {
				return QColor("LemonChiffon");
								 }
			default:
				return QColor("white");
		}
	}
	// the opaque column is shown on checked or not checked
	else if (Qt::CheckStateRole == role && index.column() == BridgesDelegate::OPAQUE_COLUMN) {
		
		if (bridges_->at(index.row())->opaque_) {
			return Qt::Checked;
		}
		else {
			return Qt::Unchecked;
		}
	}
	else {
		return QVariant();
	}
}

bool BridgesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= bridges_->size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case BridgesDelegate::MASTER_COLUMN: {
				(*bridges_)[index.row()]->masterRef_ = value.toString();
				break;
												}
			case BridgesDelegate::OPAQUE_COLUMN: {
				(*bridges_)[index.row()]->opaque_ = value.toBool();
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
	else if (role == Qt::CheckStateRole) {
		bridges_->at(index.row())->opaque_ = (value == Qt::Checked);
		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else {
		return false;
	}
}

void BridgesModel::onAddItem( const QModelIndex& index ) {
	int row = bridges_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	bridges_->insert(row, QSharedPointer<SlaveInterface::Bridge>(new SlaveInterface::Bridge()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void BridgesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= bridges_->size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	bridges_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void BridgesModel::refresh( QSharedPointer<SlaveInterface> slave ) {
	Q_ASSERT(slave);
	beginResetModel();
	bridges_ = &slave->getBridges();
	endResetModel();
}

bool BridgesModel::isValid() const {

	// on each bridge the master ref must be non-empty
	foreach (QSharedPointer<SlaveInterface::Bridge> bridge, *bridges_) {
		if (bridge->masterRef_.isEmpty()) {
			return false;
		}
	}
	return true;
}
