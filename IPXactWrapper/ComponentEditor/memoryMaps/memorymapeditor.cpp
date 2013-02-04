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
#include <common/widgets/summaryLabel/summarylabel.h>
#include "memorymapproxy.h"
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

MemoryMapEditor::MemoryMapEditor(QSharedPointer<Component> component,
	LibraryInterface* handler, 
								 QSharedPointer<MemoryMap> memoryMap,
								 QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
view_(new EditableTableView(this)),
proxy_(new MemoryMapProxy(this)),
model_(new MemoryMapModel(memoryMap, this)) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Address blocks summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	//! \brief Enable import/export csv file
	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/addrBlockList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	// items can not be dragged
	view_->setItemsDraggable(false);

	view_->setSortingEnabled(true);

	view_->setItemDelegate(new MemoryMapDelegate(this));

	view_->sortByColumn(MemoryMapDelegate::BASE_COLUMN, Qt::AscendingOrder);

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

void MemoryMapEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/memorymap.html");
}

QSize MemoryMapEditor::sizeHint() const {
	return QSize(MemoryMapEditor::WIDTH, MemoryMapEditor::HEIGHT);
}
