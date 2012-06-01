/* 
 *  	Created on: 1.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesmodel.cpp
 *		Project: Kactus 2
 */

#include "filesmodel.h"

#include <QStringList>
#include <QColor>
#include <QFileInfo>

FilesModel::FilesModel( QList<QSharedPointer<File> >& files, 
					   QObject *parent ):
QAbstractTableModel(parent),
files_(files) {

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
	else if (Qt::UserRole == role && index.column() == 2) {
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
	return false;
}

void FilesModel::onAddItem( const QModelIndex& index ) {

}

void FilesModel::onRemoveItem( const QModelIndex& index ) {

}
