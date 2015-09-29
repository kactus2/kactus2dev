/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "businterface.h"
#include "../generaldeclarations.h"
#include "../masterinterface.h"
#include "../mirroredslaveinterface.h"
#include "../slaveinterface.h"
#include "../PortMap.h"
#include "../vlnv.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>

#include <QDebug>

const int DEFAULT_BITS_IN_LAU = 8;

// struct constructor
BusInterface::MonitorInterface::MonitorInterface(): 
interfaceMode_(General::MONITOR), group_()
{
}

BusInterface::AbstractionType::AbstractionType( const BusInterface::AbstractionType &other ):
	abstractionRef_(other.abstractionRef_),
	portMaps_()
{
	foreach (QSharedPointer<PortMap> portMap, other.portMaps_) {
		if (portMap) {
			QSharedPointer<PortMap> copy = QSharedPointer<PortMap>(
				new PortMap(*portMap.data()));
			portMaps_.append(copy);
		}
	}
}

BusInterface::AbstractionType & BusInterface::AbstractionType::operator=( const BusInterface::AbstractionType &other )
{
	if (this != &other)
	{	
		abstractionRef_ = other.abstractionRef_;

		portMaps_.clear();

		foreach (QSharedPointer<PortMap> portMap, other.portMaps_)
		{
			if (portMap)
			{
				QSharedPointer<PortMap> copy = QSharedPointer<PortMap>(new PortMap(*portMap.data()));
				portMaps_.append(copy);
			}
		}
	}

	return *this;
}

BusInterface::BusInterface(): NameGroup(),
		attributes_(),
		busType_(),
        interfaceMode_(General::INTERFACE_MODE_COUNT),
		connectionRequired_(false), abstractionTypes_(),
		bitsInLau_(DEFAULT_BITS_IN_LAU),
		bitSteering_(General::BITSTEERING_UNSPECIFIED),
		bitSteeringAttributes_(),
		endianness_(General::LITTLE),
        parameters_(new QList<QSharedPointer<Parameter> >()), 
        master_(QSharedPointer<MasterInterface>(new MasterInterface)),
        slave_(), system_(),
        monitor_(),
        defaultPos_()
{
}

BusInterface::BusInterface( const BusInterface &other ):
NameGroup(other),
attributes_(other.attributes_),
busType_(other.busType_),
interfaceMode_(other.interfaceMode_),
connectionRequired_(other.connectionRequired_),
abstractionTypes_(),
bitsInLau_(other.bitsInLau_),
bitSteering_(other.bitSteering_),
bitSteeringAttributes_(other.bitSteeringAttributes_),
endianness_(other.endianness_),
parameters_(new QList<QSharedPointer<Parameter> >()),
master_(),
slave_(),
system_(other.system_),
monitor_(),
mirroredSlave_(),
defaultPos_(other.defaultPos_)
{
	foreach (QSharedPointer<AbstractionType> abstractionType, other.abstractionTypes_) {
		if (abstractionType) {
			QSharedPointer<AbstractionType> copy = QSharedPointer<AbstractionType>(
				new AbstractionType(*abstractionType.data()));
			abstractionTypes_.append(copy);
		}
	}

	foreach (QSharedPointer<Parameter> param, *other.parameters_) {
        QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*param.data()));
        parameters_->append(copy);
    }

	if (other.master_) {
		master_ = QSharedPointer<MasterInterface>(
			new MasterInterface(*other.master_.data()));
	}

	if (other.slave_) {
		slave_ = QSharedPointer<SlaveInterface>(
			new SlaveInterface(*other.slave_.data()));
	}

	if (other.monitor_) {
		monitor_ = QSharedPointer<MonitorInterface>(
			new MonitorInterface(*other.monitor_.data()));
	}

	if (other.mirroredSlave_) {
		mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(
			new MirroredSlaveInterface(*other.mirroredSlave_));
	}
}

