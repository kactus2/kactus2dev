/* 
 *
 *  Created on: 15.10.2010
 *      Author: Antti Kamppi
 */

#include "channel.h"
#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"
#include "generaldeclarations.h"

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QList>

Channel::Channel(QDomNode &channelNode): name_(),
displayName_(), description_(), busInterfaces_() {
	// go through all the childnodes
	for (int i = 0; i < channelNode.childNodes().count(); ++i) {

		QDomNode tempNode = channelNode.childNodes().at(i);

		// get the name
		if (tempNode.nodeName() == QString("spirit:name")) {

			// strip all whitespace characters
			name_ = tempNode.childNodes().at(0).nodeValue();
			name_ = General::removeWhiteSpace(name_);
		}

		// get the display name
		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ =
					tempNode.childNodes().at(0).nodeValue();
		}

		// get the description
		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ =
					tempNode.childNodes().at(0).nodeValue();
		}

		// get the bus interface refs
		else if (tempNode.nodeName() == QString("spirit:busInterfaceRef")) {

			// appends string stripped of white space characters
			QString temp = tempNode.childNodes().at(0).nodeValue();
			busInterfaces_.append(General::removeWhiteSpace(temp));
		}
	}

	// if mandatory elements are missing
// 	if (name_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element spirit:name was not"
// 				" found"));
// 	}
// 	if (busInterfaces_.size() < 2) {
// 		throw Parse_error(QObject::tr("Mandatory element spirit:busInterfaceRef"
// 				" was not found"));
// 	}
}

Channel::Channel(): name_(),
displayName_(), description_(), busInterfaces_() {
}

Channel::Channel( const Channel& other ):
name_(other.name_),
displayName_(other.displayName_),
description_(other.description_),
busInterfaces_(other.busInterfaces_) {
}

Channel& Channel::operator=( const Channel& other ) {
	if (this != &other) {
		name_ = other.name_;
		displayName_ = other.displayName_;
		description_ = other.description_;
		busInterfaces_ = other.busInterfaces_;
	}
	return *this;
}

Channel::~Channel() {
}

void Channel::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:channel");

	// make sure the mandatory elements are defined
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element spirit:name is empty"));
	}
	// if name was found its written
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	// optional element
	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	// optional element
	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// atleast 2 businterface refs must be found
	if (busInterfaces_.size() < 2) {
		throw Write_error(QObject::tr("Mandatory element spirit:busInterfaceRef is"
				" missing"));
	}
	// if they were found then write them to document
	else {
		for (int i = 0; i < busInterfaces_.size(); ++i) {
			writer.writeTextElement("spirit:busInterfaceRef",
					busInterfaces_.at(i));
		}
	}
	writer.writeEndElement(); // spirit:channel
	return;
}

bool Channel::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for channel within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (busInterfaces_.size() < 2) {
		errorList.append(QObject::tr("At least two interfaces must be listed in"
			" a channel %1 within %2").arg(name_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool Channel::isValid() const {
	if (name_.isEmpty()) {
		return false;
	}
	else if (busInterfaces_.size() < 2) {
		return false;
	}
	return true;
}

QString Channel::getName() const {
	return name_;
}

QString Channel::getDisplayName() const {
	return displayName_;
}

QString Channel::getDescription() const {
	return description_;
}

const QStringList& Channel::getInterfaces() const {
	return busInterfaces_;
}

void Channel::setInterfaces( const QStringList& interfaceNames ) {
	busInterfaces_ = interfaceNames;
}

void Channel::setName( const QString& name ) {
	name_ = name;
}

void Channel::setDisplayName( const QString& name ) {
	displayName_ = name;
}

void Channel::setDescription( const QString& description ) {
	description_ = description;
}
