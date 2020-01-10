//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2018
//
// Description:
// Editor for the transactional ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionTransactionalPortsEditor.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <editors/BusDefinitionEditor/AbstractionPortsModel.h>
#include <editors/BusDefinitionEditor/AbstractionTransactionalPortsSortFilter.h>
#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::AbstractionTransactionalPortsEditor()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsEditor::AbstractionTransactionalPortsEditor(AbstractionPortsModel* portModel,
    LibraryInterface* libraryAccess, QWidget *parent):
QWidget(parent),
portView_(this),
portProxy_(),
portModel_(portModel),
portDelegate_(libraryAccess, this)
{
    portProxy_ = new AbstractionTransactionalPortsSortFilter(this);
    portProxy_->setSourceModel(portModel_);

    portView_.setModel(portProxy_);
    portView_.setSortingEnabled(true);
    portView_.setItemDelegate(&portDelegate_);
    portView_.setAllowImportExport(true);
    portView_.setItemsDraggable(false);
    portView_.setAlternatingRowColors(false);
    portView_.sortByColumn(0, Qt::AscendingOrder);

    connect(&portView_, SIGNAL(addMaster()), this, SLOT(onAddMaster()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(addSlave()), this, SLOT(onAddSlave()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(addSystem()), this, SLOT(onAddSystem()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(addAllSystems()), this, SLOT(onAddAllSystems()), Qt::UniqueConnection);

    connect(portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)),
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(&portView_, SIGNAL(addItem(const QModelIndex&)),
        portModel_, SLOT(addTransactionalSignal()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(removeItem(const QModelIndex&)),
        portModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::onAddMaster()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::onAddMaster()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addMaster(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::onAddSlave()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::onAddSlave()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addSlave(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::onAddSystem()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::onAddSystem()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addSystem(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::onAddAllSystems()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::onAddAllSystems()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addAllSystems(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::getSelectedIndexes()
//-----------------------------------------------------------------------------
QModelIndexList AbstractionTransactionalPortsEditor::getSelectedIndexes()
{
    QModelIndexList selection;
    foreach(QModelIndex index, portView_.selected())
    {
        selection.append(portProxy_->mapToSource(index));
    }

    return selection;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::save()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::save()
{
    portModel_->save();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    portDelegate_.setBusDef(busDefinition);
    portModel_->setBusDef(busDefinition);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::setupLayout()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&portView_);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsEditor::hideTransactionalColumns()
//-----------------------------------------------------------------------------
void AbstractionTransactionalPortsEditor::hideWireColumns()
{

    QHeaderView* viewHeader = portView_.horizontalHeader();

    viewHeader->hideSection(LogicalPortColumns::DIRECTION);
    viewHeader->hideSection(LogicalPortColumns::WIDTH);
    viewHeader->hideSection(LogicalPortColumns::DEFAULT_VALUE);
    viewHeader->hideSection(LogicalPortColumns::DRIVER);
}
