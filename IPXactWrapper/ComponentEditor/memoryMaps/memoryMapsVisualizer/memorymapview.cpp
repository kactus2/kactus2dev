/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapview.cpp
 *		Project: Kactus 2
 */

#include "memorymapview.h"
#include "memorymapscene.h"
#include <common/graphicsItems/visualizeritem.h>

#include <QRect>
#include <QResizeEvent>

MemoryMapView::MemoryMapView(QWidget *parent):
QGraphicsView(parent) {
}

MemoryMapView::~MemoryMapView() {
}

void MemoryMapView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    //QRect rect(-VisualizerItem::ITEM_HEIGHT,0,event->size().width(),event->size().height());
    //scene()->update();
    //MemoryMapScene* memoryscene = dynamic_cast<MemoryMapScene*>(scene());
    //Q_ASSERT(memoryscene);
    //memoryscene->setViewWidth(event->size().width());
    //scene()->setSceneRect(rect);
    //scene()->update(rect);
    //fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    //memoryscene->rePosition();
}