/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#include "remapstate.h"
#include "remapport.h"
#include "generaldeclarations.h"


#include "../exceptions/write_error.h"

#include <QList>
#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

// the constructor
RemapState::RemapState(QDomNode &remapNode): name_(QString()), remapPorts_() {
	for (int i = 0; i < remapNode.childNodes().count(); ++i) {

		QDomNode tempNode = remapNode.childNodes().at(i);

		// get name
		if (tempNode.nodeName() == QString("spirit:name")) {

			// strip all whitespace characters
			name_ = tempNode.childNodes().at(0).nodeValue();
			name_ = General::removeWhiteSpace(name_);
		}

		// get remapPorts
		else if (tempNode.nodeName() == QString("spirit:remapPorts")) {

			// go through all the remapPorts
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode remapPortNode = tempNode.childNodes().at(j);
				remapPorts_.append(QSharedPointer<RemapPort>(
						new RemapPort(remapPortNode)));
			}
		}
	}

	// if mandatory field is missing
// 	if (name_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element name missing in "
// 				"spirit:remapState"));
// 	}
	return;
}

RemapState::RemapState( const RemapState &other ):
name_(other.name_),
remapPorts_() {

	foreach (QSharedPointer<RemapPort> remapPort, other.remapPorts_) {
		if (remapPort) {
			QSharedPointer<RemapPort> copy = QSharedPointer<RemapPort>(
				new RemapPort(*remapPort.data()));
			remapPorts_.append(copy);
		}
	}
}

RemapState & RemapState::operator=( const RemapState &other ) {
	if (this != &other) {
		name_ = other.name_;

		foreach (QSharedPointer<RemapPort> remapPort, other.remapPorts_) {
			if (remapPort) {
				QSharedPointer<RemapPort> copy = QSharedPointer<RemapPort>(
					new RemapPort(*remapPort.data()));
				remapPorts_.append(copy);
			}
		}
	}
	return *this;
}

// the destructor
RemapState::~RemapState() {
	remapPorts_.clear();
}

void RemapState::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:remapState");

	// if mandatory name is empty
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element name missing in "
				"spirit:remapState"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}
	// if remapPorts exist
	if (remapPorts_.size() != 0) {
		writer.writeStartElement("spirit:remapPorts");

		// tell remapPorts to write themselves
		for (int i = 0; i < remapPorts_.size(); ++i) {
			remapPorts_.at(i)->write(writer);
		}
		writer.writeEndElement(); // spirit:remapPorts
	}

	writer.writeEndElement(); // spirit:remapState
	return;
}

bool RemapState::isValid( const QStringList& portNames,
						 QStringList& errorList,
						 const QString& parentIdentifier ) const {

	bool valid = true;

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for a remap state within %1").arg(
			parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<RemapPort> remapPort, remapPorts_) {
		if (!remapPort->isValid(portNames, errorList, QObject::tr("remap port %1").arg(
			name_))) {
				valid = false;
		}
	}

	return valid;
}

bool RemapState::isValid( const QStringList& portNames ) const {
	if (name_.isEmpty()) {
		return false;
	}

	foreach (QSharedPointer<RemapPort> remapPort, remapPorts_) {
		if (!remapPort->isValid(portNames)) {
				return false;
		}
	}
	return true;
}

void RemapState::setRemapPorts(const QList<QSharedPointer<RemapPort> > &remapPorts) {
	// delete the old remap ports
	remapPorts_.clear();

	// save the new remap ports
	remapPorts_ = remapPorts;
}

const QList<QSharedPointer<RemapPort> >& RemapState::getRemapPorts() {
	return remapPorts_;
}
void RemapState::setName(const QString &name) {
	name_ = name;
}

QString RemapState::getName() const {
	return name_;
}
