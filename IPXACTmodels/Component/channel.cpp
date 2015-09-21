/* 
 *
 *  Created on: 15.10.2010
 *      Author: Antti Kamppi
 */

#include "channel.h"

#include <QString>
#include <QList>

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

//-----------------------------------------------------------------------------
// Function: Channel::getIsPresent()
//-----------------------------------------------------------------------------
QString Channel::getIsPresent() const
{
	return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Channel::setIsPresent()
//-----------------------------------------------------------------------------
void Channel::setIsPresent(QString const& newIsPresent)
{
	isPresent_ = newIsPresent;
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
