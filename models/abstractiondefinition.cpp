/* 
 *
 *  Created on: 12.8.2010
 *      Author: Antti Kamppi
 */

#include "abstractiondefinition.h"
#include "portabstraction.h"
#include <LibraryManager/vlnv.h>
#include "librarycomponent.h"

#include <exceptions/parse_error.h>
#include <exceptions/write_error.h>

#include <QDomDocument>
#include <QDomNode>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMap>

// the constructor
AbstractionDefinition::AbstractionDefinition(QDomDocument& doc):
LibraryComponent(doc),
busType_(), 
ports_(), 
extends_(), 
attributes_() {

	LibraryComponent::vlnv_->setType(VLNV::ABSTRACTIONDEFINITION);

	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;
	}

	QDomNode absNode = doc.childNodes().item(i);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	for (int i = 0; i < absNode.childNodes().count(); ++i) {
		QDomNode tempNode = absNode.childNodes().at(i);

		if (tempNode.isComment()) {
			continue;
		}

		if (tempNode.nodeName() == QString("spirit:busType")) {
			busType_ = General::createVLNV(tempNode, VLNV::BUSDEFINITION);
		}

		else if (tempNode.nodeName() == QString("spirit:extends")) {
			extends_ = General::createVLNV(tempNode, VLNV::ABSTRACTIONDEFINITION);
		}

		else if (tempNode.nodeName() == QString("spirit:ports")) {

			// go through each port
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode portNode = tempNode.childNodes().at(j);

				// make sure the node is not comment
				if (!portNode.isComment()) {
					ports_.append(QSharedPointer<PortAbstraction>(
							new PortAbstraction(portNode)));
				}
			}
		}
	}

	// if mandatory fields is missing
	if (!busType_.isValid()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:busType "
				"missing in spirit:abstractionDefinition"));
	}
}

AbstractionDefinition::AbstractionDefinition( const AbstractionDefinition& other ):
LibraryComponent(other),
busType_(other.busType_), 
ports_(),
extends_(other.extends_),
attributes_(other.attributes_) {

	foreach (QSharedPointer<PortAbstraction> portAbs, other.ports_) {
		
		if (portAbs) {
			QSharedPointer<PortAbstraction> copy = QSharedPointer<PortAbstraction>(
				new PortAbstraction(*portAbs.data()));
			ports_.append(copy);
		}
	}
}

AbstractionDefinition::AbstractionDefinition():
LibraryComponent(),
busType_(),
ports_(), 
extends_(), 
attributes_() {
}

AbstractionDefinition& AbstractionDefinition::operator=( const AbstractionDefinition& other ) {
	if (this != &other) {
		LibraryComponent::operator=(other);

		busType_ = other.busType_;
		extends_ = other.extends_;
		attributes_ = other.attributes_;

		ports_.clear();
		foreach (QSharedPointer<PortAbstraction> portAbs, other.ports_) {

			if (portAbs) {
				QSharedPointer<PortAbstraction> copy = QSharedPointer<PortAbstraction>(
					new PortAbstraction(*portAbs.data()));
				ports_.append(copy);
			}
		}
	}
	return *this;
}

// the destructor
AbstractionDefinition::~AbstractionDefinition() {
	ports_.clear();
}

QSharedPointer<LibraryComponent> AbstractionDefinition::clone() {
	return QSharedPointer<LibraryComponent>(new AbstractionDefinition(*this));
}

