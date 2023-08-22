//-----------------------------------------------------------------------------
// File: SubspaceMapsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.01.2022
//
// Description:
// The editor to edit the subspace maps of a single memory map.
//-----------------------------------------------------------------------------

#include "SubspaceMapsEditor.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/memoryMaps/MemoryBlockFilter.h>
#include <editors/ComponentEditor/memoryMaps/SubspaceMapColumns.h>
#include <editors/ComponentEditor/memoryMaps/SubspaceMapModel.h>
#include <editors/ComponentEditor/memoryMaps/SubspaceMapDelegate.h>
#include <KactusAPI/include/SubspaceMapInterface.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: MemoryMapEditor::MemoryMapEditor()
//-----------------------------------------------------------------------------
SubspaceMapsEditor::SubspaceMapsEditor(QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    SubspaceMapInterface* subspaceMapInterface, QSharedPointer<QList<QSharedPointer<MemoryBlockBase>>> blocks,
    QWidget* parent):
QGroupBox(tr("Subspace maps summary"), parent),
view_(new EditableTableView(this)),
model_(new SubspaceMapModel(subspaceMapInterface, expressionParser, parameterFinder, this)),
interface_(subspaceMapInterface),
blocks_(blocks),
component_(component)
{
    interface_->setMemoryBlocks(blocks_);
    interface_->setupSubInterfaces(component_);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);

    MemoryBlockFilter* proxy = new MemoryBlockFilter(expressionParser, subspaceMapInterface, this);
    proxy->setColumnToAcceptExpressions(SubspaceMapColumns::BASE);
    proxy->setColumnToAcceptExpressions(SubspaceMapColumns::IS_PRESENT);

	proxy->setSourceModel(model_);
	view_->setModel(proxy);
    view_->setAllowElementCopying(true);
	view_->setItemsDraggable(false);
	view_->setSortingEnabled(true);

    view_->setItemDelegate(new SubspaceMapDelegate(componentParameterModel, parameterFinder, interface_,
        component->getAddressSpaces(), this));
    view_->sortByColumn(SubspaceMapColumns::BASE, Qt::AscendingOrder);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(graphicsChanged(int)), this, SIGNAL(graphicsChanged(int)), Qt::UniqueConnection);
    
	connect(model_, SIGNAL(itemAdded(int)),	this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

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

    connect(model_, SIGNAL(blockNameChanged(QString const&, QString const&)),
        this, SIGNAL(subspaceMapNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

    connect(model_, SIGNAL(invalidateOtherFilter()), this, SIGNAL(invalidateOtherFilter()), Qt::UniqueConnection);

    connect(model_, SIGNAL(childAddressingChanged(int)),
        this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);

    connect(this, SIGNAL(invalidateThisFilter()), proxy, SLOT(invalidate()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapsEditor::refresh()
//-----------------------------------------------------------------------------
void SubspaceMapsEditor::refresh()
{
    view_->update();

    interface_->setMemoryBlocks(blocks_);
    interface_->setupSubInterfaces(component_);
}

