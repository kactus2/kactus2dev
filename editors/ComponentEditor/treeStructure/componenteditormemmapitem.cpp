/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapitem.h"
#include <editors/ComponentEditor/memoryMaps/memorymapeditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include "componenteditoraddrblockitem.h"
#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::ComponentEditorMemMapItem()
//-----------------------------------------------------------------------------
ComponentEditorMemMapItem::ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap, 
													 ComponentEditorTreeModel* model,
													 LibraryInterface* libHandler,
													 QSharedPointer<Component> component,
                                                     QSharedPointer<ReferenceCounter> referenceCounter,
                                                     QSharedPointer<ParameterFinder> parameterFinder,
                                                     QSharedPointer<ExpressionFormatter> expressionFormatter,
													 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryMap_(memoryMap),
items_(memoryMap->getItems()),
visualizer_(NULL),
graphItem_(NULL)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorMemMapItem"));

	// create the child objects in the tree
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
		
		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) 
        {
			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, referenceCounter_,
                parameterFinder_, expressionFormatter_, this));
			childItems_.append(addrBlockItem);

            addrBlockItem->addressUnitBitsChanged(memoryMap_->getAddressUnitBits());
		}
	}

	Q_ASSERT(memoryMap_);
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::~ComponentEditorMemMapItem()
//-----------------------------------------------------------------------------
ComponentEditorMemMapItem::~ComponentEditorMemMapItem()
{

}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorMemMapItem::text() const
{
	return memoryMap_->getName();
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorMemMapItem::isValid() const
{
	return memoryMap_->isValid(component_->getChoices());
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorMemMapItem::editor()
{
	if (!editor_)
    {
		editor_ = new MemoryMapEditor(component_, libHandler_, memoryMap_, parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorMemMapItem::getTooltip() const
{
	return tr("Contains the details of a single memory map that can be referenced"
		" by containing component's slave interfaces");
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::createChild( int index )
{
	QSharedPointer<MemoryMapItem> memItem = items_[index];
	QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
	if (addrBlock) {
		QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
			new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, referenceCounter_,
            parameterFinder_, expressionFormatter_, this));
		
        addrBlockItem->setLocked(locked_);
        addrBlockItem->addressUnitBitsChanged(memoryMap_->getAddressUnitBits());

		if (visualizer_)
        {
			addrBlockItem->setVisualizer(visualizer_);
		}

        updateGraphics();

		childItems_.insert(index, addrBlockItem);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorMemMapItem::visualizer()
{
	return visualizer_;
}	

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	graphItem_ = new MemoryMapGraphItem(memoryMap_);
	visualizer_->addMemoryMapItem(graphItem_);

	// update the visualizers of address block items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_) {
		QSharedPointer<ComponentEditorAddrBlockItem> addrItem = item.staticCast<ComponentEditorAddrBlockItem>();
		addrItem->setVisualizer(visualizer_);
	}

    updateGraphics();

	connect(graphItem_, SIGNAL(selectEditor()),	this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorMemMapItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::updateGraphics()
{
	graphItem_->refresh();
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::removeGraphicsItem()
{
	Q_ASSERT(graphItem_);

	// remove the graph item from the scene
	visualizer_->removeMemoryMapItem(graphItem_);

	disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

	// delete the graph item
	delete graphItem_;
	graphItem_ = NULL;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::changeAdressUnitBitsOnAddressBlocks()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::changeAdressUnitBitsOnAddressBlocks()
{
    foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_)
    {
        QSharedPointer<ComponentEditorAddrBlockItem> castChildItem = 
            qobject_cast<QSharedPointer<ComponentEditorAddrBlockItem> >(childItem);
        castChildItem->addressUnitBitsChanged(memoryMap_->getAddressUnitBits());
    }
}