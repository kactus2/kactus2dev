/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#include "enumeratedvalue.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>
#include <QObject>

EnumeratedValue::EnumeratedValue(QDomNode& enumerationNode):
usage_(EnumeratedValue::READWRITE), 
name_(), 
displayName_(),
description_(),
value_() {

	// parse the spirit:usage attribute
	QDomNamedNodeMap attributeMap = enumerationNode.attributes();
	QString usage = attributeMap.namedItem(QString("spirit:usage")).nodeValue();
	usage_ = str2Usage(usage, EnumeratedValue::READWRITE);

	for (int i = 0; i < enumerationNode.childNodes().count(); ++i) {
		QDomNode tempNode = enumerationNode.childNodes().at(i);

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
		else if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();
		}

	}

	// if mandatory name is not specified
// 	if (name_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory name is missing in spirit:"
// 				"enumeratedValue"));
// 	}
// 
	return;
}

EnumeratedValue::EnumeratedValue( const EnumeratedValue& other ):
usage_(other.usage_),
name_(other.name_),
displayName_(other.displayName_),
description_(other.description_),
value_(other.value_) {
}

EnumeratedValue& EnumeratedValue::operator=( const EnumeratedValue& other ) {
	if (this != &other) {
		usage_ = other.usage_;
		name_ = other.name_;
		displayName_ = other.displayName_;
		description_ = other.description_;
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

	// if mandatory name is missing
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in spirit:"
				"enumeratedValue"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	// if optional displayName is defined
	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	// is optional description is defined
	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// if mandatory value is not defined
	if (value_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory value missing in spirit:"
				"enumeratedValue"));
	}
	else {
		writer.writeTextElement("spirit:value", value_);
	}

	writer.writeEndElement(); // spirit:enumeratedValue
}

bool EnumeratedValue::isValid( QStringList& errorList, 
							  const QString& parentIdentifier ) const {

	bool valid = true;

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for enumerated value"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (value_.isEmpty()) {
		errorList.append(QObject::tr("No value set for enumerated value %1 "
			"within %2").arg(name_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

QString EnumeratedValue::getDescription() const {
    return description_;
}

QString EnumeratedValue::getDisplayName() const {
    return displayName_;
}

QString EnumeratedValue::getName() const {
    return name_;
}

EnumeratedValue::EnumeratedUsage EnumeratedValue::getUsage() const {
    return usage_;
}

QString EnumeratedValue::getValue() const {
    return value_;
}

void EnumeratedValue::setDescription(const QString& description) {
    this->description_ = description;
}

void EnumeratedValue::setDisplayName(const QString& displayName) {
    this->displayName_ = displayName;
}

void EnumeratedValue::setName(const QString& name) {
    this->name_ = name;
}

void EnumeratedValue::setUsage(EnumeratedValue::EnumeratedUsage usage) {
    this->usage_ = usage;
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
                QString& usage, EnumeratedUsage defaultValue) {
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
