/* 
 *  Created on: 18.6.2010
 *  Author: Antti Kamppi
 */

#include "ipxactitem.h"

#include <QtXml>

// The constructor
IPXactItem::IPXactItem(QDomNode &node, int row, IPXactItem *parent):
domNode_(node), childItems_(),
parentItem_(parent), rowNumber_(row) {
}

// The destructor
IPXactItem::~IPXactItem() {
	// free up allocated memory
	childItems_.clear();
}

// get the wrapped node
QDomNode* IPXactItem::node() {
	return &domNode_;
}

// create a new wrapper
IPXactItem *IPXactItem::child(int index) {
	// if the item is already wrapped then pointer to it is returned
	if (childItems_.contains(index)) {
		return childItems_[index].data();
	}

	// a new wrapper is constructed and a pointer returned
	if (index >= 0 && index < domNode_.childNodes().count()) {
		QDomNode childNode = domNode_.childNodes().item(index);
		IPXactItem *child = new IPXactItem(childNode, index, this);
		childItems_[index] = QSharedPointer<IPXactItem>(child);
		return child;
	}
	// no valid wrapper was created
	return 0;
}

// get the row number
int IPXactItem::row() {
	return rowNumber_;
}

// get the parent of the item
IPXactItem *IPXactItem::parent() {
	return parentItem_;
}
