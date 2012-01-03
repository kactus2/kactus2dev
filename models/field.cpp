/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#include "field.h"
#include "generaldeclarations.h"
#include "parameter.h"
#include "enumeratedvalue.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QString>
#include <QXmlStreamWriter>
#include <QMap>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QDomNamedNodeMap>

#include <QDebug>

Field::Field(QDomNode& fieldNode): id_(), name_(), displayName_(),
description_(), bitOffset_(-1), typeIdentifier_(), bitWidth_(0),
bitWidthAttributes_(), enumeratedValues_(), parameters_() {

	// parse the spirit:id attribute
	QDomNamedNodeMap attributeMap = fieldNode.attributes();
	id_ = attributeMap.namedItem("spirit:id").nodeValue();
	id_ = General::removeWhiteSpace(id_);

	// go through all nodes and parse them
	for (int i = 0; i < fieldNode.childNodes().count(); ++i) {
		QDomNode tempNode = fieldNode.childNodes().at(i);

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
		else if (tempNode.nodeName() == QString("spirit:bitOffset")) {
			bitOffset_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:typeIdentifier")) {
			typeIdentifier_ = tempNode.childNodes().at(0).nodeValue();
			typeIdentifier_ = General::removeWhiteSpace(typeIdentifier_);
		}
		else if (tempNode.nodeName() == QString("spirit:bitWidth")) {
			bitWidth_ = tempNode.childNodes().at(0).nodeValue().toInt();
			General::parseAttributes(tempNode, bitWidthAttributes_);
		}
		else if (tempNode.nodeName() == QString("spirit:enumeratedValues")) {

			// parse each enumerated value
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode enumeratedNode = tempNode.childNodes().at(j);

				enumeratedValues_.append(QSharedPointer<EnumeratedValue>(
						new EnumeratedValue(enumeratedNode)));
			}
		}
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// parse each parameter
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);

				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
	}

	// if mandatory name is missing
// 	if (name_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory name is missing in spirit:"
// 				"field"));
// 	}
// 
// 	// if mandatory bitOffset is missing
// 	if (bitOffset_ < 0) {
// 		throw Parse_error(QObject::tr("Mandatory bitOffset is missing in"
// 				"spirit:field"));
// 	}
// 
// 	// if mandatory bitWidth is missing
//         if (bitWidth_ <= 0) {
// 		throw Parse_error(QObject::tr("Mandatory bitWidth missing in spirit:"
// 				"field"));
// 	}

	return;
}

Field::Field( const Field& other ):
id_(other.id_),
name_(other.name_),
displayName_(other.displayName_),
description_(other.description_),
bitOffset_(other.bitOffset_),
typeIdentifier_(other.typeIdentifier_),
bitWidth_(other.bitWidth_),
bitWidthAttributes_(other.bitWidthAttributes_),
enumeratedValues_(),
parameters_() {

	foreach (QSharedPointer<EnumeratedValue> enumValue, other.enumeratedValues_) {
		if (enumValue) {
			QSharedPointer<EnumeratedValue> copy = QSharedPointer<EnumeratedValue>(
			new EnumeratedValue(*enumValue.data()));
			enumeratedValues_.append(copy);
		}
	}

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

Field& Field::operator=( const Field& other ) {
	if (this != &other) {
		id_ = other.id_;
		name_ = other.name_;
		displayName_ = other.displayName_;
		description_ = other.description_;
		bitOffset_ = other.bitOffset_;
		typeIdentifier_ = other.typeIdentifier_;
		bitWidth_ = other.bitWidth_;
		bitWidthAttributes_ = other.bitWidthAttributes_;

		enumeratedValues_.clear();
		foreach (QSharedPointer<EnumeratedValue> enumValue, other.enumeratedValues_) {
			if (enumValue) {
				QSharedPointer<EnumeratedValue> copy = QSharedPointer<EnumeratedValue>(
					new EnumeratedValue(*enumValue.data()));
				enumeratedValues_.append(copy);
			}
		}

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}
	}
	return *this;
}

Field::~Field() {
	enumeratedValues_.clear();
	parameters_.clear();
	bitWidthAttributes_.clear();
}

