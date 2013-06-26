/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "businterface.h"
#include "generaldeclarations.h"
#include <LibraryManager/vlnv.h>
#include "masterinterface.h"
#include "slaveinterface.h"
#include "mirroredslaveinterface.h"

#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QXmlStreamWriter>

#include <QDebug>

const int DEFAULT_BITS_IN_LAU = 8;

// struct constructor
BusInterface::MonitorInterface::MonitorInterface(QDomNode& monitorNode):
interfaceMode_(General::MONITOR),
group_() {
	// get the interfaceMode attribute
	QDomNamedNodeMap attributeMap = monitorNode.attributes();
	QString interfaceMode = attributeMap.namedItem(QString(
			"spirit:interfaceMode")).nodeValue();
	interfaceMode = General::removeWhiteSpace(interfaceMode);

	interfaceMode_ = General::str2Interfacemode(interfaceMode,
			General::MONITOR);

	// get the monitor group element
	for (int i = 0; i < monitorNode.childNodes().count(); ++i) {
		if (monitorNode.childNodes().at(i).nodeName() == QString(
				"spirit:group")) {
			group_ = monitorNode.childNodes().at(i).childNodes().at(0).
					nodeValue();
			group_ = General::removeWhiteSpace(group_);
		}
	}
}

BusInterface::MonitorInterface::MonitorInterface():
interfaceMode_(General::MONITOR),
group_() {
}

// class constructor
BusInterface::BusInterface(QDomNode &busInterface):
nameGroup_(busInterface),
attributes_(),
busType_(), abstractionType_(),
interfaceMode_(General::INTERFACE_MODE_COUNT),
connectionRequired_(false), portMaps_(),
bitsInLau_(DEFAULT_BITS_IN_LAU),
bitSteering_(General::BITSTEERING_UNSPECIFIED),
bitSteeringAttributes_(),
endianness_(General::LITTLE),
parameters_(), master_(), slave_(), system_(),
monitor_() {

	// get the attributes for the bus interface
	QDomNamedNodeMap attributeMap = busInterface.attributes();
	for (int j = 0; j < attributeMap.size(); ++j) {
		QString name = attributeMap.item(j).nodeName();
		QString value = attributeMap.item(j).nodeValue();
		attributes_[name] = value;
	}

	// go through all the child nodes and call appropriate constructors
	QDomNodeList children = busInterface.childNodes();

	for (int i = 0; i < children.size(); ++i) {

		// if node is a comment then skip it
		if (children.at(i).isComment())
			continue;

		// get bus type
		if (children.at(i).nodeName() == QString("spirit:busType")) {

			QDomNode busTypeNode = children.at(i);

			// create the vlnv and set a pointer for it
			busType_ = General::createVLNV(busTypeNode, VLNV::BUSDEFINITION);
		}

		// get abstraction type
		else if (children.at(i).nodeName() ==
				QString("spirit:abstractionType")) {
			QDomNode abstractionNode = children.at(i);

			// create the vlnv and set a pointer for it
			abstractionType_ = General::createVLNV(abstractionNode,
							VLNV::ABSTRACTIONDEFINITION);
		}

		// get interface mode:

		// master
		else if (children.at(i).nodeName() == QString("spirit:master")) {
			interfaceMode_ = General::MASTER;
			QDomNode tempNode = children.at(i);

			master_ = QSharedPointer<MasterInterface>(new MasterInterface(
					tempNode));
		}
		// slave
		else if (children.at(i).nodeName() == QString("spirit:slave")) {
			interfaceMode_ = General::SLAVE;
			QDomNode tempNode = children.at(i);

			slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface(
					tempNode));
		}
		// mirroredMaster
		else if (children.at(i).nodeName() ==
				QString("spirit:mirroredMaster")) {
			interfaceMode_ = General::MIRROREDMASTER;
			QDomNode tempNode = children.at(i);

			master_ = QSharedPointer<MasterInterface>(new MasterInterface(
					tempNode));
		}
		// mirroredSlave
		else if (children.at(i).nodeName() ==
				QString("spirit:mirroredSlave")) {
			interfaceMode_ = General::MIRROREDSLAVE;
			QDomNode tempNode = children.at(i);

			mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(
					new MirroredSlaveInterface(tempNode));
		}
		// system
		else if (children.at(i).nodeName() == QString("spirit:system")) {
			interfaceMode_ = General::SYSTEM;

			QDomNode tempNode = children.at(i);
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:group")) {
					system_ = tempNode.childNodes().at(j).childNodes().at(0).
							nodeValue();
					system_ = General::removeWhiteSpace(system_);
				}
			}
		}
		// mirroredSystem
		else if (children.at(i).nodeName() ==
				QString("spirit:mirroredSystem")) {
			interfaceMode_ = General::MIRROREDSYSTEM;

			QDomNode tempNode = children.at(i);
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:group")) {
					system_ = tempNode.childNodes().at(j).childNodes().at(0).
							nodeValue();
					system_ = General::removeWhiteSpace(system_);
				}
			}
		}
		// monitor
		else if (children.at(i).nodeName() == QString("spirit:monitor")) {
			interfaceMode_ = General::MONITOR;

			QDomNode tempNode = children.at(i);
			monitor_ = QSharedPointer<BusInterface::MonitorInterface>(
					new BusInterface::MonitorInterface(tempNode));
		}
		// end of interface mode

		// get connectionRequired
		else if (children.at(i).nodeName() ==
				QString("spirit:connectionRequired")) {

			// if true is found then it is set but false is default value
			QString connection = children.at(i).childNodes().at(0).nodeValue();
			connectionRequired_ = General::str2Bool(connection, false);
		}

		// get port maps
		else if (children.at(i).nodeName() == QString("spirit:portMaps")) {

			// call constructors for all port map items
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				if (children.at(i).childNodes().at(j).isComment())
					continue;

				// call constructor and give the child node representing the
				// single choice as parameter
				QDomNode tempNode = children.at(i).childNodes().at(j);

				General::PortMap *temp = new General::PortMap(tempNode);
				portMaps_.append(QSharedPointer<General::PortMap>(temp));
			}
		}

		// get bits in lau
		else if (children.at(i).nodeName() == QString("spirit:bitsInLau")) {
			bitsInLau_ = children.at(i).childNodes().at(0).nodeValue().toInt();
		}

		// get bit steering
		else if (children.at(i).nodeName() == QString("spirit:bitSteering")) {
			QString bitSteering = children.at(i).childNodes().at(0).
					nodeValue();
			bitSteering_ = General::str2BitSteering(bitSteering);

			// get the attributes for bitSteering element
			QDomNode tempNode = children.at(i);
			General::parseAttributes(tempNode, bitSteeringAttributes_);
		}

		// get endianness
		else if (children.at(i).nodeName() == QString("spirit:endianness")) {
			endianness_ = General::str2Endianness(children.at(i).childNodes().
					at(0).nodeValue(), General::LITTLE);
		}

		// get parameters
		else if (children.at(i).nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				QDomNode parameterNode = children.at(i).childNodes().at(j);
				Parameter *temp = new Parameter(parameterNode);
				parameters_.append(QSharedPointer<Parameter>(temp));
			}
		}
	}

	// if mandatory elements are missing
