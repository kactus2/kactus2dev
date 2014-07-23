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
#include <IPXACTmodels/vlnv.h>

#include <common/utils.h>
#include <common/widgets/ScanProgressWidget/scanprogresswidget.h>

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/busdefinition.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/generatorchain.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/ComDefinition.h>
#include <IPXACTmodels/ApiDefinition.h>

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <QFileInfo>
#include <QDomDocument>
#include <QString>
#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include <QSettings>
#include <QList>
#include <QProgressBar>
#include <QMap>
#include <QSharedPointer>
#include <QTimer>
#include <QUrl>

LibraryData::LibraryData(LibraryHandler* parent, QWidget* parentWidget)
    : QObject(parent),
      parentWidget_(parentWidget),
      libraryItems_(),
      handler_(parent),
      progWidget_(0),
      timerSteps_(0),
      timerStep_(0),
      timer_(0),
      locations_(),
      iterObjects_(),
      errors_(0),
      failedObjects_(0),
      syntaxErrors_(0),
      vlnvErrors_(0),
      fileErrors_(0),
		fileCount_(0),
		urlTester_(new QRegExpValidator(Utils::URL_IDENTIFY_REG_EXP, this)),
		urlValidator_(new QRegExpValidator(Utils::URL_VALIDITY_REG_EXP, this)) {
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}

LibraryData::~LibraryData() {

	// clear all pointers
	libraryItems_.clear();
}

