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

#include <QTabWidget>
#include <QVBoxLayout>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: LibraryWidget::LibraryWidget()
//-----------------------------------------------------------------------------
LibraryWidget::LibraryWidget(LibraryHandler* library, MessageMediator* messageChannel, QWidget *parent): QWidget(parent),
    dialer_(new VLNVDialer(this)),
    library_(library),
    itemExporter_(library_, this, this),
    hierarchyWidget_(new HierarchyWidget(library_, library_->getHierarchyModel(), this)),
    treeWidget_(new LibraryTreeWidget(library_, library_->getTreeModel(), this)),
    searchBar_(new QLineEdit(this)),
    previewWidget_(new ComponentPreviewBox(library, this)),
    previewHideButton_(new QPushButton(QString(), this)),
    integrityWidget_(nullptr),
    hidePreview_(false)
{
    GraphicalMessageMediator* guiChannel = dynamic_cast<GraphicalMessageMediator*>(messageChannel);
    if (guiChannel)
    {        
        connect(guiChannel, SIGNAL(statusMessage(QString const&)), 
            this, SIGNAL(statusMessage(QString const&)));
    }

    connectLibraryFilter(hierarchyWidget_->getFilter());
    connectLibraryFilter(treeWidget_->getFilter());

    searchBar_->setPlaceholderText(tr("Search"));
    searchBar_->addAction(QIcon(":/icons/common/graphics/search.png"), QLineEdit::LeadingPosition);
    searchBar_->setClearButtonEnabled(true);

    connect(searchBar_, SIGNAL(textChanged(QString const&)),
        hierarchyWidget_, SLOT(onSearchTextChanged(QString const&)), Qt::UniqueConnection);
    connect(searchBar_, SIGNAL(textChanged(QString const&)),
        treeWidget_, SLOT(onSearchTextChanged(QString const&)), Qt::UniqueConnection);

    QSettings settings;
    hidePreview_ = !settings.value("PreviewWidget/Hidden", true).toBool();

    previewHideButton_->setFlat(true);
    previewHideButton_->setToolTip(tr("Show preview"));

    onPreviewShowHideClick();

    connect(hierarchyWidget_, SIGNAL(componentSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);
    
    connect(library_->getHierarchyModel(), SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

    connect(treeWidget_, SIGNAL(itemSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);
    
    auto treeModel = library_->getTreeModel();

    connect(treeModel, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(treeModel, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(treeModel, SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

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

     connect(library_, SIGNAL(itemSelected(const VLNV&)),
         previewWidget_, SLOT(setComponent(const VLNV&)), Qt::UniqueConnection);

     connect(previewHideButton_, SIGNAL(clicked(bool)), 
         this, SLOT(onPreviewShowHideClick()), Qt::UniqueConnection);

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
// Function: LibraryWidget::loadFilterSettings()
//-----------------------------------------------------------------------------
void LibraryWidget::loadFilterSettings(QSettings& settings)
{
    dialer_->loadFilterSettings(settings);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::saveFilterSettings()
//-----------------------------------------------------------------------------
void LibraryWidget::saveFilterSettings(QSettings& settings) const
{
    dialer_->saveFilterSettings(settings);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::selectComponent()
//-----------------------------------------------------------------------------
void LibraryWidget::selectComponent(VLNV const& componentVLNV) const
{
    treeWidget_->selectItem(componentVLNV);
    hierarchyWidget_->selectItems(componentVLNV);
    previewWidget_->setComponent(componentVLNV);
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

    emit createAbsDef(busDefVLNV, directory);
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

         for (auto const& vlnv : library_->getAllVLNVs())
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
// Function: LibraryWidget::onPreviewHideShowClick()
//-----------------------------------------------------------------------------
void LibraryWidget::onPreviewShowHideClick()
{
    hidePreview_ = !hidePreview_;
    previewWidget_->setHidden(hidePreview_);

    if (hidePreview_)
    {
        previewHideButton_->setToolTip(tr("Show preview"));
        previewHideButton_->setIcon(QIcon(":icons/common/graphics/preview.png"));
    }
    else
    {
        previewHideButton_->setToolTip(tr("Hide preview"));
        previewHideButton_->setIcon(QIcon(":icons/common/graphics/hide.png"));
    }

    QSettings settings;
    settings.setValue("PreviewWidget/Hidden", hidePreview_);
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::connectVLNVFilter()
//-----------------------------------------------------------------------------
void LibraryWidget::connectLibraryFilter(LibraryFilter* filter) const
{
    connect(dialer_, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)),
        filter, SLOT(onFirmnessChanged(const Utils::FirmnessOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)),
        filter, SLOT(onImplementationChanged(const Utils::ImplementationOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)),
        filter, SLOT(onHierarchyChanged(const Utils::HierarchyOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(typeChanged(const Utils::TypeOptions&)),
        filter, SLOT(onTypeChanged(const Utils::TypeOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(revisionChanged(const Utils::RevisionOptions&)),
        filter, SLOT(onRevisionChanged(const Utils::RevisionOptions&)), Qt::UniqueConnection);
    connect(dialer_, SIGNAL(validityChanged(const Utils::ValidityOptions&)),
        filter, SLOT(onValidityChanged(const Utils::ValidityOptions&)), Qt::UniqueConnection);
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
    auto libraryGroup = new QWidget(this);

    auto navigationTabs = new QTabWidget(this);
    navigationTabs->addTab(treeWidget_, tr("VLNV Tree"));
    navigationTabs->addTab(hierarchyWidget_, tr("Hierarchy"));

    searchBar_->setContentsMargins(2, 0, 4, 0);

    auto libraryLayout = new QVBoxLayout(libraryGroup);
    libraryLayout->addWidget(navigationTabs, 1);
    libraryLayout->addWidget(searchBar_);
    libraryLayout->addWidget(dialer_);
    libraryLayout->setSpacing(2);
    libraryLayout->setContentsMargins(0, 0, 0, 0);

    auto previewGroup = new QWidget(this);

    auto previewLayout = new QGridLayout(previewGroup);

    auto previewLabel = new QLabel(tr("Component Preview"), this);

    previewLayout->addWidget(previewLabel, 0, 0, 1, 1);
    previewLayout->addWidget(previewHideButton_, 0, 1, 1, 1, Qt::AlignRight);
    previewLayout->addWidget(previewWidget_, 1, 0, 1, 2);
    previewLayout->setContentsMargins(4, 0, 4, 0);

    auto viewSplit = new QSplitter(this);
    viewSplit->setOrientation(Qt::Vertical);
    viewSplit->addWidget(libraryGroup);
    viewSplit->addWidget(previewGroup);
    viewSplit->setStretchFactor(0, 4);
    viewSplit->setContentsMargins(0, 0, 0, 0);

    auto handle = viewSplit->handle(1);
    auto handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setContentsMargins(2, 0, 0, 0);

    auto line = new QFrame(handle);
    line->setLineWidth(2);
    line->setMidLineWidth(2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    handleLayout->addWidget(line);

    viewSplit->setHandleWidth(10);

    auto containerLayout = new QVBoxLayout(this);
    containerLayout->addWidget(viewSplit, 1);
    containerLayout->setSpacing(0);
    containerLayout->setContentsMargins(0, 0, 0, 0);
}
