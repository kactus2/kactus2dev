/* 
 *
 *  Created on: 15.10.2010
 *      Author: Antti Kamppi
 */

#include "channel.h"

#include "XmlUtils.h"

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QList>

Channel::Channel(QDomNode &channelNode): NameGroup(), busInterfaces_() {
	// go through all the childnodes
	for (int i = 0; i < channelNode.childNodes().count(); ++i) {

		QDomNode tempNode = channelNode.childNodes().at(i);

		// get the name
		if (tempNode.nodeName() == QString("spirit:name")) {

			// strip all whitespace characters
			QString name = tempNode.childNodes().at(0).nodeValue();
			setName(XmlUtils::removeWhiteSpace(name));
		}

		// get the display name
		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			setDisplayName(tempNode.childNodes().at(0).nodeValue());
		}

		// get the description
		else if (tempNode.nodeName() == QString("spirit:description")) {
			setDescription(tempNode.childNodes().at(0).nodeValue());
		}

		// get the bus interface refs
		else if (tempNode.nodeName() == QString("spirit:busInterfaceRef")) {

			// appends string stripped of white space characters
			QString temp = tempNode.childNodes().at(0).nodeValue();
			busInterfaces_.append(XmlUtils::removeWhiteSpace(temp));
		}
	}
}

Channel::Channel(): NameGroup(), busInterfaces_() {
}

Channel::Channel( const Channel& other ):
NameGroup(other),
busInterfaces_(other.busInterfaces_)
{
}

Channel& Channel::operator=( const Channel& other ) {
	if (this != &other) {
		NameGroup::operator=(other);
		busInterfaces_ = other.busInterfaces_;
	}
	return *this;
}

Channel::~Channel() {
}

void Channel::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:channel");

	writer.writeTextElement("spirit:name", name());

	// optional element
	if (!displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName());
	}

	// optional element
	if (!description().isEmpty()) {
		writer.writeTextElement("spirit:description", description());
	}

	for (int i = 0; i < busInterfaces_.size(); ++i)
    {
        writer.writeTextElement("spirit:busInterfaceRef", busInterfaces_.at(i));
	}

	writer.writeEndElement(); // spirit:channel
	return;
}

bool Channel::isValid(const QStringList& interfaceNames,
					  QStringList& errorList, 
					  const QString& parentIdentifier ) const {
	bool valid = true;

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for channel within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (busInterfaces_.size() < 2) {
		errorList.append(QObject::tr("At least two interfaces must be listed in"
			" a channel %1 within %2").arg(name()).arg(parentIdentifier));
		valid = false;
	}

	foreach (QString interfaceRef, busInterfaces_) {
		if (!interfaceNames.contains(interfaceRef)) {
			errorList.append(QObject::tr("Channel %1 contained invalid interface "
				"reference to %2 within %3").arg(name()).arg(interfaceRef).arg(
				parentIdentifier));
			valid = false;
		}
	}

	return valid;
}

bool Channel::isValid(const QStringList& interfaceNames) const {
	if (name().isEmpty()) {
		return false;
	}
	else if (busInterfaces_.size() < 2) {
		return false;
	}

	foreach (QString interfaceRef, busInterfaces_) {
		if (!interfaceNames.contains(interfaceRef)) {
			return false;
		}
	}

	return true;
}

const QStringList& Channel::getInterfaces() const {
	return busInterfaces_;
}

bool Channel::containsInterface( const QString& interfaceName ) const {
	return busInterfaces_.contains(interfaceName);
}

void Channel::setInterfaces( const QStringList& interfaceNames ) {
	busInterfaces_ = interfaceNames;
}
