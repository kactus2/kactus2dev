/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "memorymapitem.h"
#include "generaldeclarations.h"


#include "../exceptions/write_error.h"

#include <QString>
#include <QMap>
#include <QDomNode>
#include <QObject>
#include <QXmlStreamWriter>

// the constructor
MemoryMapItem::MemoryMapItem(QDomNode &memoryMapNode): name_(QString()),
attributes_(), baseAddress_(QString()), baseAddrAttributes_() {

	// parse the attributes
	General::parseAttributes(memoryMapNode, attributes_);

	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get name
		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
			name_ = General::removeWhiteSpace(name_);
		}

		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}

		// get addressUnitBits
		else if (tempNode.nodeName() == QString("spirit:baseAddress")) {

			// get base address
			baseAddress_ = tempNode.childNodes().at(0).nodeValue();

			// get the base address attributes
			General::parseAttributes(tempNode, baseAddrAttributes_);
		}
	}

	// check that mandatory elements were found
// 	if (name_.isNull() ) {
// 		throw Parse_error(QObject::tr("Mandatory element name missing in "
// 				"MemoryMapItem"));
// 	}
	return;
}

MemoryMapItem::MemoryMapItem( const MemoryMapItem &other ):
name_(other.name_),
displayName_(other.displayName_),
description_(other.description_),
attributes_(other.attributes_),
baseAddress_(other.baseAddress_),
baseAddrAttributes_(other.baseAddrAttributes_) {
}

MemoryMapItem & MemoryMapItem::operator=( const MemoryMapItem &other ) {
	if (this != &other) {
		name_ = other.name_;
		displayName_ = other.displayName_;
		description_ = other.description_;
		attributes_ = other.attributes_;
		baseAddress_ = other.baseAddress_;
		baseAddrAttributes_ = other.baseAddrAttributes_;
	}
	return *this;
}

// the destructor
MemoryMapItem::~MemoryMapItem() {
}

// this is the base class implementation
void MemoryMapItem::write(QXmlStreamWriter& writer) {
	// the root element must be called just before calling this function
	// otherwise the attributes are printed under the wrong element
	General::writeAttributes(writer, attributes_);

	// if mandatory element name is missing
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in "
				"MemoryMapItem"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// baseAddress is mandatory only in base bank but not in
	// banked type bank
	if (!baseAddress_.isEmpty()) {
		// start the spirit:baseAddress tag
		writer.writeStartElement("spirit:baseAddress");

		// write the attributes for the element
		General::writeAttributes(writer, baseAddrAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(baseAddress_);
		writer.writeEndElement(); // spirit:baseAddress
	}

	// go back to the actual class
	return;
}

const QMap<QString, QString>& MemoryMapItem::getAttributes() {
	return attributes_;
}

QString MemoryMapItem::getBaseAddress() const {
	return baseAddress_;
}

void MemoryMapItem::setName(const QString &name) {
	name_ = name;
}

const QMap<QString, QString>& MemoryMapItem::getBaseAddrAttributes() {
	return baseAddrAttributes_;
}

void MemoryMapItem::setBaseAddress(const QString &baseAddress) {
	baseAddress_ = baseAddress;
}

void MemoryMapItem::setAttributes(const QMap<QString, QString> &attributes) {
	// delete old attributes
	attributes_.clear();

	// save new attributes
	attributes_ = attributes;
}

void MemoryMapItem::setBaseAddrAttributes(const
		QMap<QString, QString> &baseAddrAttributes) {
	// delete old attributes
	baseAddrAttributes_.clear();

	// save new attributes
	baseAddrAttributes_ = baseAddrAttributes;
}

QString MemoryMapItem::getName() const {
	return name_;
}

QString MemoryMapItem::getDisplayName() const {
	return displayName_;
}

QString MemoryMapItem::getDescription() const {
	return description_;
}

void MemoryMapItem::setDisplayName(const QString& name) {
	displayName_ = name;
}

void MemoryMapItem::setDescription(const QString& description) {
	description_ = description;
}