// 	if (!busType_.isValid()) {
// 		throw Parse_error(QString("Mandatory element spirit:busType missing"
// 				" in spirit:busInterface"));
// 	}
// 
// 	if (nameGroup_.name_.isNull()) {
// 		throw Parse_error(QString("Mandatory element spirit:name missing"
// 				" in spirit:busInterface"));
// 	}
	return;
}

BusInterface::BusInterface(): nameGroup_(),
		attributes_(),
		busType_(), abstractionType_(),
        interfaceMode_(General::INTERFACE_MODE_COUNT),
		connectionRequired_(false), portMaps_(),
		bitsInLau_(DEFAULT_BITS_IN_LAU),
		bitSteering_(General::BITSTEERING_UNSPECIFIED),
		bitSteeringAttributes_(),
		endianness_(General::LITTLE),
        parameters_(), master_(QSharedPointer<MasterInterface>(new MasterInterface)),
        slave_(), system_(),
        monitor_() {
}

BusInterface::BusInterface( const BusInterface &other ):
nameGroup_(other.nameGroup_),
attributes_(other.attributes_),
busType_(other.busType_),
abstractionType_(other.abstractionType_),
interfaceMode_(other.interfaceMode_),
connectionRequired_(other.connectionRequired_),
portMaps_(),
bitsInLau_(other.bitsInLau_),
bitSteering_(other.bitSteering_),
bitSteeringAttributes_(other.bitSteeringAttributes_),
endianness_(other.endianness_),
parameters_(),
master_(),
slave_(),
system_(other.system_),
monitor_(),
mirroredSlave_() {

	foreach (QSharedPointer<General::PortMap> portMap, other.portMaps_) {
		if (portMap) {
			QSharedPointer<General::PortMap> copy = QSharedPointer<General::PortMap>(
				new General::PortMap(*portMap.data()));
			portMaps_.append(copy);
		}
	}

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
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

BusInterface & BusInterface::operator=( const BusInterface &other ) {
	if (this != &other) {
	
		nameGroup_ = other.nameGroup_;
		attributes_ = other.attributes_;
		busType_ = other.busType_;
		abstractionType_ = other.abstractionType_;
		interfaceMode_ = other.interfaceMode_;
		connectionRequired_ = other.connectionRequired_;
		bitsInLau_ = other.bitsInLau_;
		bitSteering_ = other.bitSteering_;
		bitSteeringAttributes_ = other.bitSteeringAttributes_;
		endianness_ = other.endianness_;
		system_ = other.system_;

		portMaps_.clear();
		foreach (QSharedPointer<General::PortMap> portMap, other.portMaps_) {
			if (portMap) {
				QSharedPointer<General::PortMap> copy = QSharedPointer<General::PortMap>(
					new General::PortMap(*portMap.data()));
				portMaps_.append(copy);
			}
		}

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
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
	portMaps_.clear();
	bitSteeringAttributes_.clear();
	parameters_.clear();
	master_.clear();
	slave_.clear();
	monitor_.clear();
}

void BusInterface::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:busInterface");

	if (!attributes_.isEmpty())
		General::writeAttributes(writer, attributes_);

	writer.writeTextElement("spirit:name", nameGroup_.name_);

	if (!nameGroup_.displayName_.isEmpty())
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);

	if (!nameGroup_.description_.isEmpty())
		writer.writeTextElement("spirit:description", nameGroup_.description_);

	writer.writeEmptyElement("spirit:busType");
	General::writeVLNVAttributes(writer, &busType_);

	writer.writeEmptyElement("spirit:abstractionType");
	General::writeVLNVAttributes(writer, &abstractionType_);

	// write the interface mode
	switch (interfaceMode_) {
	case General::MASTER: {
		if (master_) {
			writer.writeStartElement("spirit:master");
			master_->write(writer);
			writer.writeEndElement(); // spirit:master
		}
		break;
	}
	case General::SLAVE: {
		if (slave_) {
			slave_->write(writer);
		}
		break;
	}
	case General::SYSTEM: {
		if (!system_.isEmpty()) {
			writer.writeStartElement("spirit:system");
			writer.writeTextElement("spirit:group", system_);
			writer.writeEndElement(); // spirit:system
		}
		break;
	}
	case General::MIRROREDMASTER: {
		if (master_) {
			writer.writeStartElement("spirit:mirroredMaster");
			master_->write(writer);
			writer.writeEndElement(); // spirit:mirroredMaster
		}
		break;
	}
	case General::MIRROREDSLAVE: {
		if (mirroredSlave_) {
			mirroredSlave_->write(writer);
		}
		break;
	}
	case General::MIRROREDSYSTEM: {
		if (!system_.isEmpty()) {
			writer.writeStartElement("spirit:mirroredSystem");
			writer.writeTextElement("spirit:group", system_);
			writer.writeEndElement(); // spirit:system
		}
		break;
	}
	case General::MONITOR: {
		if (monitor_) {
			writer.writeStartElement("spirit:monitor");

			writer.writeAttribute("spirit:interfaceMode",
						          General::interfaceMode2Str(monitor_->interfaceMode_));

			// if mandatory group is defined
			if (!monitor_->group_.isEmpty()) {
				writer.writeTextElement("spirit:group", monitor_->group_);
			}
			writer.writeEndElement(); // spirit:monitor
		}
		break;
	}
	}

	writer.writeTextElement("spirit:connectionRequired",
			General::bool2Str(connectionRequired_));

	// if portMaps are defined
	if (portMaps_.size() != 0) {
		writer.writeStartElement("spirit:portMaps");

		// write each port map
		for (int i = 0; i < portMaps_.size(); ++i) {
			writer.writeStartElement("spirit:portMap");

			writer.writeStartElement("spirit:logicalPort");
			writer.writeTextElement("spirit:name", portMaps_.at(i)->logicalPort_);

            // if a logical vector is defined
            if (portMaps_.value(i)->logicalVector_) {
                portMaps_.value(i)->logicalVector_->write(writer);
            }
            writer.writeEndElement(); // spirit:logicalPort

            writer.writeStartElement("spirit:physicalPort");
            writer.writeTextElement("spirit:name",
                portMaps_.at(i)->physicalPort_);

            // if a physical vector is defined
            if (portMaps_.value(i)->physicalVector_) {
                portMaps_.value(i)->physicalVector_->write(writer);
            }

            writer.writeEndElement(); // spirit:physicalPort

			writer.writeEndElement(); // spirit:portMap
		}

		writer.writeEndElement(); // spirit:portMaps
	} // portMaps have been written now

	// write the bits in lau always
	writer.writeTextElement("spirit:bitsInLau", QString::number(bitsInLau_));

	// if the bitSteering has been defined
	if (bitSteering_ != General::BITSTEERING_UNSPECIFIED) {

		// start the spirit:bitSteering tag
		writer.writeStartElement("spirit:bitSteering");

		// write the attributes for the element
		General::writeAttributes(writer, bitSteeringAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(General::bitSteering2Str(bitSteering_));
		writer.writeEndElement(); // spirit:bitSteering
	}

	// always write the endianness part
	writer.writeTextElement("spirit:endianness",
			General::endianness2Str(endianness_));

	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// write each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}
		writer.writeEndElement(); // spirit:parameters
	}

	writer.writeEndElement(); // spirit:busInterface
}

