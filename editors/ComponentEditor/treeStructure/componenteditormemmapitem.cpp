//-----------------------------------------------------------------------------
// File: componenteditormemmapitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.05.2012
//
// Description:
// The item for a single memory map in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditormemmapitem.h"
#include "MemoryRemapItem.h"
#include "componenteditoraddrblockitem.h"

#include <editors/ComponentEditor/memoryMaps/SingleMemoryMapEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>

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
                                                     QSharedPointer<ExpressionParser> expressionParser,
													 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryMap_(memoryMap),
visualizer_(NULL),
graphItem_(NULL),
expressionParser_(expressionParser)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorMemMapItem"));

    QSharedPointer<MemoryRemapItem> defaultRemapItem(new MemoryRemapItem(memoryMap_, memoryMap_, model, libHandler,
        component, referenceCounter, parameterFinder, expressionFormatter, expressionParser_, this));
    defaultRemapItem->setLocked(locked_);

    childItems_.append(defaultRemapItem);

    foreach (QSharedPointer<MemoryRemap> memoryRemap, *memoryMap_->getMemoryRemaps())
    {
        QSharedPointer<MemoryRemapItem> memoryRemapItem(new MemoryRemapItem(memoryRemap, memoryMap_, model,
            libHandler, component, referenceCounter, parameterFinder, expressionFormatter, expressionParser_, this));
        memoryRemapItem->setLocked(locked_);

        childItems_.append(memoryRemapItem);
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
    bool isParentValid = ComponentEditorItem::isValid();

    bool isMemoryMapValid = memoryMap_->isValid(component_->getChoices(), component_->getRemapStateNames());

    return isParentValid && isMemoryMapValid;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorMemMapItem::editor()
{
    return childItems_.at(0)->editor();
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
    QSharedPointer<MemoryRemap> memoryRemap = memoryMap_->getMemoryRemaps()->at(index - 1);

    QSharedPointer<MemoryRemapItem> memoryRemapItem (new MemoryRemapItem(memoryRemap, memoryMap_, model_,
        libHandler_, component_, referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, this));
    memoryRemapItem->setLocked(locked_);

    childItems_.append(memoryRemapItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorMemMapItem::visualizer()
{
    return childItems_.at(0)->visualizer();
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
    QSharedPointer<ComponentEditorItem> item = childItems_.at(0);
    QSharedPointer<MemoryRemapItem> memoryRemapItem = item.staticCast<MemoryRemapItem>();

    if(memoryRemapItem)
    {
        memoryRemapItem->setVisualizer(visualizer);
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorMemMapItem::getGraphicsItem()
{
    return childItems_.at(0)->getGraphicsItem();
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::updateGraphics()
{
    childItems_.at(0)->updateGraphics();
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::removeGraphicsItem()
{
    childItems_.at(0)->removeGraphicsItem();
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::changeAdressUnitBitsOnAddressBlocks()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::changeAdressUnitBitsOnAddressBlocks()
{
    foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_)
    {
        QSharedPointer<MemoryRemapItem> memoryRemap = qobject_cast<QSharedPointer<MemoryRemapItem> >(childItem);
        memoryRemap->changeAdressUnitBitsOnAddressBlocks();
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::onMemoryRemapAdded()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::onMemoryRemapAdded(int memoryRemapIndex, QSharedPointer<MemoryMap> parentMemoryMap)
{
    if (parentMemoryMap == memoryMap_)
    {
        onAddChild(memoryRemapIndex + 1);
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::onMemoryRemapRemoved()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::onMemoryRemapRemoved(int memoryRemapIndex, QSharedPointer<MemoryMap> parentMemoryMap)
{
    if (parentMemoryMap == memoryMap_)
    {
        onRemoveChild(memoryRemapIndex + 1);
    }
}