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

#include <IPXACTmodels\kactusExtensions\Kactus2Position.h>

#include <QPen>

//-----------------------------------------------------------------------------
// Function: QGraphicsLineItem::Association()
//-----------------------------------------------------------------------------
Association::Association(Associable* startItem, Associable* endItem, 
    QSharedPointer<Kactus2Position> endpointExtension, QGraphicsItem* parent):
    QGraphicsLineItem(parent),
    startItem_(startItem),
    endItem_(endItem),
    endpointPosition_(endpointExtension)
{        
    setFlag(ItemIsSelectable);
    setPen(QPen(Qt::black, 1, Qt::DashLine));

    setZValue(-1.0);

    updateLine();
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

    updateLine();
    painter->drawLine(line());
}

//-----------------------------------------------------------------------------
// Function: Association::connect()
//-----------------------------------------------------------------------------
void Association::connect()
{
    startItem_->addAssociation(this);
    endItem_->addAssociation(this);
}

//-----------------------------------------------------------------------------
// Function: Association::disconnect()
//-----------------------------------------------------------------------------
void Association::disconnect()
{
    startItem_->removeAssociation(this);
    endItem_->removeAssociation(this);
}

//-----------------------------------------------------------------------------
// Function: Association::setEndItem()
//-----------------------------------------------------------------------------
void Association::setEndItem(Associable* endItem)
{
    endItem_->removeAssociation(this);

    endItem_ = endItem;

    endItem_->addAssociation(this);
}

//-----------------------------------------------------------------------------
// Function: Association::getEndpointExtension()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Position> Association::getEndpointExtension() const
{
    return endpointPosition_;
}

//-----------------------------------------------------------------------------
// Function: Association::updateLine()
//-----------------------------------------------------------------------------
void Association::updateLine()
{
    QPointF startPoint = startItem_->connectionPoint();
    QPointF endPoint = endItem_->connectionPoint(startPoint);
    QLineF centerLine(startPoint, endPoint);
       
    prepareGeometryChange();
    setLine(centerLine);    

    endpointPosition_->setPosition(endPoint);
}
