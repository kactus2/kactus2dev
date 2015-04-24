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
nameGroup_(memoryMapNode),
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
nameGroup_(),
attributes_(),
baseAddress_("0x0"), 
baseAddrAttributes_() {
}

MemoryMapItem::MemoryMapItem( const MemoryMapItem &other ):
nameGroup_(other.nameGroup_),
attributes_(other.attributes_),
baseAddress_(other.baseAddress_),
baseAddrAttributes_(other.baseAddrAttributes_) {
}

MemoryMapItem & MemoryMapItem::operator=( const MemoryMapItem &other )
{
	if (this != &other)
    {
        nameGroup_ = other.nameGroup_;
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

    nameGroup_.write(writer);

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

//-----------------------------------------------------------------------------
// Function: memorymapitem::setName()
//-----------------------------------------------------------------------------
void MemoryMapItem::setName(const QString &name)
{
    nameGroup_.setName(name);
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

//-----------------------------------------------------------------------------
// Function: memorymapitem::getName()
//-----------------------------------------------------------------------------
QString MemoryMapItem::getName() const
{
    return nameGroup_.name();
}

//-----------------------------------------------------------------------------
// Function: memorymapitem::getDisplayName()
//-----------------------------------------------------------------------------
QString MemoryMapItem::getDisplayName() const
{
    return nameGroup_.displayName();
}

//-----------------------------------------------------------------------------
// Function: memorymapitem::getDescription()
//-----------------------------------------------------------------------------
QString MemoryMapItem::getDescription() const
{
    return nameGroup_.description();
}

//-----------------------------------------------------------------------------
// Function: memorymapitem::setDisplayName()
//-----------------------------------------------------------------------------
void MemoryMapItem::setDisplayName(const QString& name)
{
    nameGroup_.setDisplayName(name);
}

//-----------------------------------------------------------------------------
// Function: memorymapitem::setDescription()
//-----------------------------------------------------------------------------
void MemoryMapItem::setDescription(const QString& description)
{
    nameGroup_.setDescription(description);
}

//-----------------------------------------------------------------------------
// Function: memorymapitem::getNameGroup()
//-----------------------------------------------------------------------------
NameGroup& MemoryMapItem::getNameGroup()
{
    return nameGroup_;
}
