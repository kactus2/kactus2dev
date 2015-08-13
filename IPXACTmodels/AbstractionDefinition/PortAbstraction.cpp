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
#include "WireAbstraction.h"

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
    //transactional_()//, 
    wire_()
    //portType_(General::WIRE)
{

}

//-----------------------------------------------------------------------------
// Function: PortAbstraction::PortAbstraction()
//-----------------------------------------------------------------------------
PortAbstraction::PortAbstraction(PortAbstraction const& other):
NameGroup(other),
    Extendable(other),
    isPresent_(),
    //transactional_(),
    wire_()
    //portType_(other.portType_),
{

 	if (other.wire_)
    {
 		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction(*other.wire_.data()));
 	}
	
	//if (other.transactional_) {
		//transactional_ = QSharedPointer<TransactionalAbstraction>(
		//	new TransactionalAbstraction(*other.transactional_.data()));
	//}
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
		//portType_ = other.portType_;

		if (other.wire_)
        {
			wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction(*other.wire_.data()));
		}
		else
        {
			wire_ = QSharedPointer<WireAbstraction>();
        }

		//if (other.transactional_) {
			//transactional_ = QSharedPointer<TransactionalAbstraction>(
             //   new TransactionalAbstraction(*other.transactional_.data()));
		//}
		//else
			//transactional_ = QSharedPointer<TransactionalAbstraction>();
	}
	return *this;
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
// Function: PortAbstraction::getWire()
//-----------------------------------------------------------------------------
QSharedPointer<WireAbstraction> PortAbstraction::getWire()
{
    if (wire_.isNull())
    {
        wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
    }

    return wire_;
}

/*
TransactionalAbstraction *PortAbstraction::getTransactional() const
{
	return transactional_.data();
}
*/

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
// Function: PortAbstraction::setQualifier()
//-----------------------------------------------------------------------------
void PortAbstraction::setQualifier(Qualifier::Type qualifierType)
{
    if (wire_.isNull())
    {
        wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
    }

    wire_->setQualifier(qualifierType);
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
