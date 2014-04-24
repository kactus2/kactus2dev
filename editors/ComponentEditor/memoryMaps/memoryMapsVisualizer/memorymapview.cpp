/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapview.cpp
 *		Project: Kactus 2
 */

#include "memorymapview.h"
#include "memorymapscene.h"
#include <common/graphicsItems/visualizeritem.h>

#include <QMouseEvent>
#include <QRect>
#include <QResizeEvent>
#include <QScrollBar>

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

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void MemoryMapView::mousePressEvent(QMouseEvent* mouseEvent)
{
    // Scrolling to left with side button.
    if (mouseEvent->button() == Qt::XButton1)        
    {
        if (horizontalScrollBar() != 0)
        {
            int sliderPosition = horizontalScrollBar()->value();
            int step = -horizontalScrollBar()->singleStep();
            horizontalScrollBar()->setSliderPosition(sliderPosition + step);
        }
    }

    // Scrolling to right with side button.
    else if (mouseEvent->button() == Qt::XButton2)
    {
        if (horizontalScrollBar() != 0)
        {
            int sliderPosition = horizontalScrollBar()->value();
            int step = horizontalScrollBar()->singleStep();
            horizontalScrollBar()->setSliderPosition(sliderPosition + step);
        }
    }

    // Normal operation.
    else
    {
        QGraphicsView::mousePressEvent(mouseEvent);
    }
}