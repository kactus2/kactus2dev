/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarydata.h"
#include "libraryhandler.h"
#include "ipxactwidget.h"
#include "ipxactmodel.h"
#include "librarytreemodel.h"

#include "exceptions/invalid_file.h"
#include "exceptions/parse_error.h"

#include <models/librarycomponent.h>
#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>
#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/generatorchain.h>
#include <models/generaldeclarations.h>

#include "vlnv.h"

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <QFileInfo>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QUrl>
#include <QDomDocument>
#include <QString>
#include <QObject>
#include <QBitArray>
#include <stdexcept>
#include <QWidget>
#include <QFileDialog>
#include <QColor>
#include <QSettings>
#include <QList>
#include <QProgressBar>
#include <QMap>
#include <QSharedPointer>
#include <QMutableMapIterator>

#include <QDebug>

static const QString KACTUS_LIBRARY_DIRNAME(".kactus2");
static const QString KACTUS_LIBRARY_FILENAME(".librarySettings.ini");

LibraryData::LibraryData(LibraryHandler* parent): QAbstractItemModel(parent),
		libraryItems_(), table_(), handler_(parent) {
}

LibraryData::~LibraryData() {
	
	saveLibraryFile();

	// clear all pointers
	libraryItems_.clear();
}

QModelIndex LibraryData::index(int row, int column,
		const QModelIndex& parent) const {

	// if invalid row
	if (row >= table_.size() || row < 0) {
		return QModelIndex();
	}
	// if invalid column
	else if (column < 0 || column > 4) {
		return QModelIndex();
	}
	// this is not hierarchical model so parent can not be valid
	else if (parent.isValid()) {
		return QModelIndex();
	}

	return createIndex(row, column, table_.value(row));
}

QModelIndex LibraryData::parent(const QModelIndex&) const {

	return QModelIndex();
}

int LibraryData::rowCount(const QModelIndex& parent) const {

	if (parent.isValid()) {
		return 0;
	}

	return table_.size();
}

int LibraryData::columnCount(const QModelIndex& parent) const {

	if (parent.isValid()) {
		return 0;
	}
	return 5;
}

