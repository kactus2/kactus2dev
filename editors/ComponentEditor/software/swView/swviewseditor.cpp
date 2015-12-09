//-----------------------------------------------------------------------------
// File: swviewseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The editor to add/remove/edit the software views of a component.
//-----------------------------------------------------------------------------

#include "swviewseditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SWViewsEditor::SWViewsEditor()
//-----------------------------------------------------------------------------
SWViewsEditor::SWViewsEditor(QSharedPointer<Component> component, LibraryInterface* handler, QWidget* parent):
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(handler, component, this)
{
	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Software views"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/swViewListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);

    // items can not be dragged, but drop is enabled for vlnv columns.
	view_.setItemsDraggable(false);
    view_.viewport()->setAcceptDrops(true); 
    view_.setDropIndicatorShown(true);   
    view_.setDragDropMode(QAbstractItemView::DropOnly);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(QModelIndex const&)),
        &model_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(QModelIndex const&)),
		&model_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SWViewsEditor::~SWViewsEditor()
//-----------------------------------------------------------------------------
SWViewsEditor::~SWViewsEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SWViewsEditor::isValid()
//-----------------------------------------------------------------------------
bool SWViewsEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: SWViewsEditor::refresh()
//-----------------------------------------------------------------------------
void SWViewsEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: SWViewsEditor::showEvent()
//-----------------------------------------------------------------------------
void SWViewsEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/swviews.html");
}
