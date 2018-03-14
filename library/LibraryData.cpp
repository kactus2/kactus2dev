//-----------------------------------------------------------------------------
// File: librarydata.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryData is the data model that manages the actual VLNV library.
//-----------------------------------------------------------------------------

#include "LibraryData.h"

#include "DocumentFileAccess.h"
#include "LibraryHandler.h"

#include <common/utils.h>
#include <common/ui/MessageMediator.h>

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Catalog/Catalog.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: LibraryData::LibraryData()
//-----------------------------------------------------------------------------
LibraryData::LibraryData(LibraryInterface* library, MessageMediator* messageChannel, QObject* parent):
    QObject(parent),
    libraryItems_(),
    library_(library),
    messageChannel_(messageChannel),
    failedObjects_(0),
    fileCount_(0),
    urlTester_(new QRegularExpressionValidator(Utils::URL_VALIDITY_REG_EXP, this)),   
    fileAccess_(),
    validator_(library),
    fileWatch_(new QFileSystemWatcher(this))
{
    connect(fileWatch_, SIGNAL(fileChanged(QString const&)),
            this, SLOT(onFileChanged(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::~LibraryData()
//-----------------------------------------------------------------------------
LibraryData::~LibraryData()
{
    libraryItems_.clear();
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getPath()
//-----------------------------------------------------------------------------
QString LibraryData::getPath(VLNV const& vlnv)
{
    if (!vlnv.isValid())
    {
        return QString();
    }

    if (!libraryItems_.contains(vlnv))
    {
    	messageChannel_->showError(tr("The VLNV \n"
    		"Vendor: %1\n"
    		"Library: %2\n"
    		"Name: %3\n"
    		"Version: %4\n"
    		"was not found in the library.").arg(
            vlnv.getVendor(), vlnv.getLibrary(), vlnv.getName(), vlnv.getVersion()));
    }

    return libraryItems_.value(vlnv, QString());
}

//-----------------------------------------------------------------------------
// Function: LibraryData::addVLNV()
//-----------------------------------------------------------------------------
bool LibraryData::addVLNV(VLNV const& vlnv, QString const& path)
{
    if (libraryItems_.contains(vlnv))
    {
    	messageChannel_->showError(tr("The VLNV \n"
    		"Vendor: %1\n"
    		"Library: %2\n"
    		"Name: %3\n"
    		"Version: %4\n"
    		"Already existed in the library and was not added.").arg(
    		vlnv.getVendor(), vlnv.getLibrary(), vlnv.getName(), vlnv.getVersion()));

    	return false;
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists())
    {
    	messageChannel_->showError(tr("File %1 was not found in file system").arg(path));
    	return false;
    }

    // add the component to the library
    libraryItems_.insert(vlnv, path);
    fileWatch_->addPath(path);

    emit addVLNV(vlnv);

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::contains()
//-----------------------------------------------------------------------------
bool LibraryData::contains(VLNV const& vlnv)
{
    return libraryItems_.contains(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getType()
//-----------------------------------------------------------------------------
VLNV::IPXactType LibraryData::getType(VLNV const& vlnv) const
{
    if (!libraryItems_.contains(vlnv))
    {
    	return VLNV::INVALID;
    }
    else
    {
    	return libraryItems_.find(vlnv).key().getType();
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getItems()
//-----------------------------------------------------------------------------
QList<VLNV> LibraryData::getItems() const
{
    return libraryItems_.keys();
}

//-----------------------------------------------------------------------------
// Function: LibraryData::getModel()
//-----------------------------------------------------------------------------
QSharedPointer<Document> LibraryData::getModel(VLNV const& vlnv)
{
    if (!libraryItems_.contains(vlnv))
    {
        //emit noticeMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
        return QSharedPointer<Document>();
    }

    return fileAccess_.readDocument(getPath(vlnv));
}

//-----------------------------------------------------------------------------
// Function: LibraryData::writeFile()
//-----------------------------------------------------------------------------
bool LibraryData::writeFile(QSharedPointer<Document> model, QString const& filePath)
{
    QString targetPath = filePath;
    if (filePath.isEmpty())
    {
        targetPath = getPath(model->getVlnv());
    }

    QFileInfo pathInfo(filePath);
    if (pathInfo.isSymLink() && pathInfo.exists())
    {
        targetPath = pathInfo.symLinkTarget();
    }

    // Create a new file.
    fileWatch_->removePath(targetPath);
    fileAccess_.writeDocument(model, targetPath);
    fileWatch_->addPath(targetPath);

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDocument()
//-----------------------------------------------------------------------------
bool LibraryData::validateDocument(QSharedPointer<Document> document)
{
    if (document.isNull())
    {
        return false;
    }

    VLNV documentVLNV = document->getVlnv();
    QString documentPath = getPath(documentVLNV);

    Q_ASSERT(!documentPath.isEmpty());

    if (documentPath.isEmpty() || !QFileInfo(documentPath).exists())
    {
        return false;
    }

    bool documentValid = validator_.validate(document);

    return documentValid && validateDependentVLNVReferencences(document) &&
        validateDependentDirectories(document, documentPath) &&
        validateDependentFiles(document, documentPath);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDocument()
//-----------------------------------------------------------------------------
QVector<QString> LibraryData::findErrorsInDocument(QSharedPointer<Document> document)
{
    Q_ASSERT(document);

    QVector<QString> errorList;

    QString path = getPath(document->getVlnv());
    if (!QFileInfo(path).exists())
    {
        errorList.append(tr("File %1 for the document was not found.").arg(path));
    }

    validator_.findErrorsIn(document, errorList);

    findErrorsInDependentVLNVReferencences(document, errorList);

    findErrorsInDependentDirectories(document, path, errorList);

    findErrorsInDependentFiles(document, path, errorList);

    return errorList;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::parseLibrary()
//-----------------------------------------------------------------------------
void LibraryData::parseLibrary()
{
    // clear the previous items in the library
    if (!libraryItems_.isEmpty())
    {
        fileWatch_->removePaths(libraryItems_.values());
        libraryItems_.clear();
    }

    QStringList locations = QSettings().value("Library/ActiveLocations", QStringList()).toStringList();

    messageChannel_->showStatusMessage(tr("Scanning library. Please wait..."));

    foreach (QString const& location, locations)
    {
        performParseLibraryStep(location);
    }

    checkLibraryIntegrity();

    emit resetModel();
    messageChannel_->showStatusMessage(tr("Ready."));
}

//-----------------------------------------------------------------------------
// Function: LibraryData::checkLibraryIntegrity()
//-----------------------------------------------------------------------------
void LibraryData::checkLibraryIntegrity()
{
    failedObjects_ = 0;
    fileCount_ = 0;

    messageChannel_->showStatusMessage(tr("Validating items. Please wait..."));

    foreach (VLNV const& documentVLNV, libraryItems_.keys())
    {
        performIntegrityCheck(documentVLNV);     
    }

    messageChannel_->showMessage(tr("========== Library integrity check complete =========="));
    messageChannel_->showMessage(tr("Total library object count: %1").arg(libraryItems_.size()));
    messageChannel_->showMessage(tr("Total file count in the library: %1").arg(fileCount_));

    // if errors were found then print the summary of error types
    if (failedObjects_ > 0)
    {
        messageChannel_->showError(tr("Total items containing errors: %1").arg(failedObjects_));
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::performParseLibrary()
//-----------------------------------------------------------------------------
void LibraryData::performParseLibraryStep(QString const& location)
{
    QFileInfo locationInfo(location);

    if (locationInfo.isDir())
    {
        parseDirectory(location);
    }
    else if (locationInfo.isFile())
    {
        parseFile(location);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::performIntegrityCheckStep()
//-----------------------------------------------------------------------------
void LibraryData::performIntegrityCheck(VLNV const& documentVLNV)
{
    if (!library_->isValid(documentVLNV))
    {
        failedObjects_++;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::onRemoveVLNV()
//-----------------------------------------------------------------------------
void LibraryData::onRemoveVLNV(VLNV const& vlnv)
{
    if (!libraryItems_.contains(vlnv))
    {
        return;
    }

    QString documentPath = getPath(vlnv);

    QFile xmlFile(documentPath);
    if (!xmlFile.exists())
    {
        messageChannel_->showError(tr("File %1 was not found in file system").arg(documentPath));
    }
    else if (!xmlFile.remove())
    {
        messageChannel_->showError(tr("Could not remove file %1").arg(documentPath));
    }

    fileWatch_->removePath(documentPath);
    libraryItems_.remove(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryData::resetLibrary()
//-----------------------------------------------------------------------------
void LibraryData::resetLibrary()
{
    emit resetModel();
}

//-----------------------------------------------------------------------------
// Function: LibraryData::onFileChanged()
//-----------------------------------------------------------------------------
void LibraryData::onFileChanged(QString const& path)
{
    if (QFileInfo(path).exists())
    {
        VLNV changedDocument = libraryItems_.key(path, VLNV());
        emit updatedVLNV(changedDocument);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDependentVLNVReferencences()
//-----------------------------------------------------------------------------
bool LibraryData::validateDependentVLNVReferencences(QSharedPointer<Document> document)
{
    foreach (VLNV const& vlnv, document->getDependentVLNVs())
    {
        if (!libraryItems_.contains(vlnv))
        {
           return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDependentVLNVReferencences()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDependentVLNVReferencences(QSharedPointer<const Document> document,
    QVector<QString>& errorList)
{
    foreach (VLNV const& vlnv, document->getDependentVLNVs())
    {
        if (!libraryItems_.contains(vlnv))
        {
            errorList.append(tr("The referenced VLNV was not found in the library: %1").arg(vlnv.toString()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDependentDirectories()
//-----------------------------------------------------------------------------
bool LibraryData::validateDependentDirectories(QSharedPointer<Document> document, QString const& documentPath)
{
    foreach (QString const& directory, document->getDependentDirs())
    {
        QFileInfo dirInfo(General::getAbsolutePath(documentPath, directory));
        if (!dirInfo.exists())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDependentDirectories()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDependentDirectories(QSharedPointer<const Document> document, 
    QString const& documentPath, QVector<QString>& errorList)
{
    foreach (QString const& directory, document->getDependentDirs())
    {
        QString dirPath = General::getAbsolutePath(documentPath, directory);

        if (!QFileInfo(dirPath).exists())
        {
            errorList.append(tr("\tDirectory %1 was not found in the file system.").arg(dirPath));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::validateDependentFiles()
//-----------------------------------------------------------------------------
bool LibraryData::validateDependentFiles(QSharedPointer<Document> document, QString const& documentPath)
{
    foreach (QString filePath, document->getDependentFiles())
    {
        int pos = 0;
        if (urlTester_->validate(filePath, pos) != QValidator::Acceptable)
        {
            QString absolutePath = filePath;

            if (QFileInfo(filePath).isRelative())
            {
                absolutePath = General::getAbsolutePath(documentPath, filePath);
            }

            if (!QFileInfo(absolutePath).exists())
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDependentFiles()
//-----------------------------------------------------------------------------
void LibraryData::findErrorsInDependentFiles(QSharedPointer<const Document> document, 
    QString const& documentPath, QVector<QString>& errorList)
{
    foreach (QString filePath, document->getDependentFiles())
    {
        // Check if the path is actually a URL to (external) location.
        int pos = 0;
        if (urlTester_->validate(filePath, pos) == QValidator::Acceptable)
        {
            fileCount_++;
        }

        // The path was not URL so it must be file reference on the disk.
        else 
        {
            QString absolutePath = filePath;

            if (QFileInfo(filePath).isRelative())
            {
                absolutePath = General::getAbsolutePath(documentPath, filePath);
            }

            if (QFileInfo(absolutePath).exists())
            {
                fileCount_++;
            }
            else
            {
                errorList.append(tr("File %1 was not found in the file system.").arg(filePath));               
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::parseDirectory()
//-----------------------------------------------------------------------------
void LibraryData::parseDirectory(QString const& directoryPath)
{
    if (directoryPath.isEmpty())
    {
        return;
    }

    QDir directoryHandler(directoryPath);
    directoryHandler.setNameFilters(QStringList(QLatin1String("*.xml")));

    // Get list of files and folders.
    foreach (QFileInfo const& entryInfo, 
        directoryHandler.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::Readable))
    {
        if (entryInfo.isFile())
        {
            parseFile(entryInfo.absoluteFilePath());
        }
        else if (entryInfo.isDir())
        {
            parseDirectory(entryInfo.absoluteFilePath());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::parseFile()
//-----------------------------------------------------------------------------
void LibraryData::parseFile(QString const& filePath)
{
    VLNV vlnv = fileAccess_.getDocumentVLNV(filePath);

    if (!vlnv.isValid())
    {
        return;
    }

    if (libraryItems_.contains(vlnv))
    {
        messageChannel_->showMessage(tr("VLNV %1 was already found in the library").arg(vlnv.toString()));
        return;
    }

    // add the component to the library if parsing was successful
    libraryItems_.insert(vlnv, filePath);
    fileWatch_->addPath(filePath);
}