BusInterface & BusInterface::operator=( const BusInterface &other )
{
	if (this != &other)
    {	
		NameGroup::operator=(other);
		attributes_ = other.attributes_;
		busType_ = other.busType_;
		interfaceMode_ = other.interfaceMode_;
		connectionRequired_ = other.connectionRequired_;
		bitsInLau_ = other.bitsInLau_;
		bitSteering_ = other.bitSteering_;
		bitSteeringAttributes_ = other.bitSteeringAttributes_;
		endianness_ = other.endianness_;
		system_ = other.system_;
        defaultPos_ = other.defaultPos_;

		abstractionTypes_.clear();
		foreach (QSharedPointer<AbstractionType> abstractionType, other.abstractionTypes_)
		{
			if (abstractionType)
			{
				QSharedPointer<AbstractionType> copy = QSharedPointer<AbstractionType>(new AbstractionType(*abstractionType.data()));
				abstractionTypes_.append(copy);
			}
		}

        parameters_->clear();
        foreach (QSharedPointer<Parameter> param, *other.parameters_)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*param.data()));
            parameters_->append(copy);
        }

		if (other.master_) {
			master_ = QSharedPointer<MasterInterface>(
				new MasterInterface(*other.master_.data()));
		}
		else
			master_ = QSharedPointer<MasterInterface>();


		if (other.slave_) {
			slave_ = QSharedPointer<SlaveInterface>(
				new SlaveInterface(*other.slave_.data()));
		}
		else
			slave_ = QSharedPointer<SlaveInterface>();

		if (other.monitor_) {
			monitor_ = QSharedPointer<MonitorInterface>(
				new MonitorInterface(*other.monitor_.data()));
		}
		else
			monitor_ = QSharedPointer<MonitorInterface>();

		if (other.mirroredSlave_) {
			mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(
				new MirroredSlaveInterface(*other.mirroredSlave_));
		}
		else
			mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>();

	}
	return *this;
}

// the destructor
BusInterface::~BusInterface() {
	abstractionTypes_.clear();
	bitSteeringAttributes_.clear();
	parameters_->clear();
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

	return true;
}

General::BitSteering BusInterface::getBitSteering() const {
	return bitSteering_;
}

General::InterfaceMode BusInterface::getInterfaceMode() const {
	return interfaceMode_;
}

void BusInterface::setPortMaps(
        QList<QSharedPointer<PortMap> > const& portMaps) {
	// delete old port maps stored
	abstractionTypes_.first()->portMaps_.clear();

	// save new port maps
       abstractionTypes_.first()->portMaps_ = portMaps;
}

void BusInterface::setAbstractionTypes(
	QList<QSharedPointer<BusInterface::AbstractionType> > const& abstractionTypes) {
		// delete old port maps stored
		abstractionTypes_.clear();

		// save new port maps
		abstractionTypes_ = abstractionTypes;
}

void BusInterface::setConnectionRequired(bool connectionRequired) {
	connectionRequired_ = connectionRequired;
}

void BusInterface::setEndianness(General::Endianness endianness) {
	endianness_ = endianness;
}

unsigned int BusInterface::getBitsInLau() const {
	return bitsInLau_;
}

bool BusInterface::getConnectionRequired() const {
	return connectionRequired_;
}

void BusInterface::setBitsInLau(unsigned int bitsInLau) {
	bitsInLau_ = bitsInLau;
}

QSharedPointer<QList<QSharedPointer<Parameter> > > BusInterface::getParameters() const
{
	return parameters_;
}

QList<QSharedPointer<PortMap> >& BusInterface::getPortMaps() {
	return abstractionTypes_.first()->portMaps_;
}

QList<QSharedPointer<BusInterface::AbstractionType> >& BusInterface::getAbstractionTypes() {
	return abstractionTypes_;
}

void BusInterface::setBitSteeringAttributes(
		QMap<QString, QString> &bitSteeringAttributes) {
	bitSteeringAttributes_.clear();
	bitSteeringAttributes_ = bitSteeringAttributes;
}