void Field::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:field");

	// if spirit:id attribute is defined
	if (!id_.isEmpty()) {
		writer.writeAttribute("spirit:id", id_);
	}

	// if mandatory name is not defined
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in spirit:"
				"field"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	// if optional displayName is defined
	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	// if optional description is defined
	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// if mandatory bitOffset is not defined
	if (bitOffset_ < 0) {
		throw Write_error(QObject::tr("Mandatory bitOffset missing in spirit:"
				"field"));
	}
	else {
		writer.writeTextElement("spirit:bitOffset",
				QString::number(bitOffset_));
	}

	// if optional typeIdentifier is defined
	if (!typeIdentifier_.isEmpty()) {
		writer.writeTextElement("spirit:typeIdentifier", typeIdentifier_);
	}

	// if mandatory bitWidth is not defined
	if (bitWidth_ <= 0) {
		throw Write_error(QObject::tr("Mandatory bitWidth missing in spirit:"
				"field"));
	}
	else {
		writer.writeStartElement("spirit:bitWidth");

		General::writeAttributes(writer, bitWidthAttributes_);
		writer.writeCharacters(QString::number(bitWidth_));

		writer.writeEndElement(); // spirit:bitWidth
	}

	// if optional enumeratedValues exist
	if (enumeratedValues_.size() != 0) {
		writer.writeStartElement("spirit:enumeratedValues");

		for (int i = 0; i < enumeratedValues_.size(); ++i) {
			enumeratedValues_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:enumeratedValues
	}

	// if optional parameters exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

	writer.writeEndElement(); // spirit:field
}

bool Field::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for a field within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (bitOffset_ < 0) {
		errorList.append(QObject::tr("No bit offset set for field %1 within %2").arg(
			name_).arg(parentIdentifier));
		valid = false;
	}

	if (bitWidth_ <= 0) {
		errorList.append(QObject::tr("No bit width set for field %1 within %2").arg(
			name_).arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<EnumeratedValue> enumValue, enumeratedValues_) {
		if (!enumValue->isValid(errorList, QObject::tr("field %1").arg(name_))) {
			valid = false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, QObject::tr("field %1").arg(name_))) {
			valid = false;
		}
	}

	return valid;
}

int Field::getBitOffset() const {
    return bitOffset_;
}

unsigned int Field::getBitWidth() const {
    return bitWidth_;
}

const QMap<QString,QString>& Field::getBitWidthAttributes() const {
    return bitWidthAttributes_;
}

QString Field::getDescription() const {
    return description_;
}

QString Field::getDisplayName() const {
    return displayName_;
}

const QList<QSharedPointer<EnumeratedValue> >&
Field::getEnumeratedValues() const {
    return enumeratedValues_;
}

QString Field::getName() const {
    return name_;
}

const QList<QSharedPointer<Parameter> >& Field::getParameters() const {
    return parameters_;
}

QString Field::getTypeIdentifier() const {
    return typeIdentifier_;
}

void Field::setBitOffset(int bitOffset) {
    this->bitOffset_ = bitOffset;
}

void Field::setBitWidth(unsigned int bitWidth) {
    this->bitWidth_ = bitWidth;
}

void Field::setBitWidthAttributes(
		const QMap<QString,QString>& bitWidthAttributes) {
    this->bitWidthAttributes_ = bitWidthAttributes;
}

void Field::setDescription(const QString& description) {
    this->description_ = description;
}

void Field::setDisplayName(const QString& displayName) {
    this->displayName_ = displayName;
}

void Field::setEnumeratedValues(
		const QList<QSharedPointer<EnumeratedValue> >& enumeratedValues) {
    this->enumeratedValues_ = enumeratedValues;
}

void Field::setName(const QString& name) {
    this->name_ = name;
}

void Field::setParameters(const QList<QSharedPointer<Parameter> >& parameters) {
	parameters_.clear();
    this->parameters_ = parameters;
}

void Field::setTypeIdentifier(const QString& typeIdentifier) {
    this->typeIdentifier_ = typeIdentifier;
}

QString Field::getId() const {
    return id_;
}

void Field::setId(const QString& id) {
    this->id_ = id;
}
