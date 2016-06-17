//-----------------------------------------------------------------------------
// File: BusInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date:
//
// Description:
// Describes the ipxact:busInterface element.
//-----------------------------------------------------------------------------

#include "BusInterface.h"

#include "MasterInterface.h"
#include "MirroredSlaveInterface.h"
#include "SlaveInterface.h"
#include "PortMap.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: BusInterface::MonitorInterface::MonitorInterface()
//-----------------------------------------------------------------------------
BusInterface::MonitorInterface::MonitorInterface():
interfaceMode_(General::MONITOR),
group_()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterface::BusInterface()
//-----------------------------------------------------------------------------
BusInterface::BusInterface():
NameGroup(),
Extendable(),
isPresent_(),
attributes_(),
busType_(),
abstractionTypes_(new QList<QSharedPointer<AbstractionType> > ()),
interfaceMode_(General::INTERFACE_MODE_COUNT),
connectionRequired_(),
bitsInLau_(),
bitSteering_(BusInterface::BITSTEERING_UNSPECIFIED),
bitSteeringAttributes_(),
endianness_(BusInterface::ENDIANNESS_UNSPECIFIED),
parameters_(new QList<QSharedPointer<Parameter> >()), 
master_(QSharedPointer<MasterInterface>(new MasterInterface)),
slave_(),
systemGroup_(),
monitor_(),
mirroredSlave_()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterface::BusInterface()
//-----------------------------------------------------------------------------
BusInterface::BusInterface( const BusInterface &other ):
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_),
attributes_(other.attributes_),
busType_(other.busType_),
abstractionTypes_(new QList<QSharedPointer<AbstractionType> > ()),
interfaceMode_(other.interfaceMode_),
connectionRequired_(other.connectionRequired_),
bitsInLau_(other.bitsInLau_),
bitSteering_(other.bitSteering_),
bitSteeringAttributes_(other.bitSteeringAttributes_),
endianness_(other.endianness_),
parameters_(new QList<QSharedPointer<Parameter> >()),
master_(),
slave_(),
systemGroup_(other.systemGroup_),
monitor_(),
mirroredSlave_()
{
    copyAbstractionTypes(other);
    copyParameters(other);
    copyInterfaceModes(other);
}

