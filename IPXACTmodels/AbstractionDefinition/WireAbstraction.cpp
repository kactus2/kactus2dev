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
    defaultValue_(QStringLiteral("")),
    requiresDriver_(false),
    driverType_(General::NO_DRIVER),
    onInitiator_(QSharedPointer<WirePort>()), 
    onTarget_(QSharedPointer<WirePort>()),
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
    onInitiator_(),
    onTarget_(), 
    onSystem_(new QList<QSharedPointer<WirePort> >())
{
    if (other.onInitiator_)
    {
        onInitiator_ = QSharedPointer<WirePort>(new WirePort(*other.onInitiator_.data()));
    }
    else
    {
        onInitiator_ = QSharedPointer<WirePort>();
    }

    if (other.onTarget_)
    {
        onTarget_ = QSharedPointer<WirePort>(new WirePort(*other.onTarget_.data()));
    }
    else
    {
        onTarget_ = QSharedPointer<WirePort>();
    }   

    Utilities::copyList(onSystem_, other.onSystem_);
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
      
		if (other.onInitiator_)
        {
			onInitiator_ = QSharedPointer<WirePort>(new WirePort(*other.onInitiator_.data()));
		}
		else
        {
			onInitiator_ = QSharedPointer<WirePort>();
        }

		if (other.onTarget_)
        {
			onTarget_ = QSharedPointer<WirePort>(new WirePort(*other.onTarget_.data()));
		}
		else
        {
			onTarget_ = QSharedPointer<WirePort>();
        }   

        Utilities::copyList(onSystem_, other.onSystem_);
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
void WireAbstraction::setQualifier(QString const& qualifierType)
{
    qualifier_->setQualifier(qualifierType);
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getQualifier()
//-----------------------------------------------------------------------------
QSharedPointer<Qualifier> WireAbstraction::getQualifier()
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
// Function: WireAbstraction::findSystemPort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstraction::findSystemPort(QString const& groupName) const
{
    foreach(QSharedPointer<WirePort> systemPort, *onSystem_)
    {
        if (systemPort->getSystemGroup() == groupName)
        {
            return systemPort;
        }
    }

    return QSharedPointer<WirePort>();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasInitiatorPort()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasInitiatorPort() const
{
    return !onInitiator_.isNull();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setInitiatorPort()
//-----------------------------------------------------------------------------
void WireAbstraction::setInitiatorPort(QSharedPointer<WirePort> initiatorPort)
{
    onInitiator_ = initiatorPort;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getInitiatorPort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstraction::getInitiatorPort() const
{
    return onInitiator_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasTargetPort()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasTargetPort() const
{
    return !onTarget_.isNull();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setTargetPort()
//-----------------------------------------------------------------------------
void WireAbstraction::setTargetPort(QSharedPointer<WirePort> targetPort)
{
    onTarget_ = targetPort;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getTargetPort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstraction::getTargetPort() const
{
    return onTarget_;
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasMasterPort()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasMasterPort() const
{
    return hasInitiatorPort();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setMasterPort()
//-----------------------------------------------------------------------------
void WireAbstraction::setMasterPort(QSharedPointer<WirePort> masterPort)
{
    setInitiatorPort(masterPort);
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getMasterPort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstraction::getMasterPort() const
{
    return getInitiatorPort();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasSlavePort()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasSlavePort() const
{
    return hasTargetPort();
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::setSlavePort()
//-----------------------------------------------------------------------------
void WireAbstraction::setSlavePort(QSharedPointer<WirePort> slavePort)
{
    setTargetPort(slavePort);
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::getSlavePort()
//-----------------------------------------------------------------------------
QSharedPointer<WirePort> WireAbstraction::getSlavePort() const
{
    return getTargetPort();
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
    setRequiresDriver(driverType != General::NO_DRIVER);

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
DirectionTypes::Direction WireAbstraction::getDirection(General::InterfaceMode mode,
    QString const& systemGroup) const
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
    else if (mode == General::SYSTEM)
    {
        QSharedPointer<WirePort> systemPort = findSystemPort(systemGroup);

        if (systemPort)
        {
            return systemPort->getDirection();
        }

    } 
    else if (mode == General::MIRROREDSYSTEM)
    {
        QSharedPointer<WirePort> systemPort = findSystemPort(systemGroup);

        if (systemPort)
        {
            return DirectionTypes::convert2Mirrored(systemPort->getDirection());
        }
    }

    return DirectionTypes::DIRECTION_INVALID;
}


//-----------------------------------------------------------------------------
// Function: WireAbstraction::getWidth()
//-----------------------------------------------------------------------------
QString WireAbstraction::getWidth(General::InterfaceMode mode, QString const& systemGroup) const
{
    if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && hasMasterPort())
    {
        return getMasterPort()->getWidth();
    } 
    else if ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && hasSlavePort())
    {
        return getSlavePort()->getWidth();
    }                        
    else if (mode == General::SYSTEM || mode == General::MIRROREDSYSTEM)
    {
        QSharedPointer<WirePort> systemPort = findSystemPort(systemGroup);

        if (systemPort)
        {
            return systemPort->getWidth();
        }
    } 

    return QStringLiteral("");
}

//-----------------------------------------------------------------------------
// Function: WireAbstraction::hasMode()
//-----------------------------------------------------------------------------
bool WireAbstraction::hasMode(General::InterfaceMode mode, QString const& systemGroup) const
{
    if (mode == General::SYSTEM || mode == General::MIRROREDSYSTEM)
    {
        QSharedPointer<WirePort> systemPort = findSystemPort(systemGroup);

        return !systemPort.isNull();
    }

    return (mode == General::MASTER && hasMasterPort()) ||
        (mode == General::MIRROREDMASTER && hasMasterPort()) ||
        (mode == General::SLAVE && hasSlavePort()) ||
        (mode == General::MIRROREDSLAVE && hasSlavePort());
}
