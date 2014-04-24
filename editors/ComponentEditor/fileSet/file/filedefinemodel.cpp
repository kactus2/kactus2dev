/* 
 *
 *  Created on: 15.2.2011
 *      Author: Antti Kamppi
 * 		filename: filedefinemodel.cpp
 */

#include "filedefinemodel.h"

#include <IPXACTmodels/file.h>

FileDefineModel::FileDefineModel(QObject *parent, 
								 QSharedPointer<File> file): 
QAbstractTableModel(parent), 
file_(file) {

	// get the defines from the file
	restore();
}

FileDefineModel::~FileDefineModel() {
}

int FileDefineModel::rowCount(const QModelIndex& parent) const {
	if (parent.isValid()) {
		return 0;
	}
	return defines_.size();
}

int FileDefineModel::columnCount(const QModelIndex& parent) const {
	if (parent.isValid()) {
		return 0;
	}
	return 4;
}

QVariant FileDefineModel::data(const QModelIndex& index, int role) const {

	if (!index.isValid()) {
		return QVariant();
	}
	// make sure column and row are legal
	else if (index.row() < 0 || index.row() >= defines_.size() ||
		index.column() < 0 || index.column() > 3) {
			return QVariant();
	}
	else if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case 0: 
				return defines_.value(index.row()).name_;
			case 1:
				return defines_.value(index.row()).value_;
			case 2:
				return defines_.value(index.row()).displayName_;
			case 3:
				return defines_.value(index.row()).description_;
			default:
				return QVariant();
		}
	}
	// unsupported role
	else {
		return QVariant();
	}
}

QVariant FileDefineModel::headerData(int section, Qt::Orientation orientation, 
int role) const {

	if (role == Qt::DisplayRole) {
		
		// horizontal headers
		if (orientation == Qt::Horizontal) {
			switch (section) {
				case 0:
					return tr("Name");
				case 1:
					return tr("Value");
				case 2:
					return tr("Display name");
				case 3:
					return tr("Description");
				default:
					return QVariant();
			}
		}
		// vertical headers
		else if (orientation == Qt::Vertical) {
			return section;
		}
		else
			return QVariant();
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

bool FileDefineModel::setData(const QModelIndex& index, const QVariant& value, 
int role) {

	if (!index.isValid()) {
		return false;
	}

	if (role == Qt::EditRole) {
		switch (index.column()) {
			case 0: {
				defines_.value(index.row()).name_ = value.toString();
				break;
					}
			case 1: {
				defines_.value(index.row()).value_ = value.toString();
				break;
					}
			case 2: {
				defines_.value(index.row()).displayName_ = value.toString();
				break;
					}
			case 3: {
				defines_.value(index.row()).description_ = value.toString();
				break;
					}
			default:
				return false;
		}
		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	// if unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags FileDefineModel::flags(const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool FileDefineModel::insertRows(int row, int count, 
const QModelIndex& parent) {

	// row has to be valid
	if (row < 0 || row > defines_.size()) {
		return false;
	}
	// this is not hierarchical model
	else if (parent.isValid()) {
		return false;
	}

	beginInsertRows(parent, row, row + count -1);
	// insert the right amount of empty defines
	for (int i = 0; i < count; ++i) {
		defines_.insert(row, FileDefineModel::Define());
	}
	endInsertRows();
	emit contentChanged();
	return true;
}

bool FileDefineModel::removeRows(int row, int count, 
const QModelIndex& parent ) {
	
	// row has to be valid and there must be enough rows to remove
	if (row < 0 || row >= defines_.size() || (row + count >= defines_.size())) {
		return false;
	}
	// this is not hierarchical model
	else if (parent.isValid()) {
		return false;
	}

	beginRemoveRows(parent, row, row + count -1);
	// remove the right amount of rows
	for (int i = 0; i < count; ++i) {
		defines_.removeAt(row);
	}
	endInsertRows();
	emit contentChanged();
	return true;
}

bool FileDefineModel::isValid() const {
	// check all define items
	for (int i = 0; i < defines_.size(); ++i) {
		
		// if one item has mandatory element missing
		if (defines_.value(i).name_.isEmpty() || 
			defines_.value(i).value_.isEmpty()) {
				return false;
		}
	}
	return true;
}

void FileDefineModel::apply() {

}

void FileDefineModel::restore() {
	QList<File::Define> defines = file_->getDefines();
	for (int i = 0; i < defines.size(); ++i) {

		// append the defines to the model
		defines_.append(FileDefineModel::Define(
			defines.value(i).nameGroup_.name_,
			defines.value(i).nameGroup_.displayName_,
			defines.value(i).nameGroup_.description_,
			defines.value(i).value_));
	}
}

FileDefineModel::Define::Define(const QString name, const QString value ):
name_(name), displayName_(), description_(), value_(value) {
}

FileDefineModel::Define::Define(const QString name, const QString displayName,
								const QString description, const QString value):
name_(name), displayName_(displayName), description_(description), 
value_(value) {
}

FileDefineModel::Define::Define(): name_(), displayName_(), description_(),
value_() {
}
