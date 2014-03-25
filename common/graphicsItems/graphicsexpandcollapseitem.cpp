/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: graphicsexpandcollapseitem.cpp
 *		Project: Kactus 2
 */

#include "graphicsexpandcollapseitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

GraphicsExpandCollapseItem::GraphicsExpandCollapseItem( QGraphicsItem* parent /*= 0*/ ):
QGraphicsPixmapItem(parent),
expanded_(false) {

	QPixmap pic(":/icons/graphics/triangle_arrow_right.png");
	QPixmap scaledPic = pic.scaled(GraphicsExpandCollapseItem::SIDE, 
		GraphicsExpandCollapseItem::SIDE, Qt::KeepAspectRatio);
	setPixmap(scaledPic);

	setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
}

GraphicsExpandCollapseItem::~GraphicsExpandCollapseItem() {
}

void GraphicsExpandCollapseItem::expand() {
	expanded_ = true;
	QPixmap pic(":/icons/graphics/triangle_arrow_down.png");
	QPixmap scaledPic = pic.scaled(GraphicsExpandCollapseItem::SIDE, 
		GraphicsExpandCollapseItem::SIDE, Qt::KeepAspectRatio);
	setPixmap(scaledPic);
}

bool GraphicsExpandCollapseItem::isExpanded() const {
	return expanded_;
}

void GraphicsExpandCollapseItem::collapse() {
	expanded_ = false;
	QPixmap pic(":/icons/graphics/triangle_arrow_right.png");
	QPixmap scaledPic = pic.scaled(GraphicsExpandCollapseItem::SIDE, 
		GraphicsExpandCollapseItem::SIDE, Qt::KeepAspectRatio);
	setPixmap(scaledPic);
}

bool GraphicsExpandCollapseItem::isCollapsed() const {
	return !expanded_;
}

void GraphicsExpandCollapseItem::mousePressEvent( QGraphicsSceneMouseEvent *event ) {
	event->accept();
	QGraphicsPixmapItem::mousePressEvent(event);

	// mouse left button changes the state of the item
	if (event->button() == Qt::LeftButton) {
		
		// if was expanded then collapse
		if (expanded_) {
			collapse();
		}
		// if was collapsed then expand
		else {
			expand();
		}

		emit stateChanged(expanded_);
	}
}

int GraphicsExpandCollapseItem::type() const {
	return Type;
}
