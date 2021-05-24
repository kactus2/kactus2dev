//-----------------------------------------------------------------------------
// File: memorymapscene.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 17.04.2012
//
// Description:
// The graphics scene that contains the memory map graphics items.
//-----------------------------------------------------------------------------

#include "memorymapscene.h"
#include "memorymapgraphitem.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

#include <QRectF>

#include <QDebug>

namespace
{
    const unsigned int ITEM_SPACING = 10;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapScene::MemoryMapScene()
//-----------------------------------------------------------------------------
MemoryMapScene::MemoryMapScene(QObject *parent):
QGraphicsScene(parent),
memGraphItems_(),
width_(VisualizerItem::DEFAULT_WIDTH)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapScene::addMemGraphItem()
//-----------------------------------------------------------------------------
void MemoryMapScene::addMemGraphItem( MemoryMapGraphItem* memGraphItem )
{
	Q_ASSERT(memGraphItem);

    qreal yCoordinate = 0;

    bool isFirstItem = memGraphItems_.isEmpty();
    if (isFirstItem == false)
    {   
        auto lastItem = memGraphItems_.last();
        yCoordinate = lastItem->mapToScene(lastItem->itemTotalRect().bottomLeft()).y() + ITEM_SPACING;
    }

    memGraphItem->setPos(0, yCoordinate);
    memGraphItem->setWidth(width_);

    addItem(memGraphItem);
    memGraphItems_.append(memGraphItem);

    connect(memGraphItem, SIGNAL(expandStateChanged()), this, SLOT(rePosition()), Qt::UniqueConnection);

    // Update scene rect height.   
    QRectF sceneArea = sceneRect();

    if (isFirstItem)
    {
        sceneArea = itemsBoundingRect();
    }

    sceneArea.setBottom(memGraphItem->rect().bottom());

    setSceneRect(sceneArea);
    invalidate();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapScene::removeMemGraphItem()
//-----------------------------------------------------------------------------
void MemoryMapScene::removeMemGraphItem( MemoryMapGraphItem* memGraphItem )
{
	Q_ASSERT(memGraphItem);
        
    removeItem(memGraphItem);
    memGraphItems_.removeAll(memGraphItem);

    disconnect(memGraphItem, SIGNAL(expandStateChanged()), this, SLOT(rePosition()));

    rePosition();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapScene::rePosition()
//-----------------------------------------------------------------------------
void MemoryMapScene::rePosition()
{
	qreal yCoordinate = 0;

	for (auto memMap : memGraphItems_)
    {        
		memMap->setPos(0, yCoordinate);
        yCoordinate += memMap->itemTotalRect().bottom() + ITEM_SPACING;
	}

	// Update scene rect height.   
    QRectF sceneArea = sceneRect();
    sceneArea.setBottom(yCoordinate);

    setSceneRect(sceneArea);
	invalidate();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapScene::setWidth()
//-----------------------------------------------------------------------------
void MemoryMapScene::setWidth(int width)
{
    int newWidth = width - MemoryVisualizationItem::CHILD_INDENTATION;

    if (width <= VisualizerItem::DEFAULT_WIDTH || width_ == newWidth)
    {
        return;
    }

    width_ = newWidth;

    for (auto memMap : memGraphItems_)
    {
        memMap->setWidth(width_);
    }

    // Update scene rect width.
    QRectF sceneArea = sceneRect();
    sceneArea.setWidth(width);
    setSceneRect(sceneArea);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapScene::wheelEvent()
//-----------------------------------------------------------------------------
void MemoryMapScene::wheelEvent(QGraphicsSceneWheelEvent * wheelEvent)
{   
    if (wheelEvent->modifiers() & Qt::ControlModifier)
    {
        //! Scrolling sensitivity. Bigger value results in smaller steps.
        const int WHEEL_SENSITIVITY = 6;

        int width = sceneRect().width() + wheelEvent->delta() / WHEEL_SENSITIVITY;
        setWidth(width);    
        wheelEvent->accept();
    }
    else
    {
        QGraphicsScene::wheelEvent(wheelEvent);
    }
}
