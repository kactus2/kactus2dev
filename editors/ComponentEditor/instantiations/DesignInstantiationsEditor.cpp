//-----------------------------------------------------------------------------
// File: DesignInstantiationsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Editor for design instantiations.
//-----------------------------------------------------------------------------

#include "DesignInstantiationsEditor.h"

#include "ComponentInstantiationsColumns.h"
#include "DesignInstantiationsDelegate.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsEditor::DesignInstantiationsEditor()
//-----------------------------------------------------------------------------
DesignInstantiationsEditor::DesignInstantiationsEditor(QSharedPointer<Component> component, 
    LibraryInterface* handler, QSharedPointer<InstantiationsValidator> validator, QWidget* parent):
ItemEditor(component, handler, parent),
    view_(new EditableTableView(this)),
    proxy_(new QSortFilterProxyModel(this)),
    model_(component->getDesignInstantiations(), validator, this)
{
	proxy_->setSourceModel(&model_);	
    proxy_->setDynamicSortFilter(false);

	view_->setModel(proxy_);    
    view_->setSortingEnabled(true);
    view_->sortByColumn(ComponentInstantiationsColumns::NAME, Qt::AscendingOrder);

	// items can not be dragged
	view_->setItemsDraggable(false);
    view_->setDropIndicatorShown(true);   
    view_->setDragDropMode(QAbstractItemView::DropOnly);
    
    view_->setItemDelegate(new DesignInstantiationsDelegate(this));

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(designInstantiationAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(designInstantiationRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    SummaryLabel* summaryLabel = new SummaryLabel(tr("Design instantiations summary"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(view_);
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsEditor::~DesignInstantiationsEditor()
//-----------------------------------------------------------------------------
DesignInstantiationsEditor::~DesignInstantiationsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsEditor::refresh()
//-----------------------------------------------------------------------------
void DesignInstantiationsEditor::refresh()
{    
    proxy_->invalidate();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsEditor::showEvent()
//-----------------------------------------------------------------------------
void DesignInstantiationsEditor::showEvent(QShowEvent* event)
{
    proxy_->invalidate();

	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/designInstantiations.html");
}