bool BusInterface::isValid( const QList<General::PortBounds>& physicalPorts, 
						   QStringList& errorList,
						   const QString& parentIdentifier ) const {

	bool valid = true;
	const QString thisIdentifier(QObject::tr("bus interface %1").arg(nameGroup_.name_));

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for bus interface within %1").arg(
			parentIdentifier));
		valid = false;
	}

	// check the bus type validity
	if (!busType_.isValid(errorList, QObject::tr("bus type in %1").arg(thisIdentifier))) {
		valid = false;
	}

    if (!abstractionType_.isEmpty() &&
        !abstractionType_.isValid(errorList, QObject::tr("abstraction type in %1").arg(thisIdentifier))) {
        valid = false;
    }

	switch (interfaceMode_) {
		case General::MASTER: 
		case General::MIRROREDMASTER: {
			if (!master_) {
				errorList.append(QObject::tr("The interface mode of %1 is "
					"master/mirrored master but no element for it has been defined.").arg(
					thisIdentifier));
				valid = false;
			}
			break;
									  }
		case General::SLAVE: {
			if (!slave_) {
				errorList.append(QObject::tr("The interface mode of %1 is "
					"slave but no element for it has been defined.").arg(
					thisIdentifier));
				valid = false;
			}
			break;
							 }
		case General::MIRROREDSLAVE: {
			if (!mirroredSlave_) {
				errorList.append(QObject::tr("The interface mode of %1 is "
					"mirrored slave but no element for it has been defined.").arg(
					thisIdentifier));
				valid = false;
			}
			break;
									 }
		case General::SYSTEM:
		case General::MIRROREDSYSTEM: {
			if (system_.isEmpty()) {
				errorList.append(QObject::tr("Interface mode is system/mirrored system"
					" but no group has been specified for %1 within %2").arg(
					thisIdentifier).arg(parentIdentifier));
				valid = false;
			}
			break;
									  }
		case General::MONITOR: {
			if (monitor_->interfaceMode_ == General::INTERFACE_MODE_COUNT) {
				errorList.append(QObject::tr("No interface mode set for monitor"
					" within %1").arg(thisIdentifier));
				valid = false;
			}
			break;
							   }
		// if the interface mode is invalid
		default: {
            errorList.append(QObject::tr("The interface mode of %1 is not specified.").arg(thisIdentifier));
			valid = false;
			break;
				 }
	}

	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		if (!portMap->isValid(physicalPorts, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, thisIdentifier)) {
			valid = false;
		}

	}

	return valid;
}

