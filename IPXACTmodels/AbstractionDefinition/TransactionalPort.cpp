//-----------------------------------------------------------------------------
// File: TransactionalPort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Implementation for ipxact:transactionalPort.
//-----------------------------------------------------------------------------

#include "TransactionalPort.h"

#include <IPXACTmodels/common/Protocol.h>

//-----------------------------------------------------------------------------
// Function: TransactionalPort::TransactionalPort()
//-----------------------------------------------------------------------------
TransactionalPort::TransactionalPort() : 
systemGroup_(),
    presence_(PresenceTypes::UNKNOWN), 
    initiative_(), 
    kind_(), 
    busWidth_(),
    protocol_()
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::TransactionalPort()
//-----------------------------------------------------------------------------
TransactionalPort::TransactionalPort(TransactionalPort const& other) :
systemGroup_(other.systemGroup_),
    presence_(other.presence_),
    initiative_(other.initiative_),
    kind_(other.kind_),
    busWidth_(other.busWidth_),
    protocol_()
{
    if (other.protocol_)
    {
        protocol_ = QSharedPointer<Protocol>(new Protocol(*other.protocol_));
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::~TransactionalPort()
//-----------------------------------------------------------------------------
TransactionalPort::~TransactionalPort()
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::setSystemGroup()
//-----------------------------------------------------------------------------
void TransactionalPort::setSystemGroup(QString const& group)
{
    systemGroup_ = group;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::getSystemGroup()
//-----------------------------------------------------------------------------
QString TransactionalPort::getSystemGroup() const
{
    return systemGroup_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::Presence()
//-----------------------------------------------------------------------------
void TransactionalPort::setPresence(PresenceTypes::Presence presence)
{
    presence_ = presence;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::Presence()
//-----------------------------------------------------------------------------
PresenceTypes::Presence TransactionalPort::getPresence() const
{
    return presence_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::setInitiative()
//-----------------------------------------------------------------------------
void TransactionalPort::setInitiative(QString const& initiative)
{
    initiative_ = initiative;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::getInitiative()
//-----------------------------------------------------------------------------
QString TransactionalPort::getInitiative() const
{
    return initiative_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::setKind()
//-----------------------------------------------------------------------------
void TransactionalPort::setKind(QString const& kind)
{
    kind_ = kind;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::getKind()
//-----------------------------------------------------------------------------
QString TransactionalPort::getKind() const
{
    return kind_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::setBusWidth()
//-----------------------------------------------------------------------------
void TransactionalPort::setBusWidth(QString const& widthExpression)
{
    busWidth_ = widthExpression;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::getBusWidth()
//-----------------------------------------------------------------------------
QString TransactionalPort::getBusWidth() const
{
    return busWidth_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::hasProtocol()
//-----------------------------------------------------------------------------
bool TransactionalPort::hasProtocol() const
{
    return !protocol_.isNull();
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::setProtocol()
//-----------------------------------------------------------------------------
void TransactionalPort::setProtocol(QSharedPointer<Protocol> portProtocol)
{
    protocol_ = portProtocol;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPort::getProtocol()
//-----------------------------------------------------------------------------
QSharedPointer<Protocol> TransactionalPort::getProtocol() const
{
    return protocol_;
}