QList<VLNV> LibraryData::getItems() const {
    return libraryItems_.keys();
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

const QString LibraryData::getPath( const VLNV& vlnv ) {

	if (libraryItems_.contains(vlnv)) {
		return libraryItems_.value(vlnv);
	}
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

bool LibraryData::addVLNV( const VLNV& vlnv, const QString& path) {
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

	// add the component to the library
	libraryItems_.insert(vlnv, path);

	emit addVLNV(vlnv);

	return true;
}

bool LibraryData::contains( const VLNV& vlnv ) {
	// if vlnv is found and it is of correct type
	return libraryItems_.contains(vlnv);
}

void LibraryData::onRemoveVLNV( const VLNV& vlnv ) {
	
	// if vlnv does not belong to library
	if (!libraryItems_.contains(vlnv)) {
		return;
	}

	// remove the vlnv, no delete operation is needed because VLNVs are statically created
    libraryItems_.remove(vlnv);
}

VLNV::IPXactType LibraryData::getType( const VLNV& vlnv ) const {

	if (!libraryItems_.contains(vlnv)) {
		return VLNV::INVALID;
	}
	else {
		QMap<VLNV, QString>::const_iterator i = libraryItems_.find(vlnv);
		return i.key().getType();
	}
}

void LibraryData::resetLibrary() {
	emit resetModel();
}

void LibraryData::checkLibraryIntegrity() {

	int max = libraryItems_.size();
	errors_ = 0;
	failedObjects_ = 0;
	syntaxErrors_ = 0;
	vlnvErrors_ = 0;
	fileErrors_ = 0;
	fileCount_ = 0;

	emit noticeMessage(tr("------ Library Integrity Check ------"));

    timerStep_ = 0;
    timerSteps_ = max;
    iterObjects_ = libraryItems_.begin();

    // create the progress bar that displays the progress of the check
    progWidget_ = new ScanProgressWidget(parentWidget_);
    progWidget_->setWindowTitle(tr("Checking integrity..."));
    progWidget_->setRange(0, max);
    progWidget_->setMessage(tr("Processing item %1 of %2...").arg(QString::number(timerStep_ + 1),
        QString::number(libraryItems_.size())));
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(performIntegrityCheckStep()));
    timer_->start();

    progWidget_->exec();

	emit noticeMessage(tr("========== Library integrity check complete =========="));
	emit noticeMessage(tr("Total library object count: %1").arg(libraryItems_.size()));
	emit noticeMessage(tr("Total file count in the library: %1").arg(fileCount_));

	// if errors were found then print the summary of error types
	if (errors_ > 0) {
		emit noticeMessage(tr("Found %1 errors within %2 item(s):").arg(errors_).arg(failedObjects_));
		emit noticeMessage(tr("Structural errors within item(s): %1").arg(syntaxErrors_));
		emit noticeMessage(tr("Invalid VLNV references: %1").arg(vlnvErrors_));
		emit noticeMessage(tr("Invalid file references: %1\n").arg(fileErrors_));
	}
}

void LibraryData::parseLibrary()
{
	// clear the previous items in the library
	libraryItems_.clear();

	QSettings settings(this);

	// Load the library locations.
	QStringList locations = settings.value("Library/ActiveLocations", QStringList()).toStringList();

	// if there are no library locations
	if (locations.isEmpty()) 
    {
		locations_.clear();
    }
    else
    {
        // create the progress bar that displays the progress of the scan
        progWidget_ = new ScanProgressWidget(parentWidget_);
        progWidget_->setRange(0, locations.size());
        progWidget_->setMessage("Scanning location: \n" + locations.first());
        timerStep_ = 0;
        timerSteps_ = locations.size();
        locations_ = locations;

        timer_ = new QTimer(this);
        connect(timer_, SIGNAL(timeout()), this, SLOT(performParseLibraryStep()));
        timer_->start();

        progWidget_->exec();
    }

    // check the integrity of the items in the library
	checkLibraryIntegrity();
}

void LibraryData::parseDirectory( const QString& directoryPath ) {
	
	// if the path is empty
	if (directoryPath.isEmpty()) {
		return;
	}
	QFileInfo dirInfo(directoryPath);
	
	// if the directory does not exist
	if (!dirInfo.exists()) {
		return;
	}

	// it should always be for a directory
	Q_ASSERT(dirInfo.isDir());

	QDir dirHandler(directoryPath);

	// get list of files and folders
	QFileInfoList entryInfos(dirHandler.entryInfoList(QDir::NoDotAndDotDot |
		QDir::AllDirs | QDir::Files));

	foreach (QFileInfo entryInfo, entryInfos) {
		
		// if the file/directory does not exist
		if (!entryInfo.exists()) {
			continue;
		}

		// if the entry is an xml file
		if (entryInfo.isFile() && entryInfo.suffix() == QString("xml")) {
			parseFile(entryInfo.absoluteFilePath());
		}

		// if the entry is a sub-directory
		else if (entryInfo.isDir()) {
			parseDirectory(entryInfo.absoluteFilePath());
		}
	}
}

void LibraryData::parseFile( const QString& filePath ) {

	if (filePath.isEmpty()) {
		return;
	}

	QFile docFile(filePath);

	// if the file can not be opened
	if (!docFile.open(QFile::ReadOnly)) {
		emit errorMessage(tr("Could not open file %1 for reading.").arg(filePath));
		return;
	}
	
	QDomDocument doc;

	VLNV vlnv;

	// if the document can be read to QDomDocument
	if (doc.setContent(&docFile)) {
		IPXactModel newModel(doc, this);
		docFile.close();
		vlnv = newModel.getVLNV();
	}
	// content could not be read
	else {
		emit errorMessage(tr("The file %1 was not valid xml and could not be read.").arg(
			filePath));
		docFile.close();
		return;
	}

	// if the vlnv is not valid (the document was not IP-Xact)
	if (!vlnv.isValid()) {
		return;
	}
	
	// if the VLNV already exists in the library
	if (libraryItems_.contains(vlnv)) {
		emit noticeMessage(tr("VLNV %1:%2:%3:%4 was already found in the library").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));
		return;
	}

	// add the component to the library if parsing was successful
	libraryItems_.insert(vlnv, filePath);
}

