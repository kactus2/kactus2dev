/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#include "masterinterface.h"
#include "XmlUtils.h"


#include <QString>
#include <QMap>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QXmlStreamWriter>

// the constructor
MasterInterface::MasterInterface(QDomNode& masterNode): 
addressSpaceRef_(),
baseAddress_(),
prompt_(QString("Base Address:")),
baseAttributes_() {

	// go through all child items
	for (int i = 0; i < masterNode.childNodes().count(); ++i) {
		QDomNode tempNode = masterNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:addressSpaceRef")) {
			// get the addressSpaceRef attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			addressSpaceRef_ = attributeMap.namedItem(QString(
					"spirit:addressSpaceRef")).nodeValue();
			addressSpaceRef_ = XmlUtils::removeWhiteSpace(addressSpaceRef_);

			// search the baseAddress element
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				if (tempNode.childNodes().at(j).nodeName() == QString(
						"spirit:baseAddress")) {
					tempNode = tempNode.childNodes().at(j);
					baseAddress_ = tempNode.childNodes().at(0).nodeValue();
					baseAddress_ = XmlUtils::removeWhiteSpace(baseAddress_);
					break;
				}
			}
			// get the base address attributes
			baseAttributes_ = XmlUtils::parseAttributes(tempNode);

			// if contains the optional attribute prompt then set the
			// prompt value in prompt_ and remove the attribute from the
			// QMap to avoid duplicate data
			QMap<QString, QString>::iterator i = baseAttributes_.find(
					QString("spirit:prompt"));
			if (i != baseAttributes_.end()) {

				// save the prompt value
				prompt_ = i.value();

				// remove from map
				baseAttributes_.erase(i);
			}
		}
	}
	return;
}

MasterInterface::MasterInterface():
addressSpaceRef_(), 
baseAddress_(),
prompt_(QString("Base Address:")),
baseAttributes_() {
}

MasterInterface::MasterInterface( const MasterInterface& other ):
addressSpaceRef_(other.addressSpaceRef_),
baseAddress_(other.baseAddress_),
prompt_(other.prompt_),
baseAttributes_(other.baseAttributes_) {
}


MasterInterface& MasterInterface::operator=( const MasterInterface& other ) {
	if (this != &other) {
		addressSpaceRef_ = other.addressSpaceRef_;
		baseAddress_ = other.baseAddress_;
		prompt_ = other.prompt_;
		baseAttributes_ = other.baseAttributes_;
	}
	return *this;
}


// the destructor
MasterInterface::~MasterInterface() {
}

void MasterInterface::write(QXmlStreamWriter& writer) {
	// spirit:master can't be written here because this class is used also
	// in spirit:mirroredMaster. The writing has to be done in previous level

	// if nothing is to be written just return and dont write the opening
	// tag
	if (addressSpaceRef_.isEmpty()) {
		return;
	}

	writer.writeStartElement("spirit:addressSpaceRef");
	writer.writeAttribute("spirit:addressSpaceRef", addressSpaceRef_);

	// if optional element baseAddress is not empty
	if (!baseAddress_.isEmpty()) {
		// start the spirit:baseAddress tag
		writer.writeStartElement("spirit:baseAddress");

		// if prompt attribute is defined
		if (!prompt_.isEmpty()) {
			writer.writeAttribute("spirit:prompt", prompt_);
		}
		// write rest of the attributes
		XmlUtils::writeAttributes(writer, baseAttributes_);

		// write the value of the baseAddress element and close the tag
		writer.writeCharacters(baseAddress_);
		writer.writeEndElement(); // spirit:baseAddress
	}

	writer.writeEndElement(); // spirit:addressSpaceRef
	return;
}

QString MasterInterface::getAddressSpaceRef() const {
	return addressSpaceRef_;
}

QString MasterInterface::getBaseAddress() const {
	return baseAddress_;
}

const QMap<QString,QString>& MasterInterface::getBaseAttributes() {

	return baseAttributes_;
}

QString MasterInterface::getPrompt() const {
	// if base address is not specified then there is no prompt for it either
	if (baseAddress_.isEmpty())
		return QString();

	return prompt_;
}

void MasterInterface::setAddressSpaceRef(const QString& addressSpaceRef) {
	this->addressSpaceRef_ = addressSpaceRef;
}

void MasterInterface::setBaseAddress(const QString& baseAddress) {
	this->baseAddress_ = baseAddress;
}

void MasterInterface::setBaseAttributes(const
		QMap<QString,QString>& baseAttributes) {
	this->baseAttributes_ = baseAttributes;
}

void MasterInterface::setPrompt(const QString& prompt) {
	this->prompt_ = prompt;
}