VLNV BusInterface::getBusType() const {
	return busType_;
}


const QMap<QString, QString>& BusInterface::getBitSteeringAttributes() {
	return bitSteeringAttributes_;
}

General::Endianness BusInterface::getEndianness() const {
	return endianness_;
}

void BusInterface::setInterfaceMode(General::InterfaceMode interfaceMode) {
	interfaceMode_ = interfaceMode;

	// clear the elements that dont belong to the selected interface mode
	switch (interfaceMode_) {
		case General::MASTER: {
			monitor_.clear();
			slave_.clear();
			system_.clear();
			mirroredSlave_.clear();
			if (!master_)
				master_ = QSharedPointer<MasterInterface>(new MasterInterface());
			return;
							  }
		case General::SLAVE: {
			monitor_.clear();
			master_.clear();
			system_.clear();
			mirroredSlave_.clear();
			if (!slave_)
				slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface());
			return;
							 }
		case General::SYSTEM: {
			monitor_.clear();
			slave_.clear();
			master_.clear();
			mirroredSlave_.clear();
			system_ = "default";
			return;
							  }
		case General::MIRROREDSLAVE: {
			monitor_.clear();
			slave_.clear();
			master_.clear();
			system_.clear();
			if (!mirroredSlave_)
				mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(
				new MirroredSlaveInterface());
			return;
									 }
		case General::MIRROREDMASTER: {
			monitor_.clear();
			slave_.clear();
			system_.clear();
			mirroredSlave_.clear();
			if (!master_)
				master_ = QSharedPointer<MasterInterface>(new MasterInterface()); 
			return;
									  }
		case General::MIRROREDSYSTEM: {
			monitor_.clear();
			slave_.clear();
			master_.clear();
			mirroredSlave_.clear();
			system_ = "default";
			return;
									  }
		case General::MONITOR: {
			slave_.clear();
			master_.clear();
			system_.clear();
			mirroredSlave_.clear();
			if (!monitor_)
				monitor_ = QSharedPointer<MonitorInterface>(new MonitorInterface());
			return;
							   }
		default: {
			monitor_.clear();
			slave_.clear();
			master_.clear();
			system_.clear();
			mirroredSlave_.clear();
			return;
				 }
	}
}

void BusInterface::setBusType(const VLNV& busType) {

	busType_ = busType;
}

void BusInterface::setBitSteering(General::BitSteering bitSteering) {
	bitSteering_ = bitSteering;
}

const QSharedPointer<MasterInterface> BusInterface::getMaster() const {
	return master_;
}

QSharedPointer<MasterInterface> BusInterface::getMaster() {
	return master_;
}

void BusInterface::setMaster( QSharedPointer<MasterInterface> master ) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	//  save new instance
	master_ = master;
	return;
}

const QSharedPointer<SlaveInterface> BusInterface::getSlave() const {
	return slave_;
}

QSharedPointer<SlaveInterface> BusInterface::getSlave() {
	return slave_;
}

void BusInterface::setSlave( QSharedPointer<SlaveInterface> slave ) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	// change interface type and save new instance
	interfaceMode_ = General::SLAVE;
	slave_ = slave;
	return;
}

QString BusInterface::getSystem() const {
	return system_;
}

void BusInterface::setSystem(const QString& system) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	// save new instance
	system_ = system;
	return;
}

const QSharedPointer<BusInterface::MonitorInterface> BusInterface::getMonitor() const {
	return monitor_;
}

QSharedPointer<BusInterface::MonitorInterface> BusInterface::getMonitor() {
	return monitor_;
}

void BusInterface::setMonitor( QSharedPointer<BusInterface::MonitorInterface> monitor ) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	// change interface type and save new instance
	interfaceMode_ = General::MONITOR;
	monitor_ = monitor;
	return;
}

const QSharedPointer<MirroredSlaveInterface> BusInterface::getMirroredSlave() const {
	return mirroredSlave_;
}

