//-----------------------------------------------------------------------------
// File: AbstractionTypesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.01.2018
//
// Description:
// Editor for the bus interface abstraction definitions.
//-----------------------------------------------------------------------------

#include "AbstractionTypesEditor.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/busInterfaces/AbstractionTypesModel.h>
#include <editors/ComponentEditor/busInterfaces/AbstractionTypesDelegate.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QHeaderView>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AbstractionTypesEditor::AbstractionTypesEditor()
//-----------------------------------------------------------------------------
AbstractionTypesEditor::AbstractionTypesEditor(QSharedPointer<Component> component, LibraryInterface* library,
    AbstractionTypeInterface* abstractionTypeInterface, QWidget* parentWindow, QWidget* parent):
QGroupBox(parent),
abstractionTypeInterface_(abstractionTypeInterface),
abstractionView_(new EditableTableView(this)),
abstractionModel_(new AbstractionTypesModel(abstractionTypeInterface_, this)),
abstractionDelegate_(new AbstractionTypesDelegate(component, library, parentWindow, this)),
library_(library)
{
    setTitle(QStringLiteral("Abstraction definition"));

    abstractionView_->setModel(abstractionModel_);
    abstractionView_->setItemDelegate(abstractionDelegate_);
    abstractionView_->setAlternatingRowColors(false);
    abstractionView_->setSortingEnabled(false);
    abstractionView_->horizontalHeader()->setStretchLastSection(true);
    abstractionView_->setItemsDraggable(false);
    abstractionView_->viewport()->setAcceptDrops(true); 
    abstractionView_->setDropIndicatorShown(true);   
    abstractionView_->setDragDropMode(QAbstractItemView::DropOnly);

    connectSignals();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(abstractionView_);
    layout->setContentsMargins(3, 0, 3, 3);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesEditor::~AbstractionTypesEditor()
//-----------------------------------------------------------------------------
AbstractionTypesEditor::~AbstractionTypesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesEditor::connectSignals()
//-----------------------------------------------------------------------------
void AbstractionTypesEditor::connectSignals()
{
    connect(abstractionModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(abstractionModel_, SIGNAL(dataChanged(QModelIndex const&, QModelIndex const&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(abstractionView_, SIGNAL(addItem(QModelIndex const&)),
        abstractionModel_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(abstractionView_, SIGNAL(removeItem(QModelIndex const&)),
        abstractionModel_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesEditor::setComponent()
//-----------------------------------------------------------------------------
void AbstractionTypesEditor::setComponent(QSharedPointer<Component> newComponent)
{
    abstractionDelegate_->setComponent(newComponent);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesEditor::setBusForModel()
//-----------------------------------------------------------------------------
void AbstractionTypesEditor::setBusForModel()
{
    abstractionModel_->onChangeSelectedBusInterface();
    abstractionView_->resizeColumnsToContents();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesEditor::addNewAbstraction()
//-----------------------------------------------------------------------------
void AbstractionTypesEditor::addNewAbstraction(VLNV const& newAbstraction)
{
    abstractionModel_->addNewAbstractionTypeWithVLNV(newAbstraction);
}
