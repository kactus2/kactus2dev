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

/*
#include "LibraryErrorModel.h"
#include "TableViewDialog.h"
#include "LibraryItemSelectionFactory.h"*/

#include "ItemExporter.h"

#include "MessageMediator.h"

#include "utils.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/ObjectRemoveDialog/objectremovedialog.h>
#include"ObjectSelectionListItem.h"

#include "TagManager.h"

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTimer>

LibraryHandler* LibraryHandler::instance_ = nullptr;

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getInstance()
//-----------------------------------------------------------------------------
LibraryHandler* LibraryHandler::getInstance()
{
    if (instance_ == nullptr)
    {
        initialize(nullptr, nullptr, nullptr);
    }

    return LibraryHandler::instance_;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::initialize()
//-----------------------------------------------------------------------------
void LibraryHandler::initialize(QWidget* parentWidget, MessageMediator* messageChannel, QObject* parent)
{
    if (instance_ == nullptr)
    {
        LibraryHandler::instance_ = new LibraryHandler(parentWidget, messageChannel, parent);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::LibraryHandler()
//-----------------------------------------------------------------------------
LibraryHandler::LibraryHandler(QWidget* parentWidget, MessageMediator* messageChannel, QObject* parent):
QObject(parent),
    parentWidget_(parentWidget),
    messageChannel_(messageChannel),
    fileAccess_(messageChannel),
    loader_(messageChannel),
    documentCache_(),
    urlTester_(Utils::URL_VALIDITY_REG_EXP, this),
    validator_(this),
    treeModel_(new LibraryTreeModel(this, this)),
    hierarchyModel_(new HierarchyModel(this, this)),
    integrityWidget_(0),
    saveInProgress_(false),
    fileWatch_(this),
    itemExporter_(new ItemExporter(messageChannel, this, fileAccess_, parentWidget, this)),
    checkResults_(),
    updatedPaths_()
{
    // create the connections between models and library handler
    syncronizeModels();
    connect(&fileWatch_, SIGNAL(fileChanged(QString const&)),
            this, SLOT(onFileChangedOnDisk(QString const&)), Qt::UniqueConnection);
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
        info->document = fileAccess_.readDocument(info->path);
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
        info->document = fileAccess_.readDocument(info->path);
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
        emit errorMessage(tr("VLNV: %1 was not valid hierarchical reference.").arg(hierarchyRef.toString()));
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
            model = fileAccess_.readDocument(it->path);
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
// Function: LibraryHandler::onShowErrors()
//-----------------------------------------------------------------------------
void LibraryHandler::onShowErrors(VLNV const& vlnv)
{
    if (vlnv.isValid() == false || contains(vlnv) == false)
    {
        return;
    }

    QSharedPointer<Document> document = getModel(vlnv);

    // Show error list in a dialog.
    /*TableViewDialog* dialog = new TableViewDialog(parentWidget_);
    dialog->setWindowTitle(tr("Errors in %1").arg(vlnv.toString()));
    dialog->setDescription(tr("<b>Integrity check</b><br>The following errors were found."));
    dialog->resize(700, 350);

    LibraryErrorModel* model = new LibraryErrorModel(dialog);
    model->addErrors(findErrorsInDocument(document, getPath(vlnv)), vlnv.toString());

    dialog->show();

    dialog->setModel(model);

    connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));*/
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onGenerateIntegrityReport()
//-----------------------------------------------------------------------------
void LibraryHandler::onGenerateIntegrityReport()
{
   /* if (!integrityWidget_)
    {
        integrityWidget_ = new TableViewDialog(parentWidget_);
        integrityWidget_->setWindowTitle(tr("Library Integrity Report"));
        integrityWidget_->setDescription(tr("<b>Integrity check</b><br>The following errors were found."));
        integrityWidget_->resize(1000, 800);

        LibraryErrorModel* model = new LibraryErrorModel(integrityWidget_);
        
        auto end = documentCache_.constEnd();
        for (auto it = documentCache_.constBegin(); it != end; ++it)
        {
            if (it->isValid == false)
            {
                model->addErrors(findErrorsInDocument(it->document, it->path), it.key().toString());
            }
        }             

        integrityWidget_->setModel(model);

        integrityWidget_->show();       

        connect(integrityWidget_, SIGNAL(finished(int)), this, SLOT(onCloseIntegrityReport()));
    }

    integrityWidget_->raise();*/
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenDesign(VLNV const& vlnv, QString const& viewName)
{
    if (contains(vlnv) == false)
    {
        emit errorMessage(tr("Component was not found"));
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
        emit errorMessage(tr("Component was not found"));
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
        emit errorMessage(tr("Component was not found"));
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
        emit errorMessage(tr("Component was not found"));
        return;
    }

    QStringList systemViews = component->getSystemViewNames();
    if (!systemViews.isEmpty())
    {
        emit openSystemDesign(vlnv, systemViews.first());
    }
}


//-----------------------------------------------------------------------------
// Function: LibraryHandler::onCreateDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onCreateDesign(VLNV const& vlnv)
{
    emit createDesignForExistingComponent(vlnv);
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
    fileWatch_.removePath(path);
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
    onRemoveVLNV(vlnvList);
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
// Function: LibraryHandler::onCreateNewItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onCreateNewItem(VLNV const& vlnv)
{
    //TODO: Move function.
   /* VLNV::IPXactType documentType = vlnv.getType();

    bool showAttributes = (documentType == VLNV::COMPONENT);
    NewObjectDialog newDesignDialog(this, vlnv.getType(), showAttributes, parentWidget_);

    if (contains(vlnv) && getDocumentType(vlnv) == VLNV::COMPONENT)
    {
        QSharedPointer<const Document> document = getModelReadOnly(vlnv);
        QSharedPointer<const Component> component = document.staticCast<const Component>();

        KactusAttribute::ProductHierarchy prodHier = component->getHierarchy();
        KactusAttribute::Firmness firmness = component->getFirmness();
        QVector<TagData> tags = component->getTags();

        newDesignDialog.setKactusAttributes(prodHier, firmness, tags);
    }

    newDesignDialog.setVLNV(vlnv);

    if (documentType == VLNV::ABSTRACTIONDEFINITION || documentType == VLNV::BUSDEFINITION)
    {
        newDesignDialog.setWindowTitle("New Bus");
    }
    else if (documentType == VLNV::COMPONENT)
    {
        newDesignDialog.setWindowTitle("New Component");
    }
    else if (documentType == VLNV::APIDEFINITION)
    {
        newDesignDialog.setWindowTitle(tr("New API Definition"));
    }
    else if (documentType == VLNV::COMDEFINITION)
    {
        newDesignDialog.setWindowTitle(tr("New COM Definition"));
    }

    newDesignDialog.exec();

    if (newDesignDialog.result() == QDialog::Rejected)
    {
        return;
    }

    VLNV newVlnv = newDesignDialog.getVLNV();
    QString directory = newDesignDialog.getPath();

    // create an object of correct type
    if (documentType == VLNV::ABSTRACTIONDEFINITION || documentType == VLNV::BUSDEFINITION)
    {
        emit createBus(newVlnv, directory);
    }
    else if (documentType == VLNV::COMPONENT)
    {
        emit createComponent(newDesignDialog.getProductHierarchy(), newDesignDialog.getFirmness(),
            newDesignDialog.getTags(), newVlnv, directory);
    }
    else if (documentType == VLNV::APIDEFINITION)
    {
        emit createApiDef(newVlnv, directory);
    }
    else if (documentType == VLNV::COMDEFINITION)
    {
        emit createComDef(newVlnv, directory);
    }
    else
    {
        emit noticeMessage(tr("The item type is not supported"));
    }*/
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onCreateAbsDef()
//-----------------------------------------------------------------------------
void LibraryHandler::onCreateAbsDef(VLNV const& busDefVLNV)
{
    if (busDefVLNV.isValid() == false)
    {
        return;
    }

    Q_ASSERT(getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION);
    Q_ASSERT(contains(busDefVLNV));

    QFileInfo busDefInfo(getPath(busDefVLNV));
    QString directory = busDefInfo.absolutePath();

    QList<VLNV> absDefVLNVs;
    hierarchyModel_->getChildren(absDefVLNVs, busDefVLNV);

    // if theres no previous abstraction definition for given bus def
    if (absDefVLNVs.isEmpty())
    {
        emit createAbsDef(busDefVLNV, directory, false);
    }
    else // the abstraction definition can not be identified by the bus definition.
    {
        emit createAbsDef(busDefVLNV, directory, true);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onRemoveVLNV()
//-----------------------------------------------------------------------------
void LibraryHandler::onRemoveVLNV(QList<VLNV> const& vlnvs)
{
    // create the dialog to select which items to remove
  /*  ObjectRemoveDialog* removeDialog = new ObjectRemoveDialog(parentWidget_);
    LibraryItemSelectionFactory::constructItemsForSelectionDialog(this, removeDialog, vlnvs);

    if (removeDialog->exec() == QDialog::Rejected)
    {
        return;
    }

    DocumentStatistics removeStatistics = removeSelectedObjects(removeDialog->getSelectedItems());

    emit noticeMessage(createDeleteMessage(removeStatistics));*/
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
// Function: LibraryHandler::closeIntegrityReport()
//-----------------------------------------------------------------------------
void LibraryHandler::onCloseIntegrityReport()
{
  /*  integrityWidget_->deleteLater();
    integrityWidget_ = 0;*/
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onFileChangedOnDisk()
//-----------------------------------------------------------------------------
void LibraryHandler::onFileChangedOnDisk(QString const& path)
{
    // Add timer to skip the time it takes to remove the file and rewrite it on save.
    updatedPaths_.append(path);
    QTimer::singleShot(100, Qt::CoarseTimer, this, SLOT(handleFileChange()));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::handleFileChange()
//-----------------------------------------------------------------------------
void LibraryHandler::handleFileChange()
{
    VLNV vlnv;
    QString path = updatedPaths_.takeFirst();
    
    if (QFile::exists(path))
    {
        // File updated on disk.
        QSharedPointer<Document> model = fileAccess_.readDocument(path);
        vlnv = model->getVlnv();
        documentCache_.insert(vlnv, DocumentInfo(path, model, validateDocument(model, path)));

        emit updatedVLNV(vlnv);
    }
    else
    {
        // File removed.
        auto changedDocument = std::find_if(documentCache_.begin(), documentCache_.end(),
            [path](const DocumentInfo& item) { return item.path.compare(path) == 0; });

        if (changedDocument != documentCache_.end() && changedDocument.key().isValid())
        {
            vlnv = changedDocument.key();
            documentCache_.erase(changedDocument);

            emit removeVLNV(vlnv);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::syncronizeModels()
//-----------------------------------------------------------------------------
void LibraryHandler::syncronizeModels()
{
    // connect the signals from the data model
    connect(this, SIGNAL(removeVLNV(const VLNV&)),
        treeModel_, SLOT(onRemoveVLNV(const VLNV&)), Qt::UniqueConnection);
    connect(this, SIGNAL(removeVLNV(const VLNV&)),
        hierarchyModel_, SLOT(onRemoveVLNV(const VLNV&)), Qt::UniqueConnection);

    connect(this, SIGNAL(addVLNV(const VLNV&)),
        treeModel_, SLOT(onAddVLNV(const VLNV&)), Qt::UniqueConnection);

    connect(this, SIGNAL(updatedVLNV(VLNV const&)),
        this, SLOT(onItemSaved(VLNV const&)), Qt::UniqueConnection);

    connect(this, SIGNAL(updatedVLNV(VLNV const&)),
            treeModel_, SLOT(onDocumentUpdated(VLNV const&)), Qt::UniqueConnection);
    connect(this, SIGNAL(updatedVLNV(VLNV const&)),
            hierarchyModel_, SLOT(onDocumentUpdated(VLNV const&)), Qt::UniqueConnection);

    connect(itemExporter_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

    //-----------------------------------------------------------------------------
    // connect the signals from the tree model
    //-----------------------------------------------------------------------------
    connect(treeModel_, SIGNAL(removeVLNV(const QList<VLNV>)),
        this, SLOT(onRemoveVLNV(const QList<VLNV>)), Qt::UniqueConnection);

    // signals from tree model to library handler
    connect(treeModel_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
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
    connect(treeModel_, SIGNAL(createBus(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(createAbsDef(const VLNV&)),
        this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(createComDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(createApiDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(createComponent(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(createDesign(const VLNV&)),
        this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(createSWDesign(const VLNV&)),
        this, SIGNAL(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(createSystemDesign(const VLNV&)),
        this, SIGNAL(createSystemDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(exportItems(const QList<VLNV>)),
        itemExporter_, SLOT(onExportItems(const QList<VLNV>)), Qt::UniqueConnection);
    connect(treeModel_, SIGNAL(refreshDialer()),
        this, SIGNAL(refreshDialer()), Qt::UniqueConnection);

    connect(treeModel_, SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

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

    connect(hierarchyModel_, SIGNAL(createBusDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(createComponent(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(createBus(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(createAbsDef(const VLNV&)),
        this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(createComDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(createApiDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(createDesign(const VLNV&)),
        this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(createSWDesign(const VLNV&)),
        this, SIGNAL(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_, SIGNAL(createSystemDesign(const VLNV&)),
        this, SIGNAL(createSystemDesign(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(exportItem(VLNV const&)),
        itemExporter_, SLOT(onExportItem(VLNV const&)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(removeVLNV(QList<VLNV>)),
        this, SLOT(onRemoveVLNV(QList<VLNV>)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);
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

    fileWatch_.removePath(targetPath);
    fileAccess_.writeDocument(model, targetPath);
    fileWatch_.addPath(targetPath);

    TagManager::getInstance().addNewTags(model->getTags());

    documentCache_.insert(model->getVlnv(), DocumentInfo(targetPath, model->clone(), validateDocument(model, targetPath)));

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::clearCache()
//-----------------------------------------------------------------------------
void LibraryHandler::clearCache()
{
    if (!documentCache_.isEmpty())
    {
        fileWatch_.removePaths(fileWatch_.files());
        documentCache_.clear();
    }
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
            fileWatch_.addPath(target.path);
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
    hierarchyModel_->onResetModel();
    treeModel_->onResetModel();
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
// Function: LibraryHandler::removeSelectedObjects()
//-----------------------------------------------------------------------------
LibraryHandler::DocumentStatistics LibraryHandler::removeSelectedObjects(
    QVector<ObjectSelectionListItem *> const& removedItems)
{
    DocumentStatistics removeStatistics;
    /*
    QStringList changedDirectories;
    for (ObjectSelectionListItem* removedItem : removedItems)
    {
        if (removedItem->getType() == ObjectSelectionListItem::VLNVOJBECT)
        {
            changedDirectories.append(QFileInfo(getPath(removedItem->getVLNV())).absolutePath());
            removeObject(removedItem->getVLNV());
            removeStatistics.documentCount++;
        }
        else if (removedItem->getType() == ObjectSelectionListItem::FILE)
        {
            QFileInfo fileInfo(removedItem->getPath());
            if (fileInfo.exists())
            {
                changedDirectories.append(fileInfo.absolutePath());
                if (removeFile(removedItem->getPath()))
                {
                    removeStatistics.fileCount++;
                }
            }
        }
    }

    changedDirectories.removeDuplicates();
    loader_.clean(changedDirectories);
    */
    return removeStatistics;
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
// Function: LibraryHandler::createDeleteMessage()
//-----------------------------------------------------------------------------
QString LibraryHandler::createDeleteMessage(DocumentStatistics const& statistics) const
{
    QString removeMessage = QString("Deleted %1 VLNV item(s)").arg(QString::number(statistics.documentCount));

    if (statistics.fileCount > 0)
    {
        removeMessage = removeMessage + QString(" and %1 file(s)").arg(QString::number(statistics.fileCount));
    }

    removeMessage.append('.');

    return removeMessage;
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
