/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceseditor.cpp
 *		Project: Kactus 2
 */

#include "addressspaceseditor.h"
#include "addressspacesdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

AddressSpacesEditor::AddressSpacesEditor(QSharedPointer<Component> component,
										LibraryInterface* handler):
ItemEditor(component, handler),
view_(this),
model_(component, this),
proxy_(this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Address spaces summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);

	view_.setModel(&proxy_);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/addrSpaceList.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);
	
 	view_.setItemDelegate(new AddressSpacesDelegate(this));

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
	connect(&model_, SIGNAL(addrSpaceAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(addrSpaceRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(onDoubleClick(const QModelIndex&)), Qt::UniqueConnection);
}

AddressSpacesEditor::~AddressSpacesEditor() {
}

bool AddressSpacesEditor::isValid() const {
	return model_.isValid();
}

void AddressSpacesEditor::refresh() {
	view_.update();
}

void AddressSpacesEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/addressspaces.html");
}

void AddressSpacesEditor::onDoubleClick( const QModelIndex& index ) {
	QModelIndex origIndex = proxy_.mapToSource(index);

	// index must be valid
	if (!origIndex.isValid()) {
		return;
	}

	// if the column is for interface references
	if (origIndex.column() == AddressSpacesDelegate::INTERFACE_COLUMN) {

		// get the names of the interface that refer to selected memory map
		QStringList busIfNames = model_.data(origIndex, AddressSpacesDelegate::USER_DISPLAY_ROLE).toStringList();

		// if there are no bus interfaces or there are many
		if (busIfNames.size() != 1) {
			return;
		}

		// inform component editor that bus interface editor should be selected
		emit selectBusInterface(busIfNames.first());
	}
}
