/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: expandableitem.cpp
 *		Project: Kactus 2
 */

#include "expandableitem.h"
#include "graphicsexpandcollapseitem.h"

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: ExpandableItem::ExpandableItem()
//-----------------------------------------------------------------------------
ExpandableItem::ExpandableItem(QGraphicsItem* parent):
VisualizerItem(parent),
expandCollapseItem_(new GraphicsExpandCollapseItem(this)),
expansionRect_(new QGraphicsRectItem(this))
{
	connect(expandCollapseItem_, SIGNAL(stateChanged(bool)),
        this, SLOT(onExpandStateChange(bool)), Qt::UniqueConnection);

	expandCollapseItem_->setZValue(1);
	expansionRect_->show();
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::~ExpandableItem()
//-----------------------------------------------------------------------------
ExpandableItem::~ExpandableItem()
{

}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::onExpandStateChange()
//-----------------------------------------------------------------------------
void ExpandableItem::onExpandStateChange(bool expanded)
{
	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children)
    {
		Q_ASSERT(child);

		// if the item is visualizer item
        VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
        if (childItem)
        {
            childItem->setVisible(expanded);
        }
    }

    if (expanded)
    {
        reorganizeChildren();
    }
    else
    {
        updateRectangle();
    }

    emit expandStateChanged();
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setShowExpandableItem()
//-----------------------------------------------------------------------------
void ExpandableItem::setShowExpandableItem( bool show )
{
	expandCollapseItem_->setVisible(show);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::isExpanded()
//-----------------------------------------------------------------------------
bool ExpandableItem::isExpanded() const
{
	return expandCollapseItem_->isExpanded();
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::reorganizeChildren()
//-----------------------------------------------------------------------------
void ExpandableItem::reorganizeChildren()
{
    updateRectangle();
	VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setDefaultBrush()
//-----------------------------------------------------------------------------
void ExpandableItem::setDefaultBrush(QBrush brush)
{
    VisualizerItem::setDefaultBrush(brush);
    setExpansionBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setExpansionBrush()
//-----------------------------------------------------------------------------
void ExpandableItem::setExpansionBrush(QBrush const& brush)
{
	expansionRect_->setBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setExpansionPen()
//-----------------------------------------------------------------------------
void ExpandableItem::setExpansionPen(QPen const& pen)
{
    expansionRect_->setPen(pen);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::setExpansionRectVisible()
//-----------------------------------------------------------------------------
void ExpandableItem::setExpansionRectVisible(bool visible)
{
	expansionRect_->setVisible(visible);
}

//-----------------------------------------------------------------------------
// Function: ExpandableItem::updateHeight()
//-----------------------------------------------------------------------------
void ExpandableItem::updateRectangle()
{
    // the rectangle that contains this item and children
    QRectF totalRect = itemTotalRect();

    // the rectangle is on the left side of the parent and children
    expansionRect_->setRect(-GraphicsExpandCollapseItem::SIDE, 0, 
        GraphicsExpandCollapseItem::SIDE, totalRect.height());

    // Set the position for the expand/collapse item with the icon.
    expandCollapseItem_->setPos(-GraphicsExpandCollapseItem::SIDE, 
        GraphicsExpandCollapseItem::SIDE / 2 + VisualizerItem::CORNER_INDENTATION);
}