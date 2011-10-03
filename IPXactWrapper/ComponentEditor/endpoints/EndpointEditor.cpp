//-----------------------------------------------------------------------------
// File: EndpointEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.9.2011
//
// Description:
// Endpoint editor widget for the IP-XACT component editor.
//-----------------------------------------------------------------------------

#include "EndpointEditor.h"

#include "EndpointDelegate.h"
#include <models/component.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>

EndpointEditor::EndpointEditor(QSharedPointer<Component> component,
                               void* dataPointer, QWidget *parent): 
ItemEditor(component, parent), 
addRowButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeRowButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
view_(this), model_(dataPointer, this) {

    connect(&addRowButton_, SIGNAL(clicked(bool)),
        &model_, SLOT(onAddRow()), Qt::UniqueConnection);
    connect(&removeRowButton_, SIGNAL(clicked(bool)),
        this, SLOT(onRemove()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    // set view to be sortable
    view_.setSortingEnabled(true);
    view_.horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    view_.setSelectionMode(QAbstractItemView::SingleSelection);
    view_.setAlternatingRowColors(true);
    //view_.setSelectionBehavior(QAbstractItemView::SelectRows);
    view_.verticalHeader()->hide();
    view_.setEditTriggers(QAbstractItemView::AllEditTriggers);
    view_.setWordWrap(true);

    // set the delegate
    view_.setItemDelegate(new EndpointDelegate(this));

    // set proxy to do the sorting automatically
    proxy_ = new QSortFilterProxyModel(this);

    // set source model for proxy
    proxy_->setSourceModel(&model_);
    // set proxy to be the source for the view
    view_.setModel(proxy_);

    // sort the view
    view_.sortByColumn(0, Qt::AscendingOrder);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(&addRowButton_, 0, Qt::AlignLeft);
    buttonLayout->addWidget(&removeRowButton_, 0, Qt::AlignLeft);
    buttonLayout->addStretch();

    // create the layout, add widgets to it
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&view_);
    layout->addLayout(buttonLayout);

    model_.restore();
}

EndpointEditor::~EndpointEditor() {
}

void EndpointEditor::onRemove() {

    QModelIndex index = proxy_->mapToSource(view_.currentIndex());

    // if index is valid then remove the row
    if (index.isValid())
        model_.onRemoveRow(index.row());
}

bool EndpointEditor::isValid() const {
    return model_.isValid();
}

void EndpointEditor::makeChanges() {
    model_.apply();
}
