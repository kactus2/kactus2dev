/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "memorymapgraphitem.h"
#include "addressblockgraphitem.h"

#include <models/memorymapitem.h>
#include <models/addressblock.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

#include <QDebug>

MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> memoryMap,
									   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
memoryMap_(memoryMap) {

	Q_ASSERT(memoryMap_);
	QBrush brush(KactusColors::MEM_MAP_COLOR);
	setBrush(brush);
	ExpandableItem::setExpansionBrush(brush);
}

MemoryMapGraphItem::~MemoryMapGraphItem() {
}

void MemoryMapGraphItem::refresh() {
	
	setName(memoryMap_->getName());
	//setLeftTopCorner(memoryMap_->getFirstAddress());
    //setLeftBottomCorner(memoryMap_->getLastAddress());
    setOverlappingTop(memoryMap_->getFirstAddress());
    setOverlappingBottom(memoryMap_->getLastAddress());

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryMapScene* memScene = static_cast<MemoryMapScene*>(scene());
	Q_ASSERT(memScene);
	memScene->rePosition();
}

quint64 MemoryMapGraphItem::getOffset() const {
	return memoryMap_->getFirstAddress();
}

int MemoryMapGraphItem::getBitWidth() const {
	return memoryMap_->getMaxWidth();
}

unsigned int MemoryMapGraphItem::getAddressUnitSize() const {
	return memoryMap_->getAddressUnitBits();
}

quint64 MemoryMapGraphItem::getLastAddress() const {
	return memoryMap_->getLastAddress();
}

qreal MemoryMapGraphItem::itemTotalWidth() const {

    return VisualizerItem::MAX_WIDTH;

    /*
	qreal width = VisualizerItem::MAX_WIDTH;

	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		// The larger width
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem) {
			width = qMax(width, childItem->itemTotalWidth());
		}
	}
	return width;*/
}

void MemoryMapGraphItem::setWidth( qreal width ) {
    setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);

    childWidth_ = width - MemoryVisualizationItem::CHILD_INDENTATION;

    MemoryVisualizationItem::reorganizeChildren();
}
