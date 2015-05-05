//-----------------------------------------------------------------------------
// File: MemoryRemapItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.04.2015
//
// Description:
// The item for a single memory remap in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "MemoryRemapItem.h"
#include "componenteditoraddrblockitem.h"

#include <editors/ComponentEditor/memoryMaps/SingleMemoryMapEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::MemoryRemapItem()
//-----------------------------------------------------------------------------
MemoryRemapItem::MemoryRemapItem(QSharedPointer<AbstractMemoryMap> memoryRemap,
    QSharedPointer<MemoryMap> parentMemoryMap, ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
     QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryRemap_(memoryRemap),
parentMemoryMap_(parentMemoryMap),
items_(memoryRemap->getItems()),
visualizer_(NULL),
graphItem_(NULL),
expressionParser_(expressionParser)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorMemMapItem"));

	// create the child objects in the tree
	foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) 
        {
			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, referenceCounter_,
                parameterFinder_, expressionFormatter_, expressionParser_, this));
			childItems_.append(addrBlockItem);
		}
	}

    changeAdressUnitBitsOnAddressBlocks();

    Q_ASSERT(memoryRemap_);
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::~MemoryMapItem()
//-----------------------------------------------------------------------------
MemoryRemapItem::~MemoryRemapItem()
{
    if (visualizer_)
    {
        QSharedPointer<MemoryRemap> transformedMemoryRemap = memoryRemap_.dynamicCast<MemoryRemap>();
        if (transformedMemoryRemap)
        {
            delete visualizer_;
            visualizer_ = NULL;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::text()
//-----------------------------------------------------------------------------
QString MemoryRemapItem::text() const
{
    if (memoryRemap_->getName() == parentMemoryMap_->getName())
    {
        return QString("Default");
    }

    return memoryRemap_->getName();
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::isValid()
//-----------------------------------------------------------------------------
bool MemoryRemapItem::isValid() const
{
    QSharedPointer<MemoryRemap> transformedMemoryRemap = memoryRemap_.dynamicCast<MemoryRemap>();
    if (transformedMemoryRemap)
    {
        return transformedMemoryRemap->isValid(component_->getChoices(), component_->getRemapStateNames());
    }

    return memoryRemap_->isValid(component_->getChoices());
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* MemoryRemapItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleMemoryMapEditor(component_, memoryRemap_, parentMemoryMap_, libHandler_,
            parameterFinder_, expressionFormatter_, expressionParser_);
        editor_->setProtection(locked_);
        
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
        connect(editor_, SIGNAL(addressUnitBitsChanged()),
            this, SLOT(changeAdressUnitBitsOnAddressBlocks()), Qt::UniqueConnection);

        connect(editor_, SIGNAL(addressUnitBitsChanged()),
            this, SIGNAL(addressUnitBitsChanged()), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::getTooltip()
//-----------------------------------------------------------------------------
QString MemoryRemapItem::getTooltip() const
{
    if(memoryRemap_->getName() == parentMemoryMap_->getName())
    {
        return tr("The default memory map of ") + parentMemoryMap_->getName() + ".";
    }

    return tr("Contains the details of a memory remap.");
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::createChild()
//-----------------------------------------------------------------------------
void MemoryRemapItem::createChild( int index )
{
	QSharedPointer<MemoryMapItem> memItem = items_[index];
	QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
	if (addrBlock)
    {
		QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
			new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, this));
		addrBlockItem->setLocked(locked_);
		
		if (visualizer_)
        {
			addrBlockItem->setVisualizer(visualizer_);
		}

        updateGraphics();

		childItems_.insert(index, addrBlockItem);
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* MemoryRemapItem::visualizer()
{
    return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::setVisualizer()
//-----------------------------------------------------------------------------
void MemoryRemapItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	graphItem_ = new MemoryMapGraphItem(parentMemoryMap_, memoryRemap_);
	visualizer_->addMemoryMapItem(graphItem_);
	graphItem_->refresh();

    changeAdressUnitBitsOnAddressBlocks();

	// update the visualizers of address block items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
		QSharedPointer<ComponentEditorAddrBlockItem> addrItem = item.staticCast<ComponentEditorAddrBlockItem>();
		addrItem->setVisualizer(visualizer_);
	}

	connect(graphItem_, SIGNAL(selectEditor()),	this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* MemoryRemapItem::getGraphicsItem()
{
    return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::updateGraphics()
//-----------------------------------------------------------------------------
void MemoryRemapItem::updateGraphics()
{
    if (graphItem_)
    {
        graphItem_->refresh();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void MemoryRemapItem::removeGraphicsItem()
{
    if (graphItem_)
    {
        // remove the graph item from the scene
        visualizer_->removeMemoryMapItem(graphItem_);

        disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        // delete the graph item
        delete graphItem_;
        graphItem_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::changeAdressUnitBitsOnAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryRemapItem::changeAdressUnitBitsOnAddressBlocks()
{
    foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_)
    {
        QSharedPointer<ComponentEditorAddrBlockItem> castChildItem = 
            qobject_cast<QSharedPointer<ComponentEditorAddrBlockItem> >(childItem);
        castChildItem->addressUnitBitsChanged(parentMemoryMap_->getAddressUnitBits());
    }
}
