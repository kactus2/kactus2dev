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

#include <exceptions/parse_error.h>
#include <exceptions/write_error.h>

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

	// monitor can not be connected to another monitor so its an error
	if (interfaceMode_ == General::MONITOR) {
		throw Parse_error(QObject::tr("Invalid interfaceMode specified in "
				"spirit:monitor"));
	}

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
						interfaceMode_(General::MODE_UNDEFINED),
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

			// if one of the other elements are already defined
			if (slave_ || monitor_ || !system_.isNull()) {
				throw Parse_error(QObject::tr("More than one interfaceMode "
						"defined in spirit:busInterface"));
			}
			master_ = QSharedPointer<MasterInterface>(new MasterInterface(
					tempNode));
		}
		// slave
		else if (children.at(i).nodeName() == QString("spirit:slave")) {
			interfaceMode_ = General::SLAVE;
			QDomNode tempNode = children.at(i);

			// if one of the other elements are already defined
			if (master_ || monitor_ || !system_.isNull()) {
				throw Parse_error(QObject::tr("More than one interfaceMode "
						"defined in spirit:busInterface"));
			}
			slave_ = QSharedPointer<SlaveInterface>(new SlaveInterface(
					tempNode));
		}
		// mirroredMaster
		else if (children.at(i).nodeName() ==
				QString("spirit:mirroredMaster")) {
			interfaceMode_ = General::MIRROREDMASTER;
			QDomNode tempNode = children.at(i);

			// if one of the other elements are already defined
			if (slave_ || monitor_ || !system_.isNull()) {
				throw Parse_error(QObject::tr("More than one interfaceMode "
						"defined in spirit:busInterface"));
			}
			master_ = QSharedPointer<MasterInterface>(new MasterInterface(
					tempNode));
		}
		// mirroredSlave
		else if (children.at(i).nodeName() ==
				QString("spirit:mirroredSlave")) {
			interfaceMode_ = General::MIRROREDSLAVE;
			QDomNode tempNode = children.at(i);

			// if one of the other elements are already defined
			if (master_ || monitor_ || !system_.isNull()) {
				throw Parse_error(QObject::tr("More than one interfaceMode "
						"defined in spirit:busInterface"));
			}
			mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(
					new MirroredSlaveInterface(tempNode));
		}
		// system
		else if (children.at(i).nodeName() == QString("spirit:system")) {
			interfaceMode_ = General::SYSTEM;

			// if one of the other elements are already defined
			if (slave_ || monitor_ || master_) {
				throw Parse_error(QObject::tr("More than one interfaceMode "
						"defined in spirit:busInterface"));
			}

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

			// if one of the other elements are already defined
			if (slave_ || monitor_ || master_) {
				throw Parse_error(QObject::tr("More than one interfaceMode "
						"defined in spirit:busInterface"));
			}

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

			// if one of the other elements are already defined
			if (slave_ || !system_.isNull() || master_) {
				throw Parse_error(QObject::tr("More than one interfaceMode "
						"defined in spirit:busInterface"));
			}

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
	if (!busType_.isValid()) {
		throw Parse_error(QString("Mandatory element spirit:busType missing"
				" in spirit:busInterface"));
	}

	if (nameGroup_.name_.isNull()) {
		throw Parse_error(QString("Mandatory element spirit:name missing"
				" in spirit:busInterface"));
	}
	return;
}

