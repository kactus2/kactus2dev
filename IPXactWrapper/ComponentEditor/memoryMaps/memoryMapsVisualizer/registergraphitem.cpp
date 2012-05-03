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
fields_(),
undefinedFields_() {

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

	// remove the undefined fields
	undefinedFields_.clear();

	// set the field items in order by item off set
	QMap<int, QSharedPointer<FieldGraphItem> > fieldOrganizer;
	foreach (QSharedPointer<FieldGraphItem> item, fields_) {
		fieldOrganizer.insert(item->getOffset(), item);
	}

	// set the fields so that the MSB is on the left side
	// the fields are processed from the end to start
	QSharedPointer<FieldGraphItem> previous;
	for (QMap<int, QSharedPointer<FieldGraphItem> >::iterator i = --fieldOrganizer.end();
		i != --fieldOrganizer.begin(); --i) {

		if (previous) {

			// where the next defined field starts
			int previousStart = previous->getOffset();
			int currentEnd = i.value()->getOffset() + i.value()->getWidth();

			// if theres undefined space between the items
			if (previousStart != currentEnd) {
				QSharedPointer<FieldGraphItem> undefItem(
					new FieldGraphItem(previousStart - 1, currentEnd, this));

				// set the undefined field between the two defined fields
				undefItem->setPos(mapRectFromItem(previous.data(),
					previous->itemTotalRect()).right(), rect().bottom());
				undefinedFields_.append(undefItem);

				// the field is set after the undefined field
				i.value()->setPos(mapRectFromItem(undefItem.data(),
					undefItem->itemTotalRect()).right(), rect().bottom());
			}

			// if the current field is located right before the previous there is
			// no undefined gap
			else {
				i.value()->setPos(mapRectFromItem(previous.data(),
					previous->itemTotalRect()).right(), rect().bottom());
			}
		}
		// the first item to set
		else {

			int fieldEnd = i.value()->getOffset() + i.value()->getWidth();

			// if there is undefined bit fields at the end of the register
			if (fieldEnd < maxWidth_) {
				
				// create field for the undefined bits
				QSharedPointer<FieldGraphItem> undefItem(
					new FieldGraphItem(maxWidth_ - 1, fieldEnd, this));

				// set the position for the undefined bits
				undefItem->setPos(0, rect().bottom());
				undefinedFields_.append(undefItem);

				// set the position for the actual field
				i.value()->setPos(undefItem->mapRectFromItem(undefItem.data(),
					undefItem->itemTotalRect()).right(), rect().bottom());
			}
			// if the field is located at the end of the register then there are
			// no undefined bits
			else {
				i.value()->setPos(0, rect().bottom());
			}
		}

		// if this is the last field to process (meaning the LSB)
		if (i == fieldOrganizer.begin()) {

			int fieldStart = i.value()->getOffset();

			// if the field does not start from the beginning
			if (fieldStart != 0) {

				QSharedPointer<FieldGraphItem> undefItem(
					new FieldGraphItem(i.value()->getOffset() - 1, 0, this));

				undefItem->setPos(mapRectFromItem(i.value().data(), 
					i.value()->itemTotalRect()).right(), rect().bottom());
				undefinedFields_.append(undefItem);
			}
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

int RegisterGraphItem::getWidth() const {
	return register_->getWidth();
}