//-----------------------------------------------------------------------------
// Function: BusInterface::operator=()
//-----------------------------------------------------------------------------
BusInterface & BusInterface::operator=( const BusInterface &other )
{
	if (this != &other)
    {	
		NameGroup::operator=(other);
        Extendable::operator=(other);
		attributes_ = other.attributes_;
		busType_ = other.busType_;
		interfaceMode_ = other.interfaceMode_;
		connectionRequired_ = other.connectionRequired_;
		bitsInLau_ = other.bitsInLau_;
		bitSteering_ = other.bitSteering_;
		bitSteeringAttributes_ = other.bitSteeringAttributes_;
		endianness_ = other.endianness_;
		systemGroup_ = other.systemGroup_;

        abstractionTypes_->clear();
        copyAbstractionTypes(other);

        parameters_->clear();
        copyParameters(other);

        copyInterfaceModes(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::~BusInterface()
//-----------------------------------------------------------------------------
BusInterface::~BusInterface()
{
	abstractionTypes_.clear();
	bitSteeringAttributes_.clear();
	parameters_.clear();
	master_.clear();
	slave_.clear();
	monitor_.clear();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getIsPresent()
//-----------------------------------------------------------------------------
QString BusInterface::getIsPresent() const
{
	return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setIsPresent()
//-----------------------------------------------------------------------------
void BusInterface::setIsPresent(QString const& newIsPresent)
{
	isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getBitSteering()
//-----------------------------------------------------------------------------
BusInterface::BitSteering BusInterface::getBitSteering() const
{
	return bitSteering_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterface::getInterfaceMode() const
{
	return interfaceMode_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setPortMaps()
//-----------------------------------------------------------------------------
void BusInterface::setPortMaps(QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps)
{
    abstractionTypes_->first()->getPortMaps()->clear();
    abstractionTypes_->first()->getPortMaps() = portMaps;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setAbstractionTypes()
//-----------------------------------------------------------------------------
void BusInterface::setAbstractionTypes(QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionTypes)
{
    abstractionTypes_->clear();
    abstractionTypes_ = abstractionTypes;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setConnectionRequired()
//-----------------------------------------------------------------------------
void BusInterface::setConnectionRequired(bool connectionRequired)
{
    connectionRequired_.setValue(connectionRequired);
}

//-----------------------------------------------------------------------------
// Function: BusInterface::clearConnectionRequired()
//-----------------------------------------------------------------------------
void BusInterface::clearConnectionRequired()
{
    connectionRequired_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setEndianness()
//-----------------------------------------------------------------------------
void BusInterface::setEndianness(BusInterface::Endianness endianness)
{
	endianness_ = endianness;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getBitsInLau()
//-----------------------------------------------------------------------------
QString BusInterface::getBitsInLau() const
{
	return bitsInLau_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getConnectionRequired()
//-----------------------------------------------------------------------------
QString BusInterface::getConnectionRequired() const
{
	return connectionRequired_.toString();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setBitsInLau()
//-----------------------------------------------------------------------------
void BusInterface::setBitsInLau(QString const& newBitsInLau)
{
	bitsInLau_ = newBitsInLau;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > BusInterface::getParameters() const
{
	return parameters_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getPortMaps()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortMap> > > BusInterface::getPortMaps() const
{
    if (abstractionTypes_->isEmpty())
    {
        return QSharedPointer<QList<QSharedPointer<PortMap> > > ();
    }

    return abstractionTypes_->first()->getPortMaps();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getAbstractionTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AbstractionType> > > BusInterface::getAbstractionTypes() const
{
	return abstractionTypes_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setBitSteeringAttributes()
//-----------------------------------------------------------------------------
void BusInterface::setBitSteeringAttributes(QMap<QString, QString> const& bitSteeringAttributes)
{
	bitSteeringAttributes_.clear();
	bitSteeringAttributes_ = bitSteeringAttributes;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getBusType()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference BusInterface::getBusType() const
{
	return busType_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getBitSteeringAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> BusInterface::getBitSteeringAttributes()
{
	return bitSteeringAttributes_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getEndianness()
//-----------------------------------------------------------------------------
BusInterface::Endianness BusInterface::getEndianness() const
{
	return endianness_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setInterfaceMode()
//-----------------------------------------------------------------------------
void BusInterface::setInterfaceMode(General::InterfaceMode interfaceMode)
{
	interfaceMode_ = interfaceMode;

    if (interfaceMode == General::MASTER)
    {
        monitor_.clear();
        slave_.clear();
        systemGroup_.clear();
        mirroredSlave_.clear();
        if (!master_)
        {
            master_ = QSharedPointer<MasterInterface>(new MasterInterface());
        }
    }
    else if (interfaceMode == General::SLAVE)
    {
        monitor_.clear();
        master_.clear();
        systemGroup_.clear();
        mirroredSlave_.clear();
        if (!slave_)
        {
            slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface());
        }
    }
    else if (interfaceMode == General::SYSTEM)
    {
        monitor_.clear();
        slave_.clear();
        master_.clear();
        mirroredSlave_.clear();
        systemGroup_ = "default";
    }
    else if (interfaceMode == General::MIRROREDSLAVE)
    {
        monitor_.clear();
        slave_.clear();
        master_.clear();
        systemGroup_.clear();
        if (!mirroredSlave_)
        {
            mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface());
        }
    }
    else if (interfaceMode == General::MIRROREDMASTER)
    {
        monitor_.clear();
        slave_.clear();
        systemGroup_.clear();
        mirroredSlave_.clear();
        if (!master_)
        {
            master_ = QSharedPointer<MasterInterface>(new MasterInterface()); 
        }
    }
    else if (interfaceMode == General::MIRROREDSYSTEM)
    {
        monitor_.clear();
        slave_.clear();
        master_.clear();
        mirroredSlave_.clear();
        systemGroup_ = "default";
    }
    else if (interfaceMode == General::MONITOR)
    {
        slave_.clear();
        master_.clear();
        systemGroup_.clear();
        mirroredSlave_.clear();
        if (!monitor_)
        {
            monitor_ = QSharedPointer<MonitorInterface>(new MonitorInterface());
        }
    }
    else
    {
        monitor_.clear();
        slave_.clear();
        master_.clear();
        systemGroup_.clear();
        mirroredSlave_.clear();
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setBusType()
//-----------------------------------------------------------------------------
void BusInterface::setBusType(ConfigurableVLNVReference const& newBusType)
{
    busType_ = newBusType;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setBitSteering()
//-----------------------------------------------------------------------------
void BusInterface::setBitSteering(BusInterface::BitSteering bitSteering)
{
	bitSteering_ = bitSteering;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getMaster()
//-----------------------------------------------------------------------------
QSharedPointer<MasterInterface> BusInterface::getMaster() const
{
	return master_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setMaster()
//-----------------------------------------------------------------------------
void BusInterface::setMaster( QSharedPointer<MasterInterface> master )
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	systemGroup_.clear();
	mirroredSlave_.clear();

	master_ = master;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getSlave()
//-----------------------------------------------------------------------------
QSharedPointer<SlaveInterface> BusInterface::getSlave() const
{
	return slave_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setSlave()
//-----------------------------------------------------------------------------
void BusInterface::setSlave(QSharedPointer<SlaveInterface> slave)
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	systemGroup_.clear();
	mirroredSlave_.clear();

	interfaceMode_ = General::SLAVE;
	slave_ = slave;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getSystem()
//-----------------------------------------------------------------------------
QString BusInterface::getSystem() const
{
	return systemGroup_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setSystem()
//-----------------------------------------------------------------------------
void BusInterface::setSystem(QString const& systemGroupName)
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	systemGroup_.clear();
	mirroredSlave_.clear();
    
	systemGroup_ = systemGroupName;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getMonitor()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface::MonitorInterface> BusInterface::getMonitor() const
{
	return monitor_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setMonitor()
//-----------------------------------------------------------------------------
void BusInterface::setMonitor(QSharedPointer<BusInterface::MonitorInterface> monitor)
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	systemGroup_.clear();
	mirroredSlave_.clear();

	interfaceMode_ = General::MONITOR;
	monitor_ = monitor;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getMirroredSlave()
//-----------------------------------------------------------------------------
QSharedPointer<MirroredSlaveInterface> BusInterface::getMirroredSlave() const
{
	return mirroredSlave_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setMirroredSlave()
//-----------------------------------------------------------------------------
void BusInterface::setMirroredSlave( QSharedPointer<MirroredSlaveInterface> mirroredSlave )
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	systemGroup_.clear();
	mirroredSlave_.clear();

	interfaceMode_ = General::MIRROREDSLAVE;
	mirroredSlave_ = mirroredSlave;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::hasBridge()
//-----------------------------------------------------------------------------
bool BusInterface::hasBridge() const
{
    if (!slave_)
    {
        return false;
    }

    return slave_->hasBridge();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> BusInterface::getAttributes() const
{
	return attributes_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setAttributes()
//-----------------------------------------------------------------------------
void BusInterface::setAttributes(QMap<QString, QString> const& attributes)
{
	attributes_ = attributes;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getPhysicalPortNames()
//-----------------------------------------------------------------------------
QStringList BusInterface::getPhysicalPortNames() const
{
	QStringList list;
    if (!abstractionTypes_->isEmpty())
    {
        foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->getPortMaps())
        {
            if (portMap->getPhysicalPort())
            {
                list.append(portMap->getPhysicalPort()->name_);
            }
        }
    }

	return list;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getLogicalPortNames()
//-----------------------------------------------------------------------------
QStringList BusInterface::getLogicalPortNames() const
{
	QStringList list;
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->getPortMaps())
	{
		list.append(portMap->getLogicalPort()->name_);
	}
	return list;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::hasLogicalPort()
//-----------------------------------------------------------------------------
bool BusInterface::hasLogicalPort( const QString& logicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->getPortMaps())
	{
		if (portMap->getLogicalPort()->name_ == logicalPortName)
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::hasPhysicalPort()
//-----------------------------------------------------------------------------
bool BusInterface::hasPhysicalPort( const QString& physicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->getPortMaps())
	{
		if (portMap->getPhysicalPort()->name_ == physicalPortName)
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getLogicalPortName()
//-----------------------------------------------------------------------------
QString BusInterface::getLogicalPortName( const QString& physicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->getPortMaps())
	{
		if (portMap->getPhysicalPort()->name_.compare(physicalPortName) == 0)
		{
			return portMap->getLogicalPort()->name_;
		}
	}

	return QString();
}

//-----------------------------------------------------------------------------
// Function: setMCAPIPortID()
//-----------------------------------------------------------------------------
void BusInterface::setMCAPIPortID(int portID)
{
    foreach (QSharedPointer<Parameter> param, *parameters_)
    {
        if (param->name() == "kts_port_id")
        {
            param->setValue(QString::number(portID));
            return;
        }
    }
    
    QSharedPointer<Parameter> param(new Parameter());
    param->setName("kts_port_id");
    param->setValue(QString::number(portID));
    parameters_->append(param);
}

//-----------------------------------------------------------------------------
// Function: getMCAPIPortID()
//-----------------------------------------------------------------------------
int BusInterface::getMCAPIPortID() const
{
    foreach (QSharedPointer<Parameter> param, *parameters_)
    {
        if (param->name() == "kts_port_id")
        {
            return param->getValue().toInt();
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getMemoryMapRef()
//-----------------------------------------------------------------------------
QString BusInterface::getMemoryMapRef() const
{
	if (interfaceMode_ != General::SLAVE)
    {
		return QString();
	}
	else if (!slave_)
    {
		return QString();
	}
	else
    {
		return slave_->getMemoryMapRef();
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getAddressSpaceRef()
//-----------------------------------------------------------------------------
QString BusInterface::getAddressSpaceRef() const
{
	if (interfaceMode_ == General::MASTER || interfaceMode_ == General::MIRROREDMASTER)
    {
        if (!master_)
        {
            return QString();
        }
        else
        {
            return master_->getAddressSpaceRef();
        }
	}
	else
    {
		return QString();
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setDefaultPos()
//-----------------------------------------------------------------------------
void BusInterface::setDefaultPos(QPointF const& pos)
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:position")
        {
            getVendorExtensions()->removeAll(extension);
        }
    }

    if (!pos.isNull())
    {
        QSharedPointer<Kactus2Position> newDefaultPosition (new Kactus2Position(pos));
        getVendorExtensions()->append(newDefaultPosition);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF BusInterface::getDefaultPos()
{
    foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:position")
        {
            QSharedPointer<Kactus2Position> defaultPosition = extension.dynamicCast<Kactus2Position>();
            return defaultPosition->position();
        }
    }

    return QPointF(0, 0);
}

//-----------------------------------------------------------------------------
// Function: BusInterface::copyAbstractionTypes()
//-----------------------------------------------------------------------------
void BusInterface::copyAbstractionTypes(const BusInterface& other)
{
    foreach (QSharedPointer<AbstractionType> abstractionType, *other.abstractionTypes_)
    {
        if (abstractionType)
        {
            QSharedPointer<AbstractionType> copy =
                QSharedPointer<AbstractionType>(new AbstractionType(*abstractionType.data()));
            abstractionTypes_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterface::copyParameters()
//-----------------------------------------------------------------------------
void BusInterface::copyParameters(const BusInterface& other)
{
    foreach (QSharedPointer<Parameter> param, *other.parameters_)
    {
        if (param)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*param.data()));
            parameters_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterface::copyInterfaceModes()
//-----------------------------------------------------------------------------
void BusInterface::copyInterfaceModes(const BusInterface& other)
{
    if (other.master_)
    {
        master_ = QSharedPointer<MasterInterface>(new MasterInterface(*other.master_.data()));
    }

    if (other.slave_)
    {
        slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface(*other.slave_.data()));
    }

    if (other.monitor_)
    {
        monitor_ = QSharedPointer<MonitorInterface>(new MonitorInterface(*other.monitor_.data()));
    }

    if (other.mirroredSlave_)
    {
        mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface(*other.mirroredSlave_));
    }
}