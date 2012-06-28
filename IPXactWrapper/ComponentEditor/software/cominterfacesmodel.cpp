/* 
 *  	Created on: 28.6.2012
 *      Author: Antti Kamppi
 * 		filename: cominterfacesmodel.cpp
 *		Project: Kactus 2
 */

#include "cominterfacesmodel.h"
#include "cominterfacesdelegate.h"
#include <models/generaldeclarations.h>

#include <QColor>

ComInterfacesModel::ComInterfacesModel(QSharedPointer<Component> component,
									   QObject *parent):
QAbstractTableModel(parent),
comIfs_(component->getComInterfaces()) {

	Q_ASSERT(component);
}

ComInterfacesModel::~ComInterfacesModel() {
}

int ComInterfacesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return comIfs_.size();
}

int ComInterfacesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return ComInterfacesDelegate::COLUMN_COUNT;
}

Qt::ItemFlags ComInterfacesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	// Com definition column can not be edited.
	if (ComInterfacesDelegate::COM_DEF_COLUMN == index.column()) {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant ComInterfacesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case ComInterfacesDelegate::NAME_COLUMN: {
				return tr("Name");
												}
			case ComInterfacesDelegate::COM_DEF_COLUMN: {
				return tr("COM definition");
														}
			case ComInterfacesDelegate::TRANSFER_TYPE_COLUMN: {
				return tr("Transfer type");
													 }
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
				return tr("Direction");
													   }
			case ComInterfacesDelegate::DESCRIPTION_COLUMN: {
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

QVariant ComInterfacesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= comIfs_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case ComInterfacesDelegate::NAME_COLUMN: {
				return comIfs_.at(index.row())->getName();
												}
			case ComInterfacesDelegate::COM_DEF_COLUMN: {
				return comIfs_.at(index.row())->getComType().toString(":");
														}
			case ComInterfacesDelegate::TRANSFER_TYPE_COLUMN: {
				return comIfs_.at(index.row())->getTransferType();
													 }
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
				return General::direction2Str(comIfs_.at(index.row())->getDirection());
													   }
			case ComInterfacesDelegate::DESCRIPTION_COLUMN: {
				return comIfs_.at(index.row())->getDescription();
															}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		if (comIfs_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case ComInterfacesDelegate::NAME_COLUMN:
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
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

bool ComInterfacesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= comIfs_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case ComInterfacesDelegate::NAME_COLUMN: {
				comIfs_[index.row()]->setName(value.toString());
				break;
												}
			case ComInterfacesDelegate::COM_DEF_COLUMN: {
				Q_ASSERT(false);
				return false;
														}
			case ComInterfacesDelegate::TRANSFER_TYPE_COLUMN: {
				comIfs_[index.row()]->setTransferType(value.toString());
				break;
													 }
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
				comIfs_[index.row()]->setDirection(General::str2Direction(value.toString(), General::IN));
				break;
													   }
			case ComInterfacesDelegate::DESCRIPTION_COLUMN: {
				comIfs_[index.row()]->setDescription(value.toString());
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

bool ComInterfacesModel::isValid() const {
	foreach (QSharedPointer<ComInterface> comIf, comIfs_) {
		if (!comIf->isValid()) {
			return false;
		}
	}
	return true;
}

void ComInterfacesModel::onAddItem( const QModelIndex& index ) {
	int row = comIfs_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	comIfs_.insert(row, QSharedPointer<ComInterface>(new ComInterface()));
	endInsertRows();

	// inform navigation tree that COM interface is added
	emit comIfAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ComInterfacesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= comIfs_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	comIfs_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that COM interface has been removed
	emit comIfRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
