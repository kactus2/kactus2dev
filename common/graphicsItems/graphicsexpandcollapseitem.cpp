//-----------------------------------------------------------------------------
// File: graphicsexpandcollapseitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2012
//
// Description:
// This item provides an expand/collapse item.
//-----------------------------------------------------------------------------

#include "graphicsexpandcollapseitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::GraphicsExpandCollapseItem()
//-----------------------------------------------------------------------------
GraphicsExpandCollapseItem::GraphicsExpandCollapseItem( QGraphicsItem* parent /*= 0*/ ):
QGraphicsRectItem(parent),
expanded_(false)
{
}

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::~GraphicsExpandCollapseItem()
//-----------------------------------------------------------------------------
GraphicsExpandCollapseItem::~GraphicsExpandCollapseItem()
{

}

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::expand()
//-----------------------------------------------------------------------------
void GraphicsExpandCollapseItem::expand()
{
	expanded_ = true;

    emit stateChanged(expanded_);
}

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::isExpanded()
//-----------------------------------------------------------------------------
bool GraphicsExpandCollapseItem::isExpanded() const
{
	return expanded_;
}

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::collapse()
//-----------------------------------------------------------------------------
void GraphicsExpandCollapseItem::collapse()
{
	expanded_ = false;

    emit stateChanged(expanded_);
}

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::isCollapsed()
//-----------------------------------------------------------------------------
bool GraphicsExpandCollapseItem::isCollapsed() const
{
	return !expanded_;
}

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::mousePressEvent()
//-----------------------------------------------------------------------------
void GraphicsExpandCollapseItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
	event->accept();
	QGraphicsRectItem::mousePressEvent(event);

	// mouse left button changes the state of the item
	if (event->button() == Qt::LeftButton)
    {	
		// if was expanded then collapse
		if (expanded_)
        {
			collapse();
		}
		// if was collapsed then expand
		else
        {
			expand();
		}
	}
}

//-----------------------------------------------------------------------------
// Function: graphicsexpandcollapseitem::type()
//-----------------------------------------------------------------------------
int GraphicsExpandCollapseItem::type() const
{
	return Type;
}
