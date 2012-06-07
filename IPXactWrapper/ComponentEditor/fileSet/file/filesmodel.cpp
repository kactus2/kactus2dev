/* 
 *  	Created on: 1.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesmodel.cpp
 *		Project: Kactus 2
 */

#include "filesmodel.h"
#include <models/generaldeclarations.h>
#include <LibraryManager/libraryinterface.h>

#include <QStringList>
#include <QColor>
#include <QFileInfo>

FilesModel::FilesModel(LibraryInterface* handler,
					   QSharedPointer<Component> component,
					   QSharedPointer<FileSet> fileSet,
					   QObject* parent):
QAbstractTableModel(parent),
handler_(handler),
component_(component),
fileSet_(fileSet), 
files_(fileSet->getFiles()) {

}

FilesModel::~FilesModel() {
}

int FilesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return files_.size();
}

int FilesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return 4;
}

Qt::ItemFlags FilesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	// The first two columns are not editable
	else if (index.column() <= 1) {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant FilesModel::headerData( int section,
								Qt::Orientation orientation, 
								int role /*= Qt::DisplayRole*/ ) const {

	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case 0: {
				return tr("File name");
					}
			case 1: {
				return tr("File path");
					}
			case 2: {
				return tr("File types");
					}
			case 3: {
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

QVariant FilesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= files_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {
		switch (index.column()) {
			case 0: {
				QFileInfo fileInfo(files_.at(index.row())->getName());
				return fileInfo.fileName();
					}
			case 1: {
				return files_.at(index.row())->getName();
					}
			case 2: {
				// each group name if in it's own index
				QStringList typeNames = files_.at(index.row())->getAllFileTypes();

				// concatenate the names to a single string with spaces in between
				QString str;
				foreach (QString fileType, typeNames) {
					str += fileType;
					str += " ";
				}

				// return the string with all group names
				return str;
					}
			case 3: {
				return files_.at(index.row())->getDescription();
					}
			default: {
				return QVariant();
					 }
		}
	}
	else if (FilesModel::USER_DISPLAY_ROLE == role && index.column() == 2) {
		return files_.at(index.row())->getAllFileTypes();
	}
	else if (Qt::ForegroundRole == role) {
		if (files_.at(index.row())->isValid(true)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else {
		return QVariant();
	}
}

bool FilesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= files_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			// file name can not be set directly
			case 0: {
				Q_ASSERT(false);
				return false;
					}
			// file name is set through file path
			case 1: {
				files_[index.row()]->setName(value.toString());
				break;
					}
			case 2: {
				QString str = value.toString();
				QStringList fileTypes = str.split(' ', QString::SkipEmptyParts);
				files_[index.row()]->setAllFileTypes(fileTypes);
				break;
					}
			case 3: {
				files_[index.row()]->setDescription(value.toString());
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
	// for user edit role a QStringList is used
	else if (FilesModel::USER_EDIT_ROLE == role && index.column() == 2) {
		files_[index.row()]->setAllFileTypes(value.toStringList());
		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else {
		return false;
	}
}

void FilesModel::onAddItem( const QModelIndex& index, const QString& filePath ) {
	int row = files_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	// convert the given path into relative path
	const QString xmlPath = handler_->getPath(*component_->getVlnv());
	const QString relPath = General::getRelativePath(xmlPath, filePath);

	// if relative path could not be created.
	if (relPath.isEmpty()) {
		return;
	}

	// if the file is already contained in the list
	foreach (QSharedPointer<File> file, files_) {

		if (file->getName() == relPath) {
			return;
		}
	}

	beginInsertRows(QModelIndex(), row, row);
	files_.insert(row, QSharedPointer<File>(new File(relPath, fileSet_.data())));
	endInsertRows();

	// inform navigation tree that file set is added
	emit fileAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void FilesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= files_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	files_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit fileRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void FilesModel::onMoveItem( const QModelIndex& originalPos, const QModelIndex& newPos ) {

	// if there was no item in the starting point
	if (!originalPos.isValid()) {
		return;
	}
	// if the indexes are the same
	else if (originalPos == newPos) {
		return;
	}
	else if (originalPos.row() < 0 || originalPos.row() >= files_.size()) {
		return;
	}

	int source = originalPos.row();
	int target = 0;

	// if the new position is invalid index then put the item last in the table
	if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= files_.size()) {

		beginResetModel();
		QSharedPointer<File> file = files_.takeAt(originalPos.row());
		files_.append(file);
		target = files_.size() - 1;
		endResetModel();
	}
	// if both indexes were valid
	else {
		beginResetModel();
		files_.swap(originalPos.row(), newPos.row());
		target = newPos.row();
		endResetModel();
	}

	emit fileMoved(source, target);

	emit contentChanged();
}
