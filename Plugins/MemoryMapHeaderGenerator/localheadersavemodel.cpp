/* 
 *	Created on: 20.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	localheadersavemodel.cpp
 * 	Project:	Kactus 2
*/

#include "localheadersavemodel.h"
#include <library/LibraryManager/libraryinterface.h>
#include <models/addressspace.h>
#include <models/memorymap.h>

#include <QDir>
#include <QStringList>

const QString LocalHeaderSaveModel::DEFAULT_HEADER_DIR = tr("headers");

LocalHeaderSaveModel::LocalHeaderSaveModel(LibraryInterface* handler, 
	QObject *parent ):
QAbstractTableModel(parent),
component_(),
handler_(handler),
table_() {

}

LocalHeaderSaveModel::~LocalHeaderSaveModel() {
	qDeleteAll(table_);
	table_.clear();
}

void LocalHeaderSaveModel::setComponent( QSharedPointer<Component> component ) {
	beginResetModel();
	component_ = component;

	qDeleteAll(table_);
	table_.clear();

	QString defSWView;
	QStringList swViewNames = component->getSWViewNames();
	
	// if there are no sw views then use default name to create one
	if (swViewNames.isEmpty()) {
		defSWView = "swView";
	}
	// if there is at least one sw view then use it.
	else {
		defSWView = swViewNames.first();
	}

	foreach (QSharedPointer<AddressSpace> addrSpace, component_->getAddressSpaces()) {

		QSharedPointer<MemoryMap> localMemMap = addrSpace->getLocalMemoryMap();

		// if the address space contains a local memory map and contains at least one item
		if (localMemMap && localMemMap->containsSubItems()) {

			LocalHeaderSaveModel::SaveFileOptions* options = new SaveFileOptions();
			options->localMemMap_ = localMemMap;

			// the path to the directory containing the xml metadata
			QString compPath(handler_->getDirectoryPath(*component_->getVlnv()));

			// the relative path from the xml dir to the header to generate
			QString headerPath = QString("%1/%2.h").arg(LocalHeaderSaveModel::DEFAULT_HEADER_DIR).arg(
				localMemMap->getName());

			// the absolute path to the header file
			const QString fullPath = QString("%1/%2").arg(compPath).arg(headerPath);

			// create the file info instance
			options->fileInfo_ = QFileInfo(fullPath);

			options->swView_ = defSWView;

			// append the options to the table
			table_.append(options);
		}
	}

	endResetModel();
}

int LocalHeaderSaveModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}

	return table_.size();
}

int LocalHeaderSaveModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}

	return LocalHeaderSaveModel::COLUMN_COUNT;
}

Qt::ItemFlags LocalHeaderSaveModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	if (index.column() == LocalHeaderSaveModel::SW_VIEW_NAME) {
		flags |= Qt::ItemIsEditable;
	}

	return flags;
}

QVariant LocalHeaderSaveModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
		case LocalHeaderSaveModel::SW_VIEW_NAME: {
			return tr("SW View");
															  }
		case LocalHeaderSaveModel::MEM_MAP_NAME: {
			return tr("Local memory\nmap name");
												 }
		case LocalHeaderSaveModel::FILE_NAME: {
			return tr("File name");
													  }
		case LocalHeaderSaveModel::FILE_PATH: {
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

QVariant LocalHeaderSaveModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
		case LocalHeaderSaveModel::SW_VIEW_NAME: {
			return table_.at(index.row())->swView_;
															  }
		case LocalHeaderSaveModel::MEM_MAP_NAME: {
			return table_.at(index.row())->localMemMap_->getName();
												 }
		case LocalHeaderSaveModel::FILE_NAME: {
			return table_.at(index.row())->fileInfo_.fileName();
													  }
		case LocalHeaderSaveModel::FILE_PATH: {

			// display the relative path from xml directory to the header to be generated
			QDir xmlDir(handler_->getDirectoryPath(*component_->getVlnv()));
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
			return handler_->getDirectoryPath(*component_->getVlnv());
		}		
	}
	else {
		return QVariant();
	}
}

bool LocalHeaderSaveModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
		case LocalHeaderSaveModel::SW_VIEW_NAME: {
			table_[index.row()]->swView_ = value.toString();
			break;
															  }
		case LocalHeaderSaveModel::MEM_MAP_NAME: {
			// memory map name can not be modified
			return false;
												 }
		case LocalHeaderSaveModel::FILE_NAME: {
			table_[index.row()]->fileInfo_.setFile(value.toString());

			// both indexes change
			QModelIndex pathIndex = QAbstractTableModel::index(index.row(), LocalHeaderSaveModel::FILE_PATH, QModelIndex());
			emit dataChanged(index, pathIndex);
			return true;
													  }
		case LocalHeaderSaveModel::FILE_PATH: {
			table_[index.row()]->fileInfo_.setFile(value.toString());

			// both columns change
			QModelIndex nameIndex = QAbstractTableModel::index(index.row(), LocalHeaderSaveModel::FILE_NAME, QModelIndex());
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
		QModelIndex leftIndex = QAbstractTableModel::index(index.row(), LocalHeaderSaveModel::FILE_NAME, QModelIndex());
		QModelIndex rightIndex = QAbstractTableModel::index(index.row(), LocalHeaderSaveModel::FILE_PATH, QModelIndex());

		// inform of changes
		emit dataChanged(leftIndex, rightIndex);
		return true;
	}
	else {
		return false;
	}
}

const QList<LocalHeaderSaveModel::SaveFileOptions*>& LocalHeaderSaveModel::getHeaderOptions() const {
	return table_;
}
