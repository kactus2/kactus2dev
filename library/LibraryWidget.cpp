//-----------------------------------------------------------------------------
// File: LibraryWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.05.2017
//
// Description:
// Widget for holding all IP-XACT library widgets.
//-----------------------------------------------------------------------------

#include "LibraryWidget.h"

#include <common/ui/GraphicalMessageMediator.h>

#include "LibraryErrorModel.h"

#include "LibraryTreeWidget.h"
#include "LibraryHandler.h"

#include <KactusAPI/include/hierarchymodel.h>

#include "objectremovedialog.h"
#include "ObjectSelectionListItem.h"
#include "LibraryItemSelectionFactory.h"

#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include "HierarchyView/hierarchywidget.h"

#include "VLNVDialer/vlnvdialer.h"

#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: LibraryWidget::LibraryWidget()
//-----------------------------------------------------------------------------
LibraryWidget::LibraryWidget(LibraryHandler* library, MessageMediator* messageChannel, QWidget *parent): QWidget(parent),
    dialer_(new VLNVDialer(this)),
    library_(library),
    itemExporter_(library_, this, this),
    hierarchyWidget_(new HierarchyWidget(library_, library_->getHierarchyModel(), this)),
    treeWidget_(new LibraryTreeWidget(library_, library_->getTreeModel(), this)),
    integrityWidget_(nullptr),
    statusBar_(new QStatusBar(this))
{
    GraphicalMessageMediator* guiChannel = dynamic_cast<GraphicalMessageMediator*>(messageChannel);
    if (guiChannel)
    {        
        connect(guiChannel, SIGNAL(statusMessage(QString const&)), statusBar_, SLOT(showMessage(QString const&)));
    }

    dialer_->setRootItem(library_->getTreeRoot());

    connectLibraryFilter(hierarchyWidget_->getFilter());
    connectLibraryFilter(treeWidget_->getFilter());


    connect(hierarchyWidget_, SIGNAL(componentSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);
    
    connect(library_->getHierarchyModel(), SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

    connect(treeWidget_, SIGNAL(itemSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

    auto treeModel = library_->getTreeModel();
    connect(treeModel, SIGNAL(refreshDialer()), dialer_, SLOT(refreshLibrary()), Qt::UniqueConnection);

    connect(treeModel, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(treeModel, SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

    connect(treeModel, SIGNAL(createBus(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(createComDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(createApiDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(createComponent(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(createAbsDef(const VLNV&)),
        this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(createDesign(const VLNV&)),
        this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);

    connect(treeModel, SIGNAL(createSWDesign(const VLNV&)),
        this, SIGNAL(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(createSystemDesign(const VLNV&)),
        this, SIGNAL(createSystemDesign(const VLNV&)), Qt::UniqueConnection);

    connect(treeModel, SIGNAL(removeVLNV(const QList<VLNV>)),
        this, SLOT(onRemoveVLNV(const QList<VLNV>)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(exportItems(const QList<VLNV>)),
       &itemExporter_, SLOT(onExportItems(const QList<VLNV>)), Qt::UniqueConnection);

    auto hierarchyModel = library_->getHierarchyModel();
    connect(hierarchyModel, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(removeVLNV(QList<VLNV>)),
        this, SLOT(onRemoveVLNV(QList<VLNV>)), Qt::UniqueConnection);


    connect(hierarchyModel, SIGNAL(createBusDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(createComponent(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(createBus(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(createComDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(createApiDef(const VLNV&)),
        this, SLOT(onCreateNewItem(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(createAbsDef(const VLNV&)),
        this, SLOT(onCreateAbsDef(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(createDesign(const VLNV&)),
        this, SLOT(onCreateDesign(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel, SIGNAL(createSWDesign(const VLNV&)),
        this, SIGNAL(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(hierarchyModel, SIGNAL(createSystemDesign(const VLNV&)),
        this, SIGNAL(createSystemDesign(const VLNV&)), Qt::UniqueConnection);

    connect(hierarchyModel, SIGNAL(exportItem(VLNV const&)),
         &itemExporter_, SLOT(onExportItem(VLNV const&)), Qt::UniqueConnection);

     connect(&itemExporter_, SIGNAL(noticeMessage(const QString&)),
          this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

     connect(&itemExporter_, SIGNAL(errorMessage(const QString&)),
         this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);

    setupLayout();
} 

//-----------------------------------------------------------------------------
// Function: LibraryWidget::getLibraryHandler()
//-----------------------------------------------------------------------------
LibraryHandler* LibraryWidget::getLibraryHandler() const
{
    return library_;
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::setFilters()
//-----------------------------------------------------------------------------
void LibraryWidget::setFilters(Utils::FilterOptions filters)
{
    dialer_->setFilters(filters);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::getFilters()
//-----------------------------------------------------------------------------
Utils::FilterOptions LibraryWidget::getFilters() const
{
    return dialer_->getFilters();
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::selectComponent()
//-----------------------------------------------------------------------------
void LibraryWidget::selectComponent(VLNV const& componentVLNV) const
{
    treeWidget_->selectItem(componentVLNV);
    hierarchyWidget_->selectItems(componentVLNV);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::statusMessage()
//-----------------------------------------------------------------------------
void LibraryWidget::statusMessage(QString const& message)
{
    statusBar_->showMessage(message);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::onShowErrors()
//-----------------------------------------------------------------------------
void LibraryWidget::onShowErrors(VLNV const& vlnv)
{
    if (vlnv.isValid() == false || library_->contains(vlnv) == false)
    {
        return;
    }

    QSharedPointer<Document> document = library_->getModel(vlnv);

    // Show error list in a dialog.
    TableViewDialog* dialog = new TableViewDialog(this);
    dialog->setWindowTitle(tr("Errors in %1").arg(vlnv.toString()));
    dialog->setDescription(tr("<b>Integrity check</b><br>The following errors were found."));
    dialog->resize(700, 350);

    LibraryErrorModel* model = new LibraryErrorModel(dialog);
    model->addErrors(library_->findErrorsInDocument(document, library_->getPath(vlnv)), vlnv.toString());

    dialog->show();

    dialog->setModel(model);

    connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::onCreateNewItem()
//-----------------------------------------------------------------------------
void LibraryWidget::onCreateNewItem(VLNV const& vlnv)
{
    VLNV::IPXactType documentType = vlnv.getType();

    bool showAttributes = (documentType == VLNV::COMPONENT);
    NewObjectDialog newDesignDialog(library_, vlnv.getType(), showAttributes, this);

    if (library_->contains(vlnv) && library_->getDocumentType(vlnv) == VLNV::COMPONENT)
    {
        QSharedPointer<const Document> document = library_->getModelReadOnly(vlnv);
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
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::onCreateAbsDef()
//-----------------------------------------------------------------------------
void LibraryWidget::onCreateAbsDef(VLNV const& busDefVLNV)
{
    if (busDefVLNV.isValid() == false)
    {
        return;
    }

    Q_ASSERT(library_->getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION);
    Q_ASSERT(library_->contains(busDefVLNV));

    QFileInfo busDefInfo(library_->getPath(busDefVLNV));
    QString directory = busDefInfo.absolutePath();

    QList<VLNV> absDefVLNVs;
    library_->getHierarchyModel()->getChildren(absDefVLNVs, busDefVLNV);

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
// Function: LibraryWidget::onCreateDesign()
//-----------------------------------------------------------------------------
void LibraryWidget::onCreateDesign(VLNV const& vlnv)
{
    emit createDesignForExistingComponent(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::onGenerateIntegrityReport()
//-----------------------------------------------------------------------------
void LibraryWidget::onGenerateIntegrityReport()
{
    if (!integrityWidget_)
     {
         integrityWidget_ = new TableViewDialog(this);
         integrityWidget_->setWindowTitle(tr("Library Integrity Report"));
         integrityWidget_->setDescription(tr("<b>Integrity check</b><br>The following errors were found."));
         integrityWidget_->resize(1000, 800);

         LibraryErrorModel* model = new LibraryErrorModel(integrityWidget_);

         for (auto vlnv : library_->getAllVLNVs())
         {
             if (library_->isValid(vlnv) == false)
             {
                 model->addErrors(library_->findErrorsInDocument(library_->getModel(vlnv), library_->getPath(vlnv)), vlnv.toString());
             }
         }

         integrityWidget_->setModel(model);

         integrityWidget_->show();

         connect(integrityWidget_, SIGNAL(finished(int)), this, SLOT(onCloseIntegrityReport()));
     }

     integrityWidget_->raise();
}
//-----------------------------------------------------------------------------
// Function: LibraryWidget::closeIntegrityReport()
//-----------------------------------------------------------------------------
void LibraryWidget::onCloseIntegrityReport()
{
      integrityWidget_->deleteLater();
      integrityWidget_ = 0;
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::onRemoveVLNV()
//-----------------------------------------------------------------------------
void LibraryWidget::onRemoveVLNV(const QList<VLNV> vlnvs)
{
    // create the dialog to select which items to remove
    ObjectRemoveDialog removeDialog(this);
    LibraryItemSelectionFactory::constructItemsForSelectionDialog(library_, &removeDialog, vlnvs);

    if (removeDialog.exec() == QDialog::Rejected)
    {
        return;
    }

    QList<VLNV> removedVLNVs;
    QStringList removedFilePaths;
    for (ObjectSelectionListItem const* removedItem : removeDialog.getSelectedItems())
    {
        if (removedItem->getType() == ObjectSelectionListItem::VLNVOJBECT)
        {
            removedVLNVs.append(removedItem->getVLNV());
        }
        else if (removedItem->getType() == ObjectSelectionListItem::FILE)
        {
            QFileInfo fileInfo(removedItem->getPath());
            if (fileInfo.exists())
            {
                removedFilePaths.append(removedItem->getPath());
            }
        }
    }

    library_->removeObjects(removedVLNVs);

    for (auto const& path : removedFilePaths)
    {
        QFile file(path);
        if (file.remove() == false)
        {
           emit errorMessage(tr("File %1 could not be removed from the file system.").arg(path));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::connectVLNVFilter()
//-----------------------------------------------------------------------------
void LibraryWidget::connectLibraryFilter(LibraryFilter* filter) const
{
    connect(dialer_, SIGNAL(vendorChanged(const QString&)),
        filter, SLOT(onVendorChanged(const QString&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(libraryChanged(const QString&)),
        filter, SLOT(onLibraryChanged(const QString&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(nameChanged(const QString&)),
        filter, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(versionChanged(const QString&)),
        filter, SLOT(onVersionChanged(const QString&)), Qt::UniqueConnection);

    connect(dialer_, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)),
        filter, SLOT(onFirmnessChanged(const Utils::FirmnessOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)),
        filter, SLOT(onImplementationChanged(const Utils::ImplementationOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)),
        filter, SLOT(onHierarchyChanged(const Utils::HierarchyOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(typeChanged(const Utils::TypeOptions&)),
        filter, SLOT(onTypeChanged(const Utils::TypeOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(filtersChanged(Utils::FilterOptions const&)),
        filter, SLOT(onFiltersChanged(Utils::FilterOptions const&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(tagFiltersChanged(QVector<TagData>)),
        filter, SLOT(onTagFilterChanged(QVector<TagData>)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::setupLayout()
//-----------------------------------------------------------------------------
void LibraryWidget::setupLayout()
{
    QTabWidget* navigationTabs = new QTabWidget(this);
    navigationTabs->addTab(treeWidget_, tr("VLNV Tree"));
    navigationTabs->addTab(hierarchyWidget_, tr("Hierarchy"));

    QVBoxLayout* containerLayout = new QVBoxLayout(this);
    containerLayout->addWidget(dialer_, 0);
    containerLayout->addWidget(navigationTabs, 1);
    containerLayout->addWidget(statusBar_);
    containerLayout->setSpacing(0);
    containerLayout->setContentsMargins(0, 0, 0, 0);
}
