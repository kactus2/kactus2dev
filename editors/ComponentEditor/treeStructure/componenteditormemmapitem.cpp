//-----------------------------------------------------------------------------
// File: componenteditormemmapitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 16.05.2012
//
// Description:
// The item for a single memory map in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditormemmapitem.h"
#include "MemoryRemapItem.h"

#include <editors/ComponentEditor/memoryMaps/SingleMemoryMapEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::ComponentEditorMemMapItem()
//-----------------------------------------------------------------------------
ComponentEditorMemMapItem::ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<MemoryMapValidator> memoryMapValidator, MemoryMapInterface* mapInterface,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryMap_(memoryMap),
expressionParser_(expressionParser),
memoryMapValidator_(memoryMapValidator),
mapInterface_(mapInterface)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorMemMapItem"));

    QSharedPointer<MemoryRemapItem> defaultRemapItem(new MemoryRemapItem(memoryMap_, memoryMap_, model, libHandler,
        component, referenceCounter, parameterFinder, expressionFormatter, expressionParser_, memoryMapValidator_,
        mapInterface_, this));
    defaultRemapItem->setLocked(locked_);

    MemoryMapsVisualizer* memoryRemapVisualizer = new MemoryMapsVisualizer();
    defaultRemapItem->setVisualizer(memoryRemapVisualizer);

    connect(this, SIGNAL(memoryMapNameChanged(QString const&, QString const&)),
        defaultRemapItem.data(), SIGNAL(memoryMapNameChanged(QString const&, QString const&)),
        Qt::UniqueConnection);

    connect(defaultRemapItem.data(), SIGNAL(addressUnitBitsChanged()),
        this, SLOT(changeAdressUnitBitsOnAddressBlocks()), Qt::UniqueConnection);

    childItems_.append(defaultRemapItem);

    // Create all other remaps except for default remap
    const int childCount = memoryMap_->getMemoryRemaps()->count();
    for (int i = 0; i < childCount; ++i)
    {
        ComponentEditorMemMapItem::createChild(i + 1);
    }

	Q_ASSERT(memoryMap_);
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorMemMapItem::text() const
{
	return memoryMap_->name();
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorMemMapItem::isValid() const
{
    return memoryMapValidator_->validate(memoryMap_);
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
//     QSharedPointer<MemoryRemap> memoryRemap = memoryMap_->getMemoryRemaps()->at(index);

    QSharedPointer<MemoryRemapItem> memoryRemapItem (new MemoryRemapItem(memoryRemap, memoryMap_, model_,
        libHandler_, component_, referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_,
        memoryMapValidator_, mapInterface_, this));
    memoryRemapItem->setLocked(locked_);

    MemoryMapsVisualizer* memoryRemapVisualizer = new MemoryMapsVisualizer();
    memoryRemapItem->setVisualizer(memoryRemapVisualizer);

    childItems_.append(memoryRemapItem);

    connect(this, SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)),
        memoryRemapItem.data(), SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)),
        Qt::UniqueConnection);
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

        connect(memoryRemapItem.data(), SIGNAL(addressingChanged()),
            visualizer, SLOT(redoLayout()), Qt::UniqueConnection);
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
void ComponentEditorMemMapItem::onMemoryRemapAdded(int memoryRemapIndex, QString const& mapName)
{
    if (mapName == memoryMap_->name())
    {
        onAddChild(memoryRemapIndex + 1);
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapitem::onMemoryRemapRemoved()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapItem::onMemoryRemapRemoved(int memoryRemapIndex, QString const& mapName)
{
    if (mapName == memoryMap_->name())
    {
        onRemoveChild(memoryRemapIndex + 1);
    }
}