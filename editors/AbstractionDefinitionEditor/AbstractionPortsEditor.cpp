//-----------------------------------------------------------------------------
// File: AbstractionPortsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.06.2018
//
// Description:
// Editor for the ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionPortsEditor.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include "AbstractionPortsModel.h"
#include "AbstractionWirePortsSortFilter.h"
#include "AbstractionTransactionalPortsSortFilter.h"
#include "AbstractionWirePortsDelegate.h"
#include "AbstractionTransactionalPortsDelegate.h"
#include "LogicalPortColumns.h"

#include <QHeaderView>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::AbstractionPortsEditor()
//-----------------------------------------------------------------------------
AbstractionPortsEditor::AbstractionPortsEditor(LibraryInterface* libraryAccess,
    PortAbstractionInterface* portInterface, AbstractionPortsModel* portModel,
    LogicalPortColumns::AbstractionType type, QWidget* parent):
QWidget(parent),
portView_(this),
portProxy_(nullptr),
portModel_(portModel),
portDelegate_(nullptr)
{
    if (type == LogicalPortColumns::AbstractionType::WIRE)
    {
        portProxy_ = new AbstractionWirePortsSortFilter(portInterface, this);
        portDelegate_ = new AbstractionWirePortsDelegate(libraryAccess, this);
    }
    else
    {
        portProxy_ = new AbstractionTransactionalPortsSortFilter(portInterface, this);
        portDelegate_ = new AbstractionTransactionalPortsDelegate(libraryAccess, this);
    }

    portProxy_->setSourceModel(portModel_);
    
    portView_.setModel(portProxy_);
    portView_.setSortingEnabled(true);
    portView_.setItemDelegate(portDelegate_);
    portView_.setAllowImportExport(true);
    portView_.setItemsDraggable(false);
    portView_.setAlternatingRowColors(false);
    portView_.sortByColumn(0, Qt::AscendingOrder);

    if (type == LogicalPortColumns::AbstractionType::WIRE)
    {
        hideTransactionalColumns();
    }
    else
    {
        hideWireColumns();
    }


    connect(&portView_, SIGNAL(addMaster()), this, SLOT(onAddMaster()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(addSlave()), this, SLOT(onAddSlave()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(addSystem()), this, SLOT(onAddSystem()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(addAllSystems()), this, SLOT(onAddAllSystems()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(resetExtendPorts()), portModel_, SLOT(onResetExtendPorts()), Qt::UniqueConnection);

    connect(portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(portModel_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)),
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    if (type == LogicalPortColumns::AbstractionType::WIRE)
    {
        connect(&portView_, SIGNAL(addItem(const QModelIndex&)),
            portModel_, SLOT(addWireSignal()), Qt::UniqueConnection);
    }
    else
    {
        connect(&portView_, SIGNAL(addItem(const QModelIndex&)),
            portModel_, SLOT(addTransactionalSignal()), Qt::UniqueConnection);
    }

    connect(&portView_, SIGNAL(removeItem(const QModelIndex&)),
        portModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::onAddMaster()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddMaster()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addMaster(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::onAddSlave()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddSlave()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addSlave(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::onAddSystem()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddSystem()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addSystem(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::onAddAllSystems()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddAllSystems()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addAllSystems(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::getSelectedIndexes()
//-----------------------------------------------------------------------------
QModelIndexList AbstractionPortsEditor::getSelectedIndexes()
{
    QModelIndexList selection;
    foreach(QModelIndex index, portView_.selected())
    {
        selection.append(portProxy_->mapToSource(index));
    }

    return selection;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::resetPortModel()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::resetPortModel()
{
    portModel_->resetPortModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    portDelegate_->setBusDef(busDefinition);
    portModel_->setBusDef(busDefinition);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::setupLayout()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&portView_);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::hideTransactionalColumns()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::hideTransactionalColumns()
{
    auto header = portView_.horizontalHeader();
  
    header->hideSection(LogicalPortColumns::INITIATIVE);
    header->hideSection(LogicalPortColumns::KIND);
    header->hideSection(LogicalPortColumns::BUSWIDTH);
    header->hideSection(LogicalPortColumns::PROTOCOLTYPE);
    header->hideSection(LogicalPortColumns::PAYLOADNAME);
    header->hideSection(LogicalPortColumns::PAYLOADTYPE);
    header->hideSection(LogicalPortColumns::PAYLOADEXTENSION);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsEditor::hideWireColumns()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::hideWireColumns()
{
    auto header = portView_.horizontalHeader();
    
    header->hideSection(LogicalPortColumns::DIRECTION);
    header->hideSection(LogicalPortColumns::WIDTH);
    header->hideSection(LogicalPortColumns::DEFAULT_VALUE);
    header->hideSection(LogicalPortColumns::DRIVER);
}
