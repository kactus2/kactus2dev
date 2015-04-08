//-----------------------------------------------------------------------------
// File: RemapStatesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// The editor to add / remove / edit remap states of a component.
//-----------------------------------------------------------------------------

#include "RemapStatesEditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <QVBoxLayout>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: RemapStatesEditor::RemapStatesEditor()
//-----------------------------------------------------------------------------
RemapStatesEditor::RemapStatesEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
view_(this),
model_(component->getRemapStates(), this)
{
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Remap states summary"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&view_);
    layout->setContentsMargins(0,0,0,0);

    QSortFilterProxyModel* proxy(new QSortFilterProxyModel(this));
    proxy->setSourceModel(&model_);
    proxy->setDynamicSortFilter(true);

    view_.setModel(proxy);
    view_.sortByColumn(RemapStatesModel::NAME_COLUMN, Qt::AscendingOrder);
    view_.setItemsDraggable(false);
    view_.setAlternatingRowColors(false);

    view_.setItemDelegate(new LineEditDelegate(this));

    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(contentChanged()), this, SLOT(onItemChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(remapStateAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(remapStateRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: RemapStatesEditor::~RemapStatesEditor()
//-----------------------------------------------------------------------------
RemapStatesEditor::~RemapStatesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: RemapStatesEditor::isValid()
//-----------------------------------------------------------------------------
bool RemapStatesEditor::isValid() const
{
    return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: RemapStatesEditor::refresh()
//-----------------------------------------------------------------------------
void RemapStatesEditor::refresh()
{
    onItemChanged();
    view_.update();
}

//-----------------------------------------------------------------------------
// Function: RemapStatesEditor::onItemChanged()
//-----------------------------------------------------------------------------
void RemapStatesEditor::onItemChanged()
{
    view_.sortByColumn(RemapStatesModel::NAME_COLUMN, Qt::AscendingOrder);
    view_.update();
}

//-----------------------------------------------------------------------------
// Function: RemapStatesEditor::showEvent()
//-----------------------------------------------------------------------------
void RemapStatesEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/remapStates.html");
}
