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
Channel::Channel(): NameGroup(), busInterfaces_()
{
}

//-----------------------------------------------------------------------------
// Function: Channel::Channel()
//-----------------------------------------------------------------------------
Channel::Channel(Channel const& other):
NameGroup(other),
busInterfaces_(other.busInterfaces_)
{
}

//-----------------------------------------------------------------------------
// Function: Channel::operator=()
//-----------------------------------------------------------------------------
Channel& Channel::operator=(Channel const& other)
{
	if (this != &other)
    {
		NameGroup::operator=(other);
		busInterfaces_ = other.busInterfaces_;
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
// Function: Channel::getInterfaces()
//-----------------------------------------------------------------------------
QStringList Channel::getInterfaces() const
{
	return busInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Channel::setInterfaces()
//-----------------------------------------------------------------------------
void Channel::setInterfaces(QStringList const& interfaceNames)
{
    busInterfaces_ = interfaceNames;
}