bool BusInterface::isValid( const QList<General::PortBounds>& physicalPorts ) const {
	
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	// check the bus type validity
	if (!busType_.isValid()) {
		return false;
	}

	switch (interfaceMode_) {
		case General::MASTER: 
		case General::MIRROREDMASTER: {
			if (!master_) {
				return false;
			}
			break;
									  }
		case General::SLAVE: {
			if (!slave_) {
				return false;
			}
			break;
							 }
		case General::MIRROREDSLAVE: {
			if (!mirroredSlave_) {
				return false;
			}
			break;
									 }
		case General::SYSTEM:
		case General::MIRROREDSYSTEM: {
			if (system_.isEmpty()) {
				return false;
			}
			break;
									  }
		case General::MONITOR: {
			if (monitor_->interfaceMode_ == General::INTERFACE_MODE_COUNT) {
				return false;
			}
			break;
							   }
		 // if the interface mode is invalid
		default: {
			return false;
				 }
	}

	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		if (!portMap->isValid(physicalPorts)) {
			return false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid()) {
			return false;
		}

	}
	return true;
}

General::BitSteering BusInterface::getBitSteering() const {
	return bitSteering_;
}

VLNV BusInterface::getAbstractionType() const {
	return abstractionType_;
}

General::InterfaceMode BusInterface::getInterfaceMode() const {
	return interfaceMode_;
}

