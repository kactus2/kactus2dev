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

PortsEditor::PortsEditor(QSharedPointer<Component> component,
						 void* dataPointer, 
						 LibraryInterface* handler,
						 QWidget *parent):
ItemEditor(component, parent),
addRowButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeRowButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
importButton_(QIcon(":/icons/graphics/import.png"), tr("Import CSV-file"), this),
exportButton_(QIcon(":/icons/graphics/export.png"), tr("Export CSV-file"), this),
view_(this), 
model_(dataPointer, this),
handler_(handler) {

	connect(&addRowButton_, SIGNAL(clicked(bool)),
		&model_, SLOT(onAddRow()), Qt::UniqueConnection);
	connect(&removeRowButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRemove()), Qt::UniqueConnection);
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

	// set view to be sortable
	view_.setSortingEnabled(true);
	view_.horizontalHeader()->setStretchLastSection(true);
	view_.horizontalHeader()->setResizeMode(QHeaderView::Interactive);
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setAlternatingRowColors(true);
	//view_.setSelectionBehavior(QAbstractItemView::SelectRows);
	view_.verticalHeader()->hide();
	view_.setEditTriggers(QAbstractItemView::AllEditTriggers);
	view_.setWordWrap(true);

	view_.setItemDelegate(new PortsDelegate(this));

	// set proxy to do the sorting automatically
	proxy_ = new QSortFilterProxyModel(this);

	// set source model for proxy
	proxy_->setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	//buttonLayout->addWidget(&detailButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&addRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&removeRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&importButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&exportButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->addLayout(buttonLayout);
}

PortsEditor::~PortsEditor() {
}

bool PortsEditor::isValid() const {
	return model_.isValid();
}

void PortsEditor::onRemove() {
	QModelIndex index = proxy_->mapToSource(view_.currentIndex());

	// if index is valid then remove the row
	if (index.isValid())
		model_.onRemoveRow(index.row());
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
