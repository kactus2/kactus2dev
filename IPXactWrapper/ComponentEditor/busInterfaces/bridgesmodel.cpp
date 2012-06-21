/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgesmodel.cpp
 *		Project: Kactus 2
 */

#include "bridgesmodel.h"
#include "bridgesdelegate.h"

BridgesModel::BridgesModel(QSharedPointer<SlaveInterface> slave, 
						   QObject *parent):
QAbstractTableModel(parent),
bridges_(slave->getBridges()) {

}

BridgesModel::~BridgesModel() {
}

int BridgesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return bridges_.size();
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
	else if (index.row() < 0 || index.row() >= bridges_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case BridgesDelegate::MASTER_COLUMN: {
				return bridges_.at(index.row())->masterRef_;
												}
			case BridgesDelegate::OPAQUE_COLUMN: {
				return bridges_.at(index.row())->opaque_;
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

bool BridgesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
// 	if (!index.isValid()) {
// 		return false;
// 	}
// 	else if (index.row() < 0 || index.row() >= channels_.size()) {
// 		return false;
// 	}
// 
// 	if (Qt::EditRole == role) {
// 
// 		switch (index.column()) {
// 			case ChannelsDelegate::NAME_COLUMN: {
// 				channels_[index.row()]->setName(value.toString());
// 				break;
// 												}
// 			case ChannelsDelegate::DISPLAY_NAME_COLUMN: {
// 				channels_[index.row()]->setDisplayName(value.toString());
// 				break;
// 														}
// 			case ChannelsDelegate::INTERFACE_COLUMN: {
// 				QString str = value.toString();
// 				QStringList interfaceNames = str.split(' ', QString::SkipEmptyParts);
// 				channels_[index.row()]->setInterfaces(interfaceNames);
// 				break;
// 													 }
// 			case ChannelsDelegate::DESCRIPTION_COLUMN: {
// 				channels_[index.row()]->setDescription(value.toString());
// 				break;
// 													   }
// 			default: {
// 				return false;
// 					 }
// 		}
// 
// 		emit dataChanged(index, index);
// 		emit contentChanged();
// 		return true;
// 	}
// 	// user edit role for interface column operates on QStringList
// 	else if (ChannelsDelegate::USER_EDIT_ROLE == role &&
// 		ChannelsDelegate::INTERFACE_COLUMN == index.column()) {
// 			channels_[index.row()]->setInterfaces(value.toStringList());
// 			emit dataChanged(index, index);
// 			emit contentChanged();
// 			return true;
// 	}
// 	else {
// 		return false;
// 	}
// 	
	return false;
}

void BridgesModel::onAddItem( const QModelIndex& index ) {
	int row = bridges_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	bridges_.insert(row, QSharedPointer<SlaveInterface::Bridge>(new SlaveInterface::Bridge()));
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
	else if (index.row() < 0 || index.row() >= bridges_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	bridges_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void BridgesModel::refresh( QSharedPointer<SlaveInterface> slave ) {
	beginResetModel();
	bridges_ = slave->getBridges();
	endResetModel();
}

bool BridgesModel::isValid() const {
	// interface names are needed to check that references to bus interfaces are valid
// 	QStringList interfaceNames = component_->getBusInterfaceNames();
// 
// 	// if at least one address space is invalid
// 	foreach (QSharedPointer<Channel> channel, channels_) {
// 		if (!channel->isValid(interfaceNames)) {
// 			return false;
// 		}
// 	}
	// all address spaces were valid
	return true;
}