void BusInterface::setPortMaps(
        QList<QSharedPointer<General::PortMap> > const& portMaps) {
	// delete old port maps stored
	portMaps_.clear();

	// save new port maps
        portMaps_ = portMaps;
}

void BusInterface::setConnectionRequired(bool connectionRequired) {
	connectionRequired_ = connectionRequired;
}

QString BusInterface::getName() const {
	return nameGroup_.name_;
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

QList<QSharedPointer<Parameter> >& BusInterface::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<Parameter> >& BusInterface::getParameters() const {
	return parameters_;
}

void BusInterface::setAbstractionType(const VLNV& abstractionType) {
	abstractionType_ = abstractionType;
}

void BusInterface::setParameters(
		QList<QSharedPointer<Parameter> > &parameters) {
	// delete old parameteres
	parameters_.clear();

	// save the new parameters
	parameters_ = parameters;
}

QList<QSharedPointer<General::PortMap> >& BusInterface::getPortMaps() {
	return portMaps_;
}

void BusInterface::setBitSteeringAttributes(
		QMap<QString, QString> &bitSteeringAttributes) {
	bitSteeringAttributes_.clear();
	bitSteeringAttributes_ = bitSteeringAttributes;
}

VLNV BusInterface::getBusType() const {
	return busType_;
}

void BusInterface::setName(const QString &name) {
	nameGroup_.name_ = name;
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

void BusInterface::setDisplayName( const QString& displayName ) {
	nameGroup_.displayName_ = displayName;
}

void BusInterface::setDescription( const QString& description ) {
	nameGroup_.description_ = description;
}

QString BusInterface::getDisplayName() const {
	return nameGroup_.displayName_;
}

QString BusInterface::getDescription() const {
	return nameGroup_.description_;
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
	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		list.append(portMap->physicalPort_);
	}
	return list;
}


QStringList BusInterface::getLogicalPortNames() const {
	// add each logical port name to the list
	QStringList list;
	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		list.append(portMap->logicalPort_);
	}
	return list;
}



bool BusInterface::hasLogicalPort( const QString& logicalPortName ) const {

	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {
		
		// if the port map contains the searched logical port.
		if (portMap->logicalPort_.compare(logicalPortName) == 0) {
			return true;
		}
	}

	// if no logical port with given name was found.
	return false;
}


bool BusInterface::hasPhysicalPort( const QString& physicalPortName ) const {
	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {

		// if the port map contains the searched logical port.
		if (portMap->physicalPort_.compare(physicalPortName) == 0) {
			return true;
		}
	}

	// if no logical port with given name was found.
	return false;
}

QString BusInterface::getLogicalPortName( const QString& physicalPortName ) const {
	foreach (QSharedPointer<General::PortMap> portMap, portMaps_) {

		// if the port map contains the searched physical port.
		if (portMap->physicalPort_.compare(physicalPortName) == 0) {
			return portMap->logicalPort_;
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
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (param->getName() == "kts_port_id")
        {
            param->setValue(QString::number(portID));
            return;
        }
    }

    // Otherwise create a new parameter.
    QSharedPointer<Parameter> param(new Parameter());
    param->setName("kts_port_id");
    param->setValue(QString::number(portID));
    parameters_.append(param);
}

//-----------------------------------------------------------------------------
// Function: getMCAPIPortID()
//-----------------------------------------------------------------------------
int BusInterface::getMCAPIPortID() const
{
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (param->getName() == "kts_port_id")
        {
            return param->getValue().toInt();
        }
    }

    return -1;
}

General::NameGroup& BusInterface::getNameGroup() {
	return nameGroup_;
}

const General::NameGroup& BusInterface::getNameGroup() const {
	return nameGroup_;
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
