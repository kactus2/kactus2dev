//-----------------------------------------------------------------------------
// File: PortMap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.08.2014
//
// Description:
// Implementation for ipxact:portMap.
//-----------------------------------------------------------------------------

#include "PortMap.h"

#include <IPXACTmodels/common/Vector.h>

//-----------------------------------------------------------------------------
// Function: PortMap::PortMap()
//-----------------------------------------------------------------------------
PortMap::PortMap(): 
invert_(),
isPresent_(),
logicalPort_(),
physicalPort_(),
logicalTieOff_(),
isInformative_()
{

}

//-----------------------------------------------------------------------------
// Function: PortMap::PortMap()
//-----------------------------------------------------------------------------
PortMap::PortMap(PortMap const& other): 
invert_(other.invert_),
isPresent_(other.isPresent_),
logicalPort_(),
physicalPort_(),
logicalTieOff_(other.logicalTieOff_),
isInformative_(other.isInformative_)
{
    if (other.logicalPort_)
    {
        logicalPort_ = QSharedPointer<PortMap::LogicalPort>(new PortMap::LogicalPort());
        logicalPort_->name_ = other.logicalPort_->name_;

        if (other.logicalPort_->range_)
        {
            logicalPort_->range_ = QSharedPointer<Range>(new Range(other.logicalPort_->range_->getLeft(), 
                other.logicalPort_->range_->getRight()));
        }
    }

    if (other.physicalPort_)
    {
        physicalPort_ = QSharedPointer<PortMap::PhysicalPort>(new PortMap::PhysicalPort());
        physicalPort_->name_ = other.physicalPort_->name_;

        if (other.physicalPort_->partSelect_)
        {
            physicalPort_->partSelect_ =
                QSharedPointer<PartSelect>(new PartSelect(*other.physicalPort_->partSelect_));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMap::~PortMap()
//-----------------------------------------------------------------------------
PortMap::~PortMap()
{
}

//-----------------------------------------------------------------------------
// Function: PortMap::operator=()
//-----------------------------------------------------------------------------
PortMap& PortMap::operator=(PortMap const& other)
{
    if (this != &other)
    {
        invert_ = other.invert_;
        isPresent_ = other.isPresent_;

        logicalPort_.clear();
        logicalPort_ = QSharedPointer<PortMap::LogicalPort>(new PortMap::LogicalPort(*other.logicalPort_.data()));
        
        physicalPort_.clear();
        if (!other.getPhysicalPort().isNull())
        {  
            physicalPort_ = QSharedPointer<PortMap::PhysicalPort>(new PortMap::PhysicalPort(
                *other.physicalPort_.data()));
        }
      
        logicalTieOff_ = other.logicalTieOff_;

        isInformative_ = other.isInformative_;
    }

	return *this;
}

//-----------------------------------------------------------------------------
// Function: PortMap::getInvert()
//-----------------------------------------------------------------------------
BooleanValue PortMap::getInvert() const
{
    return invert_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setInvert()
//-----------------------------------------------------------------------------
void PortMap::setInvert(bool newInvert)
{
    invert_.setValue(newInvert);
}

//-----------------------------------------------------------------------------
// Function: PortMap::clearInvert()
//-----------------------------------------------------------------------------
void PortMap::clearInvert()
{
    invert_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: PortMap::getIsPresent()
//-----------------------------------------------------------------------------
QString PortMap::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setIsPresent()
//-----------------------------------------------------------------------------
void PortMap::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: PortMap::getIsInformative()
//-----------------------------------------------------------------------------
BooleanValue PortMap::getIsInformative() const
{
    return isInformative_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setIsInformative()
//-----------------------------------------------------------------------------
void PortMap::setIsInformative(bool newInformative)
{
    isInformative_.setValue(newInformative);
}

//-----------------------------------------------------------------------------
// Function: PortMap::clearIsInformative()
//-----------------------------------------------------------------------------
void PortMap::clearIsInformative()
{
    isInformative_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: PortMap::getLogicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap::LogicalPort> PortMap::getLogicalPort() const
{
	return logicalPort_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setLogicalPort()
//-----------------------------------------------------------------------------
void PortMap::setLogicalPort(QSharedPointer<LogicalPort> logicalPort)
{
    logicalPort_ = logicalPort;
}

//-----------------------------------------------------------------------------
// Function: PortMap::getPhysicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap::PhysicalPort> PortMap::getPhysicalPort() const
{
    return physicalPort_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setPhysicalPort()
//-----------------------------------------------------------------------------
void PortMap::setPhysicalPort(QSharedPointer<PhysicalPort> physicalPort)
{
    physicalPort_ = physicalPort;
}

//-----------------------------------------------------------------------------
// Function: PortMap::getLogicalTiedOff()
//-----------------------------------------------------------------------------
QString PortMap::getLogicalTieOff() const
{
	return logicalTieOff_;
}

//-----------------------------------------------------------------------------
// Function: PortMap::setLogicalTiedOff()
//-----------------------------------------------------------------------------
void PortMap::setLogicalTieOff(QString const& logicalTieOff)
{
	logicalTieOff_ = logicalTieOff;
}