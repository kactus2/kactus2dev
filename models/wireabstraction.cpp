/* 
 *
 *  Created on: 19.8.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"
#include "wireabstraction.h"

#include <QSharedPointer>
#include <QDomNode>
#include <QDomNamedNodeMap>

WireAbstraction::WirePort::WirePort(QDomNode& wireNode):
presence_(General::OPTIONAL), 
width_(-1),
direction_(General::DIRECTION_INVALID) {
	for (int i = 0; i < wireNode.childNodes().count(); ++i) {
		QDomNode tempNode = wireNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:presence")) {
			presence_ = General::str2Presence(tempNode.childNodes().at(0).
					nodeValue(), General::OPTIONAL);
		}

		else if (tempNode.nodeName() == QString("spirit:width")) {
			bool ok = false;
			width_ = tempNode.childNodes().at(0).nodeValue().toInt(&ok);

			// if the conversion failed the default value is restored
			if (!ok) {
				width_ = -1;
			}
		}

		else if (tempNode.nodeName() == QString("spirit:direction")) {
			direction_ = General::str2Direction(tempNode.childNodes().at(0).
					nodeValue(), General::DIRECTION_PHANTOM);
		}

		// TODO add parsing of spirit:modeConstraints
	}
	return;
}

WireAbstraction::WirePort::WirePort():
presence_(General::OPTIONAL), 
width_(-1),
direction_(General::DIRECTION_INVALID) {
}

WireAbstraction::WirePort::WirePort( const WirePort& other ):
presence_(other.presence_),
width_(other.width_),
direction_(other.direction_) {
}

WireAbstraction::WirePort& WireAbstraction::WirePort::operator=( const WirePort& other ) {
	if (this != &other) {
		presence_ = other.presence_;
		width_ = other.width_;
		direction_ = other.direction_;
	}
	return *this;
}

WireAbstraction::WireAbstraction(QDomNode& wireNode): 
qualifier_(),
defaultValue_(-1),
requiresDriver_(false),
driverType_(General::ANY),
onMaster_(), 
onSlave_(),
onSystem_() {

	for (int i = 0; i < wireNode.childNodes().count(); ++i) {
		QDomNode tempNode = wireNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:qualifier")) {
			qualifier_ = QSharedPointer<General::Qualifier>(new General::Qualifier(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:onMaster")) {
			onMaster_ = QSharedPointer<WirePort>(new WirePort(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:onSlave")) {
			onSlave_ = QSharedPointer<WirePort>(new WirePort(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:onSystem")) {
			onSystem_ = QSharedPointer<WirePort>(new WirePort(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:defaultValue")) {
			bool ok = false;
			defaultValue_ = tempNode.childNodes().at(0).nodeValue().toInt(&ok);

			// if the conversion was unsuccessful the default value is assigned
			if (!ok) {
				defaultValue_ = -1;
			}
		}

		else if (tempNode.nodeName() == QString("spirit:requiresDriver")) {
			QString requiresDriver = tempNode.childNodes().at(0).nodeValue();
			requiresDriver_ = General::str2Bool(requiresDriver, false);

			QDomNamedNodeMap AttributeMap = tempNode.attributes();
			driverType_ = General::str2DriverType(AttributeMap.namedItem(
					QString("spirit:driverType")).nodeValue(), General::ANY);
		}
	}
	return;
}

WireAbstraction::WireAbstraction( const WireAbstraction& other ):
qualifier_(),
defaultValue_(other.defaultValue_),
requiresDriver_(other.requiresDriver_),
driverType_(other.driverType_),
onMaster_(),
onSlave_(), 
onSystem_() {

	if (other.qualifier_) {
		qualifier_ = QSharedPointer<General::Qualifier>(
			new General::Qualifier(*other.qualifier_.data()));
	}

	if (other.onMaster_) {
		onMaster_ = QSharedPointer<WirePort>(
			new WirePort(*other.onMaster_.data()));
	}

	if (other.onSlave_) {
		onSlave_ = QSharedPointer<WirePort>(
			new WirePort(*other.onSlave_.data()));
	}

	if (other.onSystem_) {
		onSystem_ = QSharedPointer<WirePort>(
			new WirePort(*other.onSystem_.data()));
	}
}

WireAbstraction::WireAbstraction():
qualifier_(),
defaultValue_(-1),
requiresDriver_(false),
driverType_(General::ANY),
onMaster_(), 
onSlave_(),
onSystem_() {
}

WireAbstraction& WireAbstraction::operator=( const WireAbstraction& other ) {
	if (this != &other) {
		defaultValue_ = other.defaultValue_;
		requiresDriver_ = other.requiresDriver_;
		driverType_ = other.driverType_;

		if (other.qualifier_) {
			qualifier_ = QSharedPointer<General::Qualifier>(
				new General::Qualifier(*other.qualifier_.data()));
		}
		else
			qualifier_ = QSharedPointer<General::Qualifier>();

		if (other.onMaster_) {
			onMaster_ = QSharedPointer<WirePort>(
				new WirePort(*other.onMaster_.data()));
		}
		else
			onMaster_ = QSharedPointer<WirePort>();

		if (other.onSlave_) {
			onSlave_ = QSharedPointer<WirePort>(
				new WirePort(*other.onSlave_.data()));
		}
		else
			onSlave_ = QSharedPointer<WirePort>();

		if (other.onSystem_) {
			onSystem_ = QSharedPointer<WirePort>(
				new WirePort(*other.onSystem_.data()));
		}
		else
			onSystem_ = QSharedPointer<WirePort>();
	}
	return *this;
}

WireAbstraction::~WireAbstraction() {
}

void WireAbstraction::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:wire");

	if (qualifier_) {
		writer.writeStartElement("spirit:qualifier");

		writer.writeTextElement("spirit:isAddress",
				General::bool2Str(qualifier_->isAddress_));
		writer.writeTextElement("spirit:isData",
				General::bool2Str(qualifier_->isData_));
		writer.writeTextElement("spirit:isClock",
				General::bool2Str(qualifier_->isClock_));
		writer.writeTextElement("spirit:isReset",
				General::bool2Str(qualifier_->isReset_));

		writer.writeEndElement(); // spirit:qualifier
	}

	if (onMaster_) {
		writer.writeStartElement("spirit:onMaster");
		writeWirePort(writer, onMaster_.data());
		writer.writeEndElement(); // spirit:onMaster
	}

	if (onSlave_) {
		writer.writeStartElement("spirit:onSlave");
		writeWirePort(writer, onSlave_.data());
		writer.writeEndElement(); // spirit:onSlave
	}

	if (onSystem_) {
		writer.writeStartElement("spirit:onSystem");
		writeWirePort(writer, onSystem_.data());
		writer.writeEndElement(); // spirit::onSystem
	}

	// if default value is defined then use it
	if (defaultValue_ >= 0) {
		writer.writeTextElement("spirit:defaultValue",
				QString::number(defaultValue_));
	}
	// if default value was not defined then use requiresDriver
	else {
		writer.writeEmptyElement("spirit:requiresDriver");
		writer.writeAttribute("spirit:driverType",
				General::driverType2Str(driverType_));
		writer.writeCharacters(General::bool2Str(requiresDriver_));
	}

	writer.writeEndElement(); // spirit:wire
}

void WireAbstraction::writeWirePort(QXmlStreamWriter& writer,
		WirePort* port) {
	writer.writeTextElement("spirit:presence",
			General::presence2Str(port->presence_));

	if (port->width_ > 0) {
		writer.writeTextElement("spirit:width",
				QString::number(port->width_));
	}

	if (port->direction_ != General::DIRECTION_INVALID) {
		writer.writeTextElement("spirit:direction",
				General::direction2Str(port->direction_));
	}


	// TODO add writing of spirit:modeConstraints
}

void WireAbstraction::setQualifier(General::Qualifier* qualifier) {
	if (qualifier_) {
		qualifier_.clear();
	}
	qualifier_ = QSharedPointer<General::Qualifier>(qualifier);
}

WireAbstraction::WirePort *WireAbstraction::getOnSlave() const {
	return onSlave_.data();
}

void WireAbstraction::setDefaultValue(int defaultValue) {
	// port can not have both requiresDriver and default value
	requiresDriver_ = false;
	driverType_ = General::ANY;

	defaultValue_ = defaultValue;
}

void WireAbstraction::setOnSlave(WirePort *onSlave) {
	if (onSlave_) {
		onSlave_.clear();
	}
	onSlave_ = QSharedPointer<WirePort>(onSlave);
}

General::DriverType WireAbstraction::getDriverType() const {
	return driverType_;
}

General::Qualifier *WireAbstraction::getQualifier() const {
	return qualifier_.data();
}

void WireAbstraction::setOnMaster(WirePort *onMaster) {
	if (onMaster_) {
		onMaster_.clear();
	}
	onMaster_ = QSharedPointer<WirePort>(onMaster);
}

void WireAbstraction::setDriverType(General::DriverType driverType) {
	// default value and driver are mutually exclusive
	defaultValue_ = -1;
	
	driverType_ = driverType;
	requiresDriver_ = true;
}

int WireAbstraction::getDefaultValue() const {
	return defaultValue_;
}

WireAbstraction::WirePort *WireAbstraction::getOnMaster() const {
	return onMaster_.data();
}

void WireAbstraction::setRequiresDriver(bool requiresDriver) {
	// if driver is required then default value can not be defined
	if (requiresDriver) {
		defaultValue_ = -1;
	}
	requiresDriver_ = requiresDriver;
}

bool WireAbstraction::getRequiresDriver() const {
	return requiresDriver_;
}

WireAbstraction::WirePort* WireAbstraction::getOnSystem() const {
	return onSystem_.data();
}

bool WireAbstraction::hasMaster() const {

	if (onMaster_)
		return true;
	else
		return false;
}

bool WireAbstraction::hasSlave() const {

	if (onSlave_)
		return true;
	else
		return false;
}

bool WireAbstraction::hasSystem() const {

	if (onSystem_)
		return true;
	else
		return false;
}

bool WireAbstraction::isAddress() const {
	if (!qualifier_)
		return false;
	return qualifier_->isAddress_;
}

bool WireAbstraction::isData() const {
	if (!qualifier_)
		return false;
	return qualifier_->isData_;
}

bool WireAbstraction::isClock() const {
	if (!qualifier_)
		return false;
	return qualifier_->isClock_;
}

bool WireAbstraction::isReset() const {
	if (!qualifier_)
		return false;
	return qualifier_->isReset_;
}

void WireAbstraction::setAsAddress() {

	if (!qualifier_)
		qualifier_ = QSharedPointer<General::Qualifier>(new General::Qualifier());

	qualifier_->isAddress_ = true;
	qualifier_->isData_ = false;
	qualifier_->isClock_ = false;
	qualifier_->isReset_ = false;
}

void WireAbstraction::setAsData() {

	if (!qualifier_)
		qualifier_ = QSharedPointer<General::Qualifier>(new General::Qualifier());
	
	qualifier_->isAddress_ = false;
	qualifier_->isData_ = true;
	qualifier_->isClock_ = false;
	qualifier_->isReset_ = false;
}

void WireAbstraction::setAsClock() {

	if (!qualifier_)
		qualifier_ = QSharedPointer<General::Qualifier>(new General::Qualifier());

	qualifier_->isAddress_ = false;
	qualifier_->isData_ = false;
	qualifier_->isClock_ = true;
	qualifier_->isReset_ = false;
}

void WireAbstraction::setAsReset() {

	if (!qualifier_)
		qualifier_ = QSharedPointer<General::Qualifier>(new General::Qualifier());

	qualifier_->isAddress_ = false;
	qualifier_->isData_ = false;
	qualifier_->isClock_ = false;
	qualifier_->isReset_ = true;
}

void WireAbstraction::clearQualifier() {
	qualifier_.clear();
}

void WireAbstraction::setMasterWidth( const int width ) {

	if (!onMaster_) 
		onMaster_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onMaster_->width_ = width;
}

void WireAbstraction::setSlaveWidth( const int width ) {
	if (!onSlave_)
		onSlave_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onSlave_->width_ = width;
}

void WireAbstraction::setSystemWidth( const int width ) {
	if (!onSystem_)
		onSystem_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onSystem_->width_ = width;
}

void WireAbstraction::setAllWidths( const int width ) {
	setMasterWidth(width);
	setSlaveWidth(width);
	setSystemWidth(width);
}

void WireAbstraction::setMasterDirection( const General::Direction direction ) {
	if (!onMaster_) 
		onMaster_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onMaster_->direction_ = direction;
}

void WireAbstraction::setSlaveDirection( const General::Direction direction ) {
	if (!onSlave_)
		onSlave_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onSlave_->direction_ = direction;
}

void WireAbstraction::setSystemDirection( const General::Direction direction ) {
	if (!onSystem_)
		onSystem_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onSystem_->direction_ = direction;
}

void WireAbstraction::setAllDirections( const General::Direction direction ) {
	setMasterDirection(direction);
	setSlaveDirection(direction);
	setSystemDirection(direction);
}

void WireAbstraction::setMasterPresence( const General::Presence presence ) {
	if (!onMaster_) 
		onMaster_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onMaster_->presence_ = presence;
}

void WireAbstraction::setSlavePresence( const General::Presence presence ) {
	if (!onSlave_)
		onSlave_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onSlave_->presence_ = presence;
}

void WireAbstraction::setSystemPresence( const General::Presence presence ) {
	if (!onSystem_)
		onSystem_ = QSharedPointer<WireAbstraction::WirePort>(
		new WireAbstraction::WirePort());
	onSystem_->presence_ = presence;
}

void WireAbstraction::setAllPresences( const General::Presence presence ) {
	setMasterPresence(presence);
	setSlavePresence(presence);
	setSystemPresence(presence);
}

General::Direction WireAbstraction::getDirection( General::InterfaceMode mode ) const {
	switch (mode) {
			case General::MASTER: {
				if (onMaster_)
					return onMaster_->direction_;
				else 
					return General::DIRECTION_INVALID;
								  }
			case General::MIRROREDMASTER: {
				if (onMaster_)
					return General::convert2Mirrored(onMaster_->direction_);
				else
					return General::DIRECTION_INVALID;
										  }
			case General::SLAVE: {
				if (onSlave_)
					return onSlave_->direction_;
				else
					return General::DIRECTION_INVALID;
								 }
			case General::MIRROREDSLAVE: {
				if (onSlave_)
					return General::convert2Mirrored(onSlave_->direction_);
				else
					return General::DIRECTION_INVALID;
										 }
			case General::SYSTEM: {
				if (onSystem_)
					return onSystem_->direction_;
				else
					return General::DIRECTION_INVALID;
								  }
			case General::MIRROREDSYSTEM: {
				if (onSystem_)
					return General::convert2Mirrored(onSystem_->direction_);
				else
					return General::DIRECTION_INVALID;
										  }
			default:
				return General::DIRECTION_INVALID;

	}
}

bool WireAbstraction::isRequired( General::InterfaceMode mode ) const {
	switch (mode) {
		case General::MASTER:
		case General::MIRROREDMASTER: {
			if (onMaster_ && onMaster_->presence_ == General::REQUIRED)
				return true;
			return false;
									  }
		case General::SLAVE:
		case General::MIRROREDSLAVE: {
			if (onSlave_ && onSlave_->presence_ == General::REQUIRED)
				return true;
			return false;
									 }
		case General::SYSTEM: 
		case General::MIRROREDSYSTEM: {
			if (onSystem_ && onSystem_->presence_ == General::REQUIRED)
				return true;
			return false;
									  }
									  // mode == monitor
		default:
			return false;
	}
}

bool WireAbstraction::isOptional( General::InterfaceMode mode ) const {
	switch (mode) {
		case General::MASTER:
		case General::MIRROREDMASTER: {
			if (onMaster_ && onMaster_->presence_ == General::OPTIONAL)
				return true;
			return false;
									  }
		case General::SLAVE:
		case General::MIRROREDSLAVE: {
			if (onSlave_ && onSlave_->presence_ == General::OPTIONAL)
				return true;
			return false;
									 }
		case General::SYSTEM: 
		case General::MIRROREDSYSTEM: {
			if (onSystem_ && onSystem_->presence_ == General::OPTIONAL)
				return true;
			return false;
									  }
									  // mode == monitor
		default:
			return false;
	}
}

bool WireAbstraction::isIllegal( General::InterfaceMode mode ) const {
	switch (mode) {
		case General::MASTER:
		case General::MIRROREDMASTER: {
			if (onMaster_ && onMaster_->presence_ == General::ILLEGAL)
				return true;
			return false;
									  }
		case General::SLAVE:
		case General::MIRROREDSLAVE: {
			if (onSlave_ && onSlave_->presence_ == General::ILLEGAL)
				return true;
			return false;
									 }
		case General::SYSTEM: 
		case General::MIRROREDSYSTEM: {
			if (onSystem_ && onSystem_->presence_ == General::ILLEGAL)
				return true;
			return false;
									  }
									  // mode == monitor
		default:
			return false;
	}
}

int WireAbstraction::getWidth( General::InterfaceMode mode ) const {
	switch (mode) {
		case General::MASTER:
		case General::MIRROREDMASTER: {
			if (onMaster_)
				return onMaster_->width_;
			return -1;
									  }
		case General::SLAVE:
		case General::MIRROREDSLAVE: {
			if (onSlave_)
				return onSlave_->width_;
			return -1;
									 }
		case General::SYSTEM: 
		case General::MIRROREDSYSTEM: {
			if (onSystem_)
				return onSystem_->width_;
			return -1;
									  }
									  // mode == monitor
		default:
			return -1;
	}
}
