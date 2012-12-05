/* 
 *  	Created on: 11.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapseditor.cpp
 *		Project: Kactus 2
 */

#include "memorymapseditor.h"
#include <common/views/EditableTableView/editabletableview.h>
#include "memorymapsmodel.h"
#include "memorymapsdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QVBoxLayout>

MemoryMapsEditor::MemoryMapsEditor( QSharedPointer<Component> component,
								   QWidget *parent ):
ItemEditor(component, parent),
view_(new EditableTableView(this)),
proxy_(new QSortFilterProxyModel(this)),
model_(new MemoryMapsModel(component, this)) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Memory maps summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	// items can not be dragged
	view_->setItemsDraggable(false);

	view_->setSortingEnabled(true);

	view_->setItemDelegate(new MemoryMapsDelegate(this));

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(memoryMapAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(memoryMapRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
	
	connect(view_, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(onDoubleClick(const QModelIndex&)), Qt::UniqueConnection);
}

MemoryMapsEditor::~MemoryMapsEditor() {
}

bool MemoryMapsEditor::isValid() const {
	return model_->isValid();
}

void MemoryMapsEditor::refresh() {
	view_->update();
}

void MemoryMapsEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/memorymaps.html");
}

void MemoryMapsEditor::onDoubleClick( const QModelIndex& index ) {

	QModelIndex origIndex = proxy_->mapToSource(index);
	
	// index must be valid
	if (!origIndex.isValid()) {
		return;
	}

	// if the column is for interface references
	if (origIndex.column() == MemoryMapsDelegate::INTERFACE_COLUMN) {
		
		// get the names of the interface that refer to selected memory map
		QStringList busIfNames = model_->data(origIndex, MemoryMapsDelegate::USER_DISPLAY_ROLE).toStringList();

		// if there are no bus interfaces or there are many
		if (busIfNames.size() != 1) {
			return;
		}
		
		// inform component editor that bus interface editor should be selected
		emit selectBusInterface(busIfNames.first());
	}
}
