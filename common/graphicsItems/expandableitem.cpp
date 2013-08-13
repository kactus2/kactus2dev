/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: expandableitem.cpp
 *		Project: Kactus 2
 */

#include "expandableitem.h"
#include "graphicsexpandcollapseitem.h"

#include <QDebug>

ExpandableItem::ExpandableItem( QGraphicsItem* parent /*= 0*/):
VisualizerItem(parent),
expandCollapseItem_(new GraphicsExpandCollapseItem(this)),
expansionRect_(new QGraphicsRectItem(this)) {

	connect(expandCollapseItem_, SIGNAL(stateChanged(bool)),
		this, SLOT(onExpandStateChange(bool)), Qt::UniqueConnection);
	expandCollapseItem_->setZValue(1);
	expansionRect_->show();
}

ExpandableItem::~ExpandableItem() {
}

void ExpandableItem::onExpandStateChange( bool expanded ) {
	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {
		Q_ASSERT(child);

		// if the item is visualizer item
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem) {
			
			// if the items are to be shown
			if (expanded) {
				childItem->show();
			}
			// if the items are to be hidden
			else {
				childItem->hide();
			}
		}
	}

	// when items are hidden/shown then the children must be reorganized
	refresh();
}

void ExpandableItem::setShowExpandableItem( bool show ) {
	expandCollapseItem_->setVisible(show);
}

bool ExpandableItem::isExpanded() const {
	return expandCollapseItem_->isExpanded();
}

void ExpandableItem::reorganizeChildren() {

	// the rectangle that contains this item and children
	QRectF totalRect = itemTotalRect();

	// the rectangle is on the left side of the parent and children
	expansionRect_->setRect(-GraphicsExpandCollapseItem::SIDE, 0, 
        GraphicsExpandCollapseItem::SIDE, totalRect.height());

    // Set the position for the expand/collapse item with the icon.
    expandCollapseItem_->setPos(-GraphicsExpandCollapseItem::SIDE, GraphicsExpandCollapseItem::SIDE/2);

	VisualizerItem::reorganizeChildren();
}

void ExpandableItem::setExpansionBrush( const QBrush& brush ) {
	expansionRect_->setBrush(brush);
}

void ExpandableItem::setExpansionPen( const QPen& pen)
{
    expansionRect_->setPen(pen);
}

void ExpandableItem::setExpansionRectVisible( bool visible ) {
	expansionRect_->setVisible(visible);
}
