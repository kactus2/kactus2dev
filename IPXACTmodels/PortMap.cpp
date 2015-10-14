//-----------------------------------------------------------------------------
// File: PortMap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.08.2014
//
// Description:
// Describes the mapping between the logical and physical ports.
//-----------------------------------------------------------------------------

#include "PortMap.h"

#include <IPXACTmodels/common/Vector.h>
#include "port.h"

//-----------------------------------------------------------------------------
// Function: PortMap::PortMap()
//-----------------------------------------------------------------------------
PortMap::PortMap(): 
logicalPort_(),
	physicalPort_(),
	logicalTieOff_()
{

}

//-----------------------------------------------------------------------------
// Function: PortMap::PortMap()
//-----------------------------------------------------------------------------
PortMap::PortMap( const PortMap& other ): 
logicalPort_(other.logicalPort_),
physicalPort_(other.physicalPort_),
logicalTieOff_(other.logicalTieOff_)
{
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
PortMap& PortMap::operator=( const PortMap& other )
{
    if (this != &other)
    {
        logicalPort_.clear();
        logicalPort_ = QSharedPointer<PortMap::LogicalPort> (new PortMap::LogicalPort(*other.logicalPort_.data()));
        physicalPort_.clear();
        physicalPort_ =
            QSharedPointer<PortMap::PhysicalPort> (new PortMap::PhysicalPort(*other.physicalPort_.data()));
        logicalTieOff_ = other.logicalTieOff_;
    }

	return *this;
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
void PortMap::setLogicalTieOff(QString logicalTieOff)
{
	logicalTieOff_ = logicalTieOff;
}