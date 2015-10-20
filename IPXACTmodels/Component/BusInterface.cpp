//-----------------------------------------------------------------------------
// File: AddressSpace.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date:
//
// Description:
// Describes the ipxact:busInterface element.
//-----------------------------------------------------------------------------

#include "businterface.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/slaveinterface.h>
#include <IPXACTmodels/PortMap.h>
#include <IPXACTmodels/vlnv.h>

#include <IPXACTmodels/VendorExtension.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: businterface::MonitorInterface::MonitorInterface()
//-----------------------------------------------------------------------------
BusInterface::MonitorInterface::MonitorInterface():
interfaceMode_(General::MONITOR),
group_()
{

}

//-----------------------------------------------------------------------------
// Function: businterface::AbstractionType()
//-----------------------------------------------------------------------------
BusInterface::AbstractionType::AbstractionType():
viewRef_(),
abstractionRef_(),
portMaps_(new QList<QSharedPointer<PortMap> > ())
{

}

//-----------------------------------------------------------------------------
// Function: businterface::AbstractionType()
//-----------------------------------------------------------------------------
BusInterface::AbstractionType::AbstractionType( const BusInterface::AbstractionType &other ):
viewRef_(other.viewRef_),
abstractionRef_(),
portMaps_(new QList<QSharedPointer<PortMap> > ())
{
    if (other.abstractionRef_)
    {
        abstractionRef_ = QSharedPointer<ConfigurableVLNVReference>
            (new ConfigurableVLNVReference(*other.abstractionRef_.data()));
    }

	foreach (QSharedPointer<PortMap> portMap, *other.portMaps_)
    {
        if (portMap)
        {
            QSharedPointer<PortMap> copy = QSharedPointer<PortMap>(new PortMap(*portMap.data()));
            portMaps_->append(copy);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: businterface::AbstractionType::operator=()
//-----------------------------------------------------------------------------
BusInterface::AbstractionType & BusInterface::AbstractionType::operator=
    ( const BusInterface::AbstractionType &other )
{
	if (this != &other)
	{
        viewRef_ = other.viewRef_;

        abstractionRef_.clear();
        if (other.abstractionRef_)
        {
            abstractionRef_ = QSharedPointer<ConfigurableVLNVReference>
                (new ConfigurableVLNVReference(*other.abstractionRef_.data()));
        }

		portMaps_->clear();
		foreach (QSharedPointer<PortMap> portMap, *other.portMaps_)
		{
			if (portMap)
			{
				QSharedPointer<PortMap> copy = QSharedPointer<PortMap>(new PortMap(*portMap.data()));
				portMaps_->append(copy);
			}
		}
	}

	return *this;
}

//-----------------------------------------------------------------------------
// Function: businterface::BusInterface()
//-----------------------------------------------------------------------------
BusInterface::BusInterface():
NameGroup(),
Extendable(),
attributes_(),
busType_(),
interfaceMode_(General::INTERFACE_MODE_COUNT),
connectionRequired_(),
abstractionTypes_(new QList<QSharedPointer<AbstractionType> > ()),
bitsInLau_(),
bitSteering_(General::BITSTEERING_UNSPECIFIED),
bitSteeringAttributes_(),
endianness_(General::ENDIANNESS_UNSPECIFIED),
parameters_(new QList<QSharedPointer<Parameter> >()), 
master_(QSharedPointer<MasterInterface>(new MasterInterface)),
slave_(),
system_(),
monitor_()
{

}

//-----------------------------------------------------------------------------
// Function: businterface::BusInterface()
//-----------------------------------------------------------------------------
BusInterface::BusInterface( const BusInterface &other ):
NameGroup(other),
Extendable(other),
attributes_(other.attributes_),
busType_(other.busType_),
interfaceMode_(other.interfaceMode_),
connectionRequired_(other.connectionRequired_),
abstractionTypes_(new QList<QSharedPointer<AbstractionType> > ()),
bitsInLau_(other.bitsInLau_),
bitSteering_(other.bitSteering_),
bitSteeringAttributes_(other.bitSteeringAttributes_),
endianness_(other.endianness_),
parameters_(new QList<QSharedPointer<Parameter> >()),
master_(),
slave_(),
system_(other.system_),
monitor_(),
mirroredSlave_()
{
    copyAbstractionTypes(other);
    copyParameters(other);
    copyInterfaceModes(other);
}

//-----------------------------------------------------------------------------
// Function: businterface::operator=()
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
		system_ = other.system_;

        abstractionTypes_->clear();
        copyAbstractionTypes(other);

        parameters_->clear();
        copyParameters(other);

        copyInterfaceModes(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: businterface::~BusInterface()
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
/*
bool BusInterface::isValid( const QList<General::PortBounds>& physicalPorts, QStringList const& memoryMaps,
    QStringList const& addressSpaces,
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList,
    const QString& parentIdentifier ) const 
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("bus interface %1").arg(name()));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for bus interface within %1").arg(
			parentIdentifier));
		valid = false;
	}

	// check the bus type validity
	if (!busType_.isValid(errorList, QObject::tr("bus type in %1").arg(thisIdentifier))) {
		valid = false;
	}

    if (interfaceMode_ == General::MASTER || interfaceMode_ == General::MIRROREDMASTER)
    {
        if (!master_)
        {
            errorList.append(QObject::tr("The interface mode of %1 is "
                "master/mirrored master but no element for it has been defined.").arg(
                thisIdentifier));
            valid = false;
        }
        else if (!master_->getAddressSpaceRef().isEmpty() && 
            !addressSpaces.contains(master_->getAddressSpaceRef()))
        {
            errorList.append(QObject::tr("Bus interface %1 references address space %2 which is not "
                "found within %3.").arg(name(), master_->getAddressSpaceRef(), parentIdentifier));
            valid = false;
        }

    }
    else if (interfaceMode_ == General::SLAVE)
    {
        if (!slave_)
        {
            errorList.append(QObject::tr("The interface mode of %1 is "
                "slave but no element for it has been defined.").arg(
                thisIdentifier));
            valid = false;
        }
        else if (!slave_->getMemoryMapRef().isEmpty() && !memoryMaps.contains(slave_->getMemoryMapRef()))
        {
            errorList.append(QObject::tr("Bus interface %1 references memory map %2 which is not "
                "found within %3.").arg(name(), slave_->getMemoryMapRef(), parentIdentifier));
            valid = false;
        }

    }
    else if (interfaceMode_ ==  General::MIRROREDSLAVE)
    {
        if (!mirroredSlave_) {
            errorList.append(QObject::tr("The interface mode of %1 is "
                "mirrored slave but no element for it has been defined.").arg(
                thisIdentifier));
            valid = false;
        }
    }
    else if (interfaceMode_ ==  General::SYSTEM || General::MIRROREDSYSTEM)
    {
        if (system_.isEmpty())
        {
            errorList.append(QObject::tr("Interface mode is system/mirrored system"
                " but no group has been specified for %1 within %2").arg(
                thisIdentifier).arg(parentIdentifier));
            valid = false;
        }
    }
    else if (interfaceMode_ ==  General::MONITOR)
    {
        if (monitor_->interfaceMode_ == General::INTERFACE_MODE_COUNT)
        {
            errorList.append(QObject::tr("No interface mode set for monitor"
                " within %1").arg(thisIdentifier));
            valid = false;
        }
    }
    // if the interface mode is invalid
    else
    {
        errorList.append(QObject::tr("The interface mode of %1 is not specified.").arg(thisIdentifier));
        valid = false;
    }

	/*foreach (QSharedPointer<PortMap> portMap, abstractionTypes_.first()->portMaps_) {
		if (!portMap->isValid(physicalPorts, errorList, thisIdentifier)) {
			valid = false;
		}
	}*/

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, *parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), thisIdentifier, componentChoices));
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
//     }
/*
	return valid;
}

bool BusInterface::isValid( const QList<General::PortBounds>& physicalPorts, QStringList const& memoryMaps,
    QStringList const& addressSpaces, QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const 
{
	if (name().isEmpty())
    {
		return false;
	}

	// check the bus type validity
	if (!busType_.isValid())
    {
		return false;
	}

    if (interfaceMode_ == General::MASTER || interfaceMode_ == General::MIRROREDMASTER)
    {
        if (!master_ ||
            (!master_->getAddressSpaceRef().isEmpty() && !addressSpaces.contains(master_->getAddressSpaceRef())))
        {
            return false;
        }

    }
    else if (interfaceMode_ == General::SLAVE)
    {
        if (!slave_ || (!slave_->getMemoryMapRef().isEmpty() && !memoryMaps.contains(slave_->getMemoryMapRef()))) 
        {
            return false;
        }
    }
    else if (interfaceMode_ == General::MIRROREDSLAVE)
    {
        if (!mirroredSlave_) {
            return false;
        }
    }
    else if (interfaceMode_ == General::SYSTEM || interfaceMode_ == General::MIRROREDSYSTEM)
    {
        if (system_.isEmpty())
        {
            return false;
        }
    }
    else if (interfaceMode_ == General::MONITOR)
    {
        if (monitor_->interfaceMode_ == General::INTERFACE_MODE_COUNT) {
            return false;
        }
    }
    // if the interface mode is invalid
    else
    {
        return false;
    }

	/*foreach (QSharedPointer<PortMap> portMap, abstractionTypes_.first()->portMaps_) {
		if (!portMap->isValid(physicalPorts)) {
			return false;
		}
	}*/

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, *parameters_)
//     {
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             return false;
//         }
//     }
/*
	return true;
}*/

