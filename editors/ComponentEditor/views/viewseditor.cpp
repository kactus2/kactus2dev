//-----------------------------------------------------------------------------
// File: viewseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 12.06.2012
//
// Description:
// The editor to add/remove/edit views of a component.
//-----------------------------------------------------------------------------

#include "viewseditor.h"

#include "ViewsDelegate.h"
#include "ViewColumns.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ViewsEditor::ViewsEditor()
//-----------------------------------------------------------------------------
ViewsEditor::ViewsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
                         QSharedPointer<ViewValidator> viewValidator, QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(component, viewValidator, this)
{
	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Views summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);	
    proxy_.setDynamicSortFilter(false);

	view_.setModel(&proxy_);    
    view_.sortByColumn(ViewColumns::NAME_COLUMN, Qt::AscendingOrder);
    view_.sortByColumn(ViewColumns::TYPE_COLUMN, Qt::AscendingOrder);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/viewListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new ViewsDelegate(this));

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(contentChanged()), this, SLOT(onItemChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ViewsEditor::~ViewsEditor()
//-----------------------------------------------------------------------------
ViewsEditor::~ViewsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ViewsEditor::refresh()
//-----------------------------------------------------------------------------
void ViewsEditor::refresh()
{    
    onItemChanged();
    view_.update();
}

//-----------------------------------------------------------------------------
// Function: ViewsEditor::onItemChanged()
//-----------------------------------------------------------------------------
void ViewsEditor::onItemChanged()
{
    view_.sortByColumn(ViewColumns::NAME_COLUMN, Qt::AscendingOrder);
    view_.sortByColumn(ViewColumns::TYPE_COLUMN, Qt::AscendingOrder);
    view_.update();
}

//-----------------------------------------------------------------------------
// Function: ViewsEditor::showEvent()
//-----------------------------------------------------------------------------
void ViewsEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/views.html");
}
