//-----------------------------------------------------------------------------
// File: PortAbstraction.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Implementation for ipxact:port element within abstraction definition.
//-----------------------------------------------------------------------------

#include "PortAbstraction.h"
#include "TransactionalAbstraction.h"
#include "WireAbstraction.h"
#include "WirePort.h"
#include "TransactionalPort.h"

#include <IPXACTmodels/common/Qualifier.h>

#include <QString>
#include <QSharedPointer>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: PortAbstraction::PortAbstraction()
//-----------------------------------------------------------------------------
PortAbstraction::PortAbstraction():
NameGroup(),
    Extendable(),
    isPresent_(),
    wire_(),
    transactional_()
{

}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::PortAbstraction()
//-----------------------------------------------------------------------------
PortAbstraction::PortAbstraction(PortAbstraction const& other):
NameGroup(other),
    Extendable(other),
    isPresent_(),
    wire_(),
    transactional_(),
    match_(other.match_)
{

 	if (other.wire_)
    {
 		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction(*other.wire_.data()));
 	}

    if (other.transactional_)
    {
        transactional_ = QSharedPointer<TransactionalAbstraction>(
            new TransactionalAbstraction(*other.transactional_.data()));
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::~PortAbstraction()
//-----------------------------------------------------------------------------
PortAbstraction::~PortAbstraction() 
{

}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::operator=()
//-----------------------------------------------------------------------------
PortAbstraction& PortAbstraction::operator=(PortAbstraction const& other)
{
	if (this != &other) 
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);
        isPresent_ = other.isPresent_;
        match_ = other.match_;

		if (other.wire_)
        {
			wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction(*other.wire_.data()));
		}
		else
        {
			wire_ = QSharedPointer<WireAbstraction>();
        }

		if (other.transactional_)
        {
			transactional_ = QSharedPointer<TransactionalAbstraction>(
                new TransactionalAbstraction(*other.transactional_.data()));
		}
		else
        {
			transactional_ = QSharedPointer<TransactionalAbstraction>();
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setLogicalName()
//-----------------------------------------------------------------------------
void PortAbstraction::setLogicalName(QString const& logicalName)
{
    setName(logicalName);
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getLogicalName()
//-----------------------------------------------------------------------------
QString PortAbstraction::getLogicalName() const
{
    return name();
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setIsPresent()
//-----------------------------------------------------------------------------
void PortAbstraction::setIsPresent(QString const& presentExpression)
{
    isPresent_ = presentExpression;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::isPresent()
//-----------------------------------------------------------------------------
QString PortAbstraction::isPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setMatch()
//-----------------------------------------------------------------------------
void PortAbstraction::setMatch(bool match)
{
    match_ = match;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getMatch()
//-----------------------------------------------------------------------------
bool PortAbstraction::getMatch() const
{
    return match_;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::hasWire()
//-----------------------------------------------------------------------------
bool PortAbstraction::hasWire() const
{
    return !wire_.isNull();
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setWire()
//-----------------------------------------------------------------------------
void PortAbstraction::setWire(QSharedPointer<WireAbstraction> wire)
{
    wire_ = wire;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getWire()
//-----------------------------------------------------------------------------
QSharedPointer<WireAbstraction> PortAbstraction::getWire() const
{
    return wire_;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::hasTransactional()
//-----------------------------------------------------------------------------
bool PortAbstraction::hasTransactional() const
{
    return !transactional_.isNull();
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setTransactional()
//-----------------------------------------------------------------------------
void PortAbstraction::setTransactional(QSharedPointer<TransactionalAbstraction> transactional)
{
    transactional_ = transactional;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getTransactional()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalAbstraction> PortAbstraction::getTransactional() const
{
    return transactional_;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setPackets()
//-----------------------------------------------------------------------------
void PortAbstraction::setPackets(QSharedPointer<QList<QSharedPointer<Packet> > > packets)
{
    packets_ = packets;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getPackets()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Packet>>> PortAbstraction::getPackets() const
{
    return packets_;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getDefaultValue()
//-----------------------------------------------------------------------------
QString PortAbstraction::getDefaultValue() const
{
	if (hasWire() == false)
    {
		return QString();
    }

	return wire_->getDefaultValue();
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setDefaultValue()
//-----------------------------------------------------------------------------
void PortAbstraction::setDefaultValue(QString const& defaultValue)
{
    if (hasWire() == false)
    {
        wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
    }

    wire_->setDefaultValue(defaultValue);
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getQualifier()
//-----------------------------------------------------------------------------
QSharedPointer<Qualifier> PortAbstraction::getQualifier()
{
    if (hasWire())
    {
        return wire_->getQualifier();
    }
    else if (hasTransactional())
    {
         return transactional_->getQualifier();
    }
    else
    {
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setRequiresDriver()
//-----------------------------------------------------------------------------
void PortAbstraction::setRequiresDriver(bool requiresDriver)
{
    if (hasWire() == false)
    {
        wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
    }

    wire_->setRequiresDriver(requiresDriver);
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::requiresDriver()
//-----------------------------------------------------------------------------
bool PortAbstraction::requiresDriver() const
{
    if (hasWire() == false)
    {
        return false;
    }
    else
    {
        return wire_->requiresDriver();
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getPresence()
//-----------------------------------------------------------------------------
PresenceTypes::Presence PortAbstraction::getPresence(General::InterfaceMode mode, QString const& systemGroup) const
{
    if (hasWire())
    {
        if ((mode == General::MASTER || mode == General::MIRRORED_MASTER) && wire_->hasMasterPort())
        {
            return wire_->getMasterPort()->getPresence();
        }
        else if ((mode == General::SLAVE || mode == General::MIRRORED_SLAVE) && wire_->hasSlavePort())
        {
            return wire_->getSlavePort()->getPresence();
        }
        else if ((mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM) &&
            !wire_->getSystemPorts()->isEmpty())
        {
            foreach (auto systemPort, *wire_->getSystemPorts())
            {
                if (systemPort->getSystemGroup() == systemGroup)
                {
                    return systemPort->getPresence();
                }
            }
        }
    }
    else if (hasTransactional())
    {
        if ((mode == General::MASTER || mode == General::MIRRORED_MASTER) && transactional_->hasMasterPort())
        {
            return transactional_->getMasterPort()->getPresence();
        }
        else if ((mode == General::SLAVE || mode == General::MIRRORED_SLAVE) && transactional_->hasSlavePort())
        {
            return transactional_->getSlavePort()->getPresence();
        }
        else if ((mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM) &&
            !transactional_->getSystemPorts()->isEmpty())
        {
            foreach(auto systemPort, *transactional_->getSystemPorts())
            {
                if (systemPort->getSystemGroup() == systemGroup)
                {
                    return systemPort->getPresence();
                }
            }
        }

    }

    return PresenceTypes::UNKNOWN;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::hasMode()
//-----------------------------------------------------------------------------
bool PortAbstraction::hasMode(General::InterfaceMode mode, QString const& systemGroup) const
{
    if (hasWire())
    {
        return wire_->hasMode(mode, systemGroup);
    }    
    else if (hasTransactional())
    {
        return transactional_->hasMode(mode, systemGroup);
    }

    return false;
}
