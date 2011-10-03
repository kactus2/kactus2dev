/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "parameter.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QMap>
#include <QXmlStreamWriter>

// the constructor
Parameter::Parameter(QDomNode &parameterNode): name_(QString()),
value_(QString()), valueAttributes_(), description_() {

	for (int i = 0; i < parameterNode.childNodes().count(); ++i) {
		QDomNode tempNode = parameterNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
		}
		else if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();

			// parse all the attributes
			General::parseAttributes(tempNode, valueAttributes_);
		}
		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}
	}
	// if name or value was not found then document is invalid
	if (name_.isNull() ) {
		throw Parse_error(QObject::tr("Mandatory element name missing in "
				"spirit:parameter"));
	}

	if (value_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element value missing in"
				" spirit:parameter"));
	}
	return;
}

Parameter::Parameter(): name_(), value_(), valueAttributes_(), description_() {
}

Parameter::Parameter( const Parameter &other ):
name_(other.name_),
value_(other.value_),
valueAttributes_(other.valueAttributes_),
description_(other.description_) {
}

Parameter & Parameter::operator=( const Parameter &other ) {
	if (this != &other) {
		name_ = other.name_;
		value_ = other.value_;
		valueAttributes_ = other.valueAttributes_;
		description_ = other.description_;
	}
	return *this;
}

// the destructor
Parameter::~Parameter() {
}

void Parameter::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:parameter");

	// if the mandatory name is not specified
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in "
				"spirit:parameter"));
	}
	else {
		// write the name of the attribute
		writer.writeTextElement("spirit:name", name_);
	}

	// write the description if it exists
	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// if the mandatory value is not specified
	if (value_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory value missing in spirit:"
				"parameter"));
	}
	else {
		// start the spirit:value tag
		writer.writeStartElement("spirit:value");

		// write the attributes for the element
		General::writeAttributes(writer, valueAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(value_);
		writer.writeEndElement(); // spirit:value
	}

	writer.writeEndElement(); // spirit:parameter
	return;
}

void Parameter::setValueAttributes(const QMap<QString, QString> &attributes) {
	// first delete old attributes
	valueAttributes_.clear();

	// save new attributes
	valueAttributes_ = attributes;
}

QString Parameter::getName() const {
	return name_;
}

void Parameter::setName(const QString &name) {
	name_ = name;
}

void Parameter::setValue(const QString &value) {
	value_ = value;
}

const QMap<QString, QString>& Parameter::getValueAttributes() {
	return valueAttributes_;
}

QString Parameter::getValue() const {
	return value_;
}

QString Parameter::getDescription() const {
	return description_;
}

void Parameter::setDescription(const QString& description) {
	description_ = description;
}

bool Parameter::isValid() const {

	if (name_.isEmpty())
		return false;

	else if (value_.isEmpty())
		return false;

	return true;
}
