//-----------------------------------------------------------------------------
// File: AbstractionWirePortsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.06.2018
//
// Description:
// Editor for the wire ports of an abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionWirePortsEditor.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <editors/BusDefinitionEditor/AbstractionPortsModel.h>
#include <editors/BusDefinitionEditor/AbstractionWirePortsSortFilter.h>
#include <editors/BusDefinitionEditor/AbstractionTransactionalPortsSortFilter.h>
#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

#include <QHeaderView>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::AbstractionWirePortsEditor()
//-----------------------------------------------------------------------------
AbstractionPortsEditor::AbstractionPortsEditor(LibraryInterface* libraryAccess,
    PortAbstractionInterface* portInterface, AbstractionType type, QWidget* parent):
QWidget(parent),
portView_(this),
portProxy_(nullptr),
portModel_(new AbstractionPortsModel(libraryAccess, /*portInterface,*/ this)),
portDelegate_(libraryAccess, this)
{
    if (type == AbstractionType::WIRE)
    {
        portProxy_ = new AbstractionWirePortsSortFilter(portInterface, this);
    }
    else
    {
        portProxy_ = new AbstractionTransactionalPortsSortFilter(portInterface, this);
    }

    portProxy_->setSourceModel(portModel_);
    
    portView_.setModel(portProxy_);
    portView_.setSortingEnabled(true);
    portView_.setItemDelegate(&portDelegate_);
    portView_.setAllowImportExport(true);
    portView_.setItemsDraggable(false);
    portView_.setAlternatingRowColors(false);
    portView_.sortByColumn(0, Qt::AscendingOrder);

    if (type == AbstractionType::WIRE)
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

    if (type == AbstractionType::WIRE)
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
// Function: AbstractionWirePortsEditor::onAddMaster()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddMaster()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addMaster(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::onAddSlave()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddSlave()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addSlave(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::onAddSystem()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddSystem()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addSystem(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::onAddAllSystems()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::onAddAllSystems()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_->addAllSystems(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::getSelectedIndexes()
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
// Function: AbstractionWirePortsEditor::resetPortModel()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::resetPortModel()
{
    //portModel_->resetPortModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    portDelegate_.setBusDef(busDefinition);
    portModel_->setBusDef(busDefinition);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::setupLayout()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&portView_);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::hideTransactionalColumns()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::hideTransactionalColumns()
{
    QHeaderView* viewHeader = portView_.horizontalHeader();

    viewHeader->hideSection(LogicalPortColumns::INITIATIVE);
    viewHeader->hideSection(LogicalPortColumns::KIND);
    viewHeader->hideSection(LogicalPortColumns::BUSWIDTH);
    viewHeader->hideSection(LogicalPortColumns::PROTOCOLTYPE);
    viewHeader->hideSection(LogicalPortColumns::PAYLOADNAME);
    viewHeader->hideSection(LogicalPortColumns::PAYLOADTYPE);
    viewHeader->hideSection(LogicalPortColumns::PAYLOADEXTENSION);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::hideTransactionalColumns()
//-----------------------------------------------------------------------------
void AbstractionPortsEditor::hideWireColumns()
{
    QHeaderView* viewHeader = portView_.horizontalHeader();

    viewHeader->hideSection(LogicalPortColumns::DIRECTION);
    viewHeader->hideSection(LogicalPortColumns::WIDTH);
    viewHeader->hideSection(LogicalPortColumns::DEFAULT_VALUE);
    viewHeader->hideSection(LogicalPortColumns::DRIVER);
}