//-----------------------------------------------------------------------------
// Function: businterface::getBitSteering()
//-----------------------------------------------------------------------------
General::BitSteering BusInterface::getBitSteering() const
{
	return bitSteering_;
}

//-----------------------------------------------------------------------------
// Function: businterface::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterface::getInterfaceMode() const
{
	return interfaceMode_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setPortMaps()
//-----------------------------------------------------------------------------
void BusInterface::setPortMaps(QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps)
{
    abstractionTypes_->first()->portMaps_->clear();
    abstractionTypes_->first()->portMaps_ = portMaps;
}

//-----------------------------------------------------------------------------
// Function: businterface::setAbstractionTypes()
//-----------------------------------------------------------------------------
void BusInterface::setAbstractionTypes(QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionTypes)
{
    abstractionTypes_->clear();
    abstractionTypes_ = abstractionTypes;
}

//-----------------------------------------------------------------------------
// Function: businterface::setConnectionRequired()
//-----------------------------------------------------------------------------
void BusInterface::setConnectionRequired(bool connectionRequired)
{
    connectionRequired_.setValue(connectionRequired);
}

//-----------------------------------------------------------------------------
// Function: businterface::clearConnectionRequired()
//-----------------------------------------------------------------------------
void BusInterface::clearConnectionRequired()
{
    connectionRequired_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: businterface::setEndianness()
//-----------------------------------------------------------------------------
void BusInterface::setEndianness(General::Endianness endianness)
{
	endianness_ = endianness;
}

//-----------------------------------------------------------------------------
// Function: businterface::getBitsInLau()
//-----------------------------------------------------------------------------
QString BusInterface::getBitsInLau() const
{
	return bitsInLau_;
}

//-----------------------------------------------------------------------------
// Function: businterface::getConnectionRequired()
//-----------------------------------------------------------------------------
QString BusInterface::getConnectionRequired() const
{
	return connectionRequired_.toString();
}

//-----------------------------------------------------------------------------
// Function: businterface::setBitsInLau()
//-----------------------------------------------------------------------------
void BusInterface::setBitsInLau(QString const& newBitsInLau)
{
	bitsInLau_ = newBitsInLau;
}

//-----------------------------------------------------------------------------
// Function: businterface::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > BusInterface::getParameters() const
{
	return parameters_;
}

//-----------------------------------------------------------------------------
// Function: businterface::getPortMaps()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortMap> > > BusInterface::getPortMaps() const
{
	return abstractionTypes_->first()->portMaps_;
}

//-----------------------------------------------------------------------------
// Function: businterface::getAbstractionTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<BusInterface::AbstractionType> > > BusInterface::getAbstractionTypes() const
{
	return abstractionTypes_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setBitSteeringAttributes()
//-----------------------------------------------------------------------------
void BusInterface::setBitSteeringAttributes(QMap<QString, QString> &bitSteeringAttributes)
{
	bitSteeringAttributes_.clear();
	bitSteeringAttributes_ = bitSteeringAttributes;
}

//-----------------------------------------------------------------------------
// Function: businterface::getBusType()
//-----------------------------------------------------------------------------
VLNV BusInterface::getBusType() const
{
	return busType_;
}

//-----------------------------------------------------------------------------
// Function: businterface::getBitSteeringAttributes()
//-----------------------------------------------------------------------------
const QMap<QString, QString>& BusInterface::getBitSteeringAttributes()
{
	return bitSteeringAttributes_;
}

//-----------------------------------------------------------------------------
// Function: businterface::getEndianness()
//-----------------------------------------------------------------------------
General::Endianness BusInterface::getEndianness() const
{
	return endianness_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setInterfaceMode()
//-----------------------------------------------------------------------------
void BusInterface::setInterfaceMode(General::InterfaceMode interfaceMode)
{
	interfaceMode_ = interfaceMode;

    if (interfaceMode == General::MASTER)
    {
        monitor_.clear();
        slave_.clear();
        system_.clear();
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
        system_.clear();
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
        system_ = "default";
    }
    else if (interfaceMode == General::MIRROREDSLAVE)
    {
        monitor_.clear();
        slave_.clear();
        master_.clear();
        system_.clear();
        if (!mirroredSlave_)
        {
            mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(new MirroredSlaveInterface());
        }
    }
    else if (interfaceMode == General::MIRROREDMASTER)
    {
        monitor_.clear();
        slave_.clear();
        system_.clear();
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
        system_ = "default";
    }
    else if (interfaceMode == General::MONITOR)
    {
        slave_.clear();
        master_.clear();
        system_.clear();
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
        system_.clear();
        mirroredSlave_.clear();
	}
}

//-----------------------------------------------------------------------------
// Function: businterface::setBusType()
//-----------------------------------------------------------------------------
void BusInterface::setBusType(const VLNV& busType)
{
	busType_ = busType;
}

//-----------------------------------------------------------------------------
// Function: businterface::setBitSteering()
//-----------------------------------------------------------------------------
void BusInterface::setBitSteering(General::BitSteering bitSteering)
{
	bitSteering_ = bitSteering;
}

//-----------------------------------------------------------------------------
// Function: businterface::getMaster()
//-----------------------------------------------------------------------------
QSharedPointer<MasterInterface> BusInterface::getMaster()
{
	return master_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setMaster()
//-----------------------------------------------------------------------------
void BusInterface::setMaster( QSharedPointer<MasterInterface> master )
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	master_ = master;
}

//-----------------------------------------------------------------------------
// Function: businterface::getSlave()
//-----------------------------------------------------------------------------
QSharedPointer<SlaveInterface> BusInterface::getSlave()
{
	return slave_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setSlave()
//-----------------------------------------------------------------------------
void BusInterface::setSlave( QSharedPointer<SlaveInterface> slave )
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	interfaceMode_ = General::SLAVE;
	slave_ = slave;
}

//-----------------------------------------------------------------------------
// Function: businterface::getSystem()
//-----------------------------------------------------------------------------
QString BusInterface::getSystem() const
{
	return system_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setSystem()
//-----------------------------------------------------------------------------
void BusInterface::setSystem(const QString& system)
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	system_ = system;
}

//-----------------------------------------------------------------------------
// Function: businterface::getMonitor()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface::MonitorInterface> BusInterface::getMonitor()
{
	return monitor_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setMonitor()
//-----------------------------------------------------------------------------
void BusInterface::setMonitor( QSharedPointer<BusInterface::MonitorInterface> monitor )
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	interfaceMode_ = General::MONITOR;
	monitor_ = monitor;
}

//-----------------------------------------------------------------------------
// Function: businterface::getMirroredSlave()
//-----------------------------------------------------------------------------
QSharedPointer<MirroredSlaveInterface> BusInterface::getMirroredSlave()
{
	return mirroredSlave_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setMirroredSlave()
//-----------------------------------------------------------------------------
void BusInterface::setMirroredSlave( QSharedPointer<MirroredSlaveInterface> mirroredSlave )
{
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	interfaceMode_ = General::MIRROREDSLAVE;
	mirroredSlave_ = mirroredSlave;
}

//-----------------------------------------------------------------------------
// Function: businterface::hasBridge()
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
// Function: businterface::getAttributes()
//-----------------------------------------------------------------------------
const QMap<QString, QString>& BusInterface::getAttributes() const
{
	return attributes_;
}

//-----------------------------------------------------------------------------
// Function: businterface::setAttributes()
//-----------------------------------------------------------------------------
void BusInterface::setAttributes( const QMap<QString, QString>& attributes )
{
	attributes_ = attributes;
}

//-----------------------------------------------------------------------------
// Function: businterface::getPhysicalPortNames()
//-----------------------------------------------------------------------------
QStringList BusInterface::getPhysicalPortNames() const
{
	QStringList list;
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->portMaps_)
	{
		list.append(portMap->getPhysicalPort()->name_);
	}
	return list;
}

//-----------------------------------------------------------------------------
// Function: businterface::getLogicalPortNames()
//-----------------------------------------------------------------------------
QStringList BusInterface::getLogicalPortNames() const
{
	QStringList list;
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->portMaps_)
	{
		list.append(portMap->getLogicalPort()->name_);
	}
	return list;
}

//-----------------------------------------------------------------------------
// Function: businterface::hasLogicalPort()
//-----------------------------------------------------------------------------
bool BusInterface::hasLogicalPort( const QString& logicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->portMaps_)
	{
		if (portMap->getLogicalPort()->name_ == logicalPortName)
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: businterface::hasPhysicalPort()
//-----------------------------------------------------------------------------
bool BusInterface::hasPhysicalPort( const QString& physicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->portMaps_)
	{
		if (portMap->getPhysicalPort()->name_ == physicalPortName)
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: businterface::getLogicalPortName()
//-----------------------------------------------------------------------------
QString BusInterface::getLogicalPortName( const QString& physicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, *abstractionTypes_->first()->portMaps_)
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
// Function: businterface::getMemoryMapRef()
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
// Function: businterface::getAddressSpaceRef()
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
QPointF const& BusInterface::getDefaultPos()
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
// Function: businterface::copyAbstractionTypes()
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
// Function: businterface::copyParameters()
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
// Function: businterface::copyInterfaceModes()
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