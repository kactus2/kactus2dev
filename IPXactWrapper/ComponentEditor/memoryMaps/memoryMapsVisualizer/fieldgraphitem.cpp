/* 
 *  	Created on: 25.4.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgraphitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgraphitem.h"

#include <QBrush>
#include <QColor>

FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field,
							   QGraphicsItem* parent,
							   int maxWidth):
MemoryVisualizerItem(parent, maxWidth),
field_(field) {

	int fieldWidth = field->getBitWidth();
	setRect(0, 0, MemoryVisualizerItem::MIN_BIT_WIDTH * fieldWidth, 
		MemoryVisualizerItem::ITEM_HEIGHT);

	Q_ASSERT(field_);
	setName(field_->getName());
	setBrush(QBrush(QColor(40, 140, 255)));

	setRightCornerText(QString::number(field_->getBitOffset()));
	setLeftCornerText(QString::number(field_->getBitOffset() + field_->getBitWidth() - 1));

	reorganizeChildren();
}

FieldGraphItem::~FieldGraphItem() {
}

void FieldGraphItem::reorganizeChildren() {
	MemoryVisualizerItem::reorganizeChildren();
}

int FieldGraphItem::getOffset() const {
	return field_->getBitOffset();
}
