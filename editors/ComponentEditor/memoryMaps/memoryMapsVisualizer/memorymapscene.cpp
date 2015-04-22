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
memGraphItems_(),
width_(VisualizerItem::DEFAULT_WIDTH)
{
}

//-----------------------------------------------------------------------------
// Function: ~MemoryMapScene()
//-----------------------------------------------------------------------------
MemoryMapScene::~MemoryMapScene()
{
}

//-----------------------------------------------------------------------------
// Function: addMemGraphItem()
//-----------------------------------------------------------------------------
void MemoryMapScene::addMemGraphItem( VisualizerItem* memGraphItem )
{
	Q_ASSERT(memGraphItem);

    qreal yCoordinate = 0;

    if (!memGraphItems_.isEmpty())
    {        
        yCoordinate = memGraphItems_.last()->itemTotalRect().bottom() + 10;
    }

    memGraphItem->setPos(0, yCoordinate);
    memGraphItem->setWidth(width_);

    addItem(memGraphItem);
    memGraphItems_.append(memGraphItem);

    connect(memGraphItem, SIGNAL(sizeChanged()), this, SLOT(rePosition()), Qt::UniqueConnection);

    // Update scene rect height.   
    QRectF rect = sceneRect();
    rect.setBottom(yCoordinate + memGraphItem->rect().height());

    setSceneRect(rect);
    invalidate();
}

//-----------------------------------------------------------------------------
// Function: removeMemGraphItem()
//-----------------------------------------------------------------------------
void MemoryMapScene::removeMemGraphItem( VisualizerItem* memGraphItem )
{
	Q_ASSERT(memGraphItem);

    int startIndex = memGraphItems_.indexOf(memGraphItem);
    qreal yCoordinate = memGraphItem->pos().y();

    removeItem(memGraphItem);
    memGraphItems_.removeAll(memGraphItem);

    disconnect(memGraphItem, SIGNAL(sizeChanged()), this, SLOT(rePosition()));

    for (int i = startIndex; i < memGraphItems_.count(); i++)
    {     
        VisualizerItem* memMap = memGraphItems_.at(i);

        memMap->setPos(0, yCoordinate);
        yCoordinate += memMap->itemTotalRect().bottom() + 10;
    }

    // Update scene rect height.   
    QRectF rect = sceneRect();
    rect.setBottom(yCoordinate);

    setSceneRect(rect);
    invalidate();
}

//-----------------------------------------------------------------------------
// Function: rePosition()
//-----------------------------------------------------------------------------
void MemoryMapScene::rePosition()
{
	qreal yCoordinate = 0;

	foreach (VisualizerItem* memMap, memGraphItems_)
    {        
		memMap->setPos(0, yCoordinate);
        yCoordinate += memMap->itemTotalRect().bottom() + 10;
	}

	// Update scene rect height.   
    QRectF rect = sceneRect();
    rect.setBottom(yCoordinate);

    setSceneRect(rect);
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
       
        foreach (VisualizerItem* memMap, memGraphItems_)
        {        
            memMap->setWidth(width_);
        }

        // Update scene rect width.
        QRectF rect = sceneRect();
        rect.setWidth(width);
        setSceneRect(rect);        
    }
}

//-----------------------------------------------------------------------------
// Function: wheelEvent()
//-----------------------------------------------------------------------------
void MemoryMapScene::wheelEvent(QGraphicsSceneWheelEvent * wheelEvent)
{   
    if (wheelEvent->modifiers() & Qt::ControlModifier)
    {
        int width = sceneRect().width() + wheelEvent->delta() / WHEEL_SENSITIVITY;
        setWidth(width);    
        wheelEvent->accept();
    }
    else
    {
        QGraphicsScene::wheelEvent(wheelEvent);
    }
}
