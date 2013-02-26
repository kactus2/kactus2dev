/* 
 *  	Created on: 17.2.2012
 *      Author: Antti Kamppi
 * 		filename: segment.cpp
 *		Project: Kactus 2
 */

#include "segment.h"

#include <common/utils.h>

Segment::Segment( QDomNode &segmentNode ):
nameGroup_(segmentNode),
addressOffset_(),
offsetAttributes_(),
range_(),
rangeAttributes_() {

	for (int i = 0; i < segmentNode.childNodes().count(); ++i) {
		QDomNode tempNode = segmentNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:addressOffset")) {
			addressOffset_ = tempNode.childNodes().at(0).nodeValue();

			General::parseAttributes(tempNode, offsetAttributes_);
		}

		else if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue();

			General::parseAttributes(tempNode, rangeAttributes_);
		}
	}
}

Segment::Segment():
nameGroup_(),
addressOffset_(),
offsetAttributes_(),
range_(),
rangeAttributes_() {

}

Segment::Segment( const Segment& other ):
nameGroup_(other.nameGroup_),
addressOffset_(other.addressOffset_),
offsetAttributes_(other.offsetAttributes_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_) {
}

Segment::~Segment() {
}

Segment& Segment::operator=( const Segment& other ) {

	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		addressOffset_ = other.addressOffset_;
		offsetAttributes_ = other.offsetAttributes_;
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;
	}
	return *this;
}

void Segment::write( QXmlStreamWriter& writer ) {

	writer.writeStartElement("spirit:segment");

	if (!nameGroup_.name_.isEmpty()) {
		writer.writeTextElement("spirit:name", nameGroup_.name_);
	}

	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

	if (!addressOffset_.isEmpty()) {
		writer.writeStartElement("spirit:addressOffset");
		
		General::writeAttributes(writer, offsetAttributes_);
		writer.writeCharacters(addressOffset_);

		writer.writeEndElement(); // spirit:addressOffset
	}

	if (!range_.isEmpty()) {
		writer.writeStartElement("spirit:range");

		General::writeAttributes(writer, rangeAttributes_);
		writer.writeCharacters(range_);

		writer.writeEndElement(); // spirit:range
	}

	writer.writeEndElement(); // spirit:segment
}

bool Segment::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("segment %1").arg(nameGroup_.name_));

	if (nameGroup_.name_.isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No name set for segment within %1").arg(parentIdentifier));
	}

	if (addressOffset_.isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No address offset set for segment %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
	}

	if (range_.isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No range set for segment %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
	}

	return valid;
}

bool Segment::isValid() const {
	
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}
	else if (addressOffset_.isEmpty()) {
		return false;
	}
	else if (range_.isEmpty()) {
		return false;
	}

	return true;
}

QString Segment::getName() const {
	return nameGroup_.name_;
}

QString Segment::getDisplayName() const {
	return nameGroup_.displayName_;
}

QString Segment::getDescription() const {
	return nameGroup_.description_;
}

QString Segment::getAddressOffset() const {
	return addressOffset_;
}

QString Segment::getRange() const {
	return range_;
}

const QMap<QString, QString>& Segment::getOffsetAttributes() const {
	return offsetAttributes_;
}

const QMap<QString, QString>& Segment::getRangeAttributes() const {
	return rangeAttributes_;
}

void Segment::setName( const QString& name ) {
	nameGroup_.name_ = name;
}

void Segment::setDisplayName( const QString& dispName ) {
	nameGroup_.displayName_ = dispName;
}

void Segment::setDescription( const QString& description ) {
	nameGroup_.description_ = description;
}

void Segment::setOffset( const QString& addressOffset ) {
	addressOffset_ = addressOffset;
}

void Segment::setRange( const QString& range ) {
	range_ = range;
}

quint64 Segment::getLastAddress() const {
	quint64 base = Utils::str2Uint(addressOffset_);
	quint64 range = Utils::str2Uint(range_);

	quint64 lastAddr = base + range;

	// if the base and range are undefined then return 0
	if (lastAddr == 0) {
		return 0;
	}

	// if they are defined then return actual last address
	return lastAddr - 1;
}
