//-----------------------------------------------------------------------------
// File: ComponentInstantiationsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Editor for component instantiations.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationsEditor.h"

#include "ComponentInstantiationsColumns.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsEditor::ComponentInstantiationsEditor()
//-----------------------------------------------------------------------------
ComponentInstantiationsEditor::ComponentInstantiationsEditor(QSharedPointer<Component> component, 
    LibraryInterface* handler, QSharedPointer<InstantiationsValidator> validator, QWidget* parent):
ItemEditor(component, handler, parent),
    view_(new EditableTableView(this)),
    proxy_(new QSortFilterProxyModel(this)),
    model_(component, validator, this)
{
	proxy_->setSourceModel(&model_);	
    proxy_->setDynamicSortFilter(false);

	view_->setModel(proxy_);    
    view_->setSortingEnabled(true);
    view_->sortByColumn(ComponentInstantiationsColumns::NAME, Qt::AscendingOrder);

	// items can not be dragged
	view_->setItemsDraggable(false);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(contentChanged()), this, SLOT(onItemChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    SummaryLabel* summaryLabel = new SummaryLabel(tr("Component instantiations summary"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(view_);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsEditor::~ComponentInstantiationsEditor()
//-----------------------------------------------------------------------------
ComponentInstantiationsEditor::~ComponentInstantiationsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsEditor::refresh()
//-----------------------------------------------------------------------------
void ComponentInstantiationsEditor::refresh()
{    
    proxy_->invalidate();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsEditor::showEvent()
//-----------------------------------------------------------------------------
void ComponentInstantiationsEditor::showEvent(QShowEvent* event)
{
    proxy_->invalidate();

	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/designConfigurationInstantiations.html");
}
