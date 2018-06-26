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

#include <editors/BusDefinitionEditor/AbstractionDefinitionPortsSortFilter.h>
#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::AbstractionWirePortsEditor()
//-----------------------------------------------------------------------------
AbstractionWirePortsEditor::AbstractionWirePortsEditor(QWidget *parent) :
QWidget(parent),
portView_(this),
portProxy_(),
portModel_(this),
portDelegate_(this)
{
    AbstractionDefinitionPortsSortFilter::ColumnHandles wireColumns;
    wireColumns.nameColumn_ = LogicalPortColumns::NAME;
    wireColumns.modeColumn_ = LogicalPortColumns::MODE;
    wireColumns.systemGroupColumn_ = LogicalPortColumns::SYSTEM_GROUP;
    wireColumns.descriptionColumn_ = LogicalPortColumns::DESCRIPTION;

    portProxy_ = new AbstractionDefinitionPortsSortFilter(wireColumns, this);
    portProxy_->setSourceModel(&portModel_);

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

    connect(&portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRenamed(const QString&, const QString&)),
        this, SIGNAL(portRenamed(const QString&, const QString&)), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRenamed(const QString&, const QString&)),
        portProxy_, SLOT(invalidate()), Qt::UniqueConnection);
    connect(&portModel_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)),
        this, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);

    connect(&portView_, SIGNAL(addItem(const QModelIndex&)), &portModel_, SLOT(addSignal()), Qt::UniqueConnection);
    connect(&portView_, SIGNAL(removeItem(const QModelIndex&)),
        &portModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::onAddMaster()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::onAddMaster()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_.addMaster(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::onAddSlave()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::onAddSlave()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_.addSlave(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::onAddSystem()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::onAddSystem()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_.addSystem(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::onAddAllSystems()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::onAddAllSystems()
{
    QModelIndexList selection = getSelectedIndexes();
    portModel_.addAllSystems(selection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::getSelectedIndexes()
//-----------------------------------------------------------------------------
QModelIndexList AbstractionWirePortsEditor::getSelectedIndexes()
{
    QModelIndexList selection;
    foreach(QModelIndex index, portView_.selected())
    {
        selection.append(portProxy_->mapToSource(index));
    }

    return selection;
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::save()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::save()
{
    portModel_.save();
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::setAbsDef()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    portModel_.setAbsDef(absDef);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::setBusDef(QSharedPointer<BusDefinition> busDefinition)
{
    portDelegate_.setBusDef(busDefinition);
    portModel_.setBusDef(busDefinition);
}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsEditor::setupLayout()
//-----------------------------------------------------------------------------
void AbstractionWirePortsEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&portView_);
}
