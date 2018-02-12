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

#include "LibraryTreeWidget.h"
#include "LibraryHandler.h"
#include "HierarchyView/hierarchymodel.h"
#include "HierarchyView/hierarchywidget.h"

#include "VLNVDialer/vlnvdialer.h"

#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: LibraryWidget::LibraryWidget()
//-----------------------------------------------------------------------------
LibraryWidget::LibraryWidget(LibraryHandler* library, QWidget *parent): QWidget(parent),
    dialer_(new VLNVDialer(this)),
    library_(library),
    hierarchyWidget_(new HierarchyWidget(library_, library_->getHierarchyModel(), this)),
    treeWidget_(new LibraryTreeWidget(library_, library_->getTreeModel(), this)),
    statusBar_(new QStatusBar(this))
{
    library_->setParent(this);

    dialer_->setRootItem(library_->getTreeRoot());

    connectLibraryFilter(hierarchyWidget_->getFilter());
    connectLibraryFilter(treeWidget_->getFilter());

    connect(library_, SIGNAL(refreshDialer()), dialer_, SLOT(refreshLibrary()), Qt::UniqueConnection);
    connect(library_, SIGNAL(statusMessage(QString const&)), statusBar_, SLOT(showMessage(QString const&)), 
        Qt::UniqueConnection);

    connect(hierarchyWidget_, SIGNAL(componentSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

    connect(treeWidget_, SIGNAL(itemSelected(const VLNV&)),
        library_, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: LibraryWidget::~LibraryWidget()
//-----------------------------------------------------------------------------
LibraryWidget::~LibraryWidget()
{

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
// Function: LibraryWidget::statusMessage()
//-----------------------------------------------------------------------------
void LibraryWidget::statusMessage(QString const& message)
{
    statusBar_->showMessage(message);
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
