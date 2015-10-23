//-----------------------------------------------------------------------------
// File: componenteditoraddrspaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2012
//
// Description:
// The item for a single address space in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditoraddrspaceitem.h"
#include "componenteditoraddrblockitem.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/addressSpaces/localMemoryMap/localmemorymapgraphitem.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
                                                           QSharedPointer<ReferenceCounter> referenceCounter,
                                                           QSharedPointer<ParameterFinder> parameterFinder,
                                                           QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                           QSharedPointer<ExpressionParser> expressionParser,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpace_(addrSpace),
localMemMap_(addrSpace->getLocalMemoryMap()),
memoryBlocks_(addrSpace->getLocalMemoryMap()->getMemoryBlocks()),
graphItem_(NULL),
localMemMapVisualizer_(new MemoryMapsVisualizer()),
addrSpaceVisualizer_(new AddressSpaceVisualizer(addrSpace, expressionParser)),
expressionParser_(expressionParser)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorAddrSpaceItem"));

    graphItem_ = new LocalMemoryMapGraphItem(addrSpace_, localMemMap_, expressionParser_);
	localMemMapVisualizer_->addMemoryMapItem(graphItem_);
	graphItem_->refresh();

	foreach (QSharedPointer<MemoryBlockBase> block, *memoryBlocks_)
    {
		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = block.dynamicCast<AddressBlock>();
		if (addrBlock)
        {
            QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem
                (new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, referenceCounter_,
                parameterFinder_, expressionFormatter_,expressionParser_, this));
            
            int addressUnitBits = expressionParser_->parseExpression(addrSpace_->getAddressUnitBits()).toInt();
            addrBlockItem->addressUnitBitsChanged(addressUnitBits);

			addrBlockItem->setVisualizer(localMemMapVisualizer_);
			childItems_.append(addrBlockItem);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::~ComponentEditorAddrSpaceItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrSpaceItem::~ComponentEditorAddrSpaceItem()
{
	if (localMemMapVisualizer_)
    {
		delete localMemMapVisualizer_;
		localMemMapVisualizer_ = NULL;
	}
	if (addrSpaceVisualizer_)
    {
		delete addrSpaceVisualizer_;
		addrSpaceVisualizer_ = NULL;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrSpaceItem::text() const
{
	return addrSpace_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorAddrSpaceItem::isValid() const
{
// 	return addrSpace_->isValid(component_->getChoices(), component_->getRemapStateNames());
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorAddrSpaceItem::editor()
{
	if (!editor_)
    {
		editor_ = new AddressSpaceEditor(component_, libHandler_, addrSpace_, parameterFinder_, 
            expressionFormatter_, expressionParser_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),	this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrSpaceItem::getTooltip() const
{
	return tr("Address space defines a logical address space seen by a master bus interface");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::createChild(int index)
{
    QSharedPointer<MemoryBlockBase> block = memoryBlocks_->at(index);
	QSharedPointer<AddressBlock> addrBlock = block.dynamicCast<AddressBlock>();
	if (addrBlock)
    {
		QSharedPointer<ComponentEditorAddrBlockItem> addressBlockItem
            (new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, this));
		addressBlockItem->setLocked(locked_);

        int adddressUnitBits = expressionParser_->parseExpression(addrSpace_->getAddressUnitBits()).toInt();
        addressBlockItem->addressUnitBitsChanged(adddressUnitBits);

		if (localMemMapVisualizer_)
        {
			addressBlockItem->setVisualizer(localMemMapVisualizer_);
		}
		childItems_.insert(index, addressBlockItem);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorAddrSpaceItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::updateGraphics()
{
	graphItem_->refresh();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::removeGraphicsItem()
{
	Q_ASSERT(graphItem_);

	// remove the graph item from the scene
	localMemMapVisualizer_->removeMemoryMapItem(graphItem_);

	disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

	// delete the graph item
	delete graphItem_;
	graphItem_ = NULL;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorAddrSpaceItem::visualizer()
{
	return addrSpaceVisualizer_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::onGraphicsChanged()
{
	addrSpaceVisualizer_->refresh();
}
