/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "memorymap.h"
#include "generaldeclarations.h"
#include "memorymapitem.h"
#include "subspacemap.h"
#include "addressblock.h"
#include "bank.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QString>
#include <QDomNode>
#include <QList>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

MemoryMap::MemoryMap(QDomNode &memoryMapNode): name_(), id_(), items_(),
addressUnitBits_(0) {
	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNamedNodeMap attributeMap = memoryMapNode.attributes();
		id_ = attributeMap.namedItem("spirit:id").nodeValue();
		id_ = General::removeWhiteSpace(id_);

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get name
		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
			name_ = General::removeWhiteSpace(name_);
		}

		// get addressUnitBits
		else if (tempNode.nodeName() == QString("spirit:addressUnitBits")) {
			addressUnitBits_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}

		// get SubSpaceMap
		else if (tempNode.nodeName() == QString("spirit:subspaceMap")) {
			SubspaceMap *temp = new SubspaceMap(tempNode);
			items_.append(QSharedPointer<SubspaceMap>(temp));
		}

		// get Bank
		else if (tempNode.nodeName() == QString("spirit:bank")) {
			Bank *temp = new Bank(tempNode);
			items_.append(QSharedPointer<Bank>(temp));
		}

		// get addressBlock
		else if (tempNode.nodeName() == QString("spirit:addressBlock")) {
			AddressBlock *temp = new AddressBlock(tempNode);
			items_.append(QSharedPointer<AddressBlock>(temp));
		}
	}

	// if mandatory element name is missing
	if (name_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element name is missing in"
				" spirit:memoryMap"));
	}
	return;
}

MemoryMap::MemoryMap( const MemoryMap &other ):
name_(other.name_),
id_(other.id_),
items_(),
addressUnitBits_(other.addressUnitBits_) {

	foreach (QSharedPointer<MemoryMapItem> item, other.items_) {
		if (item) {
			QSharedPointer<MemoryMapItem> copy = QSharedPointer<MemoryMapItem>(
				new MemoryMapItem(*item.data()));
			items_.append(copy);
		}
	}
}

MemoryMap & MemoryMap::operator=( const MemoryMap &other ) {
	if (this != &other) {
		name_ = other.name_;
		id_ = other.id_;
		addressUnitBits_ = other.addressUnitBits_;
		
		items_.clear();
		foreach (QSharedPointer<MemoryMapItem> item, other.items_) {
			if (item) {
				QSharedPointer<MemoryMapItem> copy = QSharedPointer<MemoryMapItem>(
					new MemoryMapItem(*item.data()));
				items_.append(copy);
			}
		}
	}
	return *this;
}

MemoryMap::~MemoryMap() {
	items_.clear();
}

void MemoryMap::write(QXmlStreamWriter& writer) {
	// spirit:memoryMap can't be written here because this function
	// is used also as spirit:localMemoryMap

	// if optional attribute id is defined
	if (!id_.isEmpty()) {
		writer.writeAttribute("spirit:id", id_);
	}

	// if mandatory name is missing
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name is missing in spirit:"
				"memoryMap"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	// write all addressblock, bank and subspaceMap elements
	for (int i = 0; i < items_.size(); ++i) {
		items_.at(i)->write(writer);
	}

	if (addressUnitBits_ != 0) {
		writer.writeTextElement("spirit:addressUnitBits",
				QString::number(addressUnitBits_));
	}

}

void MemoryMap::setName(const QString &name) {
	name_ = name;
}

void MemoryMap::setAddressUnitBits(unsigned int addressUnitBits) {
	addressUnitBits_ = addressUnitBits;
}

const QList<QSharedPointer<MemoryMapItem> >& MemoryMap::getItems() {
	return items_;
}

QString MemoryMap::getName() const {
	return name_;
}

unsigned int MemoryMap::getAddressUnitBits() const {
	return addressUnitBits_;
}

void MemoryMap::setItems(const QList<QSharedPointer<MemoryMapItem> > &items) {
	// first delete the old items
	items_.clear();

	// save the new items
	items_ = items;
}
