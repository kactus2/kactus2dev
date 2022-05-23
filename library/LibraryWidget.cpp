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
    hierarchyWidget_(new HierarchyWidget(library_, library_->getHierarchyModel(), this)),
    treeWidget_(new LibraryTreeWidget(library_, library_->getTreeModel(), this)),
    integrityWidget_(nullptr),
    statusBar_(new QStatusBar(this))
{
    GraphicalMessageMediator* guiChannel = dynamic_cast<GraphicalMessageMediator*>(messageChannel);
    if (guiChannel)
    {
        guiChannel->setStatusBar(statusBar_);
    }

    dialer_->setRootItem(library_->getTreeRoot());

    connectLibraryFilter(hierarchyWidget_->getFilter());
    connectLibraryFilter(treeWidget_->getFilter());

    connect(library_, SIGNAL(refreshDialer()), dialer_, SLOT(refreshLibrary()), Qt::UniqueConnection);
    connect(library_, SIGNAL(statusMessage(QString const&)), statusBar_, SLOT(showMessage(QString const&)), 
        Qt::UniqueConnection);

    connect(hierarchyWidget_, SIGNAL(componentSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);
    
    connect(library_->getHierarchyModel(), SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

    connect(treeWidget_, SIGNAL(itemSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

    connect(library_->getTreeModel(), SIGNAL(showErrors(const VLNV)),
        this, SLOT(onShowErrors(const VLNV)), Qt::UniqueConnection);

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
