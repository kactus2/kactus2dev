/* 
 *  	Created on: 25.4.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgraphitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgraphitem.h"
#include "registergraphitem.h"

#include <QBrush>
#include <QColor>

FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field,
							   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
field_(field) {
	Q_ASSERT(field_);
	setBrush(QBrush(QColor(40, 140, 255)));
	setShowExpandableItem(false);
}

FieldGraphItem::~FieldGraphItem() {
}

void FieldGraphItem::refresh() {
	// the size depends on how many bits the field contains, always at least one
	int fieldWidth = field_->getBitWidth();
	
	// at least one bit
	if (fieldWidth == 0) {
		fieldWidth = 1;
	}
	setRect(0, 0, VisualizerItem::MIN_WIDTH * fieldWidth, VisualizerItem::ITEM_HEIGHT);

	// the name depends on the size of the rectangle (if too small then name is chopped)
	setName(field_->getName());
	setLeftTopCorner(QString::number(field_->getMSB()));
	setRightTopCorner(QString::number(field_->getBitOffset()));
	ExpandableItem::reorganizeChildren();

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

quint64 FieldGraphItem::getOffset() const {
	return field_->getBitOffset();
}

int FieldGraphItem::getBitWidth() const {
	return field_->getBitWidth();
}

unsigned int FieldGraphItem::getAddressUnitSize() const {
	RegisterGraphItem* regItem = static_cast<RegisterGraphItem*>(parentItem());
	Q_ASSERT(regItem);
	return regItem->getAddressUnitSize();
}

quint64 FieldGraphItem::getLastAddress() const {
	return field_->getMSB();
}

qreal FieldGraphItem::itemTotalWidth() const {
	return rect().width();
}

