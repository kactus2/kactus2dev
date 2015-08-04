/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "memorymapitem.h"
#include "XmlUtils.h"

#include <QString>
#include <QMap>
#include <QDomNode>
#include <QObject>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: memorymapitem::MemoryMapItem()
//-----------------------------------------------------------------------------
MemoryMapItem::MemoryMapItem(QDomNode &memoryMapNode): 
NameGroup(),
attributes_(),
baseAddress_(), 
baseAddrAttributes_() {

	// parse the attributes
	attributes_ = XmlUtils::parseAttributes(memoryMapNode);

	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get addressUnitBits
        if (tempNode.nodeName() == QString("spirit:baseAddress"))
        {
			// get base address
			baseAddress_ = tempNode.childNodes().at(0).nodeValue();

			// get the base address attributes
			baseAddrAttributes_ = XmlUtils::parseAttributes(tempNode);
		}
	}
	return;
}

MemoryMapItem::MemoryMapItem():
NameGroup(),
attributes_(),
baseAddress_("0x0"), 
baseAddrAttributes_() {
}

MemoryMapItem::MemoryMapItem( const MemoryMapItem &other ):
NameGroup(other),
attributes_(other.attributes_),
baseAddress_(other.baseAddress_),
baseAddrAttributes_(other.baseAddrAttributes_) {
}

MemoryMapItem & MemoryMapItem::operator=( const MemoryMapItem &other )
{
	if (this != &other)
    {
        NameGroup::operator=(other);
		attributes_ = other.attributes_;
		baseAddress_ = other.baseAddress_;
		baseAddrAttributes_ = other.baseAddrAttributes_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: memorymapitem::~MemoryMapItem()
//-----------------------------------------------------------------------------
MemoryMapItem::~MemoryMapItem()
{

}

//-----------------------------------------------------------------------------
// Function: memorymapitem::write()
//-----------------------------------------------------------------------------
void MemoryMapItem::write(QXmlStreamWriter& writer)
{
	// the root element must be called just before calling this function
	// otherwise the attributes are printed under the wrong element
	XmlUtils::writeAttributes(writer, attributes_);

    writer.writeTextElement("ipxact:name", name());

    if (!displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", displayName());
    }

    if (!description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", description());
    }


	// baseAddress is mandatory only in base bank but not in
	// banked type bank
	if (!baseAddress_.isEmpty()) {
		// start the spirit:baseAddress tag
		writer.writeStartElement("spirit:baseAddress");

		// write the attributes for the element
		XmlUtils::writeAttributes(writer, baseAddrAttributes_);

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
