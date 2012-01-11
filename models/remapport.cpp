/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"
#include "remapport.h"


#include "../exceptions/write_error.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QObject>
#include <QDomNamedNodeMap>

RemapPort::RemapPort(QDomNode &remapPortNode): value_(QString()),
portNameRef_(QString()), portIndex_(-1) {

	// get the attributes
	QDomNamedNodeMap attributeMap = remapPortNode.attributes();

	// get the portNameRef attribute and strip the whitespace characters
	portNameRef_ = attributeMap.namedItem(QString("spirit:portNameRef")).
			childNodes().at(0).nodeValue();
	portNameRef_ = General::removeWhiteSpace(portNameRef_);

	QString portIndex = attributeMap.namedItem(QString("spirit:portIndex")).
			childNodes().at(0).nodeValue();
	if (!portIndex.isNull()) {
		portIndex_ = portIndex.toInt();
	}

	value_ = remapPortNode.childNodes().at(0).nodeValue();

	// if mandatory elements are missing
// 	if (value_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory value for spirit:remapPort"
// 				" missing"));
// 	}
// 
// 	if (portNameRef_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory attribute spirit:portNameRef"
// 				" missing in spirit:remapPort"));
// 	}
	return;
}

RemapPort::RemapPort( const RemapPort &other ):
value_(other.value_),
portNameRef_(other.portNameRef_),
portIndex_(other.portIndex_) {
}


RemapPort & RemapPort::operator=( const RemapPort &other ) {
	if (this != &other) {

		value_ = other.value_;
		portNameRef_ = other.portNameRef_;
		portIndex_ = other.portIndex_;
	}
	return *this;
}


RemapPort::~RemapPort() {
}

void RemapPort::write(QXmlStreamWriter& writer) {
	if (value_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory value for spirit:remapPort is"
				" missing"));
	}

	// start the remapPort element
	writer.writeStartElement("spirit:remapPort");

	// if mandatory portNameRef is empty
	if (portNameRef_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element spirit:portNameRef"
				" is missing"));
	}
	else {
		writer.writeAttribute("spirit:portNameRef", portNameRef_);
	}

	// if optional portIndex is positive its written
	if (portIndex_ >= 0) {
            writer.writeAttribute("spirit:portIndex", QString::number(portIndex_));
	}

	// write the remapPort element value after attributes and close tag
	writer.writeCharacters(value_);
	writer.writeEndElement(); // spirit:remapPort
	return;
}

bool RemapPort::isValid( const QStringList& portNames, 
						QStringList& errorList, 
						const QString& parentIdentifier ) const {
	bool valid = true;

	if (value_.isEmpty()) {
		errorList.append(QObject::tr("No value set for remap port within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (portNameRef_.isEmpty()) {
		errorList.append(QObject::tr("No port reference assigned to remap port"
			" within %1").arg(parentIdentifier));
		valid = false;
	}
	else if (!portNames.contains(portNameRef_)) {
		errorList.append(QObject::tr("The port %1 referenced in remap port "
			" within %2 is not found in the containing component.").arg(
			portNameRef_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool RemapPort::isValid( const QStringList& portNames ) const {
	if (value_.isEmpty()) {
		return false;
	}

	if (portNameRef_.isEmpty()) {
		return false;
	}
	else if (!portNames.contains(portNameRef_)) {
		return false;
	}

	return true;
}

void RemapPort::setValue(const QString &value) {
	value_ = value;
}

QString RemapPort::getValue() const {
	return value_;
}

void RemapPort::setPortNameRef(const QString &portNameRef) {
	portNameRef_ = portNameRef;
}

QString RemapPort::getPortNameRef() const {
	return portNameRef_;
}

int RemapPort::getPortIndex() const {
	return portIndex_;
}

void RemapPort::setPortIndex(int portIndex) {
	portIndex_ = portIndex;
}
