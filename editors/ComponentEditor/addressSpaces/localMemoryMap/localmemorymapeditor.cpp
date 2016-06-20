//-----------------------------------------------------------------------------
// File: localmemorymapeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 13.10.2012
//
// Description:
// LocalMemoryMapEditor is used to edit a local memory map of an address space.
//-----------------------------------------------------------------------------

#include "localmemorymapeditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/memoryMaps/MemoryMapColumns.h>
#include <editors/ComponentEditor/memoryMaps/memorymapdelegate.h>
#include <editors/ComponentEditor/memoryMaps/memorymapmodel.h>
#include <editors/ComponentEditor/memoryMaps/ExpressionProxyModel.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>

#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::LocalMemoryMapEditor()
//-----------------------------------------------------------------------------
LocalMemoryMapEditor::LocalMemoryMapEditor(QSharedPointer<AddressSpace> addressSpace,
                                           QSharedPointer<Component> component, LibraryInterface* handler,
                                           QSharedPointer<ParameterFinder> parameterFinder,
                                           QSharedPointer<ExpressionFormatter> expressionFormatter,
                                           QSharedPointer<MemoryMapBaseValidator> memoryMapBaseValidator,
                                           QWidget *parent):
QGroupBox(tr("Local memory map"), parent),
addressSpace_(addressSpace),
localMemoryMap_(0),
nameEditor_(0),
view_(new EditableTableView(this)),
model_(0),
component_(component),
handler_(handler)
{
    bool hasLocalMemoryMap = !addressSpace->getLocalMemoryMap().isNull();

    localMemoryMap_ = addressSpace->getLocalMemoryMap();
    if (!hasLocalMemoryMap)
    {
        localMemoryMap_ = QSharedPointer<MemoryMapBase>(new MemoryMapBase());
    }

    nameEditor_ = (new NameGroupEditor(localMemoryMap_, this));

    setCheckable(true);
    setChecked(hasLocalMemoryMap && !localMemoryMap_->getMemoryBlocks()->isEmpty());

    nameEditor_->setTitle(tr("Memory map name and description"));

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    model_ = new MemoryMapModel(localMemoryMap_, expressionParser, parameterFinder, expressionFormatter,
        memoryMapBaseValidator->getAddressBlockValidator(), addressSpace->getAddressUnitBits(), this);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParameterModel);

    ExpressionProxyModel* proxy = new ExpressionProxyModel(expressionParser, this);
    proxy->setColumnToAcceptExpressions(MemoryMapColumns::BASE_COLUMN);
    proxy->setColumnToAcceptExpressions(MemoryMapColumns::RANGE_COLUMN);

	proxy->setSourceModel(model_);
	view_->setModel(proxy);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Address blocks"), this);

	proxy->setSourceModel(model_);
	view_->setModel(proxy);

	const QString compPath = handler_->getDirectoryPath(component_->getVlnv());
	QString defPath = QString("%1/localAddrBlockList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	// items can not be dragged
	view_->setItemsDraggable(false);
    view_->setItemDelegate(new MemoryMapDelegate(parameterCompleter, parameterFinder, this));
	view_->setSortingEnabled(true);
    view_->setAllowElementCopying(true);

	view_->sortByColumn(MemoryMapColumns::BASE_COLUMN, Qt::AscendingOrder);

    connect(this, SIGNAL(toggled(bool)), this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);

	connect(nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    
	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)), this, SIGNAL(itemAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(itemRemoved(int)), Qt::UniqueConnection);
    
    connect(model_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(itemAdded(int)), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

	// connect view to model
	connect(view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(model_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);

    connect(view_, SIGNAL(copyRows(QModelIndexList)),
        model_, SLOT(onCopyRows(QModelIndexList)), Qt::UniqueConnection);
    connect(view_, SIGNAL(pasteRows()), model_, SLOT(onPasteRows()), Qt::UniqueConnection);

    connect(this, SIGNAL(assignNewAddressUnitBits(QString const&)),
        model_, SLOT(addressUnitBitsUpdated(QString const&)), Qt::UniqueConnection);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(nameEditor_);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::~LocalMemoryMapEditor()
//-----------------------------------------------------------------------------
LocalMemoryMapEditor::~LocalMemoryMapEditor()
{

}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::refresh()
//-----------------------------------------------------------------------------
void LocalMemoryMapEditor::refresh()
{
	nameEditor_->refresh();
	view_->update();
    
    onCheckStateChanged();
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::onCheckStateChanged()
//-----------------------------------------------------------------------------
void LocalMemoryMapEditor::onCheckStateChanged()
{
    /*if (!isChecked() && !localMemoryMap_->getMemoryBlocks()->isEmpty())
    {
        setChecked(true);
        emit errorMessage("Cannot unselect local memory map since all fields are not empty.");        
    }
    */

    if (isChecked())
    {
        addressSpace_->setLocalMemoryMap(localMemoryMap_);
    }
    else
    {
        addressSpace_->setLocalMemoryMap(QSharedPointer<MemoryMapBase>(0));
    }
}
