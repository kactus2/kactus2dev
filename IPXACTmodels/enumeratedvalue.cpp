/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#include "enumeratedvalue.h"

#include "GenericVendorExtension.h"

#include <QStringList>
#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>
#include <QObject>
#include "XmlUtils.h"

EnumeratedValue::EnumeratedValue(QDomNode& enumerationNode): NameGroup(),
usage_(EnumeratedValue::READWRITE), 
value_(),
vendorExtensions_()
{

	// parse the spirit:usage attribute
	QDomNamedNodeMap attributeMap = enumerationNode.attributes();
	QString usage = attributeMap.namedItem(QString("spirit:usage")).nodeValue();
	usage_ = str2Usage(usage, EnumeratedValue::READWRITE);

	for (int i = 0; i < enumerationNode.childNodes().count(); ++i) {
		QDomNode tempNode = enumerationNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
}

EnumeratedValue::EnumeratedValue():
NameGroup(),
usage_(EnumeratedValue::READWRITE), 
value_(),
vendorExtensions_()
{
}

EnumeratedValue::EnumeratedValue( const EnumeratedValue& other ):
NameGroup(other),
usage_(other.usage_),
value_(other.value_),
vendorExtensions_(other.vendorExtensions_)
{
}

EnumeratedValue& EnumeratedValue::operator=( const EnumeratedValue& other ) {
	if (this != &other) {
        NameGroup::operator=(other);
		usage_ = other.usage_;
		value_ = other.value_;
        vendorExtensions_ = other.vendorExtensions_;
	}
	return *this;
}

EnumeratedValue::~EnumeratedValue() {

}

void EnumeratedValue::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:enumeratedValue");

	// write the spirit:usage attribute
	writer.writeAttribute("spirit:usage", EnumeratedValue::usage2Str(usage_));

	writer.writeTextElement("spirit:name", name());
	
	// if optional displayName is defined
	if (!displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName());
	}

	// is optional description is defined
	if (!description().isEmpty()) {
		writer.writeTextElement("spirit:description", description());
	}

	writer.writeTextElement("spirit:value", value_);

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:enumeratedValue
}

bool EnumeratedValue::isValid( QStringList& errorList, 
							  const QString& parentIdentifier ) const {

	bool valid = true;

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for enumerated value"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (value_.isEmpty()) {
		errorList.append(QObject::tr("No value set for enumerated value %1 "
			"within %2").arg(name()).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool EnumeratedValue::isValid() const {
	if (name().isEmpty()) {
		return false;
	}

	if (value_.isEmpty()) {
		return false;
	}

	return true;
}	

EnumeratedValue::EnumeratedUsage EnumeratedValue::getUsage() const {
    return usage_;
}

QString EnumeratedValue::getValue() const {
    return value_;
}

void EnumeratedValue::setUsage(EnumeratedValue::EnumeratedUsage usage) {
    usage_ = usage;
}

void EnumeratedValue::setValue( const QString& value ) {
    value_ = value;
}

QString EnumeratedValue::usage2Str(EnumeratedValue::EnumeratedUsage usage) {
	switch (usage) {
	case EnumeratedValue::READ: {
		return QString("read");
	}
	case EnumeratedValue::WRITE: {
		return QString("write");
	}
	case EnumeratedValue::READWRITE: {
		return QString("read-write");
	}
	// this should never happen
	default: {
		return QString();
	}
	}
}

EnumeratedValue::EnumeratedUsage EnumeratedValue::str2Usage(
                QString const& usage, EnumeratedUsage defaultValue) {
	if (usage == QString("read")) {
		return EnumeratedValue::READ;
	}
	else if (usage == QString("write")) {
		return EnumeratedValue::WRITE;
	}
	else if (usage == QString("read-write")) {
		return EnumeratedValue::READWRITE;
	}
	else {
		return defaultValue;
	}
}
