//-----------------------------------------------------------------------------
// File: ModeReferenceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 10.8.2023
//
// Description:
// Editor for mode references.
//-----------------------------------------------------------------------------

#include "ModeReferenceEditor.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QMap>
#include <QVBoxLayout>
#include <QScrollArea>

#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::ModeReferenceEditor()
//-----------------------------------------------------------------------------
ModeReferenceEditor::ModeReferenceEditor(ModeReferenceModel* model, QWidget* parent) :
    QWidget(parent),
    model_(model),
    view_(new EditableTableView(this))
{
    setAttribute(Qt::WA_NoMousePropagation);
    setAutoFillBackground(true);

    auto delegate = new ModeReferenceDelegate(this);
    auto proxy = new QSortFilterProxyModel(this);

    view_->setSortingEnabled(true);
    proxy->setSourceModel(model_);
    view_->setModel(proxy);
    view_->setItemDelegate(delegate);
    view_->setContentsMargins(0, 0, 0, 0);
    view_->sortByColumn(0, Qt::AscendingOrder); // sort by priority by default
    view_->setItemsDraggable(false);

    setupLayout();

    connect(view_, SIGNAL(addItem(QModelIndex const&)), 
        model_, SLOT(onAddRow(QModelIndex const&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(QModelIndex const&)), 
        model_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(invalidateFilter()), proxy, SLOT(invalidate()), Qt::UniqueConnection);
    connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::setModeRefs()
//-----------------------------------------------------------------------------
void ModeReferenceEditor::setModeRefs(QList<QPair<QString, int> > modeRefs) const
{
    model_->setModeRefs(modeRefs);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::getModeRefs()
//-----------------------------------------------------------------------------
QList<QPair<QString, int> > ModeReferenceEditor::getModeRefs() const
{
    return model_->getModeRefs();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::setupLayout()
//-----------------------------------------------------------------------------
void ModeReferenceEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setContentsMargins(0, 0, 0, 0);
    topLayout->setContentsMargins(0, 0, 0, 0);

    topLayout->addWidget(scrollArea);

    QHBoxLayout* scrollLayout = new QHBoxLayout();
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addWidget(view_);
    scrollArea->setLayout(scrollLayout);

    view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
