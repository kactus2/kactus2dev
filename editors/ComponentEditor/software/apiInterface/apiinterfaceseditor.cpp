//-----------------------------------------------------------------------------
// File: apiinterfaceseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The editor to add/remove/edit the API interfaces of a component.
//-----------------------------------------------------------------------------

#include "apiinterfaceseditor.h"
#include "apiinterfacesdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ApiInterfacesEditor::ApiInterfacesEditor()
//-----------------------------------------------------------------------------
ApiInterfacesEditor::ApiInterfacesEditor(QSharedPointer<Component> component, 
    LibraryInterface* handler, QWidget* parent):
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(component, this)
{

    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("API interfaces"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&view_);
    layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/apiIfListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);
    view_.viewport()->setAcceptDrops(true); 
    view_.setDropIndicatorShown(true);   
    view_.setDragDropMode(QAbstractItemView::DropOnly);
	view_.setItemDelegate(new ApiInterfacesDelegate(this));

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(apiAdded(int)),	this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(apiRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesEditor::~ApiInterfacesEditor()
//-----------------------------------------------------------------------------
ApiInterfacesEditor::~ApiInterfacesEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesEditor::isValid()
//-----------------------------------------------------------------------------
bool ApiInterfacesEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesEditor::refresh()
//-----------------------------------------------------------------------------
void ApiInterfacesEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesEditor::showEvent()
//-----------------------------------------------------------------------------
void ApiInterfacesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/apiinterfaces.html");
}
