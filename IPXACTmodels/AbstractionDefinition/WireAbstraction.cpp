//-----------------------------------------------------------------------------
// File: WireAbstraction.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Implementation for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------

#include "WireAbstraction.h"

#include "WirePort.h"

#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: WireAbstraction::WireAbstraction()
//-----------------------------------------------------------------------------
WireAbstraction::WireAbstraction():
qualifier_(),
    defaultValue_(""),
    requiresDriver_(false),
    driverType_(General::NO_DRIVER),
    onMaster_(QSharedPointer<WirePort>()), 
    onSlave_(QSharedPointer<WirePort>()),
    onSystem_(new QList<QSharedPointer<WirePort> >())
{

}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::WireAbstraction()
//-----------------------------------------------------------------------------
WireAbstraction::WireAbstraction(WireAbstraction const& other):
qualifier_(other.qualifier_),
    defaultValue_(other.defaultValue_),
    requiresDriver_(other.requiresDriver_),
    driverType_(other.driverType_),
    onMaster_(),
    onSlave_(), 
    onSystem_()
{
    if (other.onMaster_)
    {
        onMaster_ = QSharedPointer<WirePort>(new WirePort(*other.onMaster_.data()));
    }
    else
    {
        onMaster_ = QSharedPointer<WirePort>();
    }

    if (other.onSlave_)
    {
        onSlave_ = QSharedPointer<WirePort>(new WirePort(*other.onSlave_.data()));
    }
    else
    {
        onSlave_ = QSharedPointer<WirePort>();
    }   

    onSystem_ = other.onSystem_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::operator=()
//-----------------------------------------------------------------------------
WireAbstraction& WireAbstraction::operator=(WireAbstraction const& other)
{
	if (this != &other)
    {
        qualifier_ = other.qualifier_;
		defaultValue_ = other.defaultValue_;
		requiresDriver_ = other.requiresDriver_;
        driverType_ = other.driverType_;
      
		if (other.onMaster_)
        {
			onMaster_ = QSharedPointer<WirePort>(new WirePort(*other.onMaster_.data()));
		}
		else
        {
			onMaster_ = QSharedPointer<WirePort>();
        }

		if (other.onSlave_)
        {
			onSlave_ = QSharedPointer<WirePort>(new WirePort(*other.onSlave_.data()));
		}
		else
        {
			onSlave_ = QSharedPointer<WirePort>();
        }   

	    onSystem_ = other.onSystem_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::~WireAbstraction()
//-----------------------------------------------------------------------------
WireAbstraction::~WireAbstraction()
{

}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setDefaultValue()
//-----------------------------------------------------------------------------
void WireAbstraction::setDefaultValue(QString const& defaultValue)
{
    requiresDriver_ = false;
    driverType_ = General::NO_DRIVER;

    defaultValue_ = defaultValue;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getDefaultValue()
//-----------------------------------------------------------------------------
QString WireAbstraction::getDefaultValue() const
{
    return defaultValue_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setQualifier()
//-----------------------------------------------------------------------------
void WireAbstraction::setQualifier(Qualifier::Type qualifierType)
{
    qualifier_.setType(qualifierType);
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getQualifier()
//-----------------------------------------------------------------------------
Qualifier WireAbstraction::getQualifier() const
{
    return qualifier_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::addSystemPort()
//-----------------------------------------------------------------------------
void WireAbstraction::addSystemPort(QSharedPointer<WirePort> systemPort)
{
    if (!onSystem_->contains(systemPort))
    {
        onSystem_->append(systemPort);
    }    
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getSystemPort()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<WirePort> > > WireAbstraction::getSystemPorts() const
{
    return onSystem_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasMasterPort()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasMasterPort() const
{
    return !onMaster_.isNull();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setMasterPort()
//-----------------------------------------------------------------------------
void WireAbstraction::setMasterPort(QSharedPointer<WirePort> masterPort)
{
    onMaster_ = masterPort;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getMasterPort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstraction::getMasterPort() const
{
    return onMaster_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasSlavePort()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasSlavePort() const
{
    return !onSlave_.isNull();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setSlavePort()
//-----------------------------------------------------------------------------
void WireAbstraction::setSlavePort(QSharedPointer<WirePort> slavePort)
{
    onSlave_ = slavePort;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getSlavePort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstraction::getSlavePort() const
{
    return onSlave_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setRequiresDriver()
//-----------------------------------------------------------------------------
void WireAbstraction::setRequiresDriver(bool requiresDriver)
{
    if (requiresDriver) 
    {
        defaultValue_.clear();
    }

    requiresDriver_ = requiresDriver;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::requiresDriver()
//-----------------------------------------------------------------------------
bool WireAbstraction::requiresDriver() const
{
    return requiresDriver_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setDriverType()
//-----------------------------------------------------------------------------
void WireAbstraction::setDriverType(General::DriverType driverType)
{
    setRequiresDriver(true);

    driverType_ = driverType;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getDriverType()
//-----------------------------------------------------------------------------
General::DriverType WireAbstraction::getDriverType() const
{
    return driverType_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction WireAbstraction::getDirection(General::InterfaceMode mode) const
{
    if (mode == General::MASTER && hasMasterPort())
    {
        return getMasterPort()->getDirection();
    } 
    else if (mode == General::MIRROREDMASTER && hasMasterPort())
    {
        return DirectionTypes::convert2Mirrored(getMasterPort()->getDirection());
    }
    else if (mode == General::SLAVE && hasSlavePort())
    {
        return getSlavePort()->getDirection();
    }                       
    else if (mode == General::MIRROREDSLAVE && hasSlavePort())
    {
        return DirectionTypes::convert2Mirrored(getSlavePort()->getDirection());
    }   
    else if (mode == General::SYSTEM && !getSystemPorts()->isEmpty())
    {
        return getSystemPorts()->first()->getDirection();
    } 
    else if (mode == General::MIRROREDSYSTEM && !getSystemPorts()->isEmpty())
    {
        return DirectionTypes::convert2Mirrored(getSystemPorts()->first()->getDirection());
    }
    else
    {
        return DirectionTypes::DIRECTION_INVALID;
    }
}


//-----------------------------------------------------------------------------
// Function: WireAbstraction::getDirection()
//-----------------------------------------------------------------------------
QString WireAbstraction::getWidth(General::InterfaceMode mode) const
{
    if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && hasMasterPort())
    {
        return getMasterPort()->getWidth();
    } 
    else if ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && hasSlavePort())
    {
        return getSlavePort()->getWidth();
    }                        
    else if ((mode == General::SYSTEM || mode == General::MIRROREDSYSTEM) && !getSystemPorts()->isEmpty())
    {
        return getSystemPorts()->first()->getWidth();
    } 

    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasMode()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasMode(General::InterfaceMode mode) const
{
    if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && hasMasterPort() ||
        ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && hasSlavePort()) ||
        ((mode == General::SYSTEM || mode == General::MIRROREDSYSTEM) && !getSystemPorts()->isEmpty()))
    {
        return true;
    }
    else
    {
        return false;
    }
}
