/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.cpp
 *		Project: Kactus 2
 */

#include "registergraphitem.h"
#include "fieldgraphitem.h"
#include <models/field.h>

#include <QBrush>
#include <QColor>

RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg,
									 QGraphicsItem* parent):
MemoryVisualizationItem(parent),
register_(reg) {

	Q_ASSERT(register_);
	setBrush(QBrush(QColor(60, 160, 255)));
}

RegisterGraphItem::~RegisterGraphItem() {
}

void RegisterGraphItem::refresh() {
	setName(register_->getName());

	QList<QSharedPointer<Field> >& fields = register_->getFields();
	foreach (QSharedPointer<Field> field, fields) {

		// create the item
		FieldGraphItem* fieldGraph = new FieldGraphItem(field, this);

		// get the offset of the item
		int offset = fieldGraph->getOffset();

		// make sure the items are in correct order for the offset
		childItems_.insert(offset, fieldGraph);

		// tell child to check its children
		fieldGraph->refresh();
	}

	// set the positions for the children MUUTA OMAAN TOTEUTUKSEEN!
	reorganizeChildren();
}

int RegisterGraphItem::getOffset() const {
	return 0;
}

int RegisterGraphItem::getBitWidth() const {
	return 0;
}
