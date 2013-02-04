/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfaceseditor.cpp
 *		Project: Kactus 2
 */

#include "businterfaceseditor.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include "businterfacesdelegate.h"
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

BusInterfacesEditor::BusInterfacesEditor(LibraryInterface* handler,
										 QSharedPointer<Component> component, 
										 QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(handler, component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Bus interfaces"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/busIfListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new BusInterfacesDelegate(this));

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(busifAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(busifRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

BusInterfacesEditor::~BusInterfacesEditor() {
}

bool BusInterfacesEditor::isValid() const {
	return model_.isValid();
}

void BusInterfacesEditor::refresh() {
	view_.update();
}

void BusInterfacesEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/businterfaces.html");
}
