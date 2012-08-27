/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockeditor.cpp
 *		Project: Kactus 2
 */

#include "addressblockeditor.h"
#include <common/views/EditableTableView/editabletableview.h>
#include "addressblockmodel.h"
#include "addressblockdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>


#include <QVBoxLayout>

AddressBlockEditor::AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
									   QSharedPointer<Component> component,
									   QWidget* parent /*= 0*/):
ItemEditor(component, parent),
view_(new EditableTableView(this)),
proxy_(new QSortFilterProxyModel(this)),
model_(new AddressBlockModel(addressBlock, this)) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Registers summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	// items can not be dragged
	view_->setItemsDraggable(false);

	view_->setSortingEnabled(false);

	view_->setItemDelegate(new AddressBlockDelegate(this));

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

AddressBlockEditor::~AddressBlockEditor() {
}

bool AddressBlockEditor::isValid() const {
	return model_->isValid();
}

void AddressBlockEditor::refresh() {
	view_->update();
}
