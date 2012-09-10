//-----------------------------------------------------------------------------
// File: AddressEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.08.2012
//
// Description:
// Dockable address editor for HW designs.
//-----------------------------------------------------------------------------

#include "AddressEditor.h"

#include "AddressDelegate.h"
#include "AddressTableView.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/GenericEditProvider.h>
#include <common/DesignDiagram.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSettings>
#include <QCoreApplication>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: AddressEditor::AddressEditor()
//-----------------------------------------------------------------------------
AddressEditor::AddressEditor(QWidget* parent)
    : QWidget(parent),
      component_(0),
      autoAssignButton_(new QPushButton(tr("Auto-assign Addresses"), this)),
      csvImportButton_(new QPushButton(tr("Import CSV"), this)),
      csvExportButton_(new QPushButton(tr("Export CSV"), this)),
      model_(this),
      view_(new AddressTableView(this)),
      filter_(new QSortFilterProxyModel(this))
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    filter_->setSourceModel(&model_);

    //view_->setModel(filter_);
    view_->setModel(&model_);
    //view_->setSortingEnabled(true);
    view_->setItemDelegate(new AddressDelegate(this));
    view_->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->hide();

    view_->setColumnWidth(ADDRESS_COL_INTERFACE_NAME, 150);
    view_->setColumnWidth(ADDRESS_COL_MAP_NAME, 150);
    view_->resizeColumnToContents(ADDRESS_COL_LOCKED);
    view_->resizeColumnToContents(ADDRESS_COL_MAP_AUB);
    view_->horizontalHeader()->setResizeMode(ADDRESS_COL_LOCKED, QHeaderView::Fixed);

    setupLayout();
    setupConnections();

    clear();
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::~AddressEditor()
//-----------------------------------------------------------------------------
AddressEditor::~AddressEditor()
{
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::setComponent()
//-----------------------------------------------------------------------------
void AddressEditor::setComponent(ComponentItem* component)
{
    Q_ASSERT(component);

    parentWidget()->raise();

    // Disconnect the previously assigned component instance.
    if (component_)
    {
        component_->disconnect(this);
    }

    component_ = component;
    model_.setComponent(component_);

    view_->resizeRowsToContents();

    bool locked = false;

    // Retrieve the edit provider that manages the undo/redo stack.
    DesignDiagram* diagram = static_cast<DesignDiagram*>(component->scene());
    editProvider_ = &diagram->getEditProvider();
    locked = diagram->isProtected();

    autoAssignButton_->setEnabled(!locked);
    csvImportButton_->setEnabled(!locked);
    csvExportButton_->setEnabled(true);
    view_->setEnabled(!locked);

    connect(component_, SIGNAL(destroyed(ComponentItem*)), this, SLOT(clear()), Qt::UniqueConnection);

    //parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::clear()
//-----------------------------------------------------------------------------
void AddressEditor::clear()
{
    // Disconnect the previously assigned component instance.
    if (component_)
    {
        component_->disconnect(this);
    }

    component_ = 0;
    model_.clear();

    autoAssignButton_->setEnabled(false);
    csvImportButton_->setEnabled(false);
    csvExportButton_->setEnabled(false);
    view_->setEnabled(false);
    //parentWidget()->setMaximumHeight(20);
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::onRefresh()
//-----------------------------------------------------------------------------
void AddressEditor::onRefresh()
{
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::setLocked()
//-----------------------------------------------------------------------------
void AddressEditor::setLocked(bool locked)
{
    autoAssignButton_->setEnabled(!locked);
    csvImportButton_->setEnabled(!locked);
    csvExportButton_->setEnabled(true);
    view_->setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::setupLayout()
//-----------------------------------------------------------------------------
void AddressEditor::setupLayout()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(autoAssignButton_);
    buttonLayout->addWidget(csvImportButton_);
    buttonLayout->addWidget(csvExportButton_);
    buttonLayout->addStretch(1);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(buttonLayout);
    topLayout->addWidget(view_);
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::setupConnections()
//-----------------------------------------------------------------------------
void AddressEditor::setupConnections()
{
    connect(&model_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(autoAssignButton_, SIGNAL(clicked()), &model_, SLOT(autoAssignAddresses()), Qt::UniqueConnection);
    connect(csvImportButton_, SIGNAL(clicked()), this, SLOT(onImport()), Qt::UniqueConnection);
    connect(csvExportButton_, SIGNAL(clicked()), this, SLOT(onExport()), Qt::UniqueConnection);

    connect(view_, SIGNAL(moveItem(QModelIndex const&, QModelIndex const&)),
            &model_, SLOT(onMoveItem(QModelIndex const&, QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::onImport()
//-----------------------------------------------------------------------------
void AddressEditor::onImport()
{
    QSettings settings;
    QString homePath = settings.value(QString("Library/DefaultLocation"), 
                                      QCoreApplication::applicationDirPath()).toString();

    QString path = QFileDialog::getOpenFileName(this, tr("Select CSV file to import"), homePath);

    if (!path.isEmpty())
    {
        model_.importCSV(path);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressEditor::onExport()
//-----------------------------------------------------------------------------
void AddressEditor::onExport()
{
    QSettings settings;
    QString homePath = settings.value(QString("Library/DefaultLocation"), 
        QCoreApplication::applicationDirPath()).toString();

    QString path = QFileDialog::getSaveFileName(this, tr("Save a CSV file"), 
                                                homePath, tr("CSV files (*.csv)"));

    if (!path.isEmpty())
    {
        model_.exportCSV(path);
    }
}
