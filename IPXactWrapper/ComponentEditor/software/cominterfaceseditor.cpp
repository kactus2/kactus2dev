/* 
 *  	Created on: 28.6.2012
 *      Author: Antti Kamppi
 * 		filename: cominterfaceseditor.cpp
 *		Project: Kactus 2
 */

#include <common/widgets/summaryLabel/summarylabel.h>
#include <LibraryManager/libraryinterface.h>

#include "cominterfaceseditor.h"
#include "cominterfacesdelegate.h"

ComInterfacesEditor::ComInterfacesEditor(LibraryInterface* libHandler,
										 QSharedPointer<Component> component,
										 QWidget* parent /*= 0*/):
ItemEditor(component, libHandler, parent),
view_(this),
proxy_(this),
model_(libHandler, component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("COM interfaces"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/comIfListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new ComInterfacesDelegate(this));
	view_.setColumnWidth(ComInterfacesDelegate::NAME_COLUMN, NAME_COLUMN_WIDTH);
	view_.setColumnWidth(ComInterfacesDelegate::COM_DEF_COLUMN, COM_DEF_COLUMN_WIDTH);
	view_.setColumnWidth(ComInterfacesDelegate::TRANSFER_TYPE_COLUMN, TRANSFER_COLUMN_WIDTH);
	view_.setColumnWidth(ComInterfacesDelegate::DIRECTION_COLUMN, DIRECTION_COLUMN_WIDTH);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(comIfAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(comIfRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

ComInterfacesEditor::~ComInterfacesEditor() {
}

bool ComInterfacesEditor::isValid() const {
	return model_.isValid();
}

void ComInterfacesEditor::refresh() {
	view_.update();
}

void ComInterfacesEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/cominterfaces.html");
}