QSharedPointer<LibraryComponent> LibraryData::getModel( const VLNV& vlnv ) {
	if (!libraryItems_.contains(vlnv)) {
		emit noticeMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));
		return QSharedPointer<LibraryComponent>();
	}

	VLNV toCreate = vlnv;
	// make sure the vlnv is of correct type
	toCreate.setType(libraryItems_.find(vlnv).key().getType());

	// get path to the document
	QString path = libraryItems_.value(toCreate);

	// if the file was not found
	if (path.isEmpty()) {
		return QSharedPointer<LibraryComponent>();
	}

	// create file handle and use it to read the IP-Xact document into memory
	QFile file(path);
	QDomDocument doc;
	if (!doc.setContent(&file)) {
		emit errorMessage(tr("The document %1 in file %2 could not be opened.").arg(
			toCreate.toString()).arg(path));
		return QSharedPointer<LibraryComponent>();
	}
	file.close();

	QSharedPointer<LibraryComponent> libComp;

	try {
		// create correct type of object and cast the pointer
		switch (toCreate.getType()) {
			case VLNV::BUSDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new BusDefinition(doc));
				break;
									  }
			case VLNV::COMPONENT: {
				libComp = QSharedPointer<LibraryComponent>(new Component(doc));
				break;
								  }
			case VLNV::DESIGN: {
				libComp = QSharedPointer<LibraryComponent>(new Design(doc));
				break;
							   }

			case VLNV::GENERATORCHAIN: {
				libComp = QSharedPointer<LibraryComponent>(new GeneratorChain(doc));
				break;
									   }
			case VLNV::DESIGNCONFIGURATION: {
				libComp = QSharedPointer<LibraryComponent>(new DesignConfiguration(doc));
				break;
											}

			case VLNV::ABSTRACTIONDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new AbstractionDefinition(doc));
				break;
											  }

            case VLNV::COMDEFINITION: {
                libComp = QSharedPointer<LibraryComponent>(new ComDefinition(doc));
                break;
                                      }

            case VLNV::APIDEFINITION: {
                libComp = QSharedPointer<LibraryComponent>(new ApiDefinition(doc));
                break;
                                      }
			default: {
				emit noticeMessage(tr("Document was not supported type"));
				return QSharedPointer<LibraryComponent>();
					 }
		}
	}
	// if an exception occurred during the parsing
	catch (...) {
		emit errorMessage(
			tr("Error occurred during parsing of the document %1").arg(path));
		return QSharedPointer<LibraryComponent>();
	}

	return libComp;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::performParseLibrary()
