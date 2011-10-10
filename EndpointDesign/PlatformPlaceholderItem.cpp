//-----------------------------------------------------------------------------
// File: PlatformPlaceholderItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.9.2011
//
// Description:
// Placeholder box for the platform component.
//-----------------------------------------------------------------------------

#include "PlatformPlaceholderItem.h"

#include <common/diagramgrid.h>

#include <QPen>

//-----------------------------------------------------------------------------
// Function: PlatformPlaceholderItem()
//-----------------------------------------------------------------------------
PlatformPlaceholderItem::PlatformPlaceholderItem(QGraphicsItem* parent /*= 0*/) : QGraphicsRectItem(parent)
{
    setRect(-WIDTH / 2, 0, WIDTH, HEIGHT);
    setPen(QPen(Qt::black, 0, Qt::DashLine));

    QGraphicsTextItem* platformText = new QGraphicsTextItem(this);
    platformText->setHtml("<center>Drag a <b>SW platform</b> from the library OR double-click to create new</center>");
    platformText->setTextWidth(WIDTH);
    platformText->setPos(-0.5 * platformText->boundingRect().width(), 4);
}

//-----------------------------------------------------------------------------
// Function: ~PlatformPlaceholderItem()
//-----------------------------------------------------------------------------
PlatformPlaceholderItem::~PlatformPlaceholderItem()
{
}
