/* 
 *  	Created on: 29.8.2012
 *      Author: Antti Kamppi
 * 		filename: writevalueconstraint.cpp
 *		Project: Kactus 2
 */

#include "writevalueconstraint.h"
#include "generaldeclarations.h"

WriteValueConstraint::WriteValueConstraint():
type_(WriteValueConstraint::TYPE_COUNT),
writeAsRead_(false),
useEnumValues_(false),
min_(0),
max_(0) {
}

WriteValueConstraint::WriteValueConstraint( QDomNode& constrNode ):
type_(WriteValueConstraint::TYPE_COUNT),
writeAsRead_(false),
useEnumValues_(false),
min_(0),
max_(0) {
	
	// go through all nodes and parse them
	for (int i = 0; i < constrNode.childNodes().count(); ++i) {
		QDomNode tempNode = constrNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:writeAsRead")) {
			type_ = WriteValueConstraint::WRITE_AS_READ;
			QString str = tempNode.childNodes().at(0).nodeValue();
			writeAsRead_ = General::str2Bool(str, false);
		}
		else if (tempNode.nodeName() == QString("spirit:useEnumeratedValues")) {
			type_ = WriteValueConstraint::USE_ENUM;
			QString str = tempNode.childNodes().at(0).nodeValue();
			useEnumValues_ = General::str2Bool(str, false);
		}
		else if (tempNode.nodeName() == QString("spirit:minimum")) {
			type_ = WriteValueConstraint::MIN_MAX;
			min_ = tempNode.childNodes().at(0).nodeValue().toUInt();
		}
		else if (tempNode.nodeName() == QString("spirit:maximum")) {
			type_ = WriteValueConstraint::MIN_MAX;
			max_ = tempNode.childNodes().at(0).nodeValue().toUInt();
		}
	}
}

WriteValueConstraint::WriteValueConstraint( const WriteValueConstraint& other):
type_(other.type_),
writeAsRead_(other.writeAsRead_),
useEnumValues_(other.useEnumValues_),
min_(other.min_),
max_(other.max_) {
}

WriteValueConstraint::~WriteValueConstraint() {
}

WriteValueConstraint& WriteValueConstraint::operator=( const WriteValueConstraint& other) {
	if (&other != this) {
		type_ = other.type_;
		writeAsRead_ = other.writeAsRead_;
		useEnumValues_ = other.useEnumValues_;
		min_ = other.min_;
		max_ = other.max_;
	}
	return *this;
}

void WriteValueConstraint::write( QXmlStreamWriter& writer ) {
	// if the type is not valid then don't write anything
	if (type_ == WriteValueConstraint::TYPE_COUNT) {
		return;
	}
	
	writer.writeStartElement("spirit:writeValueConstraint");

	switch (type_) {
		case WriteValueConstraint::WRITE_AS_READ: {
			writer.writeTextElement("spirit:writeAsRead", General::bool2Str(writeAsRead_));
			break;
												  }
		case WriteValueConstraint::USE_ENUM: {
			writer.writeTextElement("spirit:useEnumeratedValues", General::bool2Str(useEnumValues_));
			break;
											 }
		case WriteValueConstraint::MIN_MAX: {
			writer.writeTextElement("spirit:minimum", QString::number(min_, 10));
			writer.writeTextElement("spirit:maximum", QString::number(max_, 10));
			break;
											}
		default: {
			Q_ASSERT(false);
			return;
				 }
	}

	writer.writeEndElement(); // spirit:writeValueConstraint
}

WriteValueConstraint::Type WriteValueConstraint::getType() const {
	return type_;
}

void WriteValueConstraint::setType(WriteValueConstraint::Type type ) {
	type_ = type;

	switch (type_) {
		case WriteValueConstraint::WRITE_AS_READ: {
			writeAsRead_ = true;
			break;
												  }
		case WriteValueConstraint::USE_ENUM: {
			useEnumValues_ = true;
			break;
											 }
		case WriteValueConstraint::MIN_MAX: {
			min_ = 0;
			max_ = 0;
			break;
											}
	}
}

unsigned int WriteValueConstraint::getMinimum() const {
	switch (type_) {
		case WriteValueConstraint::MIN_MAX: {
			return min_;
											}
		case WriteValueConstraint::WRITE_AS_READ:
		case WriteValueConstraint::USE_ENUM: 
		default: {
			return 0;
				 }
	}
}

void WriteValueConstraint::setMinimum( unsigned int minimum ) {
	type_ = WriteValueConstraint::MIN_MAX;
	min_ = minimum;
}

unsigned int WriteValueConstraint::getMaximum() const {
	switch (type_) {
		case WriteValueConstraint::MIN_MAX: {
			return max_;
											}
		case WriteValueConstraint::WRITE_AS_READ:
		case WriteValueConstraint::USE_ENUM: 
		default: {
			return 0;
				 }
	}
}

void WriteValueConstraint::setMaximum( unsigned int maximum ) {
	type_ = WriteValueConstraint::MIN_MAX;
	max_ = maximum;
}
