//-----------------------------------------------------------------------------
// File: GraphicsRectButton.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2011
//
// Description:
// Generic graphics rectangle button item.
//-----------------------------------------------------------------------------

#include "GraphicsRectButton.h"

#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: GraphicsRectButton()
//-----------------------------------------------------------------------------
GraphicsRectButton::GraphicsRectButton(QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ~GraphicsRectButton()
//-----------------------------------------------------------------------------
GraphicsRectButton::~GraphicsRectButton()
{
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void GraphicsRectButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}

