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

#include "libraryhandler.h"

#include "LibraryFilter.h"
#include "LibraryErrorModel.h"

#include <library/LibraryManager/VLNVDialer/vlnvdialer.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/ObjectRemoveDialog/objectremovedialog.h>
#include <common/dialogs/ObjectRemoveDialog/objectremovemodel.h>
#include <common/dialogs/TableViewDialog/TableViewDialog.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinitionWriter.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinitionWriter.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/DesignWriter.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/designConfiguration/DesignConfigurationWriter.h>

#include <IPXACTmodels/kactusExtensions/ApiDefinitionWriter.h>
#include <IPXACTmodels/kactusExtensions/ComDefinitionWriter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ComponentWriter.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QApplication>
#include <QCoreApplication>
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
LibraryHandler::LibraryHandler(VLNVDialer* dialer, QWidget* parent): 
QTabWidget(parent), 
    data_(new LibraryData(this, parent)), 
    treeModel_(new LibraryTreeModel(this, data_.data(), this)),
    hierarchyModel_(new HierarchyModel(data_.data(), this, this)),
    treeWidget_(0),
    hierarchyWidget_(0),
    objects_(),
    objectValidity_(),
    saveInProgress_(false),
    itemsToAdd_(),
    modifiedItems_()
{
	setWindowTitle(tr("LibraryHandler"));

	// create the connections between models and library handler
	syncronizeModels();

	treeWidget_ = new LibraryTreeWidget(this, treeModel_.data(), this);
    connectLibraryFilter(treeWidget_->getFilter(), dialer);

	connect(treeWidget_, SIGNAL(itemSelected(const VLNV&)),
        this, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

	hierarchyWidget_ = new HierarchyWidget(this, hierarchyModel_.data(), this);
    connectLibraryFilter(hierarchyWidget_->getFilter(), dialer);

	connect(hierarchyWidget_, SIGNAL(componentSelected(const VLNV&)),
		this, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

    addTab(treeWidget_, tr("VLNV Tree"));
	addTab(hierarchyWidget_, tr("Hierarchy"));
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
    if (objects_.contains(vlnv))
    {
        QSharedPointer<Document> copy = objects_.value(vlnv)->clone();
        return copy;
    }
    else
    {
        QSharedPointer<Document> document = data_->getModel(vlnv);
        if (document)
        {
            // save the parsed item to the map and return pointer to it
            objects_.insert(vlnv, document);
            QSharedPointer<Document> copy = document->clone();
            return copy;
        }

        // if item was not found
        return document;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getModelReadOnly()
//-----------------------------------------------------------------------------
QSharedPointer<Document const> LibraryHandler::getModelReadOnly(VLNV const& vlnv)
{
    // if object has already been previously parsed
    if (objects_.contains(vlnv))
    {
        return objects_.value(vlnv);
    }
    else
    { 
        QSharedPointer<Document> libComp = data_->getModel(vlnv);
        if (libComp)
        {
            // save the parsed item to the map and return pointer to it
            objects_.insert(vlnv, libComp);
        }

        return libComp;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::contains()
//-----------------------------------------------------------------------------
bool LibraryHandler::contains(const VLNV& vlnv)
{
    if (objects_.contains(vlnv))
    {
        return true;
    }

    return data_->contains(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getPath()
//-----------------------------------------------------------------------------
const QString LibraryHandler::getPath(VLNV const& vlnv) const
{
    if (!vlnv.isValid())
    {
        return QString();
    }
    else
    {
        return data_->getPath(vlnv);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::getDirectoryPath()
//-----------------------------------------------------------------------------
QString LibraryHandler::getDirectoryPath(VLNV const& vlnv) const
{
    QString path = getPath(vlnv);

    QFileInfo pathInfo(path);
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
	Q_ASSERT(!data_->contains(vlnv));
	Q_ASSERT(!objects_.contains(vlnv));
    objectValidity_.remove(vlnv);

    // Create the path if it does not exist.
    if (!QDir(path).exists() && !QDir().mkpath(path))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("Could not create directory \"%1\".").arg(path));
        msgBox.exec();

        return false;
    }

	QString filePath = path + "/" + vlnv.getName() + "." + vlnv.getVersion() + ".xml";
    if (!writeFile(filePath, model))
    {
        return false;
    }

	if (!saveInProgress_)
    {
		// tell library to register the vlnv
		data_->addVLNV(vlnv, filePath);

		// the hierarchy model must be re-built
        treeModel_->onDocumentSaved(vlnv);
		hierarchyModel_->onResetModel();
	}
	else
    {
		itemsToAdd_.insert(vlnv, filePath);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::writeModelToFile()
//-----------------------------------------------------------------------------
bool LibraryHandler::writeModelToFile(QSharedPointer<Document> model)
{
	Q_ASSERT(data_->contains(model->getVlnv()));

	// Make sure the object is parsed again next time.
	VLNV objectVLNV = model->getVlnv();
	objects_.remove(objectVLNV);
    objectValidity_.remove(objectVLNV);
	QString filePath = data_->getPath(model->getVlnv());
    if (!writeFile(filePath, model))
    {
        return false;
    }

	if (!saveInProgress_)
    {
		onItemSaved(objectVLNV);
	}
    else
    {
        modifiedItems_.append(objectVLNV);
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::searchForIPXactFiles()
//-----------------------------------------------------------------------------
void LibraryHandler::searchForIPXactFiles()
{
    objects_.clear();
    objectValidity_.clear();
    data_->parseLibrary();
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

    // get the vlnvs needed by this vlnv
    QSharedPointer<const Document> document = getModelReadOnly(vlnv);

    // if there was no match for the vlnv
    if (!document)
    {
        emit errorMessage(tr("No item with following info was found in library: \n"
            "Vendor: %1\n"
            "Library: %2\n"
            "Name: %3\n"
            "Version: %4").arg(vlnv.getVendor(), vlnv.getLibrary(), vlnv.getName(), vlnv.getVersion()));
        return;
    }

    // this document is searched so add it to the list
    list.append(vlnv);

    foreach (VLNV const& dependentVLNV, document->getDependentVLNVs())
    {
        if (!list.contains(dependentVLNV))
        {
            // search recursively for vlnvs
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
    QFileInfo documentFile(data_->getPath(vlnv));

    if (!documentFile.exists())
    {
        emit errorMessage(tr("File %1 can't be found in file system").arg(documentFile.filePath()));
        return;
    }

    QDir documentDirectory(documentFile.absolutePath());

    // Convert all relative file path to an absolute to make the file accessible in the caller function.
    foreach (QString const& relativePath, document->getDependentFiles())
    {
        QString absolutePath = documentDirectory.absoluteFilePath(relativePath);

        QFileInfo dependentFile(absolutePath);
        if (dependentFile.exists() && !list.contains(dependentFile.canonicalFilePath()))
        {
            list.append(dependentFile.canonicalFilePath());
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

    // create the model of the vlnv
    QSharedPointer<Document> document = getModel(vlnv);

    // ask the dependencies of the object and call function recursively for children
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
    if (!vlnv.isValid())
    {
        return VLNV::INVALID;
    }

    return data_->getType(vlnv);
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
    if (!vlnvToSearch.isValid())
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
    if (!data_->contains(hierarchyRef))
    {
        emit errorMessage(tr("VLNV: %1 was not found in library.").arg(hierarchyRef.toString()));
        return VLNV();
    }

    if (data_->getType(hierarchyRef) == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<const DesignConfiguration> desConf = 
            getModelReadOnly(hierarchyRef).staticCast<const DesignConfiguration>();

        VLNV designVLNV = desConf->getDesignRef();
        if (!data_->contains(designVLNV)) 
        {
            QString errorMsg(tr("VLNV: %1 was not found in library.").arg(designVLNV.toString()));
            emit errorMessage(errorMsg);
            return VLNV();
        }

        else if (data_->getType(designVLNV) != VLNV::DESIGN)
        {
            QString errorMsg(tr("VLNV: %1 was not for design.").arg(designVLNV.toString()));
            emit errorMessage(errorMsg);
            return VLNV();
        }

        designVLNV.setType(VLNV::DESIGN);
        return designVLNV;
    }

    else if (data_->getType(hierarchyRef) == VLNV::DESIGN)
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
    if (!data_->contains(hierarchyRef))
    {
        emit errorMessage(tr("VLNV: %1 was not found in library.").arg(hierarchyRef.toString()));
        return QSharedPointer<Design>();
    }

    VLNV designVlnv = getDesignVLNV(hierarchyRef);
    if (designVlnv.isValid())
    {
        return getModel(designVlnv).staticCast<Design>();
    }
    else
    {
        return QSharedPointer<Design>();
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::isValid()
//-----------------------------------------------------------------------------
bool LibraryHandler::isValid(VLNV const& vlnv)
{	
    if (objectValidity_.contains(vlnv))
    {
        return objectValidity_.value(vlnv);
    }

    
    QSharedPointer<Document> document;

    // if object has already been parsed before
    if (objects_.contains(vlnv))
    {
        document = objects_.value(vlnv);
    }
    else 
    {
        document = getModel(vlnv);
        if (!document)
        {
            return false;
        }
    }

    bool valid = data_->validateDocument(document);
    objectValidity_.insert(vlnv, valid);
    return valid;
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onCheckLibraryIntegrity()
//-----------------------------------------------------------------------------
void LibraryHandler::onCheckLibraryIntegrity()
{
    saveInProgress_ = false;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    data_->checkLibraryIntegrity();
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onEditItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onEditItem(VLNV const& vlnv)
{
    if (!vlnv.isValid())
    {
        return;
    }

    VLNV::IPXactType documentType = data_->getType(vlnv);
    if (documentType == VLNV::COMPONENT)
    {
        emit openComponent(vlnv);
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

        if (!data_->contains(busDefVLNV))
        {
            QString message(tr("Library does not contain bus definition %1 "
                "required by abstraction definition %2").arg(busDefVLNV.toString(), vlnv.toString()));

            emit errorMessage(message);
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
        emit noticeMessage(tr("Item can not be edited."));
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onExportItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onExportItem(VLNV const vlnv)
{
    if (!vlnv.isValid())
    {
        return;
    }

    // get the current working directory and save it to be restored in the end of the function
    QDir savedWorkingDirectory = QDir::current();

    QSettings settings;
    QString defaultPath = settings.value("Library/DefaultLocation", QDir::homePath()).toString();

    // ask the target directory where the package is to be exported
    QString targetPath = QFileDialog::getExistingDirectory(this, tr("Select the location to export library to"),
        defaultPath);

    if (targetPath.isEmpty())
    {
        return;
    }

    QDir target(targetPath);

    // The user hasn't chosen "yes to all" or "no to all" yet
    bool yesToAll = false;
    bool noToAll = false;

    // info of copied files is stored so same file is not copied multiple times
    fileList handledFiles;
    copyFiles(target, vlnv, handledFiles, yesToAll, noToAll);

    QDir::setCurrent(savedWorkingDirectory.absolutePath());
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onExportItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onExportItems(const QList<VLNV> vlnvs )
{
    if (vlnvs.isEmpty())
    {
        return;
    }

    // get the current working directory and save it to be restored in the end of the function
    QDir savedWorkingDirectory = QDir::current();

    QSettings settings;
    QString defaultPath = settings.value("Library/DefaultLocation", QDir::homePath()).toString();

    // ask the target directory where the package is to be exported
    QString targetPath = QFileDialog::getExistingDirectory(this, tr("Select the location to export library to"),
        defaultPath);

    // if no target path was specified
    if (targetPath.isEmpty())
    {
        return;
    }

    // create a QDir instance of the target directory
    QDir target(targetPath);

    // The user hasn't chosen "yes to all" or "no to all" yet
    bool yesToAll = false;
    bool noToAll = false;

    // info of copied files is stored so same file is not copied multiple times
    fileList handledFiles;

    foreach (VLNV const& vlnv, vlnvs)
    {
        // invalid vlnvs are not exported
        if (vlnv.isValid())
        {
            copyFiles(target, vlnv, handledFiles, yesToAll, noToAll);
        }
    }

    QDir::setCurrent(savedWorkingDirectory.absolutePath());
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onShowErrors()
//-----------------------------------------------------------------------------
void LibraryHandler::onShowErrors(VLNV const& vlnv)
{
    if (!vlnv.isValid())
    {
        return;
    }

    // Retrieve the model.
    QSharedPointer<Document> document = getModel(vlnv);

    if (document != 0)
    {
        LibraryErrorModel* model = new LibraryErrorModel(this);
        model->addErrors(data_->findErrorsInDocument(document, data_->getPath(vlnv)));

        // Show error list in a dialog.
        TableViewDialog* dialog = new TableViewDialog(this);
        dialog->setWindowTitle(tr("Errors in %1").arg(vlnv.toString()));
		dialog->setDescription(tr("The following errors were found:"));
		dialog->resize(700, 350);

		dialog->show();

        dialog->setModel(model);
        
        connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onSelectionChanged()
//-----------------------------------------------------------------------------
void LibraryHandler::onSelectionChanged(VLNV const& vlnv)
{
    // if vlnv was invalid
    if (!vlnv.isValid())
    {
        return;
    }

    treeWidget_->selectItem(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onClearSelection()
//-----------------------------------------------------------------------------
void LibraryHandler::onClearSelection()
{
    // tell tree widget to select an invalid vlnv
    treeWidget_->selectItem(VLNV());
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenDesign(VLNV const& vlnv)
{
    QSharedPointer<const Document> document = getModelReadOnly(vlnv);
    if (!document)
    {
        emit errorMessage(tr("Component was not found"));
        return;
    }

    if (document->getVlnv().getType() == VLNV::COMPONENT)
    {
        QSharedPointer<const Component> component = document.staticCast<const Component>();

        QStringList views = component->getHierViews();
        if (!views.isEmpty())
        {
            emit openDesign(vlnv, views.first());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenMemoryDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenMemoryDesign(VLNV const& vlnv)
{
    QSharedPointer<const Document> document = getModelReadOnly(vlnv);
    if (!document)
    {
        emit errorMessage(tr("Component was not found"));
        return;
    }

    if (document->getVlnv().getType() == VLNV::COMPONENT)
    {
        QSharedPointer<const Component> component = document.staticCast<const Component>();

        QStringList views = component->getHierViews();
        if (!views.isEmpty())
        {
            emit openMemoryDesign(vlnv, views.first());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenSWDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenSWDesign(VLNV const& vlnv)
{
    QSharedPointer<const Document> document = getModelReadOnly(vlnv);
    if (!document)
    {
        emit errorMessage(tr("Component was not found"));
        return;
    }

    if (document->getVlnv().getType() == VLNV::COMPONENT)
    {
        QSharedPointer<const Component> component = document.staticCast<const Component>();

        QStringList swViews = component->getSWViewNames();
        if (!swViews.isEmpty())
        {
            emit openSWDesign(vlnv, swViews.first());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onOpenSystemDesign()
//-----------------------------------------------------------------------------
void LibraryHandler::onOpenSystemDesign(VLNV const& vlnv)
{
    QSharedPointer<const Document> document = getModelReadOnly(vlnv);
    if (!document)
    {
        emit errorMessage(tr("Component was not found"));
        return;
    }

    if (document->getVlnv().getType() == VLNV::COMPONENT)
    {
        QSharedPointer<const Component> component = document.staticCast<const Component>();

        QStringList systemViews = component->getSystemViewNames();
        if (!systemViews.isEmpty())
        {
            emit openSystemDesign(vlnv, systemViews.first());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::onCreateNewItem()
//-----------------------------------------------------------------------------
void LibraryHandler::onCreateNewItem(VLNV const& vlnv)
{
    VLNV::IPXactType documentType = vlnv.getType();

    bool showAttributes = (documentType == VLNV::COMPONENT);
    NewObjectDialog newDesignDialog(this, vlnv.getType(), showAttributes, this);

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
    if (!vlnv.isValid() || !data_->contains(vlnv))
    {
        return;
    }

    objects_.remove(vlnv);
    objectValidity_.remove(vlnv);

    QString documentPath = data_->getPath(vlnv);

    // tell each model to remove the object
    treeModel_->onRemoveVLNV(vlnv);
    hierarchyModel_->onRemoveVLNV(vlnv);
    data_->onRemoveVLNV(vlnv);

    QFile xmlFile(documentPath);
    if (!xmlFile.exists())
    {
        emit errorMessage(tr("File %1 was not found in file system").arg(documentPath));
    }
    else if (!xmlFile.remove())
    {
        emit errorMessage(tr("Could not remove file %1").arg(documentPath));
    }
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

    if (!itemsToAdd_.isEmpty())
    {
        for (QMap<VLNV, QString>::iterator i = itemsToAdd_.begin(); i != itemsToAdd_.end(); ++i)
        {
            data_->addVLNV(i.key(), i.value());

        }

        itemsToAdd_.clear();
    }

    if (!modifiedItems_.isEmpty())
    {
        foreach (VLNV const& modifiedVLNV, modifiedItems_)
        {
            hierarchyModel_->onDocumentSaved(modifiedVLNV);
            treeModel_->onDocumentSaved(modifiedVLNV);
            objectValidity_.remove(modifiedVLNV);
        }

        modifiedItems_.clear();
    }
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
    Q_ASSERT(data_->contains(busDefVLNV));

    QFileInfo busDefInfo(data_->getPath(busDefVLNV));
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
    ObjectRemoveDialog removeDialog(this);

    QStringList changedDirectories;

    foreach (VLNV const& vlnvToRemove, vlnvs)
    {
        if (!vlnvToRemove.isValid())
        {
            continue;
        }

        // if the vlnv is not found in the library
        if (!data_->contains(vlnvToRemove))
        {
            objects_.remove(vlnvToRemove);
            objectValidity_.remove(vlnvToRemove);
            continue;
        }

        changedDirectories.append(QFileInfo(data_->getPath(vlnvToRemove)).absolutePath());

        removeDialog.createItem(vlnvToRemove, true);

        VLNV::IPXactType vlnvType = data_->getType(vlnvToRemove);
        if (vlnvType == VLNV::COMPONENT)
        {
            QSharedPointer<const Component> componentToRemove =
                getModelReadOnly(vlnvToRemove).staticCast<const Component>();

            foreach (VLNV const& ref, componentToRemove->getHierRefs())
            {
                if (data_->contains(ref))
                {
                    removeDialog.createItem(ref);

                    if (data_->getType(ref) == VLNV::DESIGNCONFIGURATION)
                    {
                        QSharedPointer<const DesignConfiguration> desConf =
                            getModelReadOnly(ref).staticCast<const DesignConfiguration>();

                        VLNV designVLNV = desConf->getDesignRef();
                        if (data_->contains(designVLNV))
                        {
                            removeDialog.createItem(designVLNV);
                        }
                    }
                }
            }

            // ask the component for all it's file references.
            QStringList componentFiles;
            foreach (QSharedPointer<FileSet> fileset, *componentToRemove->getFileSets())
            {
                componentFiles.append(fileset->getFileNames());
            }

            QString componentPath = data_->getPath(vlnvToRemove);
            foreach (QString const& relativeFilePath, componentFiles)
            {
                QString absoluteFilePath = General::getAbsolutePath(componentPath, relativeFilePath);
                if (!absoluteFilePath.isEmpty())
                {
                    removeDialog.createItem(absoluteFilePath);
                }
            }
        }

        else if (vlnvType == VLNV::DESIGNCONFIGURATION)
        {
            QSharedPointer<const DesignConfiguration> desConf =
                getModelReadOnly(vlnvToRemove).staticCast<const DesignConfiguration>();

            VLNV designVLNV = desConf->getDesignRef();
            if (data_->contains(designVLNV))
            {
                removeDialog.createItem(designVLNV);
            }
        }

        else if (vlnvType == VLNV::BUSDEFINITION)
        {
            QList<VLNV> absDefVLNVs;
            hierarchyModel_->getChildren(absDefVLNVs, vlnvToRemove);

            // If a bus definition is removed then ask to remove all it's abstraction definitions also.
            foreach (VLNV const& absDefVLNV, absDefVLNVs)
            {
                if (data_->contains(absDefVLNV))
                {
                    removeDialog.createItem(absDefVLNV);
                }
            }
        }

        else if (vlnvType == VLNV::ABSTRACTIONDEFINITION)
        {
            QSharedPointer<const AbstractionDefinition> absDef =
                getModelReadOnly(vlnvToRemove).staticCast<const AbstractionDefinition>();

            VLNV busDefVLNV = absDef->getBusType();
            if (data_->contains(busDefVLNV))
            {
                QList<VLNV> absDefVLNVs;
                hierarchyModel_->getChildren(absDefVLNVs, busDefVLNV);

                // if theres only this abs def for the bus def
                if (absDefVLNVs.size() == 1 && absDefVLNVs.first() == vlnvToRemove)
                {
                    removeDialog.createItem(busDefVLNV);
                }
            }
        }
    }

    if (removeDialog.exec() == QDialog::Rejected)
    {
        return;
    }

    foreach (ObjectRemoveModel::Item item, removeDialog.getItemsToRemove())
    {	
        // if this is vlnv object, remove it from the library.
        if (item.type_ == ObjectRemoveModel::VLNVOBJECT)
        {
            removeObject(item.vlnv_);
        }

        else // If this is file, remove it from file system.
        {
            QFileInfo fileInfo(item.path_);
            if (fileInfo.exists())
            {
                changedDirectories.append(fileInfo.absolutePath());

                QFile file(item.path_);
                if (!file.remove())
                {
                    emit errorMessage(tr("File %1 could not be removed from file system.").arg(item.path_));
                }
            }
        }
    }

    QSettings settings;
    QStringList libraryLocations = settings.value("Library/Locations", QStringList()).toStringList();

    changedDirectories.removeDuplicates();
    foreach (QString const& changedDirectory, changedDirectories)
    {
        clearDirectoryStructure(changedDirectory, libraryLocations);
    }
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
    savedItem.setType(data_->getType(vlnv));

    // make sure the model is parsed again next time to make all changes available
    objects_.remove(vlnv);
    objectValidity_.remove(vlnv);

    treeModel_->onDocumentSaved(vlnv);
    hierarchyModel_->onDocumentSaved(savedItem);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::writeFile()
//-----------------------------------------------------------------------------
bool LibraryHandler::writeFile(QString const& filePath, QSharedPointer<Document> model)
{
    // create file info instance representing the file in the path
    QFileInfo oldFileInfo;
    oldFileInfo.setFile(filePath);

    // remove the old file
    if (oldFileInfo.exists())
    {
        QFile oldFile(oldFileInfo.filePath());
        oldFile.remove();
    }

    // create a new file
    QFile newFile(filePath);
    if (!newFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        emit errorMessage(tr("Could not open file %1 for writing").arg(filePath));
        return false;
    }

    // write the parsed model
    QXmlStreamWriter xmlWriter(&newFile);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(-1);

    VLNV::IPXactType documentType = model->getVlnv().getType();
    if (documentType == VLNV::ABSTRACTIONDEFINITION)
    {
        AbstractionDefinitionWriter writer;
        QSharedPointer<AbstractionDefinition> absDef = model.dynamicCast<AbstractionDefinition>();
        writer.writeAbstractionDefinition(xmlWriter, absDef);
    }

    else if (documentType == VLNV::BUSDEFINITION)
    {
        BusDefinitionWriter writer;
        QSharedPointer<BusDefinition> busDef = model.dynamicCast<BusDefinition>();
        writer.writeBusDefinition(xmlWriter, busDef);
    }
    else if (documentType == VLNV::COMPONENT)
    {
        ComponentWriter writer;
        QSharedPointer<Component> component = model.dynamicCast<Component>();
        writer.writeComponent(xmlWriter, component);
    }

    else if (documentType == VLNV::DESIGN)
    {
        DesignWriter designWriter;
        QSharedPointer<Design> design = model.dynamicCast<Design>();
        designWriter.writeDesign(xmlWriter, design);
    }

    else if (documentType == VLNV::DESIGNCONFIGURATION)
    {
        DesignConfigurationWriter designConfigurationWriter;
        QSharedPointer<DesignConfiguration> designConfiguration = model.dynamicCast<DesignConfiguration>();
        designConfigurationWriter.writeDesignConfiguration(xmlWriter, designConfiguration);
	}

	else if (documentType == VLNV::APIDEFINITION)
	{
		ApiDefinitionWriter apiDefinitionWriter;
		QSharedPointer<ApiDefinition> apiDefinition = model.dynamicCast<ApiDefinition>();
		apiDefinitionWriter.writeApiDefinition(xmlWriter, apiDefinition);
	}

	else if (documentType == VLNV::COMDEFINITION)
	{
		ComDefinitionWriter comDefinitionWriter;
		QSharedPointer<ComDefinition> comDefinition = model.dynamicCast<ComDefinition>();
		comDefinitionWriter.writeComDefinition(xmlWriter, comDefinition);
	}
    else
    {
        Q_ASSERT_X(false, "Libraryhandler::writeFile().", "Trying to write unknown document type to file.");
    }

    newFile.close();
    return true;
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
		emit errorMessage(tr("Could not create directory structure, aborting."));

		// restore the previous working directory to avoid messing with caller function
		QDir::setCurrent(savedWorkingDir.absolutePath());
		return;
	}

	vlnvTargetDirectory.setPath(directoryPath);

	QString documentPath = data_->getPath(vlnv);
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
                emit errorMessage(tr("Could not create directory structure for file %1").arg(
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
void LibraryHandler::copyFile(QFileInfo const& source, QDir& target, fileList& handledFiles, bool& yesToAll,
    bool& noToAll)
{
	// if the source file has already been handled previously within this copy operation.
	if (handledFiles.contains(source))
    {
		return;
	}

	// if the source doesn't exist
	if (!source.exists())
    {
		emit errorMessage(tr("Could not find file: %1").arg(source.fileName()));
		return;
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
			answer = QMessageBox::question(this, title, text, QMessageBox::Yes | QMessageBox::No |
				QMessageBox::YesToAll | QMessageBox::NoToAll ,	QMessageBox::No);
		}

		if (answer == QMessageBox::No)
        {
			// restore the current directory to the state it was before this function
			QDir::setCurrent(savedCurrentDir.absolutePath());
			return;
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
			return;
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
		emit errorMessage(tr("File %1 couldn't be copied").arg(source.fileName()));
	}

	// restore the current directory to the state it was before this function
	QDir::setCurrent(savedCurrentDir.absolutePath());
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::syncronizeModels()
//-----------------------------------------------------------------------------
void LibraryHandler::syncronizeModels()
{
    // connect the signals from the data model
    connect(data_.data(), SIGNAL(removeVLNV(const VLNV&)),
        treeModel_.data(), SLOT(onRemoveVLNV(const VLNV&)), Qt::UniqueConnection);
    connect(data_.data(), SIGNAL(removeVLNV(const VLNV&)),
        hierarchyModel_.data(), SLOT(onRemoveVLNV(const VLNV&)), Qt::UniqueConnection);

    connect(data_.data(), SIGNAL(resetModel()),
        hierarchyModel_.data(), SLOT(onResetModel()), Qt::UniqueConnection);
    connect(data_.data(), SIGNAL(resetModel()),
        treeModel_.data(), SLOT(onResetModel()), Qt::UniqueConnection);

    connect(data_.data(), SIGNAL(addVLNV(const VLNV&)),
        treeModel_.data(), SLOT(onAddVLNV(const VLNV&)), Qt::UniqueConnection);

    // signals from data model to library handler
    connect(data_.data(), SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(data_.data(), SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    //-----------------------------------------------------------------------------
    // connect the signals from the tree model
    //-----------------------------------------------------------------------------
    connect(treeModel_.data(), SIGNAL(removeVLNV(const QList<VLNV>)),
        this, SLOT(onRemoveVLNV(const QList<VLNV>)), Qt::UniqueConnection);

    // signals from tree model to library handler
    connect(treeModel_.data(), SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(openDesign(const VLNV&)),
        this, SLOT(onOpenDesign(const VLNV&)), Qt::UniqueConnection);
    //     connect(treeModel_.data(), SIGNAL(openMemoryDesign(const VLNV&)),
    //         this, SLOT(onOpenMemoryDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(openSWDesign(const VLNV&)),
        this, SLOT(onOpenSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(openSystemDesign(const VLNV&)),
        this, SLOT(onOpenSystemDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(editItem(const VLNV&)),
        this, SLOT(onEditItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createBus(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createAbsDef(const VLNV&)),
        this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createComDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createApiDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createComponent(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createDesign(const VLNV&)),
        this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createSWDesign(const VLNV&)),
        this, SIGNAL(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(createSystemDesign(const VLNV&)),
        this, SIGNAL(createSystemDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(exportItems(const QList<VLNV>)),
        this, SLOT(onExportItems(const QList<VLNV>)), Qt::UniqueConnection);
    connect(treeModel_.data(), SIGNAL(refreshDialer()),
        this, SIGNAL(refreshDialer()), Qt::UniqueConnection);

    connect(treeModel_.data(), SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

    //-----------------------------------------------------------------------------
    // connect the signals from the hierarchy model
    //-----------------------------------------------------------------------------

    connect(hierarchyModel_.data(), SIGNAL(openDesign(const VLNV&, const QString&)),
        this, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(openMemoryDesign(const VLNV&, const QString&)),
        this, SIGNAL(openMemoryDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    connect(hierarchyModel_.data(), SIGNAL(openMemoryDesign(const VLNV&, const QString&)),
        this, SLOT(onOpenMemoryDesign(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel_.data(), SIGNAL(openSWDesign(const VLNV&, const QString&)),
        this, SIGNAL(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(openSystemDesign(const VLNV&, const QString&)),
        this, SIGNAL(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    connect(hierarchyModel_.data(), SIGNAL(editItem(const VLNV&)),
        this, SLOT(onEditItem(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel_.data(), SIGNAL(createBusDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(createComponent(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(createBus(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(createAbsDef(const VLNV&)),
        this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(createComDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(createApiDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel_.data(), SIGNAL(createDesign(const VLNV&)),
        this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(createSWDesign(const VLNV&)),
        this, SIGNAL(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel_.data(), SIGNAL(createSystemDesign(const VLNV&)),
        this, SIGNAL(createSystemDesign(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel_.data(), SIGNAL(exportItem(const VLNV)),
        this, SLOT(onExportItem(const VLNV)), Qt::UniqueConnection);

    connect(hierarchyModel_.data(), SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::connectVLNVFilter()
//-----------------------------------------------------------------------------
void LibraryHandler::connectLibraryFilter(LibraryFilter* filter, VLNVDialer* dialer) const
{
    connect(dialer, SIGNAL(vendorChanged(const QString&)),
        filter, SLOT(onVendorChanged(const QString&)), Qt::UniqueConnection);
    connect(dialer, SIGNAL(libraryChanged(const QString&)),
        filter, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);
    connect(dialer, SIGNAL(nameChanged(const QString&)), 
        filter, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);
    connect(dialer, SIGNAL(versionChanged(const QString&)),
        filter, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);

    connect(dialer, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)),
        filter, SLOT(onFirmnessChanged(const Utils::FirmnessOptions&)), Qt::UniqueConnection);
    connect(dialer, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)),
        filter, SLOT(onImplementationChanged(const Utils::ImplementationOptions&)), Qt::UniqueConnection);
    connect(dialer, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)),
        filter, SLOT(onHierarchyChanged(const Utils::HierarchyOptions&)), Qt::UniqueConnection);
    connect(dialer, SIGNAL(typeChanged(const Utils::TypeOptions&)),
        filter, SLOT(onTypeChanged(const Utils::TypeOptions&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryHandler::clearDirectoryStructure()
//-----------------------------------------------------------------------------
void LibraryHandler::clearDirectoryStructure(QString const& dirPath, QStringList const& libraryLocations)
{
	QDir dir(dirPath);
	
	while (containsPath(QDir::cleanPath(dir.absolutePath()), libraryLocations))
    {	
		// if not possible to move up anymore (the dir could possibly have been destroyed already).
		if (!dir.cdUp())
        {
			return;
		}

		// if the directory is not empty then it can't be removed and we can stop.
		if (!dir.rmdir(dir.dirName()))
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
		if (path.contains(searchPath) && path != searchPath)
        {
			return true;
		}
	}

	// None of the paths to search were contained in the path.
	return false;
}
