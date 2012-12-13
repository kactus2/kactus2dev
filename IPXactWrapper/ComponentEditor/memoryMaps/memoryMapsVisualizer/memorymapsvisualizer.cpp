/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsvisualizer.cpp
 *		Project: Kactus 2
 */

#include "memorymapsvisualizer.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapview.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>
#include <common/graphicsItems/visualizeritem.h>
#include <common/graphicsItems/visualizeritem.h>

#include <QHBoxLayout>
#include <QPointF>

MemoryMapsVisualizer::MemoryMapsVisualizer(QSharedPointer<Component> component, QWidget *parent):
ItemVisualizer(component, parent),
view_(new MemoryMapView(this)),
scene_(new MemoryMapScene(this)) {

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	scene_->setMemoryMaps(component_->getMemoryMaps());
	view_->setScene(scene_);
}

MemoryMapsVisualizer::~MemoryMapsVisualizer() {
}

void MemoryMapsVisualizer::addMemoryMapItem( MemoryMapGraphItem* memMapItem ) {
	scene_->addMemGraphItem(memMapItem);
	QPointF position = memMapItem->pos();
	view_->centerOn(position.x() - VisualizerItem::ITEM_HEIGHT, position.y());
}

void MemoryMapsVisualizer::removeMemoryMapItem( MemoryMapGraphItem* memMapItem ) {
	scene_->removeMemGraphItem(memMapItem);
}

QSize MemoryMapsVisualizer::minimumSizeHint() const {
	return QSize(VisualizerItem::MAX_WIDTH + 80, VisualizerItem::ITEM_HEIGHT * 2);
}

QSize MemoryMapsVisualizer::sizeHint() const {
	return minimumSizeHint();
}
