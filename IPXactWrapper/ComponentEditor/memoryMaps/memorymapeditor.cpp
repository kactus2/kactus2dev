/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapeditor.cpp
 *		Project: Kactus 2
 */

#include "memorymapeditor.h"
#include <common/views/EditableTableView/editabletableview.h>
#include "memorymapmodel.h"
#include "memorymapdelegate.h"

#include <QVBoxLayout>

MemoryMapEditor::MemoryMapEditor(QSharedPointer<Component> component,
								 QSharedPointer<MemoryMap> memoryMap,
								 QWidget* parent /*= 0*/ ):
ItemEditor(component, parent),
view_(new EditableTableView(this)),
proxy_(new QSortFilterProxyModel(this)),
model_(new MemoryMapModel(memoryMap, this)) {

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	// items can not be dragged
	view_->setItemsDraggable(false);

	view_->setSortingEnabled(false);

	view_->setItemDelegate(new MemoryMapDelegate(this));

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

MemoryMapEditor::~MemoryMapEditor() {
}

bool MemoryMapEditor::isValid() const {
	return model_->isValid();
}

void MemoryMapEditor::refresh() {
	view_->update();
}
