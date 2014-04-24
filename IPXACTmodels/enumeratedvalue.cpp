/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#include "enumeratedvalue.h"
#include "generaldeclarations.h"

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>
#include <QObject>

EnumeratedValue::EnumeratedValue(QDomNode& enumerationNode):
usage_(EnumeratedValue::READWRITE), 
nameGroup_(enumerationNode),
value_() {

	// parse the spirit:usage attribute
	QDomNamedNodeMap attributeMap = enumerationNode.attributes();
	QString usage = attributeMap.namedItem(QString("spirit:usage")).nodeValue();
	usage_ = str2Usage(usage, EnumeratedValue::READWRITE);

	for (int i = 0; i < enumerationNode.childNodes().count(); ++i) {
		QDomNode tempNode = enumerationNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();
		}
	}
}

EnumeratedValue::EnumeratedValue():
usage_(EnumeratedValue::READWRITE), 
nameGroup_(),
value_() {
}

EnumeratedValue::EnumeratedValue( const EnumeratedValue& other ):
usage_(other.usage_),
nameGroup_(other.nameGroup_),
value_(other.value_) {
}

EnumeratedValue& EnumeratedValue::operator=( const EnumeratedValue& other ) {
	if (this != &other) {
		usage_ = other.usage_;
		nameGroup_ = other.nameGroup_;
		value_ = other.value_;
	}
	return *this;
}

EnumeratedValue::~EnumeratedValue() {

}

void EnumeratedValue::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:enumeratedValue");

	// write the spirit:usage attribute
	writer.writeAttribute("spirit:usage", EnumeratedValue::usage2Str(usage_));

	writer.writeTextElement("spirit:name", nameGroup_.name_);
	
	// if optional displayName is defined
	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	// is optional description is defined
	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

	writer.writeTextElement("spirit:value", value_);

	writer.writeEndElement(); // spirit:enumeratedValue
}

bool EnumeratedValue::isValid( QStringList& errorList, 
							  const QString& parentIdentifier ) const {

	bool valid = true;

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for enumerated value"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (value_.isEmpty()) {
		errorList.append(QObject::tr("No value set for enumerated value %1 "
			"within %2").arg(nameGroup_.name_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool EnumeratedValue::isValid() const {
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	if (value_.isEmpty()) {
		return false;
	}

	return true;
}	

QString EnumeratedValue::getDescription() const {
    return nameGroup_.description_;
}

QString EnumeratedValue::getDisplayName() const {
    return nameGroup_.displayName_;
}

QString EnumeratedValue::getName() const {
    return nameGroup_.name_;
}

EnumeratedValue::EnumeratedUsage EnumeratedValue::getUsage() const {
    return usage_;
}

QString EnumeratedValue::getValue() const {
    return value_;
}

void EnumeratedValue::setDescription(const QString& description) {
    nameGroup_.description_ = description;
}

void EnumeratedValue::setDisplayName(const QString& displayName) {
    nameGroup_.displayName_ = displayName;
}

void EnumeratedValue::setName(const QString& name) {
    nameGroup_.name_ = name;
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