QSharedPointer<MirroredSlaveInterface> BusInterface::getMirroredSlave() {
	return mirroredSlave_;
}

void BusInterface::setMirroredSlave( QSharedPointer<MirroredSlaveInterface> mirroredSlave ) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	// change interface type and save new instance
	interfaceMode_ = General::MIRROREDSLAVE;
	mirroredSlave_ = mirroredSlave;
	return;
}

bool BusInterface::hasBridge() const {

    if (!slave_)
        return false;
	// does the slave interface have bridge defined
	return slave_->hasBridge();
}

const QMap<QString, QString>& BusInterface::getAttributes() const {
	return attributes_;
}

void BusInterface::setAttributes( const QMap<QString, QString>& attributes ) {
	attributes_ = attributes;
}

QStringList BusInterface::getPhysicalPortNames() const {
	// add each physical port name to the list
	QStringList list;
	foreach (QSharedPointer<PortMap> portMap, abstractionTypes_.first()->portMaps_)
	{
		list.append(portMap->getPhysicalPort()->name_);
	}
	return list;
}


QStringList BusInterface::getLogicalPortNames() const {
	// add each logical port name to the list
	QStringList list;
	foreach (QSharedPointer<PortMap> portMap, abstractionTypes_.first()->portMaps_)
	{
		list.append(portMap->getLogicalPort()->name_);
	}
	return list;
}



bool BusInterface::hasLogicalPort( const QString& logicalPortName ) const {

	foreach (QSharedPointer<PortMap> portMap, abstractionTypes_.first()->portMaps_)
	{
		// if the port map contains the searched logical port.
		if (portMap->getLogicalPort())
		{
			return true;
		}
	}

	// if no logical port with given name was found.
	return false;
}


bool BusInterface::hasPhysicalPort( const QString& physicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, abstractionTypes_.first()->portMaps_)
	{
		// if the port map contains the searched logical port.
		if (portMap->getPhysicalPort())
		{
			return true;
		}
	}

	// if no logical port with given name was found.
	return false;
}

QString BusInterface::getLogicalPortName( const QString& physicalPortName ) const
{
	foreach (QSharedPointer<PortMap> portMap, abstractionTypes_.first()->portMaps_)
	{
		// if the port map contains the searched physical port.
		if (portMap->getPhysicalPort()->name_.compare(physicalPortName) == 0)
		{
			return portMap->getLogicalPort()->name_;
		}
	}

	// if no physical port with given name was found.
	return QString();
}

//-----------------------------------------------------------------------------
// Function: setMCAPIPortID()
//-----------------------------------------------------------------------------
void BusInterface::setMCAPIPortID(int portID)
{
    // Try to replace the old one if it already exists.
    foreach (QSharedPointer<Parameter> param, *parameters_)
    {
        if (param->name() == "kts_port_id")
        {
            param->setValue(QString::number(portID));
            return;
        }
    }
    
    // Otherwise create a new parameter.
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

QString BusInterface::getMemoryMapRef() const {

	// only slave interfaces refer to memory maps
	if (interfaceMode_ != General::SLAVE) {
		return QString();
	}
	// if there is no slave element defined
	else if (!slave_) {
		return QString();
	}
	// return the slave's memory map reference
	else {
		return slave_->getMemoryMapRef();
	}
}

QString BusInterface::getAddressSpaceRef() const {

	// only master and mirrored master refer to address spaces
	if (interfaceMode_ == General::MASTER ||
		interfaceMode_ == General::MIRROREDMASTER) {

			// if there is no master defined
			if (!master_) {
				return QString();
			}
			else {
				return master_->getAddressSpaceRef();
			}
	}
	// interface mode is other than master or mirrored master
	else {
		return QString();
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterface::setDefaultPos()
//-----------------------------------------------------------------------------
void BusInterface::setDefaultPos(QPointF const& pos)
{
    defaultPos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: BusInterface::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF const& BusInterface::getDefaultPos() const
{
    return defaultPos_;
}
