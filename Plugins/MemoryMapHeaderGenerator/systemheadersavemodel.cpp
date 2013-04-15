/* 
 *	Created on:	11.4.2013
 *	Author:		Antti Kamppi
 *	File name:	systemheadersavemodel.cpp
 *	Project:		Kactus 2
*/

#include "systemheadersavemodel.h"

#include <LibraryManager/libraryinterface.h>

#include <QDir>
#include <QFileInfo>

SystemHeaderSaveModel::SystemHeaderSaveModel( LibraryInterface* handler, QObject *parent ):
QAbstractTableModel(parent),
handler_(handler),
table_(), 
topComp_() {

	Q_ASSERT(handler_);
}

SystemHeaderSaveModel::~SystemHeaderSaveModel() {
}

void SystemHeaderSaveModel::setObjects(QSharedPointer<Component> topComponent,
	const QList<SystemHeaderSaveModel::SysHeaderOptions>& options ) {
	
	topComp_.clear();
	topComp_ = topComponent;
	Q_ASSERT(topComp_);

	beginResetModel();

	table_ = options;

	// set default paths for system headers
	for (int i = 0; i < table_.size(); ++i) {

		SystemHeaderSaveModel::SysHeaderOptions& opt = table_[i];

		// path to the directory containing the xml metadata for the system component
		QString compPath = handler_->getDirectoryPath(*topComp_->getVlnv());

		// the relative path for the header
		QString headerPath = QString("%1/systemHeaders.h").arg(opt.instanceName_);

		// the absolute path for the header file
		QString fullPath = QString("%1/%2").arg(compPath).arg(headerPath);

		opt.sysHeaderInfo_ = QFileInfo(fullPath);
	}

	endResetModel();
}

int SystemHeaderSaveModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return table_.size();
}

int SystemHeaderSaveModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return SystemHeaderSaveModel::COLUMN_COUNT;
}

Qt::ItemFlags SystemHeaderSaveModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant SystemHeaderSaveModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
		case SystemHeaderSaveModel::INSTANCE_COLUMN: {
			return tr("Instance\nname");
																 }
		case SystemHeaderSaveModel::FILENAME_COLUMN: {
			return tr("File name");
																  }
		case SystemHeaderSaveModel::FILEPATH_COLUMN: {
			return tr("File Path");
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

QVariant SystemHeaderSaveModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
		case SystemHeaderSaveModel::INSTANCE_COLUMN: {
			return table_.at(index.row()).instanceName_;
																 }
		case SystemHeaderSaveModel::FILENAME_COLUMN: {
			return table_.at(index.row()).sysHeaderInfo_.fileName();
															}
		case SystemHeaderSaveModel::FILEPATH_COLUMN: {

			// display the relative path from xml directory to the header to be generated
			QDir xmlDir(handler_->getDirectoryPath(*topComp_->getVlnv()));
			QString headerPath = table_.at(index.row()).sysHeaderInfo_.absoluteFilePath();
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
		if (table_.at(index.row()).sysHeaderInfo_.exists()) {

			// return path to the directory for headers
			return table_.at(index.row()).sysHeaderInfo_.absoluteFilePath();
		}

		// if not then at least the xml directory exists
		else {
			return handler_->getDirectoryPath(*topComp_->getVlnv());
		}		
	}
	else {
		return QVariant();
	}
}

bool SystemHeaderSaveModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
		case SystemHeaderSaveModel::INSTANCE_COLUMN: {
			// instance name can not be modified
			return false;
																 }
		case SystemHeaderSaveModel::FILENAME_COLUMN: {
			table_[index.row()].sysHeaderInfo_.setFile(value.toString());

			// both indexes change
			QModelIndex pathIndex = QAbstractTableModel::index(index.row(), SystemHeaderSaveModel::FILEPATH_COLUMN, QModelIndex());
			emit dataChanged(index, pathIndex);
			return true;
															}
		case SystemHeaderSaveModel::FILEPATH_COLUMN: {
			table_[index.row()].sysHeaderInfo_.setFile(value.toString());

			// both columns change
			QModelIndex nameIndex = QAbstractTableModel::index(index.row(), SystemHeaderSaveModel::FILENAME_COLUMN, QModelIndex());
			emit dataChanged(nameIndex, index);
			return true;
															}
		default: {
			return false;
					}
		}
	}
	else if (Qt::UserRole == role) {
		table_[index.row()].sysHeaderInfo_.setFile(value.toString());

		// the change affects to both columns
		QModelIndex leftIndex = QAbstractTableModel::index(index.row(), SystemHeaderSaveModel::FILENAME_COLUMN, QModelIndex());
		QModelIndex rightIndex = QAbstractTableModel::index(index.row(), SystemHeaderSaveModel::FILEPATH_COLUMN, QModelIndex());

		// inform of changes
		emit dataChanged(leftIndex, rightIndex);
		return true;
	}
	else {
		return false;
	}
}

const QList<SystemHeaderSaveModel::SysHeaderOptions>& SystemHeaderSaveModel::getObjects() const {
	return table_;
}

SystemHeaderSaveModel::SysHeaderOptions::SysHeaderOptions( const QString& instanceName /*= QString()*/,
	const VLNV& compVLNV /*= VLNV()*/ ):
instanceName_(instanceName),
	compVLNV_(compVLNV),
	instanceId_(),
	containingComp_(),
	containingView_(),
	sysHeaderInfo_(),
	includeFiles_() {
}

SystemHeaderSaveModel::SysHeaderOptions::SysHeaderOptions( const SysHeaderOptions& other ):
instanceName_(other.instanceName_),
	compVLNV_(other.compVLNV_),
	instanceId_(other.instanceId_),
	containingComp_(other.containingComp_),
	containingView_(other.containingView_),
	sysHeaderInfo_(other.sysHeaderInfo_),
	includeFiles_(other.includeFiles_) {
}

SystemHeaderSaveModel::SysHeaderOptions& SystemHeaderSaveModel::SysHeaderOptions::operator=( const SysHeaderOptions& other ) {
	if (this != &other) {
		instanceName_ = other.instanceName_;
		compVLNV_ = other.compVLNV_;
		instanceId_ = other.instanceId_;
		containingComp_ = other.containingComp_;
		containingView_ = other.containingView_;
		sysHeaderInfo_ = other.sysHeaderInfo_;
		includeFiles_ = other.includeFiles_;
	}
	return *this;
}

bool SystemHeaderSaveModel::SysHeaderOptions::operator==( const SysHeaderOptions& other ) {
	if (compVLNV_ == other.compVLNV_ &&
		instanceName_.compare(other.instanceName_, Qt::CaseInsensitive) == 0) {
			return true;
	}
	else {
		return false;
	}
}

bool SystemHeaderSaveModel::SysHeaderOptions::operator!=( const SysHeaderOptions& other ) {
	if (compVLNV_ != other.compVLNV_) {
		return true;
	}
	return instanceName_.compare(other.instanceName_, Qt::CaseInsensitive) != 0;
}

bool SystemHeaderSaveModel::SysHeaderOptions::operator<( const SysHeaderOptions& other ) {
	if (compVLNV_ == other.compVLNV_) {
		return instanceName_.compare(other.instanceName_, Qt::CaseInsensitive) < 0;
	}
	else {
		return compVLNV_ < other.compVLNV_;
	}
}
