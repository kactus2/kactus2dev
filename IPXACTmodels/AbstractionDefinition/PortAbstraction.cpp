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
    transactional_()
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
// Function: PortAbstraction::getDefaultValue()
//-----------------------------------------------------------------------------
QString PortAbstraction::getDefaultValue() const
{
	if (wire_.isNull())
    {
		return "";
    }

	return wire_->getDefaultValue();
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setDefaultValue()
//-----------------------------------------------------------------------------
void PortAbstraction::setDefaultValue(QString const& defaultValue)
{
    if (wire_.isNull())
    {
        wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
    }

    wire_->setDefaultValue(defaultValue);
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::getQualifier()
//-----------------------------------------------------------------------------
Qualifier PortAbstraction::getQualifier() const
{
    if (!wire_.isNull())
    {
        return wire_->getQualifier();
    }
    else if (!transactional_.isNull())
    {
         return transactional_->getQualifier();
    }
    else
    {
        return Qualifier();
    }
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::setRequiresDriver()
//-----------------------------------------------------------------------------
void PortAbstraction::setRequiresDriver(bool requiresDriver)
{
    if (wire_.isNull())
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
    if (wire_.isNull())
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
PresenceTypes::Presence PortAbstraction::getPresence(General::InterfaceMode mode) const
{
    if (hasWire())
    {
        if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && wire_->hasMasterPort())
        {
            return wire_->getMasterPort()->getPresence();
        }
        else if ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && wire_->hasSlavePort())
        {
            return wire_->getSlavePort()->getPresence();
        }
        else if ((mode == General::SYSTEM || mode == General::MIRROREDSYSTEM) &&
            !wire_->getSystemPorts()->isEmpty())
        {
            PresenceTypes::Presence systemPresence = wire_->getSystemPorts()->first()->getPresence();
            foreach (QSharedPointer<WirePort> systemPort, *wire_->getSystemPorts())
            {
                if (systemPort->getPresence() != systemPresence)
                {
                    return PresenceTypes::UNKNOWN;
                }
            }

            return systemPresence;
        }
    }

    return PresenceTypes::UNKNOWN;
}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::hasMode()
//-----------------------------------------------------------------------------
bool PortAbstraction::hasMode(General::InterfaceMode mode) const
{
    if (hasWire())
    {
        return wire_->hasMode(mode);
    }
    else
    {
        return false;
    }
}
