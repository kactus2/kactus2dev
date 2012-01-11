/* 
 *
 *  Created on: 18.8.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"
#include "portabstraction.h"
#include "wireabstraction.h"
#include "transactionalabstraction.h"

#include "../exceptions/write_error.h"

#include <QDomNode>
#include <QString>
#include <QSharedPointer>
#include <QObject>

// the constructor
PortAbstraction::PortAbstraction(QDomNode& portNode): 
logicalName_(QString()),
displayName_(QString()), 
description_(QString()), 
transactional_(), 
wire_(),
portType_(General::WIRE) {

	for (int i = 0; i < portNode.childNodes().count(); ++i) {
		QDomNode tempNode = portNode.childNodes().at(i);

		if (tempNode.isComment()) {
			continue;
		}

		if (tempNode.nodeName() == QString("spirit:logicalName")) {
			logicalName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:wire")) {
			portType_ = General::WIRE;
			wire_ = QSharedPointer<WireAbstraction>(
					new WireAbstraction(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:transactional")) {
			portType_ = General::TRANSACTIONAL;
			transactional_ = QSharedPointer<TransactionalAbstraction>(
					new TransactionalAbstraction(tempNode));
		}
	}
	return;
}

PortAbstraction::PortAbstraction( const PortAbstraction& other ):
logicalName_(other.logicalName_),
displayName_(other.displayName_),
description_(other.description_),
transactional_(),
wire_(),
portType_(other.portType_) {

	if (other.wire_) {
		wire_ = QSharedPointer<WireAbstraction>(
			new WireAbstraction(*other.wire_.data()));
	}
	
	if (other.transactional_) {
		transactional_ = QSharedPointer<TransactionalAbstraction>(
			new TransactionalAbstraction(*other.transactional_.data()));
	}
}

PortAbstraction::PortAbstraction():
logicalName_(),
displayName_(), 
description_(), 
transactional_(), 
wire_(),
portType_(General::WIRE) {
}

PortAbstraction& PortAbstraction::operator=( const PortAbstraction& other ) {
	if (this != &other) {
		logicalName_ = other.logicalName_;
		displayName_ = other.displayName_;
		description_ = other.description_;
		portType_ = other.portType_;

		if (other.wire_) {
			wire_ = QSharedPointer<WireAbstraction>(
				new WireAbstraction(*other.wire_.data()));
		}
		else
			wire_ = QSharedPointer<WireAbstraction>();

		if (other.transactional_) {
			transactional_ = QSharedPointer<TransactionalAbstraction>(
				new TransactionalAbstraction(*other.transactional_.data()));
		}
		else
			transactional_ = QSharedPointer<TransactionalAbstraction>();
	}
	return *this;
}

// the destructor
PortAbstraction::~PortAbstraction() {
}

void PortAbstraction::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:port");

	if (logicalName_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element spirit:logicalname is "
				"missing in spirit:port"));
	}
	else {
		writer.writeTextElement("spirit:logicalName", logicalName_);
	}

	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	if (wire_) {
		 wire_->write(writer);
	}
	else if (transactional_) {
		transactional_->write(writer);
	}
	else {
		throw Write_error(QObject::tr("spirit:port does not define the port "
				"type, no wire or transactional element"));
	}

	writer.writeEndElement(); // spirit:port
}

bool PortAbstraction::isValid( QStringList& errorList,
							  const QString& parentIdentifier ) const {

	if (logicalName_.isEmpty()) {
		errorList.append(QObject::tr("No logical name set for port within %1").arg(
			parentIdentifier));
		return false;
	}

	return true;
}

void PortAbstraction::setDescription(const QString& description) {
	description_ = description;
}

void PortAbstraction::setTransactional(TransactionalAbstraction *transactional) {
	if (wire_) {
		wire_.clear();
	}

	if (transactional_) {
		transactional_.clear();
	}
	transactional_ = QSharedPointer<TransactionalAbstraction>(transactional);
	portType_ = General::TRANSACTIONAL;
	return;
}

WireAbstraction *PortAbstraction::getWire() const {
	return wire_.data();
}

void PortAbstraction::setLogicalName(const QString& logicalName) {
	logicalName_ = logicalName;
}

TransactionalAbstraction *PortAbstraction::getTransactional() const {
	return transactional_.data();
}

QString PortAbstraction::getDescription() const {
	return description_;
}

void PortAbstraction::setWire(WireAbstraction *wire) {
	if (transactional_) {
		transactional_.clear();
	}

	if (wire_) {
		wire_.clear();
	}

	wire_ = QSharedPointer<WireAbstraction>(wire);
	portType_ = General::WIRE;
}

QString PortAbstraction::getDisplayName() const {
	return displayName_;
}

QString PortAbstraction::getLogicalName() const {
	return logicalName_;
}

void PortAbstraction::setDisplayName(const QString& displayName) {
	displayName_ = displayName;
}

General::PortType PortAbstraction::getPortType() const {
	return portType_;
}

bool PortAbstraction::hasMaster() const {
	if (!wire_)
		return false;
	return wire_->hasMaster();
}

bool PortAbstraction::hasSlave() const {
	if (!wire_)
		return false;
	return wire_->hasSlave();
}

bool PortAbstraction::hasSystem() const {
	if (!wire_)
		return false;
	return wire_->hasSystem();
}

WireAbstraction::WirePort* PortAbstraction::getMaster() const {

	if (!wire_)
		return NULL;
	return wire_->getOnMaster();
}

WireAbstraction::WirePort* PortAbstraction::getSlave() const {

	if (!wire_)
		return NULL;
	return wire_->getOnSlave();
}

WireAbstraction::WirePort* PortAbstraction::getSystem() const {

	if (!wire_)
		return NULL;
	return wire_->getOnSystem();
}

bool PortAbstraction::isAddress() const {
	if (!wire_)
		return false;
	return wire_->isAddress();
}

bool PortAbstraction::isData() const {
	if (!wire_)
		return false;
	return wire_->isData();
}

bool PortAbstraction::isClock() const {
	if (!wire_)
		return false;
	return wire_->isClock();
}

bool PortAbstraction::isReset() const {
	if (!wire_)
		return false;
	return wire_->isReset();
}

int PortAbstraction::getDefaultValue() const {
	if (!wire_)
		return -1;
	return wire_->getDefaultValue();
}

bool PortAbstraction::getRequiresDriver() const {
	if (!wire_)
		return false;
	return wire_->getRequiresDriver();
}

General::DriverType PortAbstraction::getDriverType() const {
	if (!getRequiresDriver())
		return General::ANY;
	return wire_->getDriverType();
}

void PortAbstraction::setAsAddress() {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());

	wire_->setAsAddress();
}

void PortAbstraction::setAsData() {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());

	wire_->setAsData();
}

void PortAbstraction::setAsClock() {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());

	wire_->setAsClock();
}

void PortAbstraction::setAsReset() {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());

	wire_->setAsReset();
}

void PortAbstraction::clearQualifier() {
	if (!wire_)
		return;
	wire_->clearQualifier();
}

void PortAbstraction::setMasterWidth( const int width ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setMasterWidth(width);
}

void PortAbstraction::setSlaveWidth( const int width ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setSlaveWidth(width);
}

void PortAbstraction::setSystemWidth( const int width ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setSystemWidth(width);
}

void PortAbstraction::setAllWidths( const int width ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setAllWidths(width);
}

void PortAbstraction::setDefaultValue( const int defaultValue ) {
	portType_ = General::WIRE;
	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setDefaultValue(defaultValue);
}

void PortAbstraction::setMasterDirection( const General::Direction direction ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setMasterDirection(direction);
}

void PortAbstraction::setSlaveDirection( const General::Direction direction ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setSlaveDirection(direction);
}

void PortAbstraction::setSystemDirection( const General::Direction direction ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setSystemDirection(direction);
}

void PortAbstraction::setAllDirections( const General::Direction direction ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setAllDirections(direction);
}

void PortAbstraction::setMasterPresence( const General::Presence presence ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setMasterPresence(presence);
}

void PortAbstraction::setSlavePresence( const General::Presence presence ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setSlavePresence(presence);
}

void PortAbstraction::setSystemPresence( const General::Presence presence ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setSystemPresence(presence);
}

void PortAbstraction::setAllPresences( const General::Presence presence ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setAllPresences(presence);
}

void PortAbstraction::setDriverType( const General::DriverType type ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setDriverType(type);
}

void PortAbstraction::setRequiresDriver( bool requiresDriver ) {
	portType_ = General::WIRE;

	if (!wire_)
		wire_ = QSharedPointer<WireAbstraction>(new WireAbstraction());
	wire_->setRequiresDriver(requiresDriver);
}

General::Direction PortAbstraction::getDirection( General::InterfaceMode mode ) const {
	if (wire_)
		return wire_->getDirection(mode);

	return General::DIRECTION_INVALID;
}

bool PortAbstraction::isRequired( General::InterfaceMode mode ) const {
	if (wire_)
		return wire_->isRequired(mode);
	return false;
}

bool PortAbstraction::isOptional( General::InterfaceMode mode ) const {
	if (wire_)
		return wire_->isOptional(mode);
	return false;
}

bool PortAbstraction::isIllegal( General::InterfaceMode mode ) const {
	if (wire_)
		return wire_->isIllegal(mode);
	return false;
}

int PortAbstraction::getWidth( General::InterfaceMode mode ) const {
	if (wire_)
		return wire_->getWidth(mode);
	return -1;
}

bool PortAbstraction::hasDefaultValue() const {
	if (wire_ && wire_->getDefaultValue() >= 0) {
		return true;
	}
	else {
		return false;
	}
}
