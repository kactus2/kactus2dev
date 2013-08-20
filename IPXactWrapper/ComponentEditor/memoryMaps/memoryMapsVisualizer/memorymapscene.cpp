/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapscene.cpp
 *		Project: Kactus 2
 */

#include "memorymapscene.h"
#include "memorymapgraphitem.h"

#include <common/graphicsItems/graphicsexpandcollapseitem.h>

#include <QRectF>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: MemoryMapScene()
//-----------------------------------------------------------------------------
MemoryMapScene::MemoryMapScene(QObject *parent):
QGraphicsScene(parent),
memoryMaps_(),
memGraphItems_(),
width_(VisualizerItem::DEFAULT_WIDTH)
{
}

//-----------------------------------------------------------------------------
// Function: ~MemoryMapScene()
//-----------------------------------------------------------------------------
MemoryMapScene::~MemoryMapScene() {
}

//-----------------------------------------------------------------------------
// Function: addMemGraphItem()
//-----------------------------------------------------------------------------
void MemoryMapScene::addMemGraphItem( MemoryMapGraphItem* memGraphItem ) {
	Q_ASSERT(memGraphItem);
	addItem(memGraphItem);
	memGraphItems_.append(memGraphItem);
	connect(memGraphItem, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: removeMemGraphItem()
//-----------------------------------------------------------------------------
void MemoryMapScene::removeMemGraphItem( MemoryMapGraphItem* memGraphItem ) {
	Q_ASSERT(memGraphItem);
	removeItem(memGraphItem);
	memGraphItems_.removeAll(memGraphItem);
}

//-----------------------------------------------------------------------------
// Function: rePosition()
//-----------------------------------------------------------------------------
void MemoryMapScene::rePosition() {
	qreal yCoordinate = 0;
	MemoryMapGraphItem* previous = 0;
	foreach (MemoryMapGraphItem* memMap, memGraphItems_) {        

		// if there is a previous then add the new after it
		if (previous) {
			QRectF previousRect = previous->itemTotalRect();
			previousRect |= previous->boundingRect();

			// update the y coordinate to avoid setting items on top of each other
			yCoordinate += previousRect.bottom() + 10;
		}

		memMap->setPos(0, yCoordinate);
        memMap->setWidth(width_);
		previous = memMap;
	}

	// tell view to draw the items
	invalidate();
}

//-----------------------------------------------------------------------------
// Function: setWidth()
//-----------------------------------------------------------------------------
void MemoryMapScene::setWidth(int width)
{
    if (width > VisualizerItem::DEFAULT_WIDTH)
    {
        width_ = width - MemoryVisualizationItem::CHILD_INDENTATION;
        QRectF rect = sceneRect();
        rect.setWidth(width);
        setSceneRect(rect);
        rePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: wheelEvent()
//-----------------------------------------------------------------------------
void MemoryMapScene::wheelEvent(QGraphicsSceneWheelEvent * wheelEvent)
{   
    int width = sceneRect().width() + wheelEvent->delta() / WHEEL_SENSITIVITY;
    setWidth(width);
    wheelEvent->accept();
}