//-----------------------------------------------------------------------------
void LibraryData::performParseLibraryStep()
{
	if (timerStep_ == timerSteps_)
	{
		return;
	}

	QString location = locations_.at(timerStep_);
	QFileInfo locationInfo(location);

	// if the location is a directory
	if (locationInfo.isDir())
	{
		parseDirectory(location);
	}
	// if the location is a direct file
	else if (locationInfo.isFile())
	{
		parseFile(location);
	}

	++timerStep_;

	// Check if all steps have been completed.
	if (timerStep_ == timerSteps_)
	{
		timer_->stop();
		delete timer_;
		delete progWidget_;
	}
	else
	{
		// Update the value and message for the next round.
		progWidget_->setMessage("Scanning location: \n" + locations_.at(timerStep_));
		progWidget_->setValue(timerStep_ + 1);
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryData::performIntegrityCheckStep()
//-----------------------------------------------------------------------------
void LibraryData::performIntegrityCheckStep()
{
	if (timerStep_ > timerSteps_)
	{
		return;
	}

	if (timerStep_ < timerSteps_)
	{

		QSharedPointer<LibraryComponent> libComp = getModel(iterObjects_.key());
		// if the object could not be parsed
		if (!libComp) {

			// remove the pair from the map and move on
			QMap<VLNV, QString>::iterator i = libraryItems_.find(iterObjects_.key());
			++iterObjects_;
            libraryItems_.erase(i);
		}
        else
        {
            checkObject(libComp, iterObjects_.value());
            ++iterObjects_;
        }
	}
	else
	{
		// inform tree model that it needs to reset model also
		emit resetModel();
	}

	++timerStep_;

	// Update the message and value for the next round.
	if (timerStep_ < timerSteps_)
	{
		progWidget_->setMessage(tr("Processing item %1 of %2...").arg(QString::number(timerStep_ + 1),
			QString::number(libraryItems_.size())));
	}
	else
	{
		progWidget_->setMessage(tr("Updating library view..."));
	}

	progWidget_->setValue(timerStep_);

	// Check if all steps have been completed.
	if (timerStep_ > timerSteps_)
	{
		timer_->stop();
		delete timer_;
		timer_ = 0;

		delete progWidget_;
		progWidget_ = 0;
	}
}

bool LibraryData::checkObject( QSharedPointer<LibraryComponent> libComp, const QString& path, bool print /*= true*/ ) {

	// in the start assume that document is valid and if errors are 
	// found the set document as invalid
	bool wasValid = true;

	Q_ASSERT(libComp);

	// used to print information to user
	VLNV vlnv(*libComp->getVlnv());

	// inform the user of the object being processed

	// make sure the file exists
	QFileInfo topFile(path);
	if (!topFile.exists()) {

		// if theres no printing then there is no reason to check further errors
		if (!print) {
			return false;
		}

		emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
		emit errorMessage(tr("The file %1 for the document was not found.").arg(path));
		++errors_;
		++failedObjects_;

		wasValid = false;
	}

	// check if the component xml is valid and if not then print errors of the component
	QStringList errorList;
	if (!libComp->isValid(errorList)) {

		// if theres no printing then there is no reason to check further errors
		if (!print) {
			return false;
		}

		if (wasValid) {
			emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
			++failedObjects_;
			wasValid = false;
		}

		foreach (QString error, errorList) {
			emit errorMessage(error);
		}
		errors_ += errorList.size();
		syntaxErrors_ += errorList.size();
	}

	// check that all VLNVs needed by this model are found in the library
	QList<VLNV> vlnvList = libComp->getDependentVLNVs();
	for (int j = 0; j < vlnvList.size(); ++j) {
		// if the document referenced by this model is not found
		if (!libraryItems_.contains(vlnvList.at(j))) {

			// if theres no printing then there is no reason to check further errors
			if (!print) {
				return false;
			}

			// if this was first failed test then increase number of failed items
			if (wasValid) {
				emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
				++failedObjects_;
				wasValid = false;
			}

			// tell user that referenced object was not found
			emit errorMessage(
				tr("The referenced VLNV was not found in the library: %1").arg(vlnvList.at(j).toString()));
			++errors_;
			++vlnvErrors_;
		}
	}

	// check all dependent directories
	QStringList dirs = libComp->getDependentDirs();
	foreach (QString dir, dirs) {
		QString dirPath = General::getAbsolutePath(path, dir);
		QFileInfo dirInfo(dirPath);

		// if the directory does not exist
		if (!dirInfo.exists()) {

			// if theres no printing then there is no reason to check further errors
			if (!print) {
				return false;
			}

			// if this is the first found error
			if (wasValid) {
				emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
				++failedObjects_;
				wasValid = false;
			}

			emit errorMessage(tr("\tDirectory %1 was not found in the file system.").arg(dirPath));

			++errors_;
			++fileErrors_;
		}
	}

	// check all files referenced by this model
	QStringList filelist = libComp->getDependentFiles();
	for (int j = 0; j < filelist.size(); ++j) {

		// the file reference in the component
		QString originalPath = filelist.at(j);

		// used by the url validators
		int pos = 0;

		// check if the path is actually a URL to (external) location
		if (urlTester_->validate(originalPath, pos) == QValidator::Acceptable) {

			pos = 0;

			// if the URL is not valid
			if (urlValidator_->validate(originalPath, pos) != QValidator::Acceptable) {

				// if theres no printing then there is no reason to check further errors
				if (!print) {
					return false;
				}

				// if this is the first found error
				if (wasValid) {

					emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
					++failedObjects_;
					wasValid = false;
				}

				emit errorMessage(tr("\tURL %1 was not valid.").arg(originalPath));
				++errors_;
				++fileErrors_;
			}

			// The URL was valid
			else {
				++fileCount_;
			}
		}

		// The path was not URL so it must be file reference on the disk.
		else {

			QString filePath;

			// if the path is relative then create absolute path
			QFileInfo originalInfo(originalPath);
			if (originalInfo.isRelative()) {
				filePath = General::getAbsolutePath(path, originalPath);
			}
			// if the reference is directly absolute
			else {
				filePath = originalPath;
			}

			// make sure that each file referenced by the model exists
			// in the file system
			QFileInfo pathInfo(filePath);

			// if the path did not exist
			if (!pathInfo.exists()) {

				// if theres no printing then there is no reason to check further errors
				if (!print) {
					return false;
				}

				// if this is the first found error
				if (wasValid) {
					emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
					++failedObjects_;
					wasValid = false;
				}

				emit errorMessage(tr("\tFile %1 was not found in the file system.").arg(filePath));
				++errors_;
				++fileErrors_;
			}

			// if the file exists then increase the total file count.
			else {
				++fileCount_;
			}
		}
	}

	return wasValid;
}
