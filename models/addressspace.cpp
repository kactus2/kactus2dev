/* 
 *
 *  Created on: 28.7.2010
 *      Author: Antti Kamppi
 */

#include "addressspace.h"
#include "memorymap.h"
#include "parameter.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QString>
#include <QObject>
#include <QList>
#include <QXmlStreamWriter>

const int DEFAULT_ADDRESS_UNIT_BITS = 8;

// the constructor
AddressSpace::AddressSpace(QDomNode &addrSpaceNode): name_(QString()),
		range_(QString()), rangeAttributes_(), width_(-1), widthAttributes_(),
		addressUnitBits_(DEFAULT_ADDRESS_UNIT_BITS), localMemoryMap_(0),
		parameters_() {
	for (int i = 0; i < addrSpaceNode.childNodes().count(); ++i) {
		QDomNode tempNode = addrSpaceNode.childNodes().at(i);

		// get name
		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
		}

		// get range
		else if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue();

			// get the attributes
			General::parseAttributes(tempNode, rangeAttributes_);
		}

		// get width
		else if (tempNode.nodeName() == QString("spirit:width")) {
			width_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get the attributes
			General::parseAttributes(tempNode, widthAttributes_);
		}

		// get addressUnitBits
		else if (tempNode.nodeName() == QString("spirit:addressUnitBits")) {
			addressUnitBits_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}

		else if (tempNode.nodeName() == QString("spirit:localMemoryMap")) {
			localMemoryMap_ = QSharedPointer<MemoryMap>(
					new MemoryMap(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:parameters")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);
				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
	}

// 	if (name_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element name missing in "
// 				"spirit:addressSpace"));
// 	}
// 
// 	if (range_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element range missing in"
// 				" spirit:addressSpace"));
// 	}
// 
// 	if (width_ < 0) {
// 		throw Parse_error(QObject::tr("Mandatory element width missing in "
// 				"spirit:addressSpace"));
// 	}
	return;
}

AddressSpace::AddressSpace( const AddressSpace &other ):
name_(other.name_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
width_(other.width_),
widthAttributes_(other.widthAttributes_),
addressUnitBits_(other.addressUnitBits_),
localMemoryMap_(),
parameters_() {

	if (other.localMemoryMap_) {
		localMemoryMap_ = QSharedPointer<MemoryMap>(
			new MemoryMap(*other.localMemoryMap_.data()));
	}

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
		}
	}
}


AddressSpace & AddressSpace::operator=( const AddressSpace &other ) {
	if (this != &other) {
		name_ = other.name_;
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;
		width_ = other.width_;
		widthAttributes_ = other.widthAttributes_;
		addressUnitBits_ = other.addressUnitBits_;

		if (other.localMemoryMap_) {
			localMemoryMap_ = QSharedPointer<MemoryMap>(
				new MemoryMap(*other.localMemoryMap_.data()));
		}
		else
			localMemoryMap_ = QSharedPointer<MemoryMap>();

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
			}
		}
	}
	return *this;
}

// the desctructor
AddressSpace::~AddressSpace() {
	rangeAttributes_.clear();

	widthAttributes_.clear();
}

void AddressSpace::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:addressSpace");

	// if mandatory name is missing
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in spirit:"
				"addressSpace"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	// if mandatory range is missing
	if (range_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element range is missing in "
				"spirit:addressSpace"));
	}
	else {
		// start the spirit:range tag
		writer.writeStartElement("spirit:range");

		// write the attributes for the element
		General::writeAttributes(writer, rangeAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(range_);
		writer.writeEndElement(); // spirit:range
	}

	// if mandatory element width is missing
	if (width_ < 0) {
		throw Write_error(QObject::tr("Mandatory element width missing in "
				"spirit:addressSpace"));
	}
	else {
		// start the spirit:width element
		writer.writeStartElement("spirit:width");

		// write the attributes for the element
		General::writeAttributes(writer, widthAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(QString::number(width_));
		writer.writeEndElement(); // spirit:width
	}

	// if addressUnitBits has illegal value
	if (addressUnitBits_ < 1) {
		throw Write_error(QObject::tr("AddressUnitBits has illegal value in "
				"spirit:addressSpace"));
	}
	else {
		writer.writeTextElement("spirit:addressUnitBits",
				QString::number(addressUnitBits_));
	}

	if (localMemoryMap_) {
		writer.writeStartElement("spirit:localMemoryMap");
		localMemoryMap_->write(writer);
		writer.writeEndElement(); // spirit:localMemoryMap
	}

	// if any parameters exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// write each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

	writer.writeEndElement(); // spirit:addressSpace
}

bool AddressSpace::isValid( QStringList& errorList, 
						   const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("address space %1").arg(name_));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (range_.isEmpty()) {
		errorList.append(QObject::tr("No range set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (width_ < 0) {
		errorList.append(QObject::tr("No width set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (localMemoryMap_ && !localMemoryMap_->isValid(errorList, thisIdentifier)) {
		valid = false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

void AddressSpace::setAddressUnitBits(unsigned int addressUnitBits) {
	addressUnitBits_ = addressUnitBits;
}

unsigned int AddressSpace::getAddressUnitBits() const {
	return addressUnitBits_;
}

void AddressSpace::setRange(const QString &range) {
	range_ = range;
}

QString AddressSpace::getRange() const {
	return range_;
}

const QMap<QString, QString>& AddressSpace::getRangeAttributes() {
	return rangeAttributes_;
}

void AddressSpace::setRangeAttributes(
		const QMap<QString, QString> &rangeAttributes) {
	// delete old range attributes
	rangeAttributes_.clear();

	// save new range attributes
	rangeAttributes_ = rangeAttributes;
}

void AddressSpace::setName(const QString &name) {
	name_ = name;
}

unsigned int AddressSpace::getWidth() const {
	return width_;
}

const QMap<QString, QString>& AddressSpace::getWidthAttributes() {
	return widthAttributes_;
}

void AddressSpace::setWidthAttributes(
		const QMap<QString, QString> &widthAttributes) {
	// delete old width attributes
	widthAttributes_.clear();

	// save new width attributes
	widthAttributes_ = widthAttributes;
}

QString AddressSpace::getName() const {
	return name_;
}

void AddressSpace::setWidth(unsigned int width) {
	width_ = width;
}

MemoryMap *AddressSpace::getLocalMemoryMap() const {
	return localMemoryMap_.data();
}

void AddressSpace::setLocalMemoryMap(MemoryMap *localMemoryMap) {
	if (localMemoryMap_) {
		localMemoryMap_.clear();
	}
	localMemoryMap_ = QSharedPointer<MemoryMap>(localMemoryMap);
}
