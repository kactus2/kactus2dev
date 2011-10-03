/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#include "modelparameter.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QString>
#include <QDomNode>
#include <QMap>
#include <QObject>
#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>

ModelParameter::ModelParameter(QDomNode &modelParameterNode): name_(QString()),
value_(QString()), attributes_(), valueAttributes_() {

	// get the modelParameter attributes
	General::parseAttributes(modelParameterNode, attributes_);

	for (int i = 0; i < modelParameterNode.childNodes().count(); ++i) {
		QDomNode tempNode = modelParameterNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
		}
		else if (tempNode.nodeName() == QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();

			// get the value attributes that define the value
			General::parseAttributes(tempNode, valueAttributes_);
		}
	}

	// if mandatory elements are missing
	if (name_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element name missing in "
				"spirit:modelParameter"));
	}
	return;
}

ModelParameter::ModelParameter(): name_(),
value_(), attributes_(), valueAttributes_() {
}

// the copy constructor
ModelParameter::ModelParameter(const ModelParameter &other ):
name_(other.name_), 
value_(other.value_), 
attributes_(other.attributes_),
valueAttributes_(other.valueAttributes_) {
}

ModelParameter & ModelParameter::operator=( const ModelParameter &other ) {
	if (this != &other) {
		name_ = other.name_;
		value_ = other.value_;
		attributes_ = other.attributes_;
		valueAttributes_ = other.valueAttributes_;
	}
	return *this;
}

ModelParameter::~ModelParameter() {
	attributes_.clear();
	valueAttributes_.clear();
	return;
}

void ModelParameter::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:modelParameter");
	General::writeAttributes(writer, attributes_);

	// if name is not defined
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in "
				"spirit:modelParameter"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	if (value_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory value missing is spirit:"
				"modelParameter"));
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

	writer.writeEndElement(); // spirit:modelParameter
}

const QMap<QString, QString>& ModelParameter::getAttributes() {
	return attributes_;
}

const QMap<QString, QString>& ModelParameter::getValueAttributes() {
	return valueAttributes_;
}

void ModelParameter::setAttributes(const QMap<QString, QString> &attributes) {
	// delete the old attributes
	attributes_.clear();

	// save the new attributes
	attributes_ = attributes;
}

void ModelParameter::setValue(const QString &value) {
	value_ = value;
}

void ModelParameter::setName(const QString &name) {
	name_ = name;
}

void ModelParameter::setValueAttributes(const
		QMap<QString, QString> &valueAttributes) {
	// delete the old value attributes
	valueAttributes_.clear();

	// save the new value attributes
	valueAttributes_ = valueAttributes;
}

QString ModelParameter::getName() const {
	return name_;
}

QString ModelParameter::getValue() const {
	return value_;
}

QString ModelParameter::getDataType() const {
	return attributes_.value(QString("spirit:dataType"));
}

void ModelParameter::setDataType( const QString& dataType ) {
	attributes_.insert(QString("spirit:dataType"), dataType);
}

QString ModelParameter::getUsageType() const {
	return attributes_.value(QString("spirit:usageType"));
}

void ModelParameter::setUsageType( const QString& usageType ) {
	attributes_.insert(QString("spirit:usageType"), usageType);
}

bool ModelParameter::isValid() const {

	// if usage type is specified
	if (attributes_.contains(QString("spirit:usageType"))) {
		
		// if usage is not valid type
		QString usage = attributes_.value(QString("spirit:usageType"));
		if (usage != QString("nontyped") && usage != QString("typed"))
			return false;
	}

	// name and value must be defined
	if (name_.isEmpty() || value_.isEmpty())
		return false;

	return true;
}
