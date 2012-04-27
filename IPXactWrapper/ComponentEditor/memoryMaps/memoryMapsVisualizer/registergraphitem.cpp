/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.cpp
 *		Project: Kactus 2
 */

#include "registergraphitem.h"

#include <models/field.h>

#include <QBrush>
#include <QColor>

RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg,
									 QGraphicsItem* parent,
									 int maxWidth):
MemoryVisualizerItem(parent, maxWidth),
register_(reg),
fields_() {

	Q_ASSERT(register_);
	setName(register_->getName());
	setBrush(QBrush(QColor(60, 160, 255)));
	setLeftCornerText(register_->getAddressOffset());

	QList<QSharedPointer<Field> > fields = register_->getFields();
	foreach (QSharedPointer<Field> field, fields) {

		QSharedPointer<FieldGraphItem> fieldGraph(new FieldGraphItem(field, this, maxWidth_));
		fields_.append(fieldGraph);
	}

	reorganizeChildren();
}

RegisterGraphItem::~RegisterGraphItem() {
}

void RegisterGraphItem::reorganizeChildren() {

	// set the field items in 
	QMap<int, QSharedPointer<FieldGraphItem> > fieldOrganizer;
	foreach (QSharedPointer<FieldGraphItem> item, fields_) {
		fieldOrganizer.insert(item->getOffset(), item);
	}

	QSharedPointer<FieldGraphItem> previous;
	for (QMap<int, QSharedPointer<FieldGraphItem> >::iterator i = --fieldOrganizer.end();
		i != --fieldOrganizer.begin(); --i) {

		if (previous) {
			i.value()->setPos(mapRectFromItem(previous.data(),
				previous->itemTotalRect()).right(), rect().bottom());
		}
		else {
			i.value()->setPos(0, rect().bottom());
		}

		previous = i.value();
	}

// 	foreach (QSharedPointer<FieldGraphItem> field, fields_) {
// 
// 		if (previous) {
// 			field->setPos(mapRectFromItem(previous.data(), previous->itemTotalRect()).right(), rect().bottom());
// 		}
// 		else {
// 			field->setPos(0, rect().bottom());
// 		}
// 		previous = field;
// 	}
}

qreal RegisterGraphItem::itemTotalWidth() const {

	qreal width = 0;

	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		FieldGraphItem* fieldItem = dynamic_cast<FieldGraphItem*>(child);
		if (fieldItem) {
			width += fieldItem->itemTotalWidth();
		}
	}

	return qMax(width, qreal(MemoryVisualizerItem::MAX_ITEM_WIDTH));
}

int RegisterGraphItem::getOffset() const {
	return register_->getAddressOffset().toInt();
}
