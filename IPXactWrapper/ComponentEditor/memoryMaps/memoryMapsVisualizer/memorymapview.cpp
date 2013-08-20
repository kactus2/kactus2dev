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

//-----------------------------------------------------------------------------
// Function: MemoryMapView()
//-----------------------------------------------------------------------------
MemoryMapView::MemoryMapView(QWidget *parent):
QGraphicsView(parent)
{
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

//-----------------------------------------------------------------------------
// Function: ~MemoryMapView()
//-----------------------------------------------------------------------------
MemoryMapView::~MemoryMapView() {
}


//-----------------------------------------------------------------------------
// Function: resizeEvent()
//-----------------------------------------------------------------------------
void MemoryMapView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
   
    if (event->size().width() != event->oldSize().width())
    {
        MemoryMapScene* memoryscene = dynamic_cast<MemoryMapScene*>(scene());
        Q_ASSERT(memoryscene);
        memoryscene->setWidth(event->size().width());
        event->accept();
    }
}
