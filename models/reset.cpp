/* 
 *
 *  Created on: 21.10.2010
 *      Author: Antti Kamppi
 */

#include "reset.h"
#include "generaldeclarations.h"

#include <QDomNode>
#include <QList>
#include <QMap>
#include <QXmlStreamWriter>
#include <QString>
#include <QObject>

Reset::Reset(QDomNode& resetNode): 
value_(),
mask_() {

	// go through each node
	for (int i = 0; i < resetNode.childNodes().count(); ++i) {
		QDomNode tempNode = resetNode.childNodes().at(i);

		if (tempNode.nodeName() ==	QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue();
		}
		else if (tempNode.nodeName() == QString("spirit:mask")) {
			mask_ = tempNode.childNodes().at(0).nodeValue();
		}
	}
}

Reset::Reset():
value_(),
mask_() {
}

Reset::Reset( const Reset& other ):
value_(other.value_),
mask_(other.mask_) {
}

Reset& Reset::operator=( const Reset& other ) {
	if (this != &other) {
		value_ = other.value_;
		mask_ = other.mask_;
	}
	return *this;
}

Reset::~Reset() {
}

void Reset::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:reset");

	writer.writeStartElement("spirit:value");
	writer.writeCharacters(value_);
	writer.writeEndElement(); // spirit:value

	// if optional mask has been defined
	if (!mask_.isEmpty()) {
		writer.writeStartElement("spirit:mask");
		writer.writeCharacters(mask_);
		writer.writeEndElement(); // spirit:mask
	}
	writer.writeEndElement(); // spirit:reset
}

bool Reset::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	if (value_.isEmpty()) {
		errorList.append(QObject::tr("No value set for reset within %1").arg(
			parentIdentifier));
		return false;
	}
	return true;
}

bool Reset::isValid() const {
	if (value_.isEmpty()) {
		return false;
	}
	return true;
}

QString Reset::getMask() const {
	return mask_;
}

QString Reset::getValue() const {
	return value_;
}

void Reset::setMask( const QString& mask ) {
	mask_ = mask;
}

void Reset::setValue( const QString& value ) {
	value_ = value;
}
