//-----------------------------------------------------------------------------
// File: PortWireTypeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// Editor for the port wire type definitions.
//-----------------------------------------------------------------------------

#include "PortWireTypeEditor.h"

#include <editors/ComponentEditor/ports/PortWireTypeView.h>
#include <editors/ComponentEditor/ports/PortWireTypeModel.h>
#include <editors/ComponentEditor/ports/PortWireTypeDelegate.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: PortWireTypeEditor::PortWireTypeEditor()
//-----------------------------------------------------------------------------
PortWireTypeEditor::PortWireTypeEditor(QSharedPointer<Component> component, QWidget* parent):
QScrollArea(parent),
typeDefinitionView_(new PortWireTypeView(this)),
typeDefinitionModel_(new PortWireTypeModel(this)),
typeDefinitionDelegate_(new PortWireTypeDelegate(component, this))
{
    setWidgetResizable(true);

    typeDefinitionView_->setItemDelegate(typeDefinitionDelegate_);
    typeDefinitionView_->setAlternatingRowColors(false);

    typeDefinitionDelegate_->setNewModel(typeDefinitionModel_);
    typeDefinitionView_->setModel(typeDefinitionModel_);
    typeDefinitionView_->setSortingEnabled(false);

    connectSignals();
    setWidget(typeDefinitionView_);
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeEditor::~PortWireTypeEditor()
//-----------------------------------------------------------------------------
PortWireTypeEditor::~PortWireTypeEditor()
{

}

//-----------------------------------------------------------------------------
// Function: PortWireTypeEditor::connectSignals()
//-----------------------------------------------------------------------------
void PortWireTypeEditor::connectSignals()
{
    connect(typeDefinitionModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(typeDefinitionModel_, SIGNAL(dataChanged(QModelIndex const&, QModelIndex const&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(typeDefinitionView_, SIGNAL(addItems(QModelIndexList const&)),
        typeDefinitionModel_, SLOT(onAddItems(const QModelIndexList&)), Qt::UniqueConnection);
    connect(typeDefinitionView_, SIGNAL(addSubItem(QModelIndexList const&)),
        typeDefinitionModel_, SLOT(onAddViewReferences(QModelIndexList const&)), Qt::UniqueConnection);
    connect(typeDefinitionView_, SIGNAL(removeSelectedItems(QModelIndexList const&)),
        typeDefinitionModel_, SLOT(onRemoveSelectedItems(QModelIndexList const&)), Qt::UniqueConnection);
    connect(typeDefinitionView_, SIGNAL(removeAllSubItems(QModelIndexList const&)), typeDefinitionModel_,
        SLOT(onRemoveAllViewsFromSelectedTypes(QModelIndexList const&)), Qt::UniqueConnection);

    connect(typeDefinitionDelegate_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeEditor::setComponent()
//-----------------------------------------------------------------------------
void PortWireTypeEditor::setComponent(QSharedPointer<Component> newComponent)
{
    typeDefinitionDelegate_->setComponent(newComponent);
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeEditor::setPortForModel()
//-----------------------------------------------------------------------------
void PortWireTypeEditor::setPortForModel(QSharedPointer<Port> newPort)
{
    typeDefinitionModel_->onChangePortSelection(newPort);
    typeDefinitionView_->expandAll();
}