QVariant LibraryData::data(const QModelIndex& index,
		int role) const {

	// if index is not valid
	if (!index.isValid()) {
		return QVariant();
	}
	// if row is invalid
	else if (index.row() >= table_.size() || index.row() < 0) {
		return QVariant();
	}
	// if column is invalid
	else if (index.column() > 4 || index.column() < 0) {
		return QVariant();
	}

	// if role is displayRole
	else if (role == Qt::DisplayRole) {

		// return the right value from vlnv identified by column
		return table_.at(index.row())->getElement(index.column());
	}

	// if role is tooltipRole
	else if (role == Qt::ToolTipRole) {
		switch (index.column()) {

		// VLNV type
		case 0: {
			return tr("The type of the IP-Xact document");
		}
		// VLNV vendor
		case 1: {

			return tr("The value of the vendor element in VLNV");
		}
		// VLNV Library
		case 2: {
			return tr("The value of the library element in VLNV");
		}
		// VLNV Name
		case 3: {
			return tr("The value of the name element in VLNV");
		}
		// VLNV Version
		default: {
			return tr("The value of the version element in VLNV");
		}
		}
	}

	// if role is foreground
	else if (role == Qt::ForegroundRole) {
		QColor textColor;

		if (table_.value(index.row())->documentIsValid()) {
			textColor = QColor("black");
		}

		// if the document is not valid then it is marked with red color
		else {
			textColor = QColor("red");
		}

		return QBrush(textColor);
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

Qt::ItemFlags LibraryData::flags(const QModelIndex& index) const {

	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant LibraryData::headerData(int section, Qt::Orientation orientation,
		int role) const {

	// if section is invalid
	if (section < 0 || section > 4) {
		return QVariant();
	}
	// only horizontal orientation is supported
	else if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	// if role is display role
	else if (role == Qt::DisplayRole) {

		switch (section) {

		// VLNV type
		case 0: {
			return tr("Type");
		}
		// VLNV vendor
		case 1: {
			return tr("Vendor");
		}
		// VLNV Library
		case 2: {
			return tr("Library");
		}
		// VLNV Name
		case 3: {
			return tr("Name");
		}
		// VLNV Version
		default: {
			return tr("Version");
		}
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

QList<VLNV*> LibraryData::getItems() const {
	return table_;
}

void LibraryData::onExportItem(const QModelIndex& index) {
	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());

	if (!vlnv->isValid())
		return;

	handler_->onExportItem(*vlnv);
}

void LibraryData::getDirectory(QStringList& list) {
	// empty the list
	list.clear();

	// create the dialog
	QFileDialog fileChooser(handler_, Qt::Dialog);

	// select directories
	fileChooser.setFileMode(QFileDialog::Directory);

	// only display directories
	fileChooser.setOptions(QFileDialog::ShowDirsOnly);

	// view as list
	fileChooser.setViewMode(QFileDialog::List);

	// the default directory is the users home directory
	fileChooser.setDirectory(QDir::toNativeSeparators(QDir::homePath()));

	// execute the dialog and if user clicks "ok" then add the selected files
	// to the list
	if (fileChooser.exec()) {
		list = fileChooser.selectedFiles();
	}
	return;
}

// scan the folder recursively for IP-Xact documents
void LibraryData::scanFolder(const QString &path/*, QXmlSchema& schema*/) {

	// set current working directory to path
	QDir dirHandler(path);

	// get list of files and folders
	QStringList dirList(dirHandler.entryList(QDir::NoDotAndDotDot |
			QDir::AllDirs | QDir::Files));

	// go through every file and folder
	for (int i = 0; i < dirList.size(); ++i) {
		QFileInfo file(dirHandler, dirList.at(i));

		if (!file.exists()) {
			return;
		}

		// if pointing to a file which is .xml document
		if (file.isFile() && (file.suffix() == QString("xml"))) {

			/*
			// if the file is valid IP-XACT document
			if (validateIPXactDoc(file, schema)) {
				addFileToLibrary(file.filePath());
			}
			else {
				emit errorMessage(tr("File was not valid IP-XACT"));
			}
			*/

			// make sure the file is one of the supported IP-Xact types
			if (checkDocType(file) != VLNV::INVALID) {
				addFileToLibrary(file.filePath());
			}
			
		}

		// if pointing to a directory
		else if (file.isDir()) {
			// call the function recursively to scan all subfolders
			scanFolder(file.filePath()/*, schema*/);
		}
	}
}


// Add a file with the path into library
void LibraryData::addFileToLibrary(const QString& filePath) {

	// open the file to be read
	QFile IPXactFile(filePath);

	// if the file can not be opened
	if (!IPXactFile.open(QIODevice::ReadOnly)) {

		emit errorMessage(tr("Could not open file %1").arg(filePath));
		return;
	}

	QDomDocument document;

	// read the document and parse the VLNV info
	if (document.setContent(&IPXactFile)) {
		IPXactModel IPModel(document, this);
		VLNV vlnv(IPModel.getVLNV());

		// close the file so its not left open accidentally
		IPXactFile.close();

		// if the VLNV already exists in the library
		if (libraryItems_.contains(vlnv)) {

			emit noticeMessage(tr("VLNV %1:%2:%3:%4 was already found in the library").arg(
				vlnv.getVendor()).arg(
				vlnv.getLibrary()).arg(
				vlnv.getName()).arg(
				vlnv.getVersion()));
			return;
		}

		// if the file is valid IP-Xact
		if (isValidIPXactFile(filePath)) {

			// add the component to the library if parsing was successful
			VLNV* vlnvP = const_cast<VLNV*>(&libraryItems_.insert(vlnv, filePath).key());
			table_.append(vlnvP);
		}

	}
	return;
}

// check the type of the IP-Xact document
VLNV::IPXactType LibraryData::checkDocType(QFileInfo & file) {
	QFile docFile(file.filePath());

	// if the file can be opened
	if (docFile.open(QIODevice::ReadOnly)) {
		QDomDocument doc;

		// if the document can be read to QDomDocument
		if (doc.setContent(&docFile)) {
			IPXactModel newModel(doc, this);
			docFile.close();
			return newModel.getDocType();
		}
		// content could not be read
		else {
			docFile.close();
			return VLNV::INVALID;
		}
	}
	// File could not be opened
	else {
		return VLNV::INVALID;
	}
}

// validate IP-Xact document
bool LibraryData::validateIPXactDoc(QFileInfo &file, QXmlSchema &schema) {

	// read the file to be validated into memory
	QFile testedFile(file.filePath());
	if (!testedFile.open(QIODevice::ReadOnly)) {
		return false;
	}

	// validate the document
	QXmlSchemaValidator validator(schema);
	bool result = validator.validate(&testedFile,
			QUrl::fromLocalFile(testedFile.fileName()));
	testedFile.close(); 	// close the file
	return result;
}

void LibraryData::searchForFiles( const QString& path ) {

	beginResetModel();

	/*
	QFile schemaFile("D:/user/kamppia/SVNwork/koski/trunk/tools/Kactus2/LibraryManager/schema/index.xsd");

	emit noticeMessage(schemaFile.fileName());

	if (!schemaFile.exists()) {
		emit errorMessage(tr("SchemaFile does not exist"));
		return;
	}

	schemaFile.open(QIODevice::ReadOnly);

	QXmlSchema schema;

	QUrl url("http://www.accellera.org/XMLSchema/SPIRIT/1685-2009/index.xsd");

	//emit noticeMessage(tr("URL path: %1").arg(url.path()));
	//emit noticeMessage(tr("URL to local: %1").arg(url.toLocalFile()));
	
	if (!schema.load(url))
		emit errorMessage(tr("Could not load schema"));

	if (!schema.isValid())
		emit errorMessage(tr("Not a valid schema file"));

	*/
	//else
	
	// scan the path for ip-xact files
	scanFolder(path/*, schema*/);

	endResetModel();

	// inform hierarchy model that it should be reset
	emit resetModel();

	saveLibraryFile();
}

void LibraryData::openLibraryFile( const QString filePath /*= QString()*/ ) {
	QString path;

	// set the path for the library file to use

	// if path was not specified then use the default
	if (filePath.isEmpty()) {
		path = QDir::homePath() + QString("/") + KACTUS_LIBRARY_DIRNAME + 
			QString("/") + KACTUS_LIBRARY_FILENAME;
	}
	// if specified then use it
	else 
		path = filePath;

	// if there is no library file created
	QFileInfo libraryFile(path);
	if (!libraryFile.exists()) {
		QString noticeMsg(tr("Library file was not found."));
		emit noticeMessage(noticeMsg);
	}

	beginResetModel();

	// create the QSettings instance to read the file
	QSettings settings(path, QSettings::IniFormat, this);

	// get the size of the array
	int size = settings.beginReadArray("libraryItems");

	// read all the entries in the array
	for (int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);

		// read the settings
		QString type = settings.value("type").toString();
		QString vendor = settings.value("vendor").toString();
		QString library = settings.value("library").toString();
		QString name = settings.value("name").toString();
		QString version = settings.value("version").toString();
		//bool hidden = settings.value("hidden").toBool();
		QString filePath = settings.value("path").toString();
		bool validDocument = settings.value("validDocument", true).toBool();

		// create a vlnv instance to match the read settings
		VLNV vlnv(type, vendor, library, name, version, false);
		vlnv.setDocumentValid(validDocument);

		// if the file does not exist in file system
		QFileInfo ipxactFile(filePath);
		if (!ipxactFile.exists()) {
			QString errorMsg(tr("File %1 was not found in file system.").arg(path));
			emit errorMessage(errorMsg);
		}

		// check if the vlnv already exists in the library
		else if (libraryItems_.contains(vlnv)) {
			QString errorMsg(tr("Duplicate VLNVs found in library: \n"
				"Vendor: %1\n"
				"Library: %2\n"
				"Name: %3\n"
				"Version: %4\n").arg(vendor).arg(library).arg(name).arg(version));
			emit errorMessage(errorMsg);
		}
		else if (!vlnv.isValid()) {
			QString errorMsg(tr("Invalid VLNV found in library file: \n"
				"Vendor: %1\n"
				"Library: %2\n"
				"Name: %3\n"
				"Version: %4\n").arg(vendor).arg(library).arg(name).arg(version));
			emit errorMessage(errorMsg);
		}
		// if file is not valid IP-Xact
		else if (!isValidIPXactFile(filePath))
			continue;

		else {
			VLNV* vlnvP = const_cast<VLNV*>(&libraryItems_.insert(vlnv, filePath).key());
			table_.append(vlnvP);
		}
	}
	settings.endArray();

	endResetModel();

	// inform tree model that it should be reset
	emit resetModel();
}

VLNV* LibraryData::getOriginalPointer( const VLNV& vlnv ) const {
	if (libraryItems_.contains(vlnv)) {
		QMap<VLNV, QString>::iterator i = libraryItems_.find(vlnv);
		VLNV* vlnvP = const_cast<VLNV*>(&i.key());
		return vlnvP;
	}
	else
		return 0;
}

void LibraryData::saveLibraryFile( const QString filePath /*= QString()*/ ) {
	QString path;

	// set the path for the library file to use

	// if path was not specified then use the default
	if (filePath.isEmpty()) {
		path = QDir::homePath() + QString("/") + KACTUS_LIBRARY_DIRNAME + 
			QString("/") + KACTUS_LIBRARY_FILENAME;
	}
	// if specified then use it
	else 
		path = filePath;

	// create the QSettings instance to read the file
	QSettings settings(path, QSettings::IniFormat, this);

	unsigned int j = 0;
	settings.beginWriteArray("libraryItems");
	for (QMap<VLNV, QString>::const_iterator i = libraryItems_.constBegin();
		i != libraryItems_.constEnd(); ++i) {

			settings.setArrayIndex(j);

			// if the file exists then write it to the library file
			QFileInfo fileInfo(i.value());
			if (fileInfo.exists()) {

				// write the settings
				settings.setValue("type", i.key().getTypestr());
				settings.setValue("vendor", i.key().getVendor());
				settings.setValue("library", i.key().getLibrary());
				settings.setValue("name", i.key().getName());
				settings.setValue("version", i.key().getVersion());
				//settings.setValue("hidden", i.key().isHidden());
				settings.setValue("path", i.value());
				settings.setValue("validDocument", i.key().documentIsValid());

				++j;
			}
			// if file did not exist don't write it and give an error message
			else {
				QString errorMsg(tr(
					"File %1 was not found in file system, skipping...").arg(
					i.value()));
				emit errorMessage(errorMsg);
			}
	}
	settings.endArray();
}

const QString LibraryData::getPath( const VLNV& vlnv ) {

	if (libraryItems_.contains(vlnv))
		return libraryItems_.value(vlnv);
	else {
		emit errorMessage(tr("The VLNV \n"
			"Vendor: %1\n"
			"Library: %2\n"
			"Name: %3\n"
			"Version: %4\n"
			"was not found in the library.").arg(
			vlnv.getVendor()).arg(vlnv.getLibrary()).arg(vlnv.getName()).arg(
			vlnv.getVersion()));
		return QString();
	}
}

bool LibraryData::addVLNV( const VLNV& vlnv, const QString& path ) {
	if (libraryItems_.contains(vlnv)) {
		emit errorMessage(tr("The VLNV \n"
			"Vendor: %1\n"
			"Library: %2\n"
			"Name: %3\n"
			"Version: %4\n"
			"Already existed in the library and was not added.").arg(
			vlnv.getVendor()).arg(vlnv.getLibrary()).arg(vlnv.getName()).arg(
			vlnv.getVersion()));
		return false;
	}
	QFileInfo fileInfo(path);
	if (!fileInfo.exists()) {
		emit errorMessage(tr("The file %1 was not found in file system").arg(path));
		return false;
	}
	// if file is not valid IP-Xact file or the vlnv does not match with the file
	else if (!isValidIPXactFile(path, vlnv))
		return false;

	beginResetModel();

	// add the component to the library
	VLNV* vlnvP = const_cast<VLNV*>(&libraryItems_.insert(vlnv, path).key());
	table_.append(vlnvP);

	emit resetModel();
	endResetModel();

	saveLibraryFile();

	return true;
}

void LibraryData::updateVLNV( const QString& path, const VLNV& newVLNV ) {

	// find the old vlnv tags for the file
	QList<VLNV> oldKeys = libraryItems_.keys(path);
	for (int i = 0; i < oldKeys.size(); ++i) {
		
		// search the matching item in the table data structure
		for (int j = 0; j < table_.size(); ++j) {

			// if match is found
			if (oldKeys.value(i) == *table_.value(j)) {

				// remove the item from the table also
				table_.removeAt(j);
			}
		}

		// remove all old keys that are associated with the given file
		libraryItems_.remove(oldKeys.value(i));
	}

	addVLNV(newVLNV, path);
}

bool LibraryData::contains( const VLNV& vlnv ) {
	// if vlnv is found and it is of correct type
	return libraryItems_.contains(vlnv);
}

void LibraryData::onRemoveVLNV( VLNV* vlnv ) {
	
	// if vlnv does not belong to library
	if (!libraryItems_.contains(*vlnv))
		return;

	int row = table_.indexOf(const_cast<VLNV*>(vlnv));

	beginRemoveRows(QModelIndex(), row, row);

	// remove the vlnv, no delete operation is needed because VLNVs are statically
	// created
	table_.removeAt(row);
	libraryItems_.remove(*vlnv);

	endRemoveRows();

	saveLibraryFile();
}

// void LibraryData::onShowItem(const QModelIndex& index) {
// 
// 	if (!index.isValid()) {
// 		return;
// 	}
// 
// 	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
// 
// 	onShowVLNV(vlnv);
// 
// 	// inform treeModel that it should reset the model to show new items
// 	emit showVLNV(vlnv);
// }
// 
// void LibraryData::onShowVLNV(VLNV* vlnv ) {
// 
// 	if (!vlnv)
// 		return;
// 
// 	beginResetModel();
// 	vlnv->show();
// 	endResetModel();
// 
// 	saveLibraryFile();
// }
// 
// void LibraryData::onHideItem(const QModelIndex& index) {
// 
// 	if (!index.isValid()) {
// 		return;
// 	}
// 
// 	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
// 
// 	onHideVLNV(vlnv);
// 
// 	// inform treeModel that is should reset the model to hide new items
// 	emit hideVLNV(vlnv);
// }
// 
// void LibraryData::onHideVLNV( VLNV* vlnv ) {
// 
// 	if (!vlnv)
// 		return;
// 
// 	beginResetModel();
// 	vlnv->hide();
// 	endResetModel();
// 
// 	saveLibraryFile();
// }

bool LibraryData::exists( const VLNV& vlnv ) const {
	return libraryItems_.contains(vlnv);
}

void LibraryData::checkIntegrity() {

	emit noticeMessage(tr("Checking library integrity..."));

	beginResetModel();

	int max = libraryItems_.size();
	int current = 0;
	int errors = 0;

	// clear the table so only valid items are appended
	table_.clear();

	// if table contains different amount of object than the actual library
// 	if (max != table_.size()) {
// 		emit errorMessage(tr("The library is corrupted, refreshing..."));
// 
// 		// clear all items from the table
// 		table_.clear();
// 
// 		// add each vlnv to the table
// 		for (QMap<VLNV, QString>::iterator i = libraryItems_.begin();
// 			i != libraryItems_.end(); ++i) {
// 				VLNV* vlnvP = const_cast<VLNV*>(&i.key());
// 				table_.append(vlnvP);
// 		}
// 	}

	// use mutable iterator so map can be modified 
	QMutableMapIterator<VLNV, QString> i(libraryItems_);
	while (i.hasNext()) {
		// move iterator
		i.next();

		// if the not valid IP-Xact file
		if (!isValidIPXactFile(i.value(), i.key())) {
			emit errorMessage(tr("Object %1:%2:%3:%4 within file %5 was not valid"
				" IP-Xact, removing object.").arg(
				i.key().getVendor()).arg(
				i.key().getLibrary()).arg(
				i.key().getName()).arg(
				i.key().getVersion()).arg(
				i.value()));
			i.remove();
		}
	}

	// create the progress bar that displays the progress of the check
	QProgressBar progBar;
	progBar.setRange(0, max);
	progBar.setValue(current);
	progBar.setOrientation(Qt::Horizontal);
	progBar.setFormat(tr("Processing item %v of %m (%p%)"));
	progBar.move(handler_->mapToGlobal(handler_->geometry().topRight()));
	progBar.setWindowTitle(tr("Checking integrity..."));
	progBar.show();

	// parse each document in the library and check it's references to files and
	// other documents
	for (QMap<VLNV, QString>::iterator i = libraryItems_.begin();
		i != libraryItems_.end(); ++i) {

			VLNV* vlnvP = const_cast<VLNV*>(&i.key());

			// in the start assume that document is valid and if errors are 
			// found the set document as invalid
			vlnvP->setDocumentValid(true);
	
			emit noticeMessage(tr("Processing item: \n"
				"\tVendor: %1\n"
				"\tLibrary: %2\n"
				"\tName: %3\n"
				"\tVersion: %4").arg(vlnvP->getVendor()).arg(
				vlnvP->getLibrary()).arg(vlnvP->getName()).arg(
				vlnvP->getVersion()));

			QFileInfo topFile(i.value());
			if (!topFile.exists()) {
				emit errorMessage(
					tr("The file %1 for the document was not found.").arg(
					i.value()));
				++errors;
				vlnvP->setDocumentValid(false);
				continue;
			}

			QSharedPointer<LibraryComponent> libComp = handler_->getModel(i.key());
			if (!libComp) {
				emit errorMessage(tr("Could not parse file %1").arg(i.value()));
				vlnvP->setDocumentValid(false);
				++errors;
				continue;
			}

			// check that all VLNVs needed by this model are found in the library
			QList<VLNV> vlnvList = libComp->getDependentVLNVs();
			for (int j = 0; j < vlnvList.size(); ++j) {
				
				// if the document referenced by this model is not found
				if (!libraryItems_.contains(vlnvList.value(j))) {
					emit errorMessage(
						tr("The following vlnv was not found in the library: \n"
						"\tVendor: %1\n"
						"\tLibrary: %2\n"
						"\tName: %3\n"
						"\tVersion: %4\n").arg(vlnvList.value(j).getVendor()).arg(
						vlnvList.value(j).getLibrary()).arg(
						vlnvList.value(j).getName()).arg(
						vlnvList.value(j).getVersion()));
					vlnvP->setDocumentValid(false);
					++errors;
				}
			}

			// check all files referenced by this model
			QStringList filelist = libComp->getDependentFiles();
			for (int j = 0; j <filelist.size(); ++j) {

				// make sure that each file referenced by the model exists
				// in the file system
				QString path = General::getAbsolutePath(i.value(), filelist.value(j));
				
				// if the path did not exist
				if (path.isEmpty()) {

					// print the relative path because absolute path does not exist
					path = filelist.value(j);
					
					emit errorMessage(
						tr("File %1 was not found in the file system.").arg(
						path));
					vlnvP->setDocumentValid(false);
					++errors;
				}
			}

			// add the item to the table
			table_.append(vlnvP);

			// update the progress bar
			++current;
			progBar.setValue(current);
	}
	
	// the progress bar can now be hidden
	progBar.hide();

	// integrity check done, save the library file
	saveLibraryFile();

	// inform tree model that it needs to reset model also
	emit resetModel();
	endResetModel();

	emit noticeMessage(
		tr("Library integrity check complete, errors: %1").arg(errors));
}

VLNV::IPXactType LibraryData::getType( const VLNV& vlnv ) const {

	if (!libraryItems_.contains(vlnv))
		return VLNV::INVALID;
	else {
		QMap<VLNV, QString>::const_iterator i = libraryItems_.find(vlnv);
		return i.key().getType();
	}
}

void LibraryData::updatePath( const VLNV& vlnv, const QString& path ) {

	// if the object does not yet exist in the library
	if (!libraryItems_.contains(vlnv))
		return;
	else {
		libraryItems_.insert(vlnv, path);
	}

}

void LibraryData::onOpenComponent(const QModelIndex& index) {
	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());

	if (!vlnv->isValid())
		return;

	if (vlnv->getType() == VLNV::COMPONENT)
		emit editItem(*vlnv);
}

void LibraryData::onDeleteItem(const QModelIndex& index) {
	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());

	// inform tree model that vlnv is to be removed
	emit removeVLNV(vlnv);

	onRemoveVLNV(vlnv);
}

void LibraryData::onOpenDesign( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit openDesign(*vlnv);
}

void LibraryData::onCreateNewComponent( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit createComponent(*vlnv);
}

void LibraryData::onCreateNewDesign( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit createDesign(*vlnv);
}

void LibraryData::onOpenBusDef( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit editItem(*vlnv);
}

void LibraryData::onCreateBusDef( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit createBusDef(*vlnv);
}

bool LibraryData::isValidIPXactFile( const QString& filePath, 
									const VLNV& vlnvToCompare /*= VLNV()*/ ) {

	QFileInfo fileInfo(filePath);
	
	// if file does not exist
	if (!fileInfo.exists())
		return false;

	// if file is not xml
	else if (fileInfo.suffix() != QString("xml"))
		return false;

	// open the file to be read
	QFile IPXactFile(filePath);

	// if the file can not be opened
	if (!IPXactFile.open(QIODevice::ReadOnly)) {

		emit errorMessage(tr("Could not open file %1").arg(filePath));
		return false;
	}

	QDomDocument document;

	// read the document and parse the VLNV info
	if (document.setContent(&IPXactFile)) {
		IPXactModel IPModel(document, this);
		VLNV vlnv(IPModel.getVLNV());

		// if vlnv to compare is given but the vlnvs don't match
		if (vlnvToCompare.isValid() && vlnvToCompare != vlnv) {
			return false;
		}

		// close the file so its not left open accidentally
		IPXactFile.close();

		// make sure the component can be parsed before adding it to the library
		QSharedPointer<LibraryComponent> libComp;
		try {
			// create correct type of object and cast the pointer
			switch (vlnv.getType()) {
			case VLNV::BUSDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new BusDefinition(document));
				break;
									  }
			case VLNV::COMPONENT: {
				libComp = QSharedPointer<LibraryComponent>(new Component(document));
				break;
								  }
			case VLNV::DESIGN: {
				libComp = QSharedPointer<LibraryComponent>(new Design(document));
				break;
							   }

			case VLNV::GENERATORCHAIN: {
				libComp = QSharedPointer<LibraryComponent>(new GeneratorChain(document));
				break;
									   }
			case VLNV::DESIGNCONFIGURATION: {
				libComp = QSharedPointer<LibraryComponent>(new DesignConfiguration(document));
				break;
											}

			case VLNV::ABSTRACTIONDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new AbstractionDefinition(document));
				break;
											  }
			default: {
				emit noticeMessage(tr("Document was not supported type"));
				return false;
					 }
			}
		}
		// if an exception occurred during the parsing
		catch (Parse_error error) {
			QString errorMsg(error.what() + QString(" ") + error.errorMsg() +
				tr(" within file: %1").arg(filePath));
			emit errorMessage(errorMsg);
			return false;
		}
		catch (...) {
			emit errorMessage(
				tr("Error occurred during parsing of the document %1").arg(filePath));
			return false;
		}
	}
	
	// everything went ok
	IPXactFile.close();
	return true;
}
