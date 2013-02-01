/* 
 *  	Created on: 13.10.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapeditor.cpp
 *		Project: Kactus 2
 */

#include "localmemorymapeditor.h"
#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/views/EditableTableView/editabletableview.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memorymapmodel.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memorymapdelegate.h>
#include <common/widgets/summaryLabel/summarylabel.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memorymapproxy.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

LocalMemoryMapEditor::LocalMemoryMapEditor(QSharedPointer<MemoryMap> memoryMap,
	QSharedPointer<Component> component,
	LibraryInterface* handler,
										   QWidget *parent):
QGroupBox(tr("Local memory map"), parent),
localMemoryMap_(memoryMap),
nameEditor_(new NameGroupEditor(memoryMap->getNameGroup(), this)),
view_(new EditableTableView(this)),
proxy_(new MemoryMapProxy(this)),
model_(new MemoryMapModel(memoryMap, this)),
component_(component),
handler_(handler) {

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Address blocks"), this);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	const QString compPath = handler_->getDirectoryPath(*component_->getVlnv());
	QString defPath = QString("%1/localAddrBlockList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	// items can not be dragged
	view_->setItemsDraggable(false);
	view_->setItemDelegate(new MemoryMapDelegate(this));
	view_->setSortingEnabled(true);

	view_->setColumnWidth(MemoryMapDelegate::USAGE_COLUMN, 80);
	view_->setColumnWidth(MemoryMapDelegate::NAME_COLUMN, 100);
	view_->setColumnWidth(MemoryMapDelegate::BASE_COLUMN, 100);
	view_->setColumnWidth(MemoryMapDelegate::RANGE_COLUMN, 80);
	view_->setColumnWidth(MemoryMapDelegate::WIDTH_COLUMN, 65);
	view_->setColumnWidth(MemoryMapDelegate::DESCRIPTION_COLUMN, 100);
	view_->setColumnWidth(MemoryMapDelegate::ACCESS_COLUMN, 100);
	view_->sortByColumn(MemoryMapDelegate::BASE_COLUMN, Qt::AscendingOrder);

	connect(nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)),
		this, SIGNAL(itemAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)),
		this, SIGNAL(itemRemoved(int)), Qt::UniqueConnection);

	// connect view to model
	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(nameEditor_);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
}

LocalMemoryMapEditor::~LocalMemoryMapEditor() {
}

bool LocalMemoryMapEditor::isValid() const {
	if (!nameEditor_->isValid()) {
		return false;
	}
	if (!model_->isValid()) {
		return false;
	}
	return true;
}

void LocalMemoryMapEditor::refresh() {
	nameEditor_->refresh();
	view_->update();
}
