/* 
 *  	Created on: 31.5.2012
 *      Author: Antti Kamppi
 * 		filename: filebuildersmodel.cpp
 *		Project: Kactus 2
 */


#include "filebuildersmodel.h"

#include <QColor>

FileBuildersModel::FileBuildersModel( QList<QSharedPointer<FileBuilder> >& fileBuilders, 
									 QObject* parent ):
QAbstractTableModel(parent),
fileBuilders_(fileBuilders) {

}

FileBuildersModel::~FileBuildersModel() {
}

int FileBuildersModel::rowCount( const QModelIndex& parent /*= QModelIndex() */ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return fileBuilders_.size();
}

int FileBuildersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const {
	if (parent.isValid()) {
		return 0;
	}

	return 4;
}

QVariant FileBuildersModel::data( const QModelIndex& index, 
								 int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid()) {
		return QVariant();
	}

	// if row is invalid
	else if (index.row() < 0 || index.row() >= fileBuilders_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {
		
		switch (index.column()) {
			case 0:
				return fileBuilders_.at(index.row())->getFileType();
			case 1:
				return fileBuilders_.at(index.row())->getCommand();
			case 2:
				return fileBuilders_.at(index.row())->getFlags();
			case 3:
				return fileBuilders_.at(index.row())->getReplaceDefaultFlags();
			default:
				return QVariant();
		}
	}
	else if (Qt::BackgroundRole == role) {
		if (index.column() == 0) {
			return QColor("LemonChiffon");
		}
		else {
			return QColor("white");
		}
	}
	else if (Qt::ForegroundRole == role) {
		if (fileBuilders_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant FileBuildersModel::headerData( int section, Qt::Orientation orientation, 
									   int role /*= Qt::DisplayRole */ ) const {

	if (orientation != Qt::Horizontal) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (section) {
			case 0:
				return tr("File type");
			case 1:
				return tr("Command");
			case 2:
				return tr("Flags");
			case 3:
				return tr("Replace default flags");
			default:
				return QVariant();
		}
	}
	// if unsupported role
	else
		return QVariant();
}

bool FileBuildersModel::setData( const QModelIndex& index, 
								const QVariant& value, 
								int role /*= Qt::EditRole */ ) {

	if (!index.isValid()) {
		return false;
	}

	// if row is invalid
	else if (index.row() < 0 || index.row() >= fileBuilders_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case 0: {
				fileBuilders_.at(index.row())->setFileType(value.toString());
				break;
					}
			case 1: {
				fileBuilders_.at(index.row())->setCommand(value.toString());
				break;
					}
			case 2: {
				fileBuilders_.at(index.row())->setFlags(value.toString());
				break;
					}
			case 3: {
				fileBuilders_.at(index.row())->setReplaceDefaultFlags(value.toBool());
				break;
					}
			default:
				return false;
		}

		emit contentChanged();
		emit dataChanged(index, index);
		return true;
	}
	// if unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags FileBuildersModel::flags( const QModelIndex& index ) const {

	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool FileBuildersModel::isValid() const {
	
	// check all file builders
	foreach (QSharedPointer<FileBuilder> fileBuilder, fileBuilders_) {

		// if one is invalid
		if (!fileBuilder->isValid()) {
			return false;
		}
	}

	return true;
}

void FileBuildersModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= fileBuilders_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	fileBuilders_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void FileBuildersModel::onAddItem( const QModelIndex& index ) {
	int row = fileBuilders_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	fileBuilders_.insert(row, QSharedPointer<FileBuilder>(new FileBuilder()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
