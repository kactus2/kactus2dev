//-----------------------------------------------------------------------------
// File: Channel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.09.2015
//
// Description:
// Implementation for ipxact:channel element.
//-----------------------------------------------------------------------------

#include "Channel.h"

#include <QString>

//-----------------------------------------------------------------------------
// Function: Channel::Channel()
//-----------------------------------------------------------------------------
Channel::Channel(): NameGroup(), Extendable()
{
}

//-----------------------------------------------------------------------------
// Function: Channel::Channel()
//-----------------------------------------------------------------------------
Channel::Channel(Channel const& other):
NameGroup(other),
Extendable(other)
{
	copyBusInterfaceRefs(other);
}

//-----------------------------------------------------------------------------
// Function: Channel::operator=()
//-----------------------------------------------------------------------------
Channel& Channel::operator=(Channel const& other)
{
	if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        copyBusInterfaceRefs(other);
	}

	return *this;
}

//-----------------------------------------------------------------------------
// Function: Channel::~Channel()
//-----------------------------------------------------------------------------
Channel::~Channel()
{
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

//-----------------------------------------------------------------------------
// Function: Channel::getInterfaceNames()
//-----------------------------------------------------------------------------
QStringList Channel::getInterfaceNames() const
{
	QStringList names;

	for (auto const& interfaceRef : *busInterfaces_)
	{
		names.append(interfaceRef->localName_);
	}

	return names;
}

//-----------------------------------------------------------------------------
// Function: Channel::getInterfaces()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Channel::BusInterfaceRef> > > Channel::getInterfaces() const
{
	return busInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Channel::setInterfaces()
//-----------------------------------------------------------------------------
void Channel::setInterfaces(QStringList const& interfaceNames)
{
    foreach (auto const& ref, *busInterfaces_)
    {
		if (interfaceNames.contains(ref->localName_) == false)
		{
			busInterfaces_->removeOne(ref);
		}
    }

	for (auto const& name : interfaceNames)
	{
		if (std::none_of(busInterfaces_->cbegin(), busInterfaces_->cend(),
			[&name](auto const& interfaceRef) { return interfaceRef->localName_ == name; }))
		{
			busInterfaces_->append(QSharedPointer<BusInterfaceRef>(new BusInterfaceRef(name)));
		}
	}
}

//-----------------------------------------------------------------------------
// Function: Channel::copyBusInterfaceRefs()
//-----------------------------------------------------------------------------
void Channel::copyBusInterfaceRefs(Channel const& other)
{
	busInterfaces_->clear();
	for (auto const& ref : *other.busInterfaces_)
	{
		busInterfaces_->append(QSharedPointer<BusInterfaceRef>(new BusInterfaceRef(*ref)));
	}
}
