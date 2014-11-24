/* 
 *  	Created on: 13.10.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapeditor.cpp
 *		Project: Kactus 2
 */

#include "localmemorymapeditor.h"
#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/views/EditableTableView/editabletableview.h>
#include <editors/ComponentEditor/memoryMaps/memorymapmodel.h>
#include <editors/ComponentEditor/memoryMaps/memorymapdelegate.h>
#include <common/widgets/summaryLabel/summarylabel.h>
#include <editors/ComponentEditor/memoryMaps/memorymapproxy.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::LocalMemoryMapEditor()
//-----------------------------------------------------------------------------
LocalMemoryMapEditor::LocalMemoryMapEditor(QSharedPointer<MemoryMap> memoryMap,
                                           QSharedPointer<Component> component,
                                           LibraryInterface* handler,
                                           QWidget *parent)
    : QGroupBox(tr("Local memory map"), parent),
      enableMemMapBox_(new QCheckBox("Use local memory map", this)),
      localMemoryMap_(memoryMap),
      nameEditor_(new NameGroupEditor(memoryMap->getNameGroup(), this)),
      view_(new EditableTableView(this)),
      proxy_(new MemoryMapProxy(this)),
      model_(new MemoryMapModel(memoryMap, component->getChoices(), this)),
      component_(component),
      handler_(handler)
{
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

    connect(enableMemMapBox_, SIGNAL(stateChanged(int)), 
        this, SLOT(onEnableChanged()), Qt::UniqueConnection);

	QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(enableMemMapBox_);
	layout->addWidget(nameEditor_);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::~LocalMemoryMapEditor()
//-----------------------------------------------------------------------------
LocalMemoryMapEditor::~LocalMemoryMapEditor() {
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::isValid()
//-----------------------------------------------------------------------------
bool LocalMemoryMapEditor::isValid() const {
	if (!nameEditor_->isValid()) {
		return false;
	}
	if (!model_->isValid()) {
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::refresh()
//-----------------------------------------------------------------------------
void LocalMemoryMapEditor::refresh() {
	nameEditor_->refresh();
	view_->update();
    
    onEnableChanged();
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::onEnableChanged()
//-----------------------------------------------------------------------------
void LocalMemoryMapEditor::onEnableChanged()
{
    if (enableMemMapBox_->checkState() == Qt::Unchecked && !localMemoryMap_->isEmpty())
    {
        enableMemMapBox_->setCheckState(Qt::Checked);
        emit errorMessage("Cannot unselect local memory map since all fields are not empty.");        
    }

    if (enableMemMapBox_->checkState() == Qt::Checked)
    {
        nameEditor_->setEnabled(true);
        view_->setEnabled(true);
    }
    else
    {
        nameEditor_->setEnabled(false);
        view_->setEnabled(false);
    }
}