BusInterface::BusInterface(): nameGroup_(),
		attributes_(),
		busType_(), abstractionType_(),
        interfaceMode_(General::MODE_UNDEFINED),
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

	// if mandatory element name is empty
	if (nameGroup_.name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element name missing in "
				"spirit:busInterface"));
	}
	else {
		writer.writeTextElement("spirit:name", nameGroup_.name_);
	}

	if (!nameGroup_.displayName_.isEmpty())
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);

	if (!nameGroup_.description_.isEmpty())
		writer.writeTextElement("spirit:description", nameGroup_.description_);

	// if mandatory element busType is missing
	if (!busType_.isValid()) {
		throw Write_error(QObject::tr("Mandatory element busType missing in "
				"spirit:busInterface"));
	}
	// if bus type is found its written
	else {
		writer.writeEmptyElement("spirit:busType");
		General::writeVLNVAttributes(writer, &busType_);
	}

	// if optional abstraction type is found
	if (abstractionType_.isValid()) {
		writer.writeEmptyElement("spirit:abstractionType");
		General::writeVLNVAttributes(writer, &abstractionType_);
	}

	// write the interface mode
	switch (interfaceMode_) {
	case General::MASTER: {
		if (master_) {
			writer.writeStartElement("spirit:master");
			master_->write(writer);
			writer.writeEndElement(); // spirit:master
		}
		else {
			throw Write_error(QObject::tr("Interface mode is set to master "
					"but spirit:master element is not defined"));
		}
		break;
	}
	case General::SLAVE: {
		if (slave_) {
			slave_->write(writer);
		}
		else {
			throw Write_error(QObject::tr("Interface mode is set to slave "
					"but spirit:slave element is not defined"));
		}
		break;
	}
	case General::SYSTEM: {
		if (!system_.isEmpty()) {
			writer.writeStartElement("spirit:system");
			writer.writeTextElement("spirit:group", system_);
			writer.writeEndElement(); // spirit:system
		}
		else {
			throw Write_error(QObject::tr("Interface mode is set to system "
					"but spirit:system element is not defined"));
		}
		break;
	}
	case General::MIRROREDMASTER: {
		if (master_) {
			writer.writeStartElement("spirit:mirroredMaster");
			master_->write(writer);
			writer.writeEndElement(); // spirit:mirroredMaster
		}
		else {
			throw Write_error(QObject::tr("Interface mode is set to master "
					"but spirit:master element is not defined"));
		}
		break;
	}
	case General::MIRROREDSLAVE: {
		if (mirroredSlave_) {
			mirroredSlave_->write(writer);
		}
		else {
			throw Write_error(QObject::tr("Interface mode is set to "
					"mirroredSlave but spirit:mirroredSlave element is not "
					"defined"));
		}
		break;
	}
	case General::MIRROREDSYSTEM: {
		if (!system_.isEmpty()) {
			writer.writeStartElement("spirit:mirroredSystem");
			writer.writeTextElement("spirit:group", system_);
			writer.writeEndElement(); // spirit:system
		}
		else {
			throw Write_error(QObject::tr("Interface mode is set to "
					"mirroredSystem but spirit:system element is not defined"));
		}
		break;
	}
	case General::MONITOR: {
		if (monitor_) {
			writer.writeStartElement("spirit:monitor");

			// monitor can't be connected to another monitor
			if (monitor_->interfaceMode_ == General::MONITOR) {
				throw Write_error(QObject::tr("InterfaceMode of a monitor can"
						" not be monitor"));
			}
			else {
				writer.writeAttribute("spirit:interfaceMode",
						General::interfaceMode2Str(monitor_->interfaceMode_));
			}

			// if mandatory group is defined
			if (!monitor_->group_.isEmpty()) {
				writer.writeTextElement("spirit:group", monitor_->group_);
			}
			writer.writeEndElement(); // spirit:monitor
		}
		break;
	}
	default: {
		throw Write_error(QObject::tr("Interface mode was not of supported"
				" type"));
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

			// if logical name of the port is missing
			if (portMaps_.at(i)->logicalPort_.isEmpty()) {
				throw Write_error(QObject::tr("Mandatory element name missing"
						" in spirit:logicalPort"));
			}
			else {
				writer.writeStartElement("spirit:logicalPort");
				writer.writeTextElement("spirit:name",
						portMaps_.at(i)->logicalPort_);

				// if a logical vector is defined
				if (portMaps_.value(i)->logicalVector_) {
					portMaps_.value(i)->logicalVector_->write(writer);
				}
				writer.writeEndElement(); // spirit:logicalPort
			}

			// if physical name of the port is missing
			if (portMaps_.at(i)->physicalPort_.isEmpty()) {
				throw Write_error(QObject::tr("Mandatory element name missing"
						" in spirit:physicalPort"));
			}
			else {
				writer.writeStartElement("spirit:physicalPort");
				writer.writeTextElement("spirit:name",
						portMaps_.at(i)->physicalPort_);

				// if a physical vector is defined
				if (portMaps_.value(i)->physicalVector_) {
					portMaps_.value(i)->physicalVector_->write(writer);
				}

				writer.writeEndElement(); // spirit:physicalPort
			}

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
		QList<QSharedPointer<General::PortMap> > &portMaps) {
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

void BusInterface::setMonitor(MonitorInterface* monitor) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	// change interface type and save new instance
	interfaceMode_ = General::MONITOR;
	monitor_ = QSharedPointer<MonitorInterface>(monitor);
	return;
}

void BusInterface::setMaster(MasterInterface *master) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	//  save new instance
	master_ = QSharedPointer<MasterInterface>(master);
	return;
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

MasterInterface *BusInterface::getMaster() const {
	return master_.data();
}

void BusInterface::setSlave(SlaveInterface *slave) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	// change interface type and save new instance
	interfaceMode_ = General::SLAVE;
	slave_ = QSharedPointer<SlaveInterface>(slave);
	return;
}

SlaveInterface *BusInterface::getSlave() const {
	return slave_.data();
}

QString BusInterface::getSystem() const {
	return system_;
}

BusInterface::MonitorInterface *BusInterface::getMonitor() const {
	return monitor_.data();
}

void BusInterface::setMirroredSlave(MirroredSlaveInterface *mirroredSlave) {
	// remove all old interfaceTypes
	monitor_.clear();
	slave_.clear();
	master_.clear();
	system_.clear();
	mirroredSlave_.clear();

	// change interface type and save new instance
	interfaceMode_ = General::MIRROREDSLAVE;
	mirroredSlave_ = QSharedPointer<MirroredSlaveInterface>(mirroredSlave);
	return;
}

MirroredSlaveInterface *BusInterface::getMirroredSlave() const {
	return mirroredSlave_.data();
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
