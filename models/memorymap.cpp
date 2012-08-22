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

#include <QString>
#include <QDomNode>
#include <QList>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

MemoryMap::MemoryMap(QDomNode &memoryMapNode): 
nameGroup_(memoryMapNode),
id_(), 
items_(),
addressUnitBits_(0) {
	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNamedNodeMap attributeMap = memoryMapNode.attributes();
		id_ = attributeMap.namedItem("spirit:id").nodeValue();
		id_ = General::removeWhiteSpace(id_);

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get addressUnitBits
		if (tempNode.nodeName() == QString("spirit:addressUnitBits")) {
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
	return;
}

MemoryMap::MemoryMap():
nameGroup_(),
id_(), 
items_(),
addressUnitBits_(0) {
}

MemoryMap::MemoryMap( const MemoryMap &other ):
nameGroup_(other.nameGroup_),
id_(other.id_),
items_(),
addressUnitBits_(other.addressUnitBits_) {

	foreach (QSharedPointer<MemoryMapItem> item, other.items_) {
		if (item) {
			QSharedPointer<MemoryMapItem> copy = item->clone();
			items_.append(copy);
		}
	}
}

MemoryMap & MemoryMap::operator=( const MemoryMap &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		id_ = other.id_;
		addressUnitBits_ = other.addressUnitBits_;
		
		items_.clear();
		foreach (QSharedPointer<MemoryMapItem> item, other.items_) {
			if (item) {
				QSharedPointer<MemoryMapItem> copy = item->clone();
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

	writer.writeTextElement("spirit:name", nameGroup_.name_);

	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

	// write all address block, bank and subspaceMap elements
	for (int i = 0; i < items_.size(); ++i) {
		items_.at(i)->write(writer);
	}

	if (addressUnitBits_ != 0) {
		writer.writeTextElement("spirit:addressUnitBits",
				QString::number(addressUnitBits_));
	}

}

bool MemoryMap::isValid( QStringList& errorList, 
						const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("memory map %1").arg(nameGroup_.name_));

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for memory map within %1").arg(
			parentIdentifier));
		valid = false;
	}

	QStringList memItemNames;
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		if (memItemNames.contains(memItem->getName())) {
			errorList.append(QObject::tr("%1 contains several memory map items"
				" with name %2").arg(thisIdentifier).arg(memItem->getName()));
			valid = false;
		}
		else {
			memItemNames.append(memItem->getName());
		}

		if (!memItem->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool MemoryMap::isValid() const {

	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	QStringList memItemNames;
	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		if (memItemNames.contains(memItem->getName())) {
			return false;
		}
		else {
			memItemNames.append(memItem->getName());
		}

		if (!memItem->isValid()) {
			return false;
		}
	}

	return true;
}

void MemoryMap::setName(const QString &name) {
	nameGroup_.name_ = name;
}

void MemoryMap::setAddressUnitBits(unsigned int addressUnitBits) {
	addressUnitBits_ = addressUnitBits;
}

const QList<QSharedPointer<MemoryMapItem> >& MemoryMap::getItems() const {
	return items_;
}

QString MemoryMap::getName() const {
	return nameGroup_.name_;
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

QList<QSharedPointer<MemoryMapItem> >* MemoryMap::getItemsPointer() {
	return &items_;
}

QString MemoryMap::getDisplayName() const {
	return nameGroup_.displayName_;
}

void MemoryMap::setDisplayName( const QString& displayName ) {
	nameGroup_.displayName_ = displayName;
}

QString MemoryMap::getDescription() const {
	return nameGroup_.description_;
}

void MemoryMap::setDescription( const QString& description ) {
	nameGroup_.description_ = description;
}
