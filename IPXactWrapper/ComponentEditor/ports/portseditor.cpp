/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: portseditor.cpp
 */

#include "portseditor.h"

#include "portsdelegate.h"

#include <LibraryManager/libraryinterface.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QHeaderView>

PortsEditor::PortsEditor(QSharedPointer<Component> component,
						 void* dataPointer, 
						 LibraryInterface* handler,
						 QWidget *parent):
ItemEditor(component, parent),
importButton_(QIcon(":/icons/graphics/import.png"), tr("Import CSV-file"), this),
exportButton_(QIcon(":/icons/graphics/export.png"), tr("Export CSV-file"), this),
view_(this), 
model_(component, dataPointer, this),
handler_(handler) {

	connect(&importButton_, SIGNAL(clicked(bool)),
		this, SLOT(onImport()), Qt::UniqueConnection);
	connect(&exportButton_, SIGNAL(clicked(bool)),
		this, SLOT(onExport()), Qt::UniqueConnection);

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
    view_.verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

	// set proxy to do the sorting automatically
	proxy_ = new QSortFilterProxyModel(this);

	// set source model for proxy
	proxy_->setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&importButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&exportButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_, 1);
	layout->addLayout(buttonLayout);
}

PortsEditor::~PortsEditor() {
}

bool PortsEditor::isValid() const {
	return model_.isValid();
}

void PortsEditor::onImport() {
	
	QString componentPath = handler_->getPath(*component()->getVlnv());
	QFileInfo fileInfo(componentPath);
	componentPath = fileInfo.absolutePath();
	if (componentPath.isEmpty()) {
		componentPath = QDir::homePath();
	}

	QString location = QFileDialog::getOpenFileName(this, tr("Open file"),
		componentPath, tr("csv-files (*.csv)"));

	// if user clicked cancel
	if (location.isEmpty())
		return;

	QFile file(location);

	// if file can not be opened 
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Error opening file"),
			tr("Could not open file %1 for reading").arg(location));
		return;
	}

	// tell model to import the port information
	model_.importPorts(file);

	file.close();
}

void PortsEditor::onExport() {

	QString componentPath = handler_->getPath(*component()->getVlnv());
	QFileInfo fileInfo(componentPath);
	componentPath = fileInfo.absolutePath();
	if (componentPath.isEmpty()) {
		componentPath = QDir::homePath();
	}

	QString target = QFileDialog::getSaveFileName(this, 
		tr("Set name and location for csv-file"),
		componentPath, tr("csv-files (*.csv)"));

	// if user clicked cancel
	if (target.isEmpty())
		return;

	QFile file(target);

	// if file can not be opened 
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
		QMessageBox::critical(this, tr("Error opening file"),
		tr("Could not open file %1 for writing").arg(target));
		return;
	}

	// tell model to save the information
	model_.exportPorts(file);

	file.close();
}

void PortsEditor::makeChanges() {
	model_.apply();
}
