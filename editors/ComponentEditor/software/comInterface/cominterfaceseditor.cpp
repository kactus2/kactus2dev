//-----------------------------------------------------------------------------
// File: cominterfaceseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 28.06.2012
//
// Description:
// The editor to add/remove/edit the COM interfaces of a component.
//-----------------------------------------------------------------------------

#include "cominterfaceseditor.h"

#include "cominterfacesdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: ComInterfacesEditor::ComInterfacesEditor()
//-----------------------------------------------------------------------------
ComInterfacesEditor::ComInterfacesEditor(LibraryInterface* libHandler, QSharedPointer<Component> component,
    QWidget* parent):
ItemEditor(component, libHandler, parent),
    view_(this),
    proxy_(this),
    model_(libHandler, component, this)
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("COM interfaces summary"), this, true);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString componentPath = libHandler->getDirectoryPath(component->getVlnv());
	QString defaultPath = QString("%1/comIfListing.csv").arg(componentPath);
	view_.setDefaultImportExportPath(defaultPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);
    view_.viewport()->setAcceptDrops(true); 
    view_.setDropIndicatorShown(true);   
    view_.setDragDropMode(QAbstractItemView::DropOnly);
	view_.setItemDelegate(new ComInterfacesDelegate(this));

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(comIfAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(comIfRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(stdRevisionMismatch()), this, SLOT(showStdRevisionMismatchWarning()), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)), 
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesEditor::~ComInterfacesEditor()
//-----------------------------------------------------------------------------
ComInterfacesEditor::~ComInterfacesEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesEditor::isValid()
//-----------------------------------------------------------------------------
bool ComInterfacesEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesEditor::refresh()
//-----------------------------------------------------------------------------
void ComInterfacesEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesEditor::showStdRevisionMismatchWarning()
//-----------------------------------------------------------------------------
void ComInterfacesEditor::showStdRevisionMismatchWarning()
{
    QMessageBox::warning(this, QCoreApplication::applicationName(),
        tr("Dropped item cannot use different IP-XACT standard revision than the item being edited."),
        QMessageBox::Close, QMessageBox::Close);
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesEditor::showEvent()
//-----------------------------------------------------------------------------
void ComInterfacesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/cominterfaces.html");
}
