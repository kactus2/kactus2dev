/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: portseditor.cpp
 */

#include "portseditor.h"

#include "portsdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include <LibraryManager/libraryinterface.h>
#include <models/component.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QHeaderView>

#include <QDebug>

PortsEditor::PortsEditor(QSharedPointer<Component> component,
						 LibraryInterface* handler,
						 bool showButtons,
						 QWidget *parent):
ItemEditor(component, handler, parent),
importButton_(QIcon(":/icons/graphics/import.png"), tr("Import CSV-file"), this),
exportButton_(QIcon(":/icons/graphics/export.png"), tr("Export CSV-file"), this),
view_(this), 
model_(component, this),
proxy_(this) {

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/portListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	connect(&importButton_, SIGNAL(clicked(bool)),
		&view_, SLOT(onCSVImport()), Qt::UniqueConnection);
	connect(&exportButton_, SIGNAL(clicked(bool)),
		&view_, SLOT(onCSVExport()), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new PortsDelegate(this));

	view_.verticalHeader()->show();

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&importButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&exportButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// if buttons are not displayed
	if (!showButtons) {
		importButton_.hide();
		exportButton_.hide();
	}

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Ports"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_, 1);
	layout->addLayout(buttonLayout);
	layout->setContentsMargins(0, 0, 0, 0);
}

PortsEditor::~PortsEditor() {
}

bool PortsEditor::isValid() const {
	return model_.isValid();
}

void PortsEditor::refresh() {
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::showEvent()
//-----------------------------------------------------------------------------
void PortsEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);
    emit helpUrlRequested("componenteditor/ports.html");
}

void PortsEditor::setAllowImportExport( bool allow ) {
	view_.setAllowImportExport(allow);
}

void PortsEditor::addPort( QSharedPointer<Port> port ) {
	model_.addPort(port);
}

void PortsEditor::removePort( const QString& portName ) {
	// find the index for the model parameter
	QModelIndex portIndex = model_.index(portName);

	// if the model parameter was found
	if (portIndex.isValid()) {
		model_.onRemoveItem(portIndex);
	}
}
