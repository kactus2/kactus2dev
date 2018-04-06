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

#include "LibraryFilter.h"
#include "LibraryErrorModel.h"
#include "TableViewDialog.h"

#include <common/ui/MessageMediator.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/ObjectRemoveDialog/objectremovedialog.h>
#include <common/dialogs/ObjectExportDialog/ObjectExportDialog.h>
#include <common/dialogs/ObjectExportDialog/ObjectSelectionListItem.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Catalog/Catalog.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTabWidget>

//-----------------------------------------------------------------------------
// Function: LibraryHandler::LibraryHandler()
//-----------------------------------------------------------------------------
LibraryHandler::LibraryHandler(QWidget* parentWidget, MessageMediator* messageChannel, QObject* parent):
QObject(parent),
    parentWidget_(parentWidget),
    messageChannel_(messageChannel),
    fileAccess_(messageChannel),
    documentCache_(),
    urlTester_(Utils::URL_VALIDITY_REG_EXP, this),
    validator_(this),
    treeModel_(new LibraryTreeModel(this, this)),
    hierarchyModel_(new HierarchyModel(this, this)),
    integrityWidget_(0),
    saveInProgress_(false),
    fileWatch_(this),
    checkResults_()
{
    // create the connections between models and library handler
    syncronizeModels();
    connect(&fileWatch_, SIGNAL(fileChanged(QString const&)),
            this, SLOT(onFileChanged(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::~LibraryHandler()
//-----------------------------------------------------------------------------
LibraryHandler::~LibraryHandler()
{
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getModel()
//-----------------------------------------------------------------------------
QSharedPointer<Document> LibraryHandler::getModel(VLNV const& vlnv)
{
    // if object has already been previously parsed
    if (documentCache_.contains(vlnv))
    {
        auto info = documentCache_.find(vlnv);
        if (info->document.isNull())
        {
            info->document = fileAccess_.readDocument(info->path);
        }

        QSharedPointer<Document> copy = info->document->clone();
        return copy;
    }

    messageChannel_->showMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
    return QSharedPointer<Document>();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getModelReadOnly()
//-----------------------------------------------------------------------------
QSharedPointer<Document const> LibraryHandler::getModelReadOnly(VLNV const& vlnv)
{
    if (documentCache_.contains(vlnv))
    {
        auto info = documentCache_.find(vlnv);
        if (info->document.isNull())
        {
            info->document = fileAccess_.readDocument(info->path);
        }

        return info->document;
    }
    
    return QSharedPointer<Document const>();
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
bool LibraryHandler::contains(const VLNV& vlnv)
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
    if (path.isEmpty())
    {
        return false;
    }

    VLNV vlnv = model->getVlnv();
    Q_ASSERT(!contains(vlnv));
    
    // Create the path if it does not exist.
    if (QDir(path).exists() == false && QDir().mkpath(path) == false)
    {
        messageChannel_->showFailure(tr("Could not create directory \"%1\".").arg(path));        
        return false;
    }

    QString filePath = path + "/" + vlnv.getName() + "." + vlnv.getVersion() + ".xml";
    if (writeFile(model, filePath) == false)
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
    VLNV objectVLNV = model->getVlnv();
    QString path = getPath(objectVLNV);

    if (writeFile(model, path) == false)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::writeFile()
//-----------------------------------------------------------------------------
bool LibraryHandler::writeFile(QSharedPointer<Document> model, QString const& filePath)
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

    documentCache_.insert(model->getVlnv(), DocumentInfo(targetPath, model, validateDocument(model, targetPath)));

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::searchForIPXactFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::searchForIPXactFiles()
{
    // clear the previous items in the library
    if (!documentCache_.isEmpty())
    {
        fileWatch_.removePaths(fileWatch_.files());
        documentCache_.clear();
    }

    messageChannel_->showStatusMessage(tr("Scanning library. Please wait..."));
    
    QStringList locations = QSettings().value("Library/ActiveLocations", QStringList()).toStringList();

    // Read all items into objects_ befor validation.
    // Validation will check for VLNVs in the library, so they must be available before validation.
    parseLibrary(locations);

    onCheckLibraryIntegrity();
    
    messageChannel_->showStatusMessage(tr("Updating library view. Please wait..."));
    hierarchyModel_->onResetModel();
    treeModel_->onResetModel();
    messageChannel_->showStatusMessage(tr("Ready."));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::parseLibrary()
//-----------------------------------------------------------------------------
void LibraryHandler::parseLibrary(QStringList const& locations)
{
    for (QString const& location : locations)
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
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::parseDirectory()
//-----------------------------------------------------------------------------
void LibraryHandler::parseDirectory(QString const& directoryPath)
{
    QDir directoryHandler(directoryPath);
    directoryHandler.setNameFilters(QStringList(QLatin1String("*.xml")));

    // Get list of files and folders.
    for (QFileInfo const& entryInfo :
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
// Function: LibraryHandler::parseFile()
//-----------------------------------------------------------------------------
void LibraryHandler::parseFile(QString const& filePath)
{
    VLNV vlnv = fileAccess_.getDocumentVLNV(filePath);

    if (vlnv.isValid() == false)
    {
        return;
    }

    if (contains(vlnv))
    {
        messageChannel_->showMessage(tr("VLNV %1 was already found in the library").arg(vlnv.toString()));
        return;
    }

    // Add the component to the library if parsing was successful.
    documentCache_.insert(vlnv, DocumentInfo(filePath));
    fileWatch_.addPath(filePath);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getNeededVLNVs()
//-----------------------------------------------------------------------------
void LibraryHandler::getNeededVLNVs(VLNV const& vlnv, QList<VLNV>& list)
{
    if (!vlnv.isValid())
    {
        return;
    }

    QSharedPointer<const Document> document = getModelReadOnly(vlnv);
    if (document.isNull())
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

    // Search recursively for VLNVs.
    foreach (VLNV const& dependentVLNV, document->getDependentVLNVs())
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
    foreach (QString const& relativePath, document->getDependentFiles())
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
// Function: LibraryHandler::getHierarchicalDependencyFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::getHierarchicalDependencyFiles(VLNV const& vlnv, QStringList& list)
{
    // first get files of the top component
    getDependencyFiles(vlnv, list);

    // ask the dependencies of the object and call function recursively for children
    QSharedPointer<Document> document = getModel(vlnv);
    foreach (VLNV const& dependency, document->getDependentVLNVs())
    {
        getHierarchicalDependencyFiles(dependency, list);
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
    QMap<VLNV, DocumentInfo>::const_iterator it = documentCache_.constFind(vlnv);

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
// Function: LibraryHandler::showNotFoundError()
//-----------------------------------------------------------------------------
void LibraryHandler::showNotFoundError(VLNV const& vlnv)
{
    messageChannel_->showError(tr("VLNV: %1 was not found in library.").arg(vlnv.toString()));
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getDesignVLNV()
//-----------------------------------------------------------------------------
VLNV LibraryHandler::getDesignVLNV(VLNV const& hierarchyRef)
{
    if (contains(hierarchyRef) == false)
    {
        showNotFoundError(hierarchyRef);
        return VLNV();
    }

    if (getDocumentType(hierarchyRef) == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<const DesignConfiguration> desConf = 
            getModelReadOnly(hierarchyRef).staticCast<const DesignConfiguration>();

        VLNV designVLNV = desConf->getDesignRef();
        if (!contains(designVLNV) or getDocumentType(designVLNV) != VLNV::DESIGN)
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
    if (contains(hierarchyRef) == false)
    {
        showNotFoundError(hierarchyRef);
        return QSharedPointer<Design>();
    }

    VLNV designVlnv = getDesignVLNV(hierarchyRef);
    
    return getModel(designVlnv).staticCast<Design>();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::isValid()
//-----------------------------------------------------------------------------
bool LibraryHandler::isValid(VLNV const& vlnv)
{    
    return documentCache_.value(vlnv).isValid;
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

    checkResults_.invalidDocumentCount = 0;
    checkResults_.totalFileCount = 0;

    for (QMap<VLNV, DocumentInfo>::iterator it = documentCache_.begin(); it != documentCache_.end(); it++)
    {
        QSharedPointer<Document> model = fileAccess_.readDocument(it->path);

        bool isValid = validateDocument(model, it->path);
        it->isValid = isValid;
        if (isValid == false)
        {
            checkResults_.invalidDocumentCount++;
        }

        validateDependentFiles(model, it->path);

        if (it->document.isNull() == false)
        {
            it->document = model;
        }
    }

    messageChannel_->showMessage(tr("========== Library integrity check complete =========="));
    messageChannel_->showMessage(tr("Total library object count: %1").arg(documentCache_.size()));
    messageChannel_->showMessage(tr("Total file count in the library: %1").arg(checkResults_.totalFileCount));

    // if errors were found then print the summary of error types
    if (checkResults_.invalidDocumentCount > 0)
    {
        messageChannel_->showError(tr("Total items containing errors: %1").arg(checkResults_.invalidDocumentCount));
    }

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
            QString message(tr("Library does not contain bus definition %1 "
                "required by abstraction definition %2").arg(busDefVLNV.toString(), vlnv.toString()));

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
// Function: LibraryHandler::onExportItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onExportItem(VLNV const& vlnv)
{  
    onExportItems(QList<VLNV>() << vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onExportItems()
//-----------------------------------------------------------------------------
void LibraryHandler::onExportItems(const QList<VLNV> vlnvs)
{
    if (vlnvs.isEmpty())
    {
        return;
    }

    ObjectExportDialog* exportDialog = new ObjectExportDialog(parentWidget_);
    constructItemsForSelectionDialog(exportDialog, vlnvs);

    if (exportDialog->exec() == QDialog::Rejected)
    {
        return;
    }

    // get the current working directory and save it to be restored in the end of the function
    QDir savedWorkingDirectory = QDir::current();

    QString destinationPath = exportDialog->getTargetDirectory();

    QPair<int, int> exportCounter = exportSelectedObjects(exportDialog->getSelectedItems(), destinationPath);

    QString exportMessage = createExportMessage(exportCounter.first, exportCounter.second, destinationPath);
    emit noticeMessage(exportMessage);

    QDir::setCurrent(savedWorkingDirectory.absolutePath());
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onFileChanged()
//-----------------------------------------------------------------------------
void LibraryHandler::onFileChanged(QString const& path)
{
    auto changedDocument = std::find_if(documentCache_.begin(), documentCache_.end(),
                                        [path] (const DocumentInfo& s) { return s.path.compare(path) == 0; } );

    if (changedDocument != documentCache_.end())
    {
        emit updatedVLNV(changedDocument.key());
    }
    else
    {
        // TODO: File removed.
    }

    /*if (QFileInfo(path).exists())
    {

        //TODO: VLNV changedDocument = objects_.key(path, VLNV());
        //emit updatedVLNV(changedDocument);
    }*/
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::exportSelectedObjects()
//-----------------------------------------------------------------------------
QPair<int, int> LibraryHandler::exportSelectedObjects(QVector<ObjectSelectionListItem*> exportedItems,
    QString const& destinationPath)
{
    fileList handledFiles;
    QDir destinationFolder(destinationPath);

    bool yesToAll = false;
    bool noToAll = false;
    int vlnvCounter = 0;
    int fileCounter = 0;
    foreach (ObjectSelectionListItem* exportedItem, exportedItems)
    {
        if (noToAll)
        {
            break;
        }

        if (exportedItem->getType() == ObjectSelectionListItem::VLNVOJBECT)
        {
            if (exportSelectedVLNVObject(
                destinationFolder, exportedItem->getVLNV(), handledFiles, yesToAll, noToAll))
            {
                vlnvCounter++;
            }
        }
        else if (copyFile(QFileInfo(exportedItem->getPath()), destinationFolder, handledFiles, yesToAll, noToAll))
        {
            fileCounter++;
        }

    }

    return qMakePair(vlnvCounter, fileCounter);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::createExportMessage()
//-----------------------------------------------------------------------------
QString LibraryHandler::createExportMessage(int vlnvCount, int fileCount, QString const& destinationPath) const
{
    QString exportMessage = "Exported ";

    if (vlnvCount > 0 || fileCount > 0)
    {
        if (vlnvCount > 0)
        {
            exportMessage += QString("%1 VLNV item").arg(QString::number(vlnvCount));
            if (vlnvCount > 1)
            {
                exportMessage.append('s');
            }
        }
        if (fileCount > 0)
        {
            if (vlnvCount > 0)
            {
                exportMessage += QString(" and ");
            }

            exportMessage += QString("%1 file").arg(QString::number(fileCount));
            if (fileCount> 1)
            {
                exportMessage.append('s');
            }
        }
    }
    else
    {
        exportMessage += "0 items";
    }

    exportMessage += QString(" to %1.").arg(destinationPath);

    return exportMessage;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::constructItemsForExportDialog()
//-----------------------------------------------------------------------------
void LibraryHandler::constructItemsForSelectionDialog(ObjectSelectionDialog* exportDialog,
    const QList<VLNV> exportedVLNVs)
{
    foreach (VLNV const& exportVLNV, exportedVLNVs)
    {
        if (!exportVLNV.isValid())
        {
            continue;
        }

        // if the vlnv is not found in the library
        if (!contains(exportVLNV))
        {
            documentCache_.remove(exportVLNV);
            continue;
        }

        exportDialog->createItem(exportVLNV.toString(), exportVLNV, true);

        VLNV::IPXactType vlnvType = getDocumentType(exportVLNV);
        if (vlnvType == VLNV::COMPONENT)
        {
            QSharedPointer<const Component> componentToExport =
                getModelReadOnly(exportVLNV).staticCast<const Component>();

            foreach (VLNV const& ref, componentToExport->getHierRefs())
            {
                if (contains(ref))
                {
                    exportDialog->createItem(ref.toString(), ref);

                    if (getDocumentType(ref) == VLNV::DESIGNCONFIGURATION)
                    {
                        QSharedPointer<const DesignConfiguration> desConf =
                            getModelReadOnly(ref).staticCast<const DesignConfiguration>();

                        VLNV designVLNV = desConf->getDesignRef();
                        if (contains(designVLNV))
                        {
                            exportDialog->createItem(designVLNV.toString(), designVLNV);
                        }
                    }
                }
            }

            // ask the component for all it's file references.
            QStringList componentFiles;
            foreach (QSharedPointer<FileSet> fileset, *componentToExport->getFileSets())
            {
                componentFiles.append(fileset->getFileNames());
            }

            QString componentPath = getPath(exportVLNV);
            foreach (QString const& relativeFilePath, componentFiles)
            {
                QString absoluteFilePath = General::getAbsolutePath(componentPath, relativeFilePath);
                if (!absoluteFilePath.isEmpty())
                {
                    exportDialog->createItem(absoluteFilePath);
                }
            }
        }

        else if (vlnvType == VLNV::DESIGNCONFIGURATION)
        {
            QSharedPointer<const DesignConfiguration> desConf =
                getModelReadOnly(exportVLNV).staticCast<const DesignConfiguration>();

            VLNV designVLNV = desConf->getDesignRef();
            if (contains(designVLNV))
            {
                exportDialog->createItem(designVLNV.toString(), designVLNV);
            }
        }

        else if (vlnvType == VLNV::BUSDEFINITION)
        {
            QList<VLNV> absDefVLNVs;
            hierarchyModel_->getChildren(absDefVLNVs, exportVLNV);

            // If a bus definition is removed then ask to remove all it's abstraction definitions also.
            foreach (VLNV const& absDefVLNV, absDefVLNVs)
            {
                if (contains(absDefVLNV))
                {
                    exportDialog->createItem(absDefVLNV.toString(), absDefVLNV);
                }
            }
        }

        else if (vlnvType == VLNV::ABSTRACTIONDEFINITION)
        {
            QSharedPointer<const AbstractionDefinition> absDef =
                getModelReadOnly(exportVLNV).staticCast<const AbstractionDefinition>();

            VLNV busDefVLNV = absDef->getBusType();
            if (contains(busDefVLNV))
            {
                QList<VLNV> absDefVLNVs;
                hierarchyModel_->getChildren(absDefVLNVs, busDefVLNV);

                // if theres only this abs def for the bus def
                if (absDefVLNVs.size() == 1 && absDefVLNVs.first() == exportVLNV)
                {
                    exportDialog->createItem(busDefVLNV.toString(), busDefVLNV);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::exportSelectedVLNVObject()
//-----------------------------------------------------------------------------
bool LibraryHandler::exportSelectedVLNVObject(QDir const& destinationFolder, VLNV const& vlnv,
    fileList& handledFiles, bool& yesToAll, bool& noToAll)
{
    QSharedPointer<Document> document = getModel(vlnv);
    if (!document)
    {
        return false;
    }

    const QDir savedWorkingDir = QDir::current();

    QDir::setCurrent(destinationFolder.absolutePath());
    QDir vlnvTargetDirectory;

    QString directoryPath = vlnv.toString("/");
    if (!vlnvTargetDirectory.mkpath(directoryPath))
    {
        messageChannel_->showError(tr("Could not create directory structure, aborting."));
        QDir::setCurrent(savedWorkingDir.absolutePath());
        return false;
    }

    vlnvTargetDirectory.setPath(directoryPath);

    QString documentPath = getPath(vlnv);
    if (documentPath.isEmpty())
    {
        QDir::setCurrent(savedWorkingDir.absolutePath());
        return false;
    }
    QFileInfo documentFileInfo(documentPath);

    QDir sourceDirectory(documentFileInfo.absoluteDir());

    bool fileWasExported = copyFile(documentFileInfo, vlnvTargetDirectory, handledFiles, yesToAll, noToAll);

    QDir::setCurrent(savedWorkingDir.absolutePath());
    return fileWasExported;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onShowErrors()
//-----------------------------------------------------------------------------
void LibraryHandler::onShowErrors(VLNV const& vlnv)
{
    if (vlnv.isValid() == false)
    {
        return;
    }

    QSharedPointer<Document> document = getModel(vlnv);
    if (document != 0)
    {
        // Show error list in a dialog.
        TableViewDialog* dialog = new TableViewDialog(parentWidget_);
        dialog->setWindowTitle(tr("Errors in %1").arg(vlnv.toString()));
        dialog->setDescription(tr("<b>Integrity check</b><br>The following errors were found."));
        dialog->resize(700, 350);

        LibraryErrorModel* model = new LibraryErrorModel(dialog);
        model->addErrors(findErrorsInDocument(document, getPath(vlnv)), vlnv.toString());

        dialog->show();

        dialog->setModel(model);               

        connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onGenerateIntegrityReport()
//-----------------------------------------------------------------------------
void LibraryHandler::onGenerateIntegrityReport()
{
    if (!integrityWidget_)
    {
        integrityWidget_ = new TableViewDialog(parentWidget_);
        integrityWidget_->setWindowTitle(tr("Library Integrity Report"));
        integrityWidget_->setDescription(tr("<b>Integrity check</b><br>The following errors were found."));
        integrityWidget_->resize(1000, 800);

        LibraryErrorModel* model = new LibraryErrorModel(integrityWidget_);

        
        QMap<VLNV, DocumentInfo>::const_iterator end = documentCache_.constEnd();
        
        for (QMap<VLNV, DocumentInfo>::const_iterator it = documentCache_.constBegin();
            it != end; ++it)
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

    integrityWidget_->raise();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onSelectionChanged()
//-----------------------------------------------------------------------------
void LibraryHandler::onSelectionChanged(VLNV const& /*vlnv*/)
{
    
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onClearSelection()
//-----------------------------------------------------------------------------
void LibraryHandler::onClearSelection()
{

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
    //TODO: view selection.
    QSharedPointer<const Component> component = getModelReadOnly(vlnv).dynamicCast<const Component>();
    if (component.isNull())
    {
        emit errorMessage(tr("Component was not found"));
        return;
    }

    foreach(QSharedPointer<View> view, *component->getViews())
    {
        VLNV reference = component->getModel()->getHierRef(view->name());

        QSharedPointer<const Document> docu = getModelReadOnly(reference);

        if (docu && docu->getImplementation() == KactusAttribute::SW)
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
// Function: LibraryHandler::onCreateNewItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onCreateNewItem(VLNV const& vlnv)
{
    VLNV::IPXactType documentType = vlnv.getType();

    bool showAttributes = (documentType == VLNV::COMPONENT);
    NewObjectDialog newDesignDialog(this, vlnv.getType(), showAttributes, parentWidget_);

    if (contains(vlnv) && getDocumentType(vlnv) == VLNV::COMPONENT)
    {
        QSharedPointer<const Document> document = getModelReadOnly(vlnv);
        QSharedPointer<const Component> component = document.staticCast<const Component>();

        KactusAttribute::ProductHierarchy prodHier = component->getHierarchy();
        KactusAttribute::Firmness firmness = component->getFirmness();

        newDesignDialog.setKactusAttributes(prodHier, firmness);
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
        emit createComponent(newDesignDialog.getProductHierarchy(), newDesignDialog.getFirmness(), newVlnv, 
            directory);
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

    fileWatch_.removePath(documentCache_.find(vlnv)->path);
    documentCache_.remove(vlnv);

    // tell each model to remove the object
    treeModel_->onRemoveVLNV(vlnv);
    hierarchyModel_->onRemoveVLNV(vlnv);

    //emit removeVLNV(vlnv);
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
// Function: LibraryHandler::onCreateAbsDef()
//-----------------------------------------------------------------------------
void LibraryHandler::onCreateAbsDef(VLNV const& busDefVLNV)
{
    if (!busDefVLNV.isValid())
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
void LibraryHandler::onRemoveVLNV(QList<VLNV> const vlnvs)
{
    // create the dialog to select which items to remove
    ObjectRemoveDialog* removeDialog = new ObjectRemoveDialog(parentWidget_);
    constructItemsForSelectionDialog(removeDialog, vlnvs);

    if (removeDialog->exec() == QDialog::Rejected)
    {
        return;
    }

    int vlnvCounter = 0;
    int fileCounter = 0;

    QStringList changedDirectories;
    foreach (ObjectSelectionListItem* removedItem, removeDialog->getSelectedItems())
    {
        if (removedItem->getType() == ObjectSelectionListItem::VLNVOJBECT)
        {
            changedDirectories.append(QFileInfo(getPath(removedItem->getVLNV())).absolutePath());
            removeObject(removedItem->getVLNV());
            vlnvCounter++;
        }
        else if (removedItem->getType() == ObjectSelectionListItem::FILE)
        {
            QFileInfo fileInfo(removedItem->getPath());
            if (fileInfo.exists())
            {
                changedDirectories.append(fileInfo.absolutePath());
                
                QFile file(removedItem->getPath());
                if (!file.remove())
                {
                    messageChannel_->showError(tr("File %1 could not be removed from the file system.").arg(
                        removedItem->getPath()));
                }
                else
                {
                    fileCounter++;
                }
            }
        }
    }

    QStringList libraryLocations = QSettings().value("Library/Locations", QStringList()).toStringList();

    changedDirectories.removeDuplicates();
    foreach (QString const& changedDirectory, changedDirectories)
    {
        clearDirectoryStructure(changedDirectory, libraryLocations);
    }

    QString removeMessage = createDeleteMessage(vlnvCounter, fileCounter);
    emit noticeMessage(removeMessage);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::createDeleteMessage()
//-----------------------------------------------------------------------------
QString LibraryHandler::createDeleteMessage(int vlnvCount, int fileCount) const
{
    QString removeMessage = QString("Deleted %1 VLNV item").arg(QString::number(vlnvCount));
    if (vlnvCount > 1)
    {
        removeMessage.append('s');
    }
    if (fileCount > 0)
    {
        removeMessage = removeMessage + QString(" and %1 file").arg(QString::number(fileCount));
        if (fileCount> 1)
        {
            removeMessage.append('s');
        }
    }
    removeMessage.append('.');

    return removeMessage;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onItemSaved()
//-----------------------------------------------------------------------------
void LibraryHandler::onItemSaved(VLNV const& vlnv) 
{
    if (!vlnv.isValid())
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
    integrityWidget_->deleteLater();
    integrityWidget_ = 0;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::validateDocument()
//-----------------------------------------------------------------------------
bool LibraryHandler::validateDocument(QSharedPointer<Document> document, QString const& documentPath)
{
    if (document.isNull())
    {
        return false;
    }

    Q_ASSERT(!documentPath.isEmpty());

    if (!QFileInfo(documentPath).exists())
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
    Q_ASSERT(document);

    QVector<QString> errorList;

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
// Function: LibraryHandler::validateDependentVLNVReferencences()
//-----------------------------------------------------------------------------
bool LibraryHandler::validateDependentVLNVReferencences(QSharedPointer<Document> document)
{
    foreach (VLNV const& vlnv, document->getDependentVLNVs())
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
    foreach (VLNV const& vlnv, document->getDependentVLNVs())
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
    foreach (QString const& directory, document->getDependentDirs())
    {
        QFileInfo dirInfo(General::getAbsolutePath(documentPath, directory));
        if (dirInfo.exists() == false)
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
    foreach (QString const& directory, document->getDependentDirs())
    {
        QString dirPath = General::getAbsolutePath(documentPath, directory);

        if (QFileInfo(dirPath).exists() == false)
        {
            errorList.append(tr("\tDirectory %1 was not found in the file system.").arg(dirPath));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::validateDependentFiles()
//-----------------------------------------------------------------------------
bool LibraryHandler::validateDependentFiles(QSharedPointer<Document> document, QString const& documentPath)
{
    foreach (QString filePath, document->getDependentFiles())
    {
        int pos = 0;
        if (urlTester_.validate(filePath, pos) != QValidator::Acceptable)
        {
            QString absolutePath = filePath;
            QFileInfo pathInfo(filePath);
            if (QFileInfo(filePath).isRelative())
            {
                absolutePath = General::getAbsolutePath(documentPath, filePath);
            }

            if (QFileInfo(absolutePath).exists() == false)
            {
                return false;
            }
            else
            {
                checkResults_.totalFileCount++;
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
    foreach (QString filePath, document->getDependentFiles())
    {
        // Check if the path is actually a URL to (external) location.
        int pos = 0;
        if (urlTester_.validate(filePath, pos) == QValidator::Acceptable)
        {
            checkResults_.totalFileCount++;
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

//-----------------------------------------------------------------------------
// Function: LibraryHandler::copyFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::copyFiles(QDir const& target, const VLNV& vlnv, fileList& handledFiles, bool& yesToAll, 
    bool& noToAll)
{
    // Parse the vlnv to get the dependencies and also copy them.
    QSharedPointer<Document> document = getModel(vlnv);
    if (!document)
    {    
        return;
    }

    // save the current working directory to be restored later
    QDir savedWorkingDir = QDir::current();

    QDir::setCurrent(target.absolutePath());
    QDir vlnvTargetDirectory;

    QString directoryPath = vlnv.toString("/");
    if (!vlnvTargetDirectory.mkpath(directoryPath))
    {
        messageChannel_->showError(tr("Could not create directory structure, aborting."));

        // restore the previous working directory to avoid messing with caller function
        QDir::setCurrent(savedWorkingDir.absolutePath());
        return;
    }

    vlnvTargetDirectory.setPath(directoryPath);

    QString documentPath = getPath(vlnv);
    if (documentPath.isEmpty())
    {
        // restore the previous working directory to avoid messing with caller function
        QDir::setCurrent(savedWorkingDir.absolutePath());
        return;
    }
    QFileInfo documentFileInfo(documentPath);

    // The source directory where all relative paths must be compared to.
    QDir sourceDirectory(documentFileInfo.absoluteDir());

    copyFile(documentFileInfo, vlnvTargetDirectory, handledFiles, yesToAll, noToAll);

    copyFiles(document->getDependentFiles(), sourceDirectory, vlnvTargetDirectory, handledFiles, yesToAll, noToAll);

    foreach (VLNV const& dependentVLNV, document->getDependentVLNVs()) 
    {
        copyFiles(target, dependentVLNV, handledFiles, yesToAll, noToAll);
    }

    // restore the previous working directory to avoid messing with caller function
    QDir::setCurrent(savedWorkingDir.absolutePath());
    return;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::copyFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::copyFiles(QStringList const& files, QDir& sourceDir, QDir& targetDir, fileList& handledFiles,
    bool& yesToAll, bool& noToAll)
{
    // save the current directory to be restored later
    QDir savedCurrentDir = QDir::current();

    QDir::setCurrent(targetDir.absolutePath());

    foreach (QString const& file, files)
    {
        QFileInfo sourceInfo(sourceDir.absoluteFilePath(file));
        if (sourceInfo.exists())
        {
            QFileInfo targetInfo(file);

            // create directories to store the file in a correct relative path
            if (!QDir::current().mkpath(targetInfo.absolutePath()))
            {
                messageChannel_->showError(tr("Could not create directory structure for file %1").arg(
                    targetInfo.fileName()));
            }

            // set the target directory to copy the file into
            QDir targetDir(targetInfo.absolutePath());
            copyFile(sourceInfo, targetDir, handledFiles, yesToAll, noToAll);
        }
    }

    // set the current directory to the state it was before this function
    QDir::setCurrent(savedCurrentDir.absolutePath());
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::copyFile()
//-----------------------------------------------------------------------------
bool LibraryHandler::copyFile(QFileInfo const& source, QDir& target, fileList& handledFiles, bool& yesToAll,
    bool& noToAll)
{
    if (handledFiles.contains(source))
    {
        return false;
    }

    if (!source.exists())
    {
        messageChannel_->showError(tr("Could not find file: %1").arg(source.fileName()));
        return false;
    }

    // save the current directory to be restored at the end of the function
    QDir savedCurrentDir = QDir::current();

    QDir::setCurrent(target.absolutePath());

    // Add file to handledFiles list to indicate that we don't need to check later in possible recursive calls.
    handledFiles.append(source);

    if (QFile::exists(source.fileName()))
    {
        QMessageBox::StandardButton answer = QMessageBox::Yes;

        if (noToAll)
        {
            answer = QMessageBox::No;
        }

        // if "yes to all" or "no to all" has not been clicked then ask user what to do
        else if (!yesToAll && !noToAll)
        {
            QString title = tr("overwrite file?");
            QString text = tr("The file ") + source.fileName() + tr(" already exists, would you like "
                "to overwrite the file?");

            // ask the user to overwrite the file, if user says no we quit the function and return false
            answer = QMessageBox::question(parentWidget_, title, text, 
                QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll,
                QMessageBox::No);
        }

        if (answer == QMessageBox::No)
        {
            // restore the current directory to the state it was before this function
            QDir::setCurrent(savedCurrentDir.absolutePath());
            return false;
        }

        else if (answer == QMessageBox::YesToAll)
        {
            yesToAll = true;
            noToAll = false;
        }

        else if (answer == QMessageBox::NoToAll)
        {
            yesToAll = false;
            noToAll = true;

            // restore the current directory to the state it was before this function
            QDir::setCurrent(savedCurrentDir.absolutePath());
            return false;
        }

        if (answer == QMessageBox::Yes || answer == QMessageBox::YesToAll)
        {
            QFile fileToOverwrite(source.fileName());
            fileToOverwrite.remove();
        }
    }

    QFile sourceFile(source.filePath());

    if (!sourceFile.copy(source.fileName()))
    {
        messageChannel_->showError(tr("File %1 couldn't be copied").arg(source.fileName()));
    }

    // restore the current directory to the state it was before this function
    QDir::setCurrent(savedCurrentDir.absolutePath());

    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::syncronizeModels()
//-----------------------------------------------------------------------------
void LibraryHandler::syncronizeModels()
{
    //TODO: SIGNALS
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

    connect(this, SIGNAL(updatedVLNV(VLNV const&)),
        this, SIGNAL(updatedVLNV(VLNV const&)), Qt::UniqueConnection);

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
        this, SLOT(onExportItems(const QList<VLNV>)), Qt::UniqueConnection);
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
        this, SLOT(onExportItem(VLNV const& )), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(removeVLNV(QList<VLNV>)),
        this, SLOT(onRemoveVLNV(QList<VLNV>)), Qt::UniqueConnection);

    connect(hierarchyModel_, SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::clearDirectoryStructure()
//-----------------------------------------------------------------------------
void LibraryHandler::clearDirectoryStructure(QString const& dirPath, QStringList const& libraryLocations)
{
    QDir dir(dirPath);
    
    while (containsPath(QDir::cleanPath(dir.absolutePath()), libraryLocations))
    {    
        QString directoryName = dir.dirName();

        // if not possible to move up anymore (the dir could possibly have been destroyed already).
        if (!dir.cdUp())
        {
            return;
        }

        // if the directory is not empty then it can't be removed and we can stop.
        if (!dir.rmdir(directoryName))
        {
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::containsPath()
//-----------------------------------------------------------------------------
bool LibraryHandler::containsPath(QString const& path, QStringList const& pathsToSearch) const
{
    foreach (QString const& searchPath, pathsToSearch)
    {
        // As long as the path is not the same as search path but still contains the search path,
        // it is a parent directory of the path.
        if (path.contains(searchPath) && path.compare(searchPath) != 0)
        {
            return true;
        }
    }

    // None of the paths to search were contained in the path.
    return false;
}
