/* 
 *	Created on: 25.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	globalheadersavemodel.cpp
 * 	Project:	Kactus 2
*/

#include "globalheadersavemodel.h"

#include <LibraryManager/libraryinterface.h>

#include <QDir>

const QString GlobalHeaderSaveModel::DEFAULT_HEADER_DIR = tr("cSources");

GlobalHeaderSaveModel::GlobalHeaderSaveModel( LibraryInterface* handler,
	QObject *parent ):
QAbstractTableModel(parent),
handler_(handler),
table_(),
comp_(),
design_() {

}

GlobalHeaderSaveModel::~GlobalHeaderSaveModel() {
}

int GlobalHeaderSaveModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}

	return table_.size();
}

int GlobalHeaderSaveModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}

	return GlobalHeaderSaveModel::COLUMN_COUNT;
}

Qt::ItemFlags GlobalHeaderSaveModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant GlobalHeaderSaveModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
		case GlobalHeaderSaveModel::INSTANCE_NAME: {
			return tr("Instance\nname");
												 }
		case GlobalHeaderSaveModel::INTERFACE_NAME: {
			return tr("Interface\nname");
											  }
		case GlobalHeaderSaveModel::FILE_NAME: {
			return tr("File name");
											  }
		case GlobalHeaderSaveModel::FILE_PATH: {
			return tr("File path");
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

QVariant GlobalHeaderSaveModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
		case GlobalHeaderSaveModel::INSTANCE_NAME: {
			return table_.at(index.row())->instance_;
												 }
		case GlobalHeaderSaveModel::INTERFACE_NAME: {
			return table_.at(index.row())->interface_;
											  }
		case GlobalHeaderSaveModel::FILE_NAME: {
			return table_.at(index.row())->fileInfo_.fileName();
											   }
		case GlobalHeaderSaveModel::FILE_PATH: {

			VLNV identifier = table_.at(index.row())->comp_;

			// display the relative path from xml directory to the header to be generated
			QDir xmlDir(handler_->getDirectoryPath(*comp_->getVlnv()));
			QString headerPath = table_.at(index.row())->fileInfo_.absoluteFilePath();
			QString relPath = xmlDir.relativeFilePath(headerPath);

			return relPath;
											  }
		default: {
			return QVariant();
				 }
		}
	}
	// user role always returns the absolute file path
	else if (Qt::UserRole == role) {

		// if the header dir exists
		QFileInfo headerInfo(table_.at(index.row())->fileInfo_.absolutePath());
		if (headerInfo.exists()) {

			// return path to the directory for headers
			return headerInfo.absoluteFilePath();
		}

		// if not then at least the xml directory exists
		else {
			return handler_->getDirectoryPath(*comp_->getVlnv());
		}		
	}
	else {
		return QVariant();
	}
}

bool GlobalHeaderSaveModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
		case GlobalHeaderSaveModel::INSTANCE_NAME: {
			// instance name can not be modified
			return false;
												 }
		case GlobalHeaderSaveModel::INTERFACE_NAME: {
			// interface name can not be modified
			return false;
												   }
		case GlobalHeaderSaveModel::FILE_NAME: {
			table_[index.row()]->fileInfo_.setFile(value.toString());

			// both indexes change
			QModelIndex pathIndex = QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_PATH, QModelIndex());
			emit dataChanged(index, pathIndex);
			return true;
											  }
		case GlobalHeaderSaveModel::FILE_PATH: {
			table_[index.row()]->fileInfo_.setFile(value.toString());

			// both columns change
			QModelIndex nameIndex = QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_NAME, QModelIndex());
			emit dataChanged(nameIndex, index);
			return true;
											  }
		default: {
			return false;
				 }
		}

		emit dataChanged(index, index);
		return true;
	}
	else if (Qt::UserRole == role) {
		table_[index.row()]->fileInfo_.setFile(value.toString());

		// the change affects to both columns
		QModelIndex leftIndex = QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_NAME, QModelIndex());
		QModelIndex rightIndex = QAbstractTableModel::index(index.row(), GlobalHeaderSaveModel::FILE_PATH, QModelIndex());

		// inform of changes
		emit dataChanged(leftIndex, rightIndex);
		return true;
	}
	else {
		return false;
	}
}

const QList<GlobalHeaderSaveModel::SaveFileOptions*>& GlobalHeaderSaveModel::getHeaderOptions() const {
	return table_;
}
