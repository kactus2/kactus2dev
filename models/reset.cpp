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
value_(-1),
valueAttributes_(), 
mask_(-1),
maskAttributes_() {

	// go through each node
	for (int i = 0; i < resetNode.childNodes().count(); ++i) {
		QDomNode tempNode = resetNode.childNodes().at(i);

		if (tempNode.nodeName() ==	QString("spirit:value")) {
			value_ = tempNode.childNodes().at(0).nodeValue().toInt();
			General::parseAttributes(tempNode, valueAttributes_);
		}
		else if (tempNode.nodeName() == QString("spirit:mask")) {
                        mask_ = tempNode.childNodes().at(0).nodeValue().toInt();
			General::parseAttributes(tempNode, maskAttributes_);
		}
	}

	// if mandatory value was not found
//         if (value_ < 0) {
// 		throw Parse_error(QObject::tr("Mandatory value missing in spirit:"
// 				"reset"));
// 	}
	return;
}

Reset::Reset( const Reset& other ):
value_(other.value_),
valueAttributes_(other.valueAttributes_),
mask_(other.mask_),
maskAttributes_(other.maskAttributes_) {
}

Reset& Reset::operator=( const Reset& other ) {
	if (this != &other) {
		value_ = other.value_;
		valueAttributes_ = other.valueAttributes_;
		mask_ = other.mask_;
		maskAttributes_ = other.maskAttributes_;
	}
	return *this;
}

Reset::~Reset() {
}

void Reset::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:reset");

	writer.writeStartElement("spirit:value");

	General::writeAttributes(writer, valueAttributes_);
	writer.writeCharacters(QString::number(value_));

	writer.writeEndElement(); // spirit:value

	// if optional mask has been defined
    if (mask_ >= 0) {
		writer.writeStartElement("spirit:mask");

		General::writeAttributes(writer, maskAttributes_);
                writer.writeCharacters(QString::number(mask_));

		writer.writeEndElement(); // spirit:mask
	}

	writer.writeEndElement(); // spirit:reset
}

bool Reset::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	if (value_ < 0) {
		errorList.append(QObject::tr("No value set for reset within %1").arg(
			parentIdentifier));
		return false;
	}
	return true;
}

bool Reset::isValid() const {
	if (value_ < 0) {
		return false;
	}
	return true;
}

int Reset::getMask() const {
	return mask_;
}

const QMap<QString,QString>& Reset::getMaskAttributes() const {
	return maskAttributes_;
}

int Reset::getValue() const {
	return value_;
}

const QMap<QString,QString>& Reset::getValueAttributes() const {
	return valueAttributes_;
}

void Reset::setMask(int mask) {
	mask_ = mask;
}

void Reset::setMaskAttributes(const QMap<QString,QString>& maskAttributes) {
	maskAttributes_ = maskAttributes;
}

void Reset::setValue(int value) {
	value_ = value;
}

void Reset::setValueAttributes(const QMap<QString,QString>& valueAttributes) {
	valueAttributes_ = valueAttributes;
}
