//-----------------------------------------------------------------------------
// File: libraryhandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryHandler is the class that implements the services to manage the IP-XACT library.
//-----------------------------------------------------------------------------

#include "LibraryHandler.h"

#include "MessageMediator.h"

#include "utils.h"

#include "TagManager.h"

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

// Singleton instance.
LibraryHandler* LibraryHandler::instance_ = nullptr;

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getInstance()
//-----------------------------------------------------------------------------
LibraryHandler* LibraryHandler::getInstance()
{
    if (instance_ == nullptr)
    {
        initialize(nullptr, nullptr);
    }

    return LibraryHandler::instance_;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::initialize()
//-----------------------------------------------------------------------------
void LibraryHandler::initialize(MessageMediator* messageChannel, QObject* parent)
{
    if (instance_ == nullptr)
    {
        LibraryHandler::instance_ = new LibraryHandler(messageChannel, parent);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::LibraryHandler()
//-----------------------------------------------------------------------------
LibraryHandler::LibraryHandler(MessageMediator* messageChannel, QObject* parent):
QObject(parent),
    messageChannel_(messageChannel),
    loader_(messageChannel),
    documentCache_(),
    urlTester_(Utils::URL_VALIDITY_REG_EXP, this),
    validator_(this),
    treeModel_(new LibraryTreeModel(this, this)),
    hierarchyModel_(new HierarchyModel(this, this)),
    saveInProgress_(false),
    checkResults_()
{
    // create the connections between models and library handler
    syncronizeModels();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getModel()
//-----------------------------------------------------------------------------
QSharedPointer<Document> LibraryHandler::getModel(VLNV const& vlnv)
{
    auto info = documentCache_.find(vlnv);
    if (info == documentCache_.end())
    {
        showNotFoundError(vlnv);
        return QSharedPointer<Document>();
    }

    // If object has not already been parsed, read it from the disk.
    if (info->document.isNull())
    {
        info->document = DocumentFileAccess::readDocument(info->path);
    }

    QSharedPointer<Document> copy;
    if (info->document.isNull() == false)
    {
        copy = info->document->clone();
    }

    return copy;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getModelReadOnly()
//-----------------------------------------------------------------------------
QSharedPointer<Document const> LibraryHandler::getModelReadOnly(VLNV const& vlnv)
{
    auto info = documentCache_.find(vlnv);
    if (info == documentCache_.end())
    {
        return QSharedPointer<Document const>();
    }

    if (info->document.isNull())
    {
        info->document = DocumentFileAccess::readDocument(info->path);
    }

    return info->document;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getAllVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> LibraryHandler::getAllVLNVs() const
{
    return documentCache_.keys();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::contains()
//-----------------------------------------------------------------------------
bool LibraryHandler::contains(const VLNV& vlnv) const
{
    return documentCache_.contains(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getPath()
//-----------------------------------------------------------------------------
const QString LibraryHandler::getPath(VLNV const& vlnv) const
{
    return documentCache_.value(vlnv).path;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getDirectoryPath()
//-----------------------------------------------------------------------------
QString LibraryHandler::getDirectoryPath(VLNV const& vlnv) const
{
    QFileInfo pathInfo(getPath(vlnv));
    return pathInfo.absolutePath();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::writeModelToFile()
//-----------------------------------------------------------------------------
bool LibraryHandler::writeModelToFile(QString const& path, QSharedPointer<Document> model)
{
    VLNV vlnv = model->getVlnv();
    Q_ASSERT(contains(vlnv) == false);
    
    // Create the path if it does not exist.
    if (QDir(path).exists() == false && QDir().mkpath(path) == false)
    {
        messageChannel_->showFailure(tr("Could not create directory \"%1\".").arg(path));        
        return false;
    }

    QString filePath = path + "/" + vlnv.getName() + "." + vlnv.getVersion() + ".xml";
    if (addObject(model, filePath) == false)
    {
        return false;
    }

    // the hierarchy model must be re-built
    hierarchyModel_->onResetModel();
    treeModel_->onAddVLNV(vlnv);
    
    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::writeModelToFile()
//-----------------------------------------------------------------------------
bool LibraryHandler::writeModelToFile(QSharedPointer<Document> model)
{
    return addObject(model, getPath(model->getVlnv()));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::searchForIPXactFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::searchForIPXactFiles()
{
    clearCache();

    loadAvailableVLNVs();

    onCheckLibraryIntegrity();
    
    resetModels();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getNeededVLNVs()
//-----------------------------------------------------------------------------
void LibraryHandler::getNeededVLNVs(VLNV const& vlnv, QList<VLNV>& list)
{
    if (contains(vlnv) == false)
    {
        messageChannel_->showError(tr("No item with following info was found in library: \n"
            "Vendor: %1\n"
            "Library: %2\n"
            "Name: %3\n"
            "Version: %4").arg(vlnv.getVendor(), vlnv.getLibrary(), vlnv.getName(), vlnv.getVersion()));
        return;
    }

    // This document is searched so add it to the list.
    list.append(vlnv);

    for (VLNV const& dependentVLNV : getModelReadOnly(vlnv)->getDependentVLNVs())
    {
        if (list.contains(dependentVLNV) == false)
        {
            getNeededVLNVs(dependentVLNV, list);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getDependencyFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::getDependencyFiles(VLNV const& vlnv, QStringList& list)
{
    QSharedPointer<const Document> document = getModelReadOnly(vlnv);

    // Get path to the IP-Xact document that is used as base for relative file paths.
    QFileInfo documentFile(getPath(vlnv));
    QDir documentDirectory(documentFile.absolutePath());

    // Convert all relative file path to an absolute to make the file accessible in the caller function.
    for (QString const& relativePath : document->getDependentFiles())
    {
        QFileInfo dependentFile(documentDirectory.absoluteFilePath(relativePath));
        
        QString path = dependentFile.canonicalFilePath();
        if (dependentFile.exists() && list.contains(path) == false)
        {
            list.append(path);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getTreeRoot()
//-----------------------------------------------------------------------------
LibraryItem const* LibraryHandler::getTreeRoot() const
{
    return treeModel_->getRoot();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getDocumentType()
//-----------------------------------------------------------------------------
VLNV::IPXactType LibraryHandler::getDocumentType(VLNV const& vlnv)
{
    auto it = documentCache_.constFind(vlnv);
    if (it == documentCache_.constEnd())
    {
        return VLNV::INVALID;
    }

    return it.key().getType();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::referenceCount()
//-----------------------------------------------------------------------------
int LibraryHandler::referenceCount(VLNV const& vlnv) const
{
    QList<VLNV> list;
    return hierarchyModel_->getOwners(list, vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getOwners()
//-----------------------------------------------------------------------------
int LibraryHandler::getOwners(QList<VLNV>& list, VLNV const& vlnvToSearch) const
{
    return hierarchyModel_->getOwners(list, vlnvToSearch);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getChildren()
//-----------------------------------------------------------------------------
int LibraryHandler::getChildren(QList<VLNV>& list, VLNV const& vlnvToSearch) const
{
    if (vlnvToSearch.isValid() == false)
    {
        return 0;
    }

    hierarchyModel_->getChildren(list, vlnvToSearch);
    return list.size();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getDesignVLNV()
//-----------------------------------------------------------------------------
VLNV LibraryHandler::getDesignVLNV(VLNV const& hierarchyRef)
{
    if (getDocumentType(hierarchyRef) == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<const DesignConfiguration> desConf = 
            getModelReadOnly(hierarchyRef).staticCast<const DesignConfiguration>();

        VLNV designVLNV = desConf->getDesignRef();
        if (!contains(designVLNV) || getDocumentType(designVLNV) != VLNV::DESIGN)
        {
            showNotFoundError(designVLNV);
            return VLNV();
        }

        designVLNV.setType(VLNV::DESIGN);
        return designVLNV;
    }
    else if (getDocumentType(hierarchyRef) == VLNV::DESIGN)
    {
        return hierarchyRef;
    }
    else if (contains(hierarchyRef) == false)
    {
        showNotFoundError(hierarchyRef);
        return VLNV();
    }
    else
    {
        messageChannel_->showError(tr("VLNV: %1 was not valid hierarchical reference.").arg(hierarchyRef.toString()));
        return VLNV();
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> LibraryHandler::getDesign(VLNV const& hierarchyRef)
{
    VLNV designVlnv = getDesignVLNV(hierarchyRef);
    if (designVlnv.isValid() == false)
    {
        showNotFoundError(hierarchyRef);
        return QSharedPointer<Design>();
    }   

    return getModel(designVlnv).staticCast<Design>();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::isValid()
//-----------------------------------------------------------------------------
bool LibraryHandler::isValid(VLNV const& vlnv)
{    
    auto it = documentCache_.constFind(vlnv);
    if (it != documentCache_.cend())
    {
        return documentCache_.value(vlnv).isValid;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getHierarchyModel()
//-----------------------------------------------------------------------------
HierarchyModel* LibraryHandler::getHierarchyModel()
{
    return hierarchyModel_;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getTreeModel()
//-----------------------------------------------------------------------------
LibraryTreeModel* LibraryHandler::getTreeModel()
{
    return treeModel_;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onCheckLibraryIntegrity()
//-----------------------------------------------------------------------------
void LibraryHandler::onCheckLibraryIntegrity()
{
    messageChannel_->showStatusMessage(tr("Validating items. Please wait..."));

    checkResults_.documentCount = 0;
    checkResults_.fileCount = 0;

    QVector<TagData> documentTags;
    for (auto it = documentCache_.begin(); it != documentCache_.end(); ++it)
    {
        // TODO: Add model to cache only, if it is already previously cached.
        // Current hierarchy model forces all models to be loaded, but this should be changed.
        QSharedPointer<Document> model = it->document;
        if (model.isNull())
        {
            model = DocumentFileAccess::readDocument(it->path);
            it->document = model;
        }           

        if (model.isNull() == false)
        {
            documentTags += (model->getTags());
        }

        it->isValid = validateDocument(model, it->path);
        if (it->isValid == false)
        {
            checkResults_.documentCount++;
        }
    }

    TagManager& manager = TagManager::getInstance();
    manager.setTags(documentTags);

    showIntegrityResults();

    messageChannel_->showStatusMessage(tr("Ready."));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onEditItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onEditItem(VLNV const& vlnv)
{
    if (vlnv.isValid() == false)
    {
        return;
    }

    VLNV::IPXactType documentType = getDocumentType(vlnv);
    if (documentType == VLNV::COMPONENT)
    {
        emit openComponent(vlnv);
    }

    else if (documentType == VLNV::CATALOG)
    {
        emit openCatalog(vlnv);
    }

    else if (documentType == VLNV::COMDEFINITION)
    {
        emit openComDefinition(vlnv);
    }

    else if (documentType == VLNV::APIDEFINITION)
    {
        emit openApiDefinition(vlnv);
    }

    else if (documentType == VLNV::BUSDEFINITION)
    {
        QList<VLNV> absDefVLNVs;
        hierarchyModel_->getChildren(absDefVLNVs, vlnv);

        if (absDefVLNVs.size() == 1)
        {
            emit openBus(vlnv, absDefVLNVs.first(), false);
        }

        else // the abstraction definition can not be identified by the bus definition.
        {
            emit openBus(vlnv, VLNV(), false);
        }
    }

    else if (documentType == VLNV::ABSTRACTIONDEFINITION)
    {
        QSharedPointer<const Document> document = getModelReadOnly(vlnv);
        QSharedPointer<const AbstractionDefinition> absDef = document.staticCast<const AbstractionDefinition>();

        VLNV busDefVLNV = absDef->getBusType();

        if (contains(busDefVLNV) == false)
        {
            QString message(
                tr("Library does not contain bus definition %1 required by abstraction definition %2").arg(
                busDefVLNV.toString(), vlnv.toString()));

            messageChannel_->showError(message);
            return;
        }

        QList<VLNV> absDefVLNVs;
        hierarchyModel_->getChildren(absDefVLNVs, busDefVLNV);

        // If there is exactly one abstraction definition per bus definition, open it.
        if (absDefVLNVs.size() == 1 && absDefVLNVs.first() == vlnv)
        {
            emit openBus(busDefVLNV, vlnv, false);
        }
        else  // If there are multiple abstraction definitions for that one bus def, disable bus definition.
        {
            emit openBus(busDefVLNV, vlnv, true);
        }
    }

    else
    {
        messageChannel_->showMessage(tr("Item can not be edited."));
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenDesign(VLNV const& vlnv, QString const& viewName)
{
    if (contains(vlnv) == false)
    {
        messageChannel_->showError(tr("Component was not found"));
        return;
    }

    if (getDocumentType(vlnv) == VLNV::COMPONENT)
    {
         emit openDesign(vlnv, viewName);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenMemoryDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenMemoryDesign(VLNV const& vlnv, QString const& activeView)
{
    QSharedPointer<const Component> component = getModelReadOnly(vlnv).dynamicCast<const Component>();
    if (component.isNull())
    {
        messageChannel_->showError(tr("Component was not found"));
        return;
    }
       
    if (component->getHierViews().contains(activeView))
    {
        emit openMemoryDesign(vlnv, activeView);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenSWDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenSWDesign(VLNV const& vlnv)
{
    QSharedPointer<const Component> component = getModelReadOnly(vlnv).dynamicCast<const Component>();
    if (component.isNull())
    {
        messageChannel_->showError(tr("Component was not found"));
        return;
    }

    for (QSharedPointer<View> view : *component->getViews())
    {
        VLNV reference = component->getModel()->getHierRef(view->name());

        QSharedPointer<const Document> document = getModelReadOnly(reference);

        if (document && document->getImplementation() == KactusAttribute::SW)
        {
            emit openSWDesign(vlnv, view->name());
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenSystemDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenSystemDesign(VLNV const& vlnv)
{
    QSharedPointer<const Component> component = getModelReadOnly(vlnv).dynamicCast<const Component>();
    if (component.isNull())
    {
        messageChannel_->showError(tr("Component was not found"));
        return;
    }

    QStringList systemViews = component->getSystemViewNames();
    if (!systemViews.isEmpty())
    {
        emit openSystemDesign(vlnv, systemViews.first());
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::removeObject()
//-----------------------------------------------------------------------------
void LibraryHandler::removeObject(VLNV const& vlnv)
{
    if (vlnv.isValid() == false || contains(vlnv) == false)
    {
        return;
    }

    QString path = documentCache_.find(vlnv)->path;

    documentCache_.remove(vlnv);

    treeModel_->onRemoveVLNV(vlnv);
    hierarchyModel_->onRemoveVLNV(vlnv);

    removeFile(path);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::removeObjects()
//-----------------------------------------------------------------------------
void LibraryHandler::removeObjects(QList<VLNV> const& vlnvList)
{
    QStringList changedDirectories;
    for (auto const& removedItem : vlnvList)
    {
        changedDirectories.append(QFileInfo(getPath(removedItem)).absolutePath());
        removeObject(removedItem);
    }

    changedDirectories.removeDuplicates();
    loader_.clean(changedDirectories);

    QString removeMessage = QString("Deleted %1 VLNV item(s).").arg(QString::number(vlnvList.count()));

    messageChannel_->showMessage(removeMessage);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::beginSave()
//-----------------------------------------------------------------------------
void LibraryHandler::beginSave()
{
    saveInProgress_ = true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::endSave()
//-----------------------------------------------------------------------------
void LibraryHandler::endSave()
{
    saveInProgress_ = false;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onItemSaved()
//-----------------------------------------------------------------------------
void LibraryHandler::onItemSaved(VLNV const& vlnv) 
{
    if (vlnv.isValid() == false)
    {
        return;
    }

    VLNV savedItem = vlnv;
    savedItem.setType(getDocumentType(vlnv));

    // Replace overwritten item with new and check validity.
    QSharedPointer<Document> model = getModel(vlnv);
    documentCache_.insert(vlnv, DocumentInfo(getPath(vlnv), model, validateDocument(model, getPath(vlnv))));
    
    treeModel_->onDocumentUpdated(vlnv);
    hierarchyModel_->onDocumentUpdated(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::syncronizeModels()
//-----------------------------------------------------------------------------
void LibraryHandler::syncronizeModels()
{
    // signals from tree model to library handler
 
    connect(treeModel_, SIGNAL(openDesign(const VLNV&, QString const&)),
        this, SLOT(onOpenDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(openMemoryDesign(const VLNV&, QString const&)),
        this, SLOT(onOpenMemoryDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(openSWDesign(const VLNV&)),
        this, SLOT(onOpenSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(openSystemDesign(const VLNV&)),
        this, SLOT(onOpenSystemDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(editItem(const VLNV&)),
        this, SLOT(onEditItem(const VLNV&)), Qt::UniqueConnection);

    connect(this, SIGNAL(resetModel()),
        treeModel_, SLOT(onResetModel()), Qt::UniqueConnection);

    //-----------------------------------------------------------------------------
    // connect the signals from the hierarchy model
    //-----------------------------------------------------------------------------

    connect(hierarchyModel_, SIGNAL(openDesign(const VLNV&, const QString&)),
        this, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(openMemoryDesign(const VLNV&, const QString&)),
        this, SLOT(onOpenMemoryDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(openSWDesign(const VLNV&, const QString&)),
        this, SIGNAL(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(openSystemDesign(const VLNV&, const QString&)),
        this, SIGNAL(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(editItem(const VLNV&)),
        this, SLOT(onEditItem(const VLNV&)), Qt::UniqueConnection);


    connect(this, SIGNAL(resetModel()),
        hierarchyModel_, SLOT(onResetModel()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::showNotFoundError()
//-----------------------------------------------------------------------------
void LibraryHandler::showNotFoundError(VLNV const& vlnv) const
{
    messageChannel_->showError(tr("VLNV: %1 was not found in library.").arg(vlnv.toString()));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::addObject()
//-----------------------------------------------------------------------------
bool LibraryHandler::addObject(QSharedPointer<Document> model, QString const& filePath)
{
    QString targetPath = filePath;

    QFileInfo pathInfo(filePath);
    if (pathInfo.isSymLink() && pathInfo.exists())
    {
        targetPath = pathInfo.symLinkTarget();
    }

    DocumentFileAccess::writeDocument(model, targetPath);

    TagManager::getInstance().addNewTags(model->getTags());

    documentCache_.insert(model->getVlnv(), DocumentInfo(targetPath, model->clone(), validateDocument(model, targetPath)));

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::clearCache()
//-----------------------------------------------------------------------------
void LibraryHandler::clearCache()
{
    documentCache_.clear();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::loadAvailableVLNVs()
//-----------------------------------------------------------------------------
void LibraryHandler::loadAvailableVLNVs()
{
    messageChannel_->showStatusMessage(tr("Scanning library. Please wait..."));

    // Read all items before validation.
    // Validation will check for VLNVs in the library, so they must be available before validation.    
    for (auto const& target: loader_.parseLibrary())
    {
        if (contains(target.vlnv))
        {
            messageChannel_->showMessage(tr("VLNV %1 was already found in the library").arg(
                target.vlnv.toString()));
        }
        else
        {
            documentCache_.insert(target.vlnv, DocumentInfo(target.path));
        }
    }
    messageChannel_->showStatusMessage(tr("Ready."));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::resetModels()
//-----------------------------------------------------------------------------
void LibraryHandler::resetModels()
{
    messageChannel_->showStatusMessage(tr("Updating library view. Please wait..."));

    emit resetModel();

    messageChannel_->showStatusMessage(tr("Ready."));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::showIntegrityResults()
//-----------------------------------------------------------------------------
void LibraryHandler::showIntegrityResults() const
{
    messageChannel_->showMessage(tr("========== Library integrity check complete =========="));
    messageChannel_->showMessage(tr("Total library object count: %1").arg(documentCache_.size()));
    messageChannel_->showMessage(tr("Total file count in the library: %1").arg(checkResults_.fileCount));

    // if errors were found then print the summary of error types
    if (checkResults_.documentCount > 0)
    {
        messageChannel_->showError(tr("Total items containing errors: %1").arg(checkResults_.documentCount));
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryData::removeFile()
//-----------------------------------------------------------------------------
bool LibraryHandler::removeFile(QString const& filePath) const
{
    QFile file(filePath);
    if (file.remove() == false)
    {
        messageChannel_->showError(tr("File %1 could not be removed from the file system.").arg(filePath));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::validateDocument()
//-----------------------------------------------------------------------------
bool LibraryHandler::validateDocument(QSharedPointer<Document> document, QString const& documentPath)
{
    if (document.isNull() || QFileInfo(documentPath).exists() == false)
    {
        return false;
    }

    bool documentValid = validator_.validate(document);

    return documentValid && validateDependentVLNVReferencences(document) &&
        validateDependentDirectories(document, documentPath) &&
        validateDependentFiles(document, documentPath);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::findErrorsInDocument()
//-----------------------------------------------------------------------------
QVector<QString> LibraryHandler::findErrorsInDocument(QSharedPointer<Document> document, QString const& path)
{
    QVector<QString> errorList;
    if (document.isNull())
    {
        if (path.isEmpty() == false)
        {
            auto it = std::find_if(documentCache_.cbegin(), documentCache_.cend(),
                [path](DocumentInfo const& info) { return info.path == path;  });
            
            if (it != documentCache_.cend())
            {
                errorList.append(tr("File %1 could not be read.").arg(path));
            }
        }

        return errorList;
    }

    if (QFileInfo(path).exists() == false)
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
// Function: LibraryHandler::validateDependentVLNVReferencences()
//-----------------------------------------------------------------------------
bool LibraryHandler::validateDependentVLNVReferencences(QSharedPointer<Document> document)
{
    for (VLNV const& vlnv : document->getDependentVLNVs())
    {
        if (contains(vlnv) == false)
        {
           return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::findErrorsInDependentVLNVReferencences()
//-----------------------------------------------------------------------------
void LibraryHandler::findErrorsInDependentVLNVReferencences(QSharedPointer<const Document> document,
    QVector<QString>& errorList)
{
    for (VLNV const& vlnv : document->getDependentVLNVs())
    {
        if (contains(vlnv) == false)
        {
            errorList.append(tr("The referenced VLNV was not found in the library: %1").arg(vlnv.toString()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::validateDependentDirectories()
//-----------------------------------------------------------------------------
bool LibraryHandler::validateDependentDirectories(QSharedPointer<Document> document, QString const& documentPath)
{
    for (QString const& directoryPath : document->getDependentDirs())
    {
        QFileInfo directory(General::getAbsolutePath(documentPath, directoryPath));
        if (directory.exists() == false)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::findErrorsInDependentDirectories()
//-----------------------------------------------------------------------------
void LibraryHandler::findErrorsInDependentDirectories(QSharedPointer<const Document> document,
    QString const& documentPath, QVector<QString>& errorList)
{
    for (QString const& directoryPath : document->getDependentDirs())
    {
        QFileInfo directory(General::getAbsolutePath(documentPath, directoryPath));
        if (directory.exists() == false)
        {
            errorList.append(tr("\tDirectory %1 was not found in the file system.").arg(directoryPath));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::validateDependentFiles()
//-----------------------------------------------------------------------------
bool LibraryHandler::validateDependentFiles(QSharedPointer<Document> document, QString const& documentPath)
{
    for (QString filePath : document->getDependentFiles())
    {
        int pos = 0;
        if (urlTester_.validate(filePath, pos) != QValidator::Acceptable)
        {
            if (QFileInfo(filePath).isRelative())
            {
                filePath = General::getAbsolutePath(documentPath, filePath);
            }

            if (QFileInfo(filePath).exists() == false)
            {
                return false;
            }
            else
            {
                checkResults_.fileCount++;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryData::findErrorsInDependentFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::findErrorsInDependentFiles(QSharedPointer<const Document> document,
    QString const& documentPath, QVector<QString>& errorList)
{
    for (QString filePath : document->getDependentFiles())
    {
        // Check if the path is actually a URL to (external) location.
        int pos = 0;
        if (urlTester_.validate(filePath, pos) == QValidator::Acceptable)
        {
            checkResults_.fileCount++;
        }

        // The path was not URL so it must be file reference on the disk.
        else
        {
            QString absolutePath = filePath;
            if (QFileInfo(absolutePath).isRelative())
            {
                absolutePath = General::getAbsolutePath(documentPath, filePath);
            }

            if (QFileInfo(absolutePath).exists() == false)
            {
                errorList.append(tr("File %1 was not found in the file system.").arg(filePath));
            }
        }
    }
}
