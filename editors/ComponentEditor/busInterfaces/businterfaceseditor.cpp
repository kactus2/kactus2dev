//-----------------------------------------------------------------------------
// File: businterfaceseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// Editor to add/remove/edit the bus interfaces of a component.
//-----------------------------------------------------------------------------

#include "businterfaceseditor.h"

#include "businterfacesdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::BusInterfacesEditor()
//-----------------------------------------------------------------------------
BusInterfacesEditor::BusInterfacesEditor(LibraryInterface* handler,
    QSharedPointer<Component> component,
    QSharedPointer<BusInterfaceValidator> validator,
    QSharedPointer<ParameterFinder> parameterFinder,
    QWidget* parent):
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(handler, component, validator, parameterFinder, this)
{
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Bus interfaces"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString componentPath = handler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/busIfListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);

    // Items can be dragged to change positions. Drop is enabled for vlnv columns.
    view_.setItemsDraggable(true);
    view_.viewport()->setAcceptDrops(true); 
    view_.setDropIndicatorShown(true);   
    view_.setDragDropMode(QAbstractItemView::DropOnly);
    view_.setItemDelegate(new BusInterfacesDelegate(this));    
    
	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(busifAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(busifRemoved(int)),	this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(busIfMoved(int, int)), this, SIGNAL(childMoved(int, int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
        &model_, SLOT(onMoveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::~BusInterfacesEditor()
//-----------------------------------------------------------------------------
BusInterfacesEditor::~BusInterfacesEditor()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::refresh()
//-----------------------------------------------------------------------------
void BusInterfacesEditor::refresh()
{
	view_.setModel(&model_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::showEvent()
//-----------------------------------------------------------------------------
void BusInterfacesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/businterfaces.html");
}