void AbstractionDefinition::write(QFile& file) {
	// create a writer instance and set it to operate on the given file
	QXmlStreamWriter writer(&file);

	// writer automatically adds whitespaces to make document more readable
	writer.setAutoFormatting(true);
	// writer uses one tab as indentation
	writer.setAutoFormattingIndent(-1);

	// call the base class implementation to write the top comment and
	// vlvn info. It also starts the <spirit:busDefinition> element
	LibraryComponent::write(writer);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// write the attributes for the spirit:designConfiguration element
	General::writeAttributes(writer, attributes_);

	// call base class to write the VLNV info
	LibraryComponent::writeVLNV(writer);

	if (!LibraryComponent::description_.isEmpty()) {
		writer.writeTextElement("spirit:description",
			LibraryComponent::description_);
	}

	// if mandatory reference to a busDefinition is missing
	if (!busType_.isValid()) {
		throw Write_error(QObject::tr("Mandatory element spirit:busType is "
				"missing in spirit:abstractionDefinition"));
	}
	else {
		writer.writeEmptyElement("spirit:busType");
		General::writeVLNVAttributes(writer, &busType_);
	}

	// if extension is defined
	if (extends_.isValid()) {
		writer.writeEmptyElement("spirit:extends");
		General::writeVLNVAttributes(writer, &extends_);
	}

	if (!ports_.isEmpty()) {
		writer.writeStartElement("spirit:ports");

		for (int i = 0; i < ports_.size(); ++i) {
			ports_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:ports
	}

	writer.writeEndElement(); // spirit:abstractionDefinition
	writer.writeEndDocument();
	return;

}

// get the attributes
const QMap<QString, QString>& AbstractionDefinition::getAttributes() {
	return attributes_;
}

// set the attributes
void AbstractionDefinition::setAttributes(
		const QMap<QString, QString>& attributes) {
	// clear old attributes
	attributes_.clear();

	attributes_ = attributes;
}

VLNV AbstractionDefinition::getExtends() const {
	return extends_;
}

VLNV AbstractionDefinition::getBusType() const {
	return busType_;
}

void AbstractionDefinition::setPorts(
		QList<QSharedPointer<PortAbstraction> >& ports) {
	// delete the old ports
	ports_.clear();

	// save the new ports
	ports_ = ports;
}

void AbstractionDefinition::setBusType(const VLNV& busType) {
	busType_ = busType;
	busType_.setType(VLNV::BUSDEFINITION);
}

QList<QSharedPointer<PortAbstraction> >&
AbstractionDefinition::getPorts() {
	return ports_;
}

void AbstractionDefinition::setExtends(const VLNV& extends) {
	extends_ = extends;
}

// abstractionDefinition does not have any dependency files, it is pure
// IP-Xact element with only VLNV type dependencies
const QStringList AbstractionDefinition::getDependentFiles() const {
	return QStringList();
}

const QList<VLNV> AbstractionDefinition::getDependentVLNVs() const {
	QList<VLNV> vlnvList;

	// append the busType element representing the busDefinition of this
	// abstraction definition
	if (busType_.isValid()) {
		vlnvList.append(busType_);
	}

	// append the VLNV representing the abstractionDefinition that is
	// extended by this abstractionDefinition
	if (extends_.isValid()) {
		vlnvList.append(extends_);
	}

	return vlnvList;
}

QStringList AbstractionDefinition::getPortNames() const {

	QStringList portNames;

	// go through each port and add it's logical name to the list.
	foreach (QSharedPointer<PortAbstraction> port, ports_) {
		portNames.append(port->getLogicalName());
	}
	return portNames;
}

QStringList AbstractionDefinition::getPortNames( General::InterfaceMode mode ) const {
	QStringList portNames;

	// go through each port
	foreach (QSharedPointer<PortAbstraction> port, ports_) {

		// if signal is required or optional in given mode
		if (port->isRequired(mode) || port->isOptional(mode))
			portNames.append(port->getLogicalName());
	}
	return portNames;
}


General::Direction AbstractionDefinition::getPortDirection( const QString& portName, 
														   General::InterfaceMode mode ) const {
	// search all ports 
	foreach (QSharedPointer<PortAbstraction> port, ports_) {

		// if the named port was found
		if (port && port->getLogicalName() == portName) {
			return port->getDirection(mode);
		}
	}

	// if no port with given name was found
	return General::DIRECTION_INVALID;
}

bool AbstractionDefinition::isRequired( const QString& portName, General::InterfaceMode mode ) const {

	// search all ports
	foreach (QSharedPointer<PortAbstraction> port, ports_) {
		if (port->getLogicalName() == portName) {
			return port->isRequired(mode);
		}
	}
	return false;
}

bool AbstractionDefinition::isOptional( const QString& portName, General::InterfaceMode mode ) const {
	// search all ports
	foreach (QSharedPointer<PortAbstraction> port, ports_) {
		if (port->getLogicalName() == portName) {
			return port->isOptional(mode);
		}
	}
	return false;
}

bool AbstractionDefinition::isIllegal( const QString& portName, General::InterfaceMode mode ) const {
	// search all ports
	foreach (QSharedPointer<PortAbstraction> port, ports_) {
		if (port->getLogicalName() == portName) {
			return port->isIllegal(mode);
		}
	}
	return false;
}

bool AbstractionDefinition::hasPort( const QString& portName, General::InterfaceMode mode ) const {
	return isRequired(portName, mode) || isOptional(portName, mode);
}

int AbstractionDefinition::getPortSize( const QString& port, General::InterfaceMode mode ) const {
	// search all ports
	foreach (QSharedPointer<PortAbstraction> portAbs, ports_) {
		if (portAbs->getLogicalName() == port) {
			return portAbs->getWidth(mode);
		}
	}
	return -1;
}

void AbstractionDefinition::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::ABSTRACTIONDEFINITION);
}

int AbstractionDefinition::getDefaultValue( const QString& portName ) const {
	// search all ports
	foreach (QSharedPointer<PortAbstraction> port, ports_) {
		// if the port is the searched one
		if (port->getLogicalName() == portName) {
			return port->getDefaultValue();
		}
	}
	// if port was not found
	return -1;
}

bool AbstractionDefinition::hasDefaultValue( const QString& portName ) const {
	// search all ports
	foreach (QSharedPointer<PortAbstraction> port, ports_) {
		// if the port is the searched one
		if (port->getLogicalName() == portName) {
			return port->hasDefaultValue();
		}
	}
	// if port was not found
	return false;
}

