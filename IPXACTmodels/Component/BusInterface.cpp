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

#include "InitiatorInterface.h"
#include "MirroredTargetInterface.h"
#include "TargetInterface.h"
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
BusInterface::MonitorInterface::MonitorInterface()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterface::BusInterface()
//-----------------------------------------------------------------------------
BusInterface::BusInterface():
NameGroup(),
Extendable()
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
interfaceMode_(other.interfaceMode_),
connectionRequired_(other.connectionRequired_),
bitsInLau_(other.bitsInLau_),
bitSteering_(other.bitSteering_),
bitSteeringAttributes_(other.bitSteeringAttributes_),
endianness_(other.endianness_),
systemGroup_(other.systemGroup_)
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
	initiator_.clear();
	target_.clear();
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
QString BusInterface::getBitSteering() const
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

    if (interfaceMode == General::MASTER || interfaceMode == General::INITIATOR)
    {
        monitor_.clear();
        target_.clear();
        systemGroup_.clear();
        mirroredTarget_.clear();
        if (!initiator_)
        {
            initiator_ = QSharedPointer<InitiatorInterface>(new InitiatorInterface());
        }
    }
    else if (interfaceMode == General::SLAVE || interfaceMode == General::TARGET)
    {
        monitor_.clear();
        initiator_.clear();
        systemGroup_.clear();
        mirroredTarget_.clear();
        if (!target_)
        {
            target_ = QSharedPointer<TargetInterface>(new TargetInterface());
        }
    }
    else if (interfaceMode == General::SYSTEM)
    {
        monitor_.clear();
        target_.clear();
        initiator_.clear();
        mirroredTarget_.clear();
        systemGroup_ = QStringLiteral("default");
    }
    else if (interfaceMode == General::MIRRORED_SLAVE || interfaceMode == General::MIRRORED_TARGET)
    {
        monitor_.clear();
        target_.clear();
        initiator_.clear();
        systemGroup_.clear();
        if (!mirroredTarget_)
        {
            mirroredTarget_ = QSharedPointer<MirroredTargetInterface>(new MirroredTargetInterface());
        }
    }
    else if (interfaceMode == General::MIRRORED_MASTER || interfaceMode == General::MIRRORED_INITIATOR)
    {
        monitor_.clear();
        target_.clear();
        systemGroup_.clear();
        mirroredTarget_.clear();
        initiator_.clear();
    }
    else if (interfaceMode == General::MIRRORED_SYSTEM)
    {
        monitor_.clear();
        target_.clear();
        initiator_.clear();
        mirroredTarget_.clear();
        systemGroup_ = QStringLiteral("default");
    }
    else if (interfaceMode == General::MONITOR)
    {
        target_.clear();
        initiator_.clear();
        systemGroup_.clear();
        mirroredTarget_.clear();
        if (!monitor_)
        {
            monitor_ = QSharedPointer<MonitorInterface>(new MonitorInterface());
        }
    }
    else
    {
        monitor_.clear();
        target_.clear();
        initiator_.clear();
        systemGroup_.clear();
        mirroredTarget_.clear();
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
void BusInterface::setBitSteering(QString const& bitSteering)
{
	bitSteering_ = bitSteering;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getMaster()
//-----------------------------------------------------------------------------
QSharedPointer<InitiatorInterface> BusInterface::getMaster() const
{
	return getInitiator();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setMaster()
//-----------------------------------------------------------------------------
void BusInterface::setMaster( QSharedPointer<InitiatorInterface> master )
{
    setInitiator(master); 
    interfaceMode_ = General::MASTER;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getInitiator()
//-----------------------------------------------------------------------------
QSharedPointer<InitiatorInterface> BusInterface::getInitiator() const
{
    return initiator_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setInitiator()
//-----------------------------------------------------------------------------
void BusInterface::setInitiator(QSharedPointer<InitiatorInterface> initiator)
{
    monitor_.clear();
    target_.clear();
    initiator_.clear();
    systemGroup_.clear();
    mirroredTarget_.clear();

    interfaceMode_ = General::INITIATOR;
    initiator_ = initiator;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getSlave()
//-----------------------------------------------------------------------------
QSharedPointer<TargetInterface> BusInterface::getSlave() const
{
	return target_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setSlave()
//-----------------------------------------------------------------------------
void BusInterface::setSlave(QSharedPointer<TargetInterface> slave)
{
    setTarget(slave);
    interfaceMode_ = General::SLAVE;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getTarget()
//-----------------------------------------------------------------------------
QSharedPointer<TargetInterface> BusInterface::getTarget() const
{
    return target_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setSlave()
//-----------------------------------------------------------------------------
void BusInterface::setTarget(QSharedPointer<TargetInterface> target)
{
    monitor_.clear();
    target_.clear();
    initiator_.clear();
    systemGroup_.clear();
    mirroredTarget_.clear();

    interfaceMode_ = General::TARGET;
    target_ = target;
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
	target_.clear();
	initiator_.clear();
	systemGroup_.clear();
	mirroredTarget_.clear();
    
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
	target_.clear();
	initiator_.clear();
	systemGroup_.clear();
	mirroredTarget_.clear();

	interfaceMode_ = General::MONITOR;
	monitor_ = monitor;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getMirroredSlave()
//-----------------------------------------------------------------------------
QSharedPointer<MirroredTargetInterface> BusInterface::getMirroredSlave() const
{
    return getMirroredTarget();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setMirroredSlave()
//-----------------------------------------------------------------------------
void BusInterface::setMirroredSlave( QSharedPointer<MirroredTargetInterface> mirroredSlave )
{
    setMirroredTarget(mirroredSlave);
    interfaceMode_ = General::MIRRORED_SLAVE;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getMirroredTarget()
//-----------------------------------------------------------------------------
QSharedPointer<MirroredTargetInterface> BusInterface::getMirroredTarget() const
{
    return mirroredTarget_;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setMirroredTarget()
//-----------------------------------------------------------------------------
void BusInterface::setMirroredTarget(QSharedPointer<MirroredTargetInterface> mirroredTarget)
{
    monitor_.clear();
    target_.clear();
    initiator_.clear();
    systemGroup_.clear();
    mirroredTarget_.clear();

    interfaceMode_ = General::MIRRORED_TARGET;
    mirroredTarget_ = mirroredTarget;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::hasBridge()
//-----------------------------------------------------------------------------
bool BusInterface::hasBridge() const
{
    if (!target_)
    {
        return false;
    }

    return target_->hasBridge();
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
// Function: BusInterface::containsMappedPhysicalPort()
//-----------------------------------------------------------------------------
bool BusInterface::containsMappedPhysicalPort(QString const& portName) const
{
    if (abstractionTypes_)
    {
        for (QSharedPointer<AbstractionType> abstraction : *abstractionTypes_)
        {
            if (abstraction->hasPhysicalPort(portName))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::containsMappedlogicalPort()
//-----------------------------------------------------------------------------
bool BusInterface::containsMappedlogicalPort(QString const& portName) const
{
    if (abstractionTypes_)
    {
        for (QSharedPointer<AbstractionType> abstraction : *abstractionTypes_)
        {
            if (abstraction->hasLogicalPort(portName))
            {
                return true;
            }
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getAllMappedPhysicalPorts()
//-----------------------------------------------------------------------------
QStringList BusInterface::getAllMappedPhysicalPorts() const
{
    QStringList portNames;

    for (QSharedPointer<AbstractionType> abstraction : *abstractionTypes_)
    {
        for (QString const& port : abstraction->getPhysicalPortNames())
        {
            portNames.append(port);
        }
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getAllPortMaps()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PortMap> > > BusInterface::getAllPortMaps() const
{
    QSharedPointer<QList<QSharedPointer<PortMap> > > containedPortMaps(new QList<QSharedPointer<PortMap> > ());

    for (QSharedPointer<AbstractionType> abstraction : *abstractionTypes_)
    {
        for (QSharedPointer<PortMap> portMap : *abstraction->getPortMaps())
        {
            containedPortMaps->append(portMap);
        }
    }

    return containedPortMaps;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getPortMapsForView()
//-----------------------------------------------------------------------------
QList<QSharedPointer<PortMap> > BusInterface::getPortMapsForView(QString const& view) const
{
    QList<QSharedPointer<PortMap> > portMaps;

    for (QSharedPointer<AbstractionType> abstraction : *abstractionTypes_)
    {
        if (abstraction->getViewReferences()->isEmpty() || abstraction->getViewReferences()->contains(view))
        {
            for (QSharedPointer<PortMap> abstractionMap : *abstraction->getPortMaps())
            {
                portMaps.append(abstractionMap);
            }
        }
    }

    return portMaps;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getAbstractionContainingView()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionType> BusInterface::getAbstractionContainingView(QString const& view) const
{
    for (QSharedPointer<AbstractionType> abstraction : *abstractionTypes_)
    {
        if (abstraction->getViewReferences()->isEmpty() || abstraction->getViewReferences()->contains(view))
        {
            return abstraction;
        }
    }

    return QSharedPointer<AbstractionType>();
}

//-----------------------------------------------------------------------------
// Function: BusInterface::clearAllPortMaps()
//-----------------------------------------------------------------------------
void BusInterface::clearAllPortMaps()
{
    if (abstractionTypes_)
    {
        for (QSharedPointer<AbstractionType> abstraction : *abstractionTypes_)
        {
            abstraction->getPortMaps()->clear();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: setMCAPIPortID()
//-----------------------------------------------------------------------------
void BusInterface::setMCAPIPortID(int portID)
{
    for (QSharedPointer<Parameter> param : *parameters_)
    {
        if (param->name() == QLatin1String("kts_port_id"))
        {
            param->setValue(QString::number(portID));
            return;
        }
    }
    
    QSharedPointer<Parameter> param(new Parameter());
    param->setName(QStringLiteral("kts_port_id"));
    param->setValue(QString::number(portID));
    parameters_->append(param);
}

//-----------------------------------------------------------------------------
// Function: getMCAPIPortID()
//-----------------------------------------------------------------------------
int BusInterface::getMCAPIPortID() const
{
    for (QSharedPointer<Parameter> param : *parameters_)
    {
        if (param->name() == QLatin1String("kts_port_id"))
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
	if ((interfaceMode_ != General::SLAVE && interfaceMode_ != General::TARGET) || !target_)
    {
		return QString();
	}
	else
    {
		return target_->getMemoryMapRef();
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getAddressSpaceRef()
//-----------------------------------------------------------------------------
QString BusInterface::getAddressSpaceRef() const
{
	if (interfaceMode_ == General::MASTER || interfaceMode_ == General::MIRRORED_MASTER)
    {
        if (!initiator_)
        {
            return QString();
        }
        else
        {
            return initiator_->getAddressSpaceRef();
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
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:position"))
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
    for (QSharedPointer<VendorExtension> extension : *getVendorExtensions())
    {
        if (extension->type() == QLatin1String("kactus2:position"))
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
    for (QSharedPointer<AbstractionType> abstractionType : *other.abstractionTypes_)
    {
        QSharedPointer<AbstractionType> copy =
            QSharedPointer<AbstractionType>(new AbstractionType(*abstractionType));
        abstractionTypes_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterface::copyParameters()
//-----------------------------------------------------------------------------
void BusInterface::copyParameters(const BusInterface& other)
{
    for (QSharedPointer<Parameter> param : *other.parameters_)
    {
        QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*param));
        parameters_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterface::copyInterfaceModes()
//-----------------------------------------------------------------------------
void BusInterface::copyInterfaceModes(const BusInterface& other)
{
    if (other.initiator_)
    {
        initiator_ = QSharedPointer<InitiatorInterface>(new InitiatorInterface(*other.initiator_));
    }

    if (other.target_)
    {
        target_ = QSharedPointer<TargetInterface>(new TargetInterface(*other.target_));
    }

    if (other.monitor_)
    {
        monitor_ = QSharedPointer<MonitorInterface>(new MonitorInterface(*other.monitor_));
    }

    if (other.mirroredTarget_)
    {
        mirroredTarget_ = QSharedPointer<MirroredTargetInterface>(new MirroredTargetInterface(*other.mirroredTarget_));
    }
}
