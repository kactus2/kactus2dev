/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "memorymapgraphitem.h"
#include "addressblockgraphitem.h"

#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> memoryMap,
									   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
memoryMap_(memoryMap) 
{
	Q_ASSERT(memoryMap_);
	QBrush brush(KactusColors::MEM_MAP_COLOR);
	setDefaultBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::~MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::~MemoryMapGraphItem() 
{
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::refresh()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::refresh() 
{
	
	setName(memoryMap_->getName());
    setOverlappingTop(memoryMap_->getFirstAddress());
    setOverlappingBottom(memoryMap_->getLastAddress());

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + memoryMap_->getName() + "<br>" +
        "<b>AUB: </b>" + QString::number(memoryMap_->getAddressUnitBits()) + "<br>" +
        "<b>First address: </b>" + AddressSpaceVisualizationItem::addr2Str(getOffset(),getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + AddressSpaceVisualizationItem::addr2Str(memoryMap_->getLastAddress(), 
        getBitWidth()));
  
    // set the positions for the children.
    if (isExpanded())
    {
        foreach (MemoryVisualizationItem* child, childItems_)
        {
            AddressBlockGraphItem* childRegister = qobject_cast<AddressBlockGraphItem*>(child);
            if (childRegister)
            {
                childRegister->refreshItem();
            }        
        }
        MemoryVisualizationItem::reorganizeChildren();
    }
    else
    {
        ExpandableItem::reorganizeChildren();
    }

	// set the positions for the children
	//MemoryVisualizationItem::reorganizeChildren();

	MemoryMapScene* memScene = static_cast<MemoryMapScene*>(scene());
	Q_ASSERT(memScene);
	memScene->rePosition();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getOffset() const 
{
	return memoryMap_->getFirstAddress();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int MemoryMapGraphItem::getBitWidth() const 
{
	return memoryMap_->getMaxWidth();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int MemoryMapGraphItem::getAddressUnitSize() const 
{
	return memoryMap_->getAddressUnitBits();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getLastAddress() const 
{
	return memoryMap_->getLastAddress();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::itemTotalWidth()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphItem::itemTotalWidth() const 
{
    return VisualizerItem::DEFAULT_WIDTH;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::setWidth()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::setWidth( qreal width ) {
    if (childWidth_ != width - MemoryVisualizationItem::CHILD_INDENTATION)
    {
        VisualizerItem::setWidth(width);
        childWidth_ = width - MemoryVisualizationItem::CHILD_INDENTATION;

        reorganizeChildren();
    }
}
