//-----------------------------------------------------------------------------
// File: Association.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.6.2014
//
// Description:
// Defines an association between to items in a design.
//-----------------------------------------------------------------------------

#include "Association.h"

#include <QPen>

//-----------------------------------------------------------------------------
// Function: QGraphicsLineItem::Association()
//-----------------------------------------------------------------------------
Association::Association(Associable* startItem, Associable* endItem, QGraphicsItem* parent):
    QGraphicsLineItem(parent),
    startItem_(startItem),
    endItem_(endItem)
{        
    setFlag(ItemIsSelectable);
    setPen(QPen(Qt::black, 1, Qt::DashLine));

    setZValue(-1.0);
}

//-----------------------------------------------------------------------------
// Function: QGraphicsLineItem::~Association()
//-----------------------------------------------------------------------------
Association::~Association()
{

}

//-----------------------------------------------------------------------------
// Function: Association::paint()
//-----------------------------------------------------------------------------
void Association::paint(QPainter* painter, QStyleOptionGraphicsItem const*, QWidget*)
{
    if (isSelected()) 
    {
        painter->setPen(QPen(Qt::blue, 2, Qt::SolidLine));
    } 
    else
    {
        painter->setPen(pen());
    }

    QPointF startPoint = startItem_->connectionPoint();
    QLineF centerLine(startPoint, endItem_->connectionPoint(startPoint));
    setLine(centerLine);

    painter->drawLine(centerLine);
}

//-----------------------------------------------------------------------------
// Function: Association::disconnect()
//-----------------------------------------------------------------------------
void Association::disconnect()
{
    startItem_->removeAssociation(this);
    endItem_->removeAssociation(this);
}
