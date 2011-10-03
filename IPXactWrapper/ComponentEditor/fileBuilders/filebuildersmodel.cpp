/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildersmodel.cpp
 */

#include "filebuildersmodel.h"

FileBuildersModel::FileBuildersModel(void* dataPointer, QObject *parent): 
QAbstractTableModel(parent),
fileBuilders_(static_cast<QList<QSharedPointer<FileBuilder> >* >(dataPointer)),
table_() {

	restore();
}

FileBuildersModel::~FileBuildersModel() {
}

int FileBuildersModel::rowCount( const QModelIndex& parent /*= QModelIndex() */ ) const {
	if (parent.isValid())
		return 0;
	return table_.size();
}

int FileBuildersModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const {

	if (parent.isValid())
		return 0;
	return 4;
}

QVariant FileBuildersModel::data( const QModelIndex& index, 
								 int role /*= Qt::DisplayRole */ ) const {

	if (!index.isValid())
		return QVariant();

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (Qt::DisplayRole == role) {
		
		switch (index.column()) {
			case 0:
				return table_.at(index.row())->getFileType();
			case 1:
				return table_.at(index.row())->getCommand();
			case 2:
				return table_.at(index.row())->getFlags();
			case 3:
				return table_.at(index.row())->getReplaceDefaultFlags();
			default:
				return QVariant();
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant FileBuildersModel::headerData( int section, Qt::Orientation orientation, 
									   int role /*= Qt::DisplayRole */ ) const {

	if (orientation != Qt::Horizontal)
		return QVariant();

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

	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case 0: {
				table_.at(index.row())->setFileType(value.toString());
				break;
					}
			case 1: {
				table_.at(index.row())->setCommand(value.toString());
				break;
					}
			case 2: {
				table_.at(index.row())->setFlags(value.toString());
				break;
					}
			case 3: {
				table_.at(index.row())->setReplaceDefaultFlags(value.toBool());
				break;
					}
			default:
				return false;
		}

		emit contentChanged();
		return true;
	}
	// if unsupported role
	else
		return false;
}

Qt::ItemFlags FileBuildersModel::flags( const QModelIndex& index ) const {

	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool FileBuildersModel::isValid() const {
	
	// check all file builders
	foreach (QSharedPointer<FileBuilder> fileBuilder, table_) {

		// if one is invalid
		if (!fileBuilder->isValid())
			return false;
	}
	return true;
}

void FileBuildersModel::apply() {
	// remove previous file builders
	fileBuilders_->clear();

	*fileBuilders_ = table_;
}

void FileBuildersModel::restore() {
	beginResetModel();
	table_.clear();
	table_ = *fileBuilders_;
	endResetModel();
}

void FileBuildersModel::onRemoveRow( int row ) {

	// if row is invalid
	if (row < 0 || row >= table_.size())
		return;

	beginRemoveRows(QModelIndex(), row, row);

	// remove the object from the map
	table_.removeAt(row);

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void FileBuildersModel::onAddRow() {

	beginInsertRows(QModelIndex(), table_.size(), table_.size());

	table_.append(QSharedPointer<FileBuilder>(new FileBuilder()));

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
