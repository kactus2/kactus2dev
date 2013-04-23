/* 
 *
 *  Created on: 27.7.2010
 *      Author: Antti Kamppi
 */

#include "generaldeclarations.h"


#include <QDomNode>
#include <QString>
#include <QDomNodeList>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QMap>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamWriter>

#include <QDebug>

// struct constructor
General::Enumeration::Enumeration(QString &value, QString attText,
		QString attHelp): value_(value), attribute_text_(attText),
		attribute_help_(attHelp) {
	return;
}

General::Enumeration::Enumeration( const Enumeration& other ):
value_(other.value_), 
attribute_text_(other.attribute_text_),
attribute_help_(other.attribute_help_) {
}

General::Enumeration& General::Enumeration::operator=(const Enumeration& other) {
	if (this != &other) {
		value_ = other.value_;
		attribute_text_ = other.attribute_text_;
		attribute_help_ = other.attribute_help_;
	}
	return *this;
}

General::BitSteering General::str2BitSteering(const QString& str) {
	if (str == QString("on")) {
		return General::ON;
	}
	else if (str == QString("off")) {
		return General::OFF;
	}
	else {
		return General::BITSTEERING_UNSPECIFIED;
	}
}

QString General::bitSteering2Str(const General::BitSteering bitSteering) {
	switch (bitSteering) {
	case General::ON: {
		return QString("on");
	}
	case General::OFF: {
		return QString("off");
	}
	default: {
		return QString();
	}
	}
}

General::PortMap::PortMap(): 
logicalPort_(),
logicalVector_(QSharedPointer<Vector>(new Vector())), 
physicalPort_(), 
physicalVector_(QSharedPointer<Vector>(new Vector())) {
}

General::PortMap::PortMap(QDomNode &portMap): 
logicalPort_(),
logicalVector_(),
physicalPort_(), 
physicalVector_() {

	for (int i = 0; i < portMap.childNodes().count(); ++i) {

		QDomNode tempNode = portMap.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:logicalPort")) {

			// search childNodes for name and vector elements
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:name")) {

					// get the logical name and strip whitespace characters
					logicalPort_= tempNode.childNodes().at(j).childNodes().
							at(0).nodeValue();
					logicalPort_ = General::removeWhiteSpace(logicalPort_);
				}

				else if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:vector")) {
					QDomNode vectorNode = tempNode.childNodes().at(j);

					logicalVector_ = QSharedPointer<Vector>(
							new Vector(vectorNode));
				}
			}

		}
		else if (tempNode.nodeName() == QString("spirit:physicalPort")) {

			// search for a spirit:vector element
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:name")) {

					// get the physical name and strip the whitespace characters
					physicalPort_ = tempNode.childNodes().at(j).childNodes().
							at(0).nodeValue();
					physicalPort_ = General::removeWhiteSpace(physicalPort_);
				}

				else if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:vector")) {
					QDomNode vectorNode = tempNode.childNodes().at(j);

					physicalVector_ = QSharedPointer<Vector>(
							new Vector(vectorNode));
				}
			}
		}
	}
	return;
}

General::PortMap::PortMap( const PortMap& other ): 
logicalPort_(other.logicalPort_),
logicalVector_(),
physicalPort_(other.physicalPort_),
physicalVector_() {

	if (other.logicalVector_)
		logicalVector_ = QSharedPointer<Vector>(new Vector(*other.logicalVector_));

	if (other.physicalVector_)
		physicalVector_ = QSharedPointer<Vector>(new Vector(*other.physicalVector_));
}

General::PortMap& General::PortMap::operator=( const PortMap& other ) {
	if (this != &other) {
		logicalPort_ = other.logicalPort_;
		if (other.logicalVector_)
			logicalVector_ = QSharedPointer<Vector>(new Vector(*other.logicalVector_.data()));
		else
			logicalVector_ = QSharedPointer<Vector>();

		physicalPort_ = other.physicalPort_;
		if (other.physicalVector_)
			physicalVector_ = QSharedPointer<Vector>(new Vector(*other.physicalVector_.data()));
		else
			physicalVector_ = QSharedPointer<Vector>();
	}
	return *this;
}

bool General::PortMap::isValid( const QList<General::PortBounds>& physicalPorts, 
							   QStringList& errorList, 
							   const QString& parentIdentifier ) const {
	bool valid = true;

	if (physicalPort_.isEmpty()) {
		errorList.append(QObject::tr("No physical port specified for port map within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (logicalPort_.isEmpty()) {
		errorList.append(QObject::tr("No logical port specified for port map within %1").arg(
			parentIdentifier));
		valid = false;
	}

	int physSize = 1;
	int logSize = 1;

	if (physicalVector_) {
		if (!physicalVector_->isValid(errorList, 
			QObject::tr("port map within %1").arg(parentIdentifier))) {
				valid = false;
		}

		physSize = physicalVector_->getSize();
	}
	if (logicalVector_) {
		if (!logicalVector_->isValid(errorList,
			QObject::tr("port map within %1").arg(parentIdentifier))) {
				valid = false;
		}

		logSize = logicalVector_->getSize();
	}

	// if the sizes of the ports don't match
	if (physSize != logSize) {
		errorList.append(QObject::tr("The sizes of the vectors don't match in"
			" port map within %1").arg(parentIdentifier));
		valid = false;
	}

	// if there is a physical port specified
	if (!physicalPort_.isEmpty()) {
		
		bool foundPhysPort = false;
		foreach (General::PortBounds port, physicalPorts) {

			// if the referenced physical port was found
			if (port.portName_ == physicalPort_) {
				foundPhysPort = true;

				// calculate the size of the actual physical port.
				int actualPortSize = port.left_ - port.right_ + 1;

				// if the actual port size is smaller than the referenced vector in 
				// the port map
				if (actualPortSize < physSize) {
					errorList.append(QObject::tr("The port map within %1 is larger"
						" than the actual size of the port %2.").arg(
						parentIdentifier).arg(physicalPort_));
					valid = false;
				}

				break;
			}
		}
		// if the referenced port was not found within the component
		if (!foundPhysPort) {
			errorList.append(QObject::tr("The port map within %1 contained reference to"
				" physical port %2 which is not found in the component.").arg(
				parentIdentifier).arg(physicalPort_));
			valid = false;
		}
	}

	return valid;
}

bool General::PortMap::isValid( const QList<General::PortBounds>& physicalPorts ) const {

	if (physicalPort_.isEmpty()) {
		return false;
	}

	if (logicalPort_.isEmpty()) {
		return false;
	}

	int physSize = 1;
	int logSize = 1;

	if (physicalVector_) {
		if (!physicalVector_->isValid()) {
				return false;
		}

		physSize = physicalVector_->getSize();
	}
	if (logicalVector_) {
		if (!logicalVector_->isValid()) {
				return false;
		}

		logSize = logicalVector_->getSize();
	}

	// if the sizes of the ports don't match
	if (physSize != logSize) {
		return false;
	}

	// if there is a physical port specified
	if (!physicalPort_.isEmpty()) {

		bool foundPhysPort = false;
		foreach (General::PortBounds port, physicalPorts) {

			// if the referenced physical port was found
			if (port.portName_ == physicalPort_) {
				foundPhysPort = true;

				// calculate the size of the actual physical port.
				int actualPortSize = port.left_ - port.right_ + 1;

				// if the actual port size is smaller than the referenced vector in 
				// the port map
				if (actualPortSize < physSize) {
					return false;
				}

				break;
			}
		}
		// if the referenced port was not found within the component
		if (!foundPhysPort) {
			return false;
		}
	}
	return true;
}

General::PortBounds::PortBounds():
portName_(),
left_(0),
right_(0) {
}

General::PortBounds::PortBounds( const QString& portName ):
portName_(portName),
left_(0),
right_(0) {
}

General::PortBounds::PortBounds( const QString& portName, const int left, const int right ):
portName_(portName),
left_(left),
right_(right) {
}

General::PortBounds::PortBounds( const PortBounds& other ):
portName_(other.portName_),
left_(other.left_),
right_(other.right_) {
}

General::PortBounds& General::PortBounds::operator=( const PortBounds& other ) {
	if (&other != this) {
		portName_ = other.portName_;
		left_ = other.left_;
		right_ = other.right_;
	}
	return *this;
}

bool General::PortBounds::operator<( const PortBounds& other ) const {
	return portName_ < other.portName_;
}

bool General::PortBounds::operator==( const PortBounds& other ) const {
	return portName_ == other.portName_;
}

bool General::PortBounds::operator!=( const PortBounds& other ) const {
	return portName_ != other.portName_;
}

General::PortAlignment::PortAlignment():
port1Left_(-1),
port1Right_(-1),
port2Left_(-1),
port2Right_(-1),
invalidAlignment_(true) {
}

General::PortAlignment::PortAlignment( const PortAlignment& other ):
port1Left_(other.port1Left_),
port1Right_(other.port1Right_),
port2Left_(other.port2Left_),
port2Right_(other.port2Right_),
invalidAlignment_(other.invalidAlignment_) {
}

General::PortAlignment& General::PortAlignment::operator=( const PortAlignment& other ) {
	if (this != &other) {
		port1Left_ = other.port1Left_;
		port1Right_ = other.port1Right_;
		port2Left_ = other.port2Left_;
		port2Right_ = other.port2Right_;
		invalidAlignment_ = other.invalidAlignment_;
	}
	return *this;
}

General::PortAlignment General::calculatePortAlignment( 
	const PortMap* portMap1, 
	int phys1LeftBound, 
	int phys1RightBound, 
	const PortMap* portMap2,
	int phys2LeftBound, 
	int phys2RightBound ) {

	General::PortAlignment alignment;
	
	// if the port maps are for different logical ports
	if (portMap1->logicalPort_ != portMap2->logicalPort_) {
		return alignment;
	}

	// if port 1 is vectored on the port map
	if (portMap1->physicalVector_) {
		phys1LeftBound = portMap1->physicalVector_->getLeft();
		phys1RightBound = portMap1->physicalVector_->getRight();
	}

	// if port 2 is vectored on the port map
	if (portMap2->physicalVector_) {
		phys2LeftBound = portMap2->physicalVector_->getLeft();
		phys2RightBound = portMap2->physicalVector_->getRight();
	}

	// if both have vectored logical signals
	if (portMap1->logicalVector_ && portMap2->logicalVector_) {

		// if the vectored ports don't have any common bounds
		if (portMap1->logicalVector_->getRight() > portMap2->logicalVector_->getLeft() ||
			portMap1->logicalVector_->getLeft() < portMap2->logicalVector_->getRight()) {
				return alignment;
		}

		int logicalLeft = qMin(portMap1->logicalVector_->getLeft(), 
			portMap2->logicalVector_->getLeft());
		int logicalRight = qMax(portMap1->logicalVector_->getRight(),
			portMap2->logicalVector_->getRight());

		{
			// count how much the left bound of port 1 has to be adjusted down
			int downSize = abs(portMap1->logicalVector_->getLeft() - logicalLeft);
			// count how must the right bound of  port 1 has to be adjusted up
			int upSize = abs(logicalRight - portMap1->logicalVector_->getRight());

			alignment.port1Left_ = phys1LeftBound - downSize;
			alignment.port1Right_ = phys1RightBound + upSize;
		}
		{
			// count how much the left bound of port 2 has to be adjusted down
			int downSize = abs(portMap2->logicalVector_->getLeft() - logicalLeft);
			// count how must the right bound of  port 2 has to be adjusted up
			int upSize = abs(logicalRight - portMap2->logicalVector_->getRight());

			alignment.port2Left_ = phys2LeftBound - downSize;
			alignment.port2Right_ = phys2RightBound + upSize;
		}
	}
	// if port map1 has vectored logical signal
	else if (portMap1->logicalVector_ && !portMap2->logicalVector_) {

		// port 1 uses the original physical bounds
		alignment.port1Left_ = phys1LeftBound;
		alignment.port1Right_ = phys1RightBound;

		// port 2 uses the bounds of the logical port of port 1
		alignment.port2Left_ = portMap1->logicalVector_->getLeft();
		alignment.port2Right_ = portMap1->logicalVector_->getRight();
	}
	// if port map2 has vectored logical signal
	else if (!portMap1->logicalVector_ && portMap2->logicalVector_) {

		// port 1 uses the bounds of the logical port of port 2
		alignment.port1Left_ = portMap2->logicalVector_->getLeft();
		alignment.port1Right_ = portMap2->logicalVector_->getRight();

		// port 2 uses the original physical bounds
		alignment.port2Left_ = phys2LeftBound;
		alignment.port2Right_ = phys2RightBound;
	}
	// if neither has vectored logical signal
	else {

		// both ports use the original physical bounds
		alignment.port1Left_ = phys1LeftBound;
		alignment.port1Right_ = phys1RightBound;
		alignment.port2Left_ = phys2LeftBound;
		alignment.port2Right_ = phys2RightBound;
	}

	// check if the sizes of the ports match
	int port1Size = alignment.port1Left_ - alignment.port1Right_ + 1;
	int port2Size = alignment.port2Left_ - alignment.port2Right_ + 1;
	if (port1Size != port2Size) {
		alignment.invalidAlignment_ = true;
	}
	else {
		alignment.invalidAlignment_ = false;
	}
	return alignment;
}

QString General::toPhysString( const PortMap& portMap ) {
	QString str = portMap.physicalPort_;
	// if the physical port is vectored.
	if (portMap.physicalVector_) {
		str += portMap.physicalVector_->toString();
	}
	return str;
}

QString General::toLogicalString( const PortMap& portMap ) {
	QString str = portMap.logicalPort_;
	// if the logical port is vectored
	if (portMap.logicalVector_) {
		str += portMap.logicalVector_->toString();
	}
	return str;
}

QString General::port2String(const QString& portName, int leftBound, int rightBound) {
	QString str(portName);
	str += QString("[%1..%2]").arg(leftBound).arg(rightBound);
	return str;
}

QString General::bool2Str(bool value) {
	if (value) {
		return QString("true");
	}
	else {
		return QString("false");
	}
}

General::Endianness General::str2Endianness(QString str,
		General::Endianness defaultValue) {

	if (str == QString("big")) {
		return General::BIG;
	}
	else if (str == QString("little")) {
		return General::LITTLE;
	}
	else {
		return defaultValue;
	}
}

QString General::endianness2Str(const General::Endianness endianness) {
	switch (endianness) {
	case General::BIG: {
		return QString("big");
	}
	case General::LITTLE: {
		return QString("little");
	}
	default: { // this should never happen
		return QString();
	}
	}
}

General::Initiative General::str2Initiative(QString str,
		General::Initiative defaultValue) {

	// select the correct enum value that matches the string
	if (str == QString("requires")) {
		return General::REQUIRES;
	}
	else if (str == QString("provides")) {
		return General::PROVIDES;
	}
	else if (str == QString("both")) {
		return General::BOTH;
	}
	else if (str == QString("phantom")) {
		return General::INITIATIVE_PHANTOM;
	}
	else {
		return defaultValue;
	}
}

QString General::Initiative2Str(const General::Initiative initiative) {
	switch (initiative) {
	case General::REQUIRES: {
		return QString("requires");
	}
	case General::PROVIDES: {
		return QString("provides");
	}
	case General::BOTH: {
		return QString("both");
	}
	case General::INITIATIVE_PHANTOM: {
		return QString("phantom");
	}
	// if initiative = NONE
	default: {
		return QString();
	}
	}
}

General::Presence General::str2Presence(QString str,
		General::Presence defaultValue) {

	// identify the correct enum value
	if (str == QString("illegal")) {
		return General::ILLEGAL;
	}

	else if (str == QString("required")) {
		return General::REQUIRED;
	}

	else if (str == QString("optional")) {
		return General::OPTIONAL;
	}
	else {
		return defaultValue;
	}
}

QString General::presence2Str(General::Presence presence) {
	switch (presence) {
	case General::ILLEGAL: {
		return QString("illegal");
	}
	case General::REQUIRED: {
		return QString("required");
	}
	case General::OPTIONAL: {
		return QString("optional");
	}
	default: {
		return QString();
	}
	}
}

General::Direction General::str2Direction(QString str,
		General::Direction defaultValue) {

	// convert QString into Direction
	if (str == QString("in")) {
		return General::IN;
	}
	else if (str == QString("out")) {
		return General::OUT;
	}
	else if (str == QString("inout")) {
		return General::INOUT;
	}
	else if (str == QString("phantom")) {
		return General::DIRECTION_PHANTOM;
	}
	else {
		return defaultValue;
	}
}

QString General::direction2Str(const General::Direction direction) {
	switch (direction) {
	case General::IN: {
		return QString("in");
	}
	case General::OUT: {
		return QString("out");
	}
	case General::INOUT: {
		return QString("inout");
	}
	case General::DIRECTION_PHANTOM: {
		return QString("phantom");
	}
	// if DIRECTION_INVALID
	default: {
		return QString();
	}
	}
}

General::Direction General::convert2Mirrored(const Direction originalDirection ) {
	switch (originalDirection) {
		case General::IN: 
			return General::OUT;
		case General::OUT:
			return General::IN;
		case General::INOUT:
			return General::INOUT;
		case General::DIRECTION_PHANTOM:
			return General::DIRECTION_PHANTOM;
		default:
			return General::DIRECTION_INVALID;
	}
}

General::Access General::str2Access(QString str,
		General::Access defaultValue) {
	if (str == QString("read-write")) {
		return General::READ_WRITE;
	}
	else if (str == QString("read-only")) {
		return General::READ_ONLY;
	}
	else if (str ==	QString("write-only")) {
		return General::WRITE_ONLY;
	}
	else if (str ==	QString("read-writeOnce")) {
		return General::READ_WRITEONCE;
	}
	else if (str == QString("writeOnce")) {
		return General::WRITEONCE;
	}
	else {
		return defaultValue;
	}
}

QString General::access2Str(const General::Access access) {
	switch (access) {
	case General::READ_WRITE: {
		return QString("read-write");
	}
	case General::READ_ONLY: {
		return QString("read-only");
	}
	case General::WRITE_ONLY: {
		return QString("write-only");
	}
	case General::READ_WRITEONCE: {
		return QString("read-writeOnce");
	}
	case General::WRITEONCE: {
		return QString("writeOnce");
	}
	// if UNSPECIFIED_ACCESS
	default: {
		return QString();
	}
	}
}

General::Usage General::str2Usage(QString str,
		General::Usage defaultValue) {
	if (str == QString("memory")) {
		return General::MEMORY;
	}
	else if (str == QString("register")) {
		return General::REGISTER;
	}
	else if (str ==	QString("reserved")) {
		return General::RESERVED;
	}
	else {
		return defaultValue;
	}
}

QString General::usage2Str(const General::Usage usage) {
	switch (usage) {
	case General::MEMORY: {
		return QString("memory");
	}
	case General::REGISTER: {
		return QString("register");
	}
	case General::RESERVED: {
		return QString("reserved");
	}
	// if UNSPECIFIED_USAGE
	default: {
		return QString();
	}
	}
}

General::GroupSelectorOperator General::str2GroupSelector(QString str,
		General::GroupSelectorOperator defaultValue) {
	if (str == QString("and")) {
		return General::AND;
	}
	else if (str == QString("or")) {
		return General::OR;
	}
	else {
		return defaultValue;
	}
}

General::TimeUnit General::str2TimeUnit(QString str,
		General::TimeUnit defaultValue) {

	if (str.compare(QString("ps"), Qt::CaseInsensitive) == 0) {
		return General::PS;
	}
	else if (str.compare(QString("ns"), Qt::CaseInsensitive) == 0) {
		return General::NS;
	}
	else {
		return defaultValue;
	}
}

QString General::timeUnit2Str(General::TimeUnit& timeUnit) {
	switch (timeUnit) {
	case General::PS: {
		return QString("ps");
	}
	default: {
		return QString("ns");
	}
	}
}

General::DriverType General::str2DriverType(QString str,
                General::DriverType defaultValue) {

	if (str == QString("any")) {
		return General::ANY;
	}
	else if (str == QString("clock")) {
		return General::CLOCK;
	}
	else if (str == QString("singleshot")) {
		return General::SINGLESHOT;
	}
	else {
		return defaultValue;
	}
}

QString General::driverType2Str(General::DriverType type) {

	switch (type) {
	case General::ANY: {
		return QString("any");
	}
	case General::CLOCK: {
		return QString("clock");
	}
	case General::SINGLESHOT: {
		return QString("singleshot");
	}
	default: {
		return QString();
	}
	}
}

bool General::str2Bool(const QString str, bool defaultValue) {
	if (str == QString("true")) {
		return true;
	}
	else if (str == QString("false")) {
		return false;
	}
	else {
		return defaultValue;
	}
}

bool General::BooleanValue2Bool(const General::BooleanValue value,
		const bool defaultValue) {
	switch (value) {
	case General::BOOL_TRUE: {
		return true;
	}
	case General::BOOL_FALSE: {
		return false;
	}
	default: {
		return defaultValue;
	}
	}
}

General::BooleanValue General::bool2BooleanValue(const bool value) {
	if (value)
		return General::BOOL_TRUE;
	else
		return General::BOOL_FALSE;
}

General::BooleanValue General::str2BooleanValue(const QString& str) {
	if (str == QString("true")) {
		return General::BOOL_TRUE;
	}
	else if (str == QString("false")) {
		return General::BOOL_FALSE;
	}
	else {
		return General::BOOL_UNSPECIFIED;
	}
}

QString General::booleanValue2Str(const General::BooleanValue value) {
	switch (value) {
	case General::BOOL_TRUE: {
		return QString("true");
	}
	case General::BOOL_FALSE: {
		return QString("false");
	}
	default: {
		return QString();
	}
	}
}

General::ClockStruct::ClockStruct(QDomNode &clockNode):
value_(0),
timeUnit_(General::NS),
attributes_() {

	QString value = clockNode.childNodes().at(0).nodeValue();
	value = General::removeWhiteSpace(value);
	bool ok = false;
	value_ = value.toDouble(&ok);


	// get the attributes
	QDomNamedNodeMap attributeMap = clockNode.attributes();
	for (int i = 0; i < attributeMap.size(); ++i) {
		QDomNode tempNode = attributeMap.item(i);

		// get spirit:units attribute value
		if (tempNode.nodeName() == QString("spirit:units")) {
			timeUnit_ = General::str2TimeUnit(tempNode.childNodes().at(i).
					nodeValue(),General::NS);
		}

		// other attributes go to QMap
		else {
			QString name = tempNode.nodeName();
			QString value = tempNode.nodeValue();
			attributes_[name] = value;
		}
	}
}

General::ClockStruct::ClockStruct( double value ):
value_(value),
timeUnit_(General::NS),
attributes_() {
}

General::ClockStruct::ClockStruct( const ClockStruct& other ):
value_(other.value_),
timeUnit_(other.timeUnit_),
attributes_(other.attributes_) {
}

General::ClockStruct& General::ClockStruct::operator=( const ClockStruct& other ) {
	if (this != &other) {
		value_ = other.value_;
		timeUnit_ = other.timeUnit_;
		attributes_ = other.attributes_;
	}
	return *this;
}

General::ClockPulseValue::ClockPulseValue(QDomNode &clockNode): 
value_(2),
attributes_() {

	QString value = clockNode.childNodes().at(0).nodeValue();
	bool ok = false;
	value_ = value.toInt(&ok);

	// get the attributes
	General::parseAttributes(clockNode, attributes_);
}

General::ClockPulseValue::ClockPulseValue( unsigned int value ):
value_(value), 
attributes_() {

}

General::ClockPulseValue::ClockPulseValue( const ClockPulseValue& other ):
value_(other.value_),
attributes_(other.attributes_) {
}

General::ClockPulseValue& General::ClockPulseValue::operator=( const ClockPulseValue& other ) {
	if (this != &other) {
		value_ = other.value_;
		attributes_ = other.attributes_;
	}
	return *this;
}

void General::parseAttributes(const QDomNode &node,
		QMap<QString, QString> &map) {
	// parse all the attributes
	QDomNamedNodeMap attributeMap = node.attributes();
	for (int j = 0; j < attributeMap.size(); ++j) {
		QString name = attributeMap.item(j).nodeName();
		QString value = attributeMap.item(j).nodeValue();
		map[name] = value;
	}
	return;
}

void General::writeAttributes(QXmlStreamWriter& writer,
		const QMap<QString, QString>& attributes) {

	// go through all attributes in the map
	for (QMap<QString, QString>::const_iterator i = attributes.begin();
			i != attributes.end(); ++i) {
		// write the current attribute
		writer.writeAttribute(i.key(), i.value());
	}
	return;
}

General::InterfaceMode General::str2Interfacemode(const QString& str, InterfaceMode defaultValue) {
	
	// check all known interface mode names
	for (unsigned int i = 0; i < General::INTERFACE_MODE_COUNT; ++i) {
		
		// if match is found
		if (str.compare(General::INTERFACE_MODE_NAMES[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::InterfaceMode>(i);
		}
	}

	// if there was no match
	return defaultValue;
}

QString General::interfaceMode2Str(const General::InterfaceMode mode) {
	return General::INTERFACE_MODE_NAMES[mode];
}

//-----------------------------------------------------------------------------
// Function: getCompatibleInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode General::getCompatibleInterfaceMode(InterfaceMode mode)
{
    switch (mode)
    {
    case General::MASTER: {
            return MIRROREDMASTER;
        }
    case General::SLAVE: {
            return MIRROREDSLAVE;
        }
    case General::SYSTEM: {
            return MIRROREDSYSTEM;
        }
    case General::MIRROREDMASTER: {
            return MASTER;
        }
    case General::MIRROREDSLAVE: {
            return SLAVE;
        }
    case General::MIRROREDSYSTEM: {
            return SYSTEM;
        }
    case General::MONITOR: {
            return MONITOR;
        }
    default: {
            Q_ASSERT(false);
            return MASTER;
        }
    }
}

VLNV General::createVLNV(const QDomNode& node,
		VLNV::IPXactType type) {
	// the vlnv info is found as attributes in the node
	QDomNamedNodeMap attributeMap = node.attributes();
	QString vendor =
			attributeMap.namedItem(SPIRIT_VENDOR).nodeValue();
	QString library =
			attributeMap.namedItem(SPIRIT_LIBRARY).nodeValue();
	QString name =
			attributeMap.namedItem(SPIRIT_NAME).nodeValue();
	QString version =
			attributeMap.namedItem(SPIRIT_VERSION).nodeValue();

	// if invalid vlnv tag
	if (vendor.isNull() ||
			library.isNull() ||
			name.isNull() ||
			version.isNull() ) {
		return VLNV();
	}

	return VLNV(type, vendor, library, name, version);
}

QString General::removeWhiteSpace(QString str) {
	QTextStream stream(&str);
	QString resultStr;

	// remove white spaces from the start and the end
	str = str.trimmed();

	// keep parsing until the end is reached
	while (!stream.atEnd()) {
		QString temp;

		// strip the whitespace if any is found
		stream.skipWhiteSpace();
		stream >> temp;
		// if there is still characters left in the stream
		if (!stream.atEnd()) {
			// replace the skipped whitespace with '_'
			temp.append("_");
		}
		// add the parsed string to the total string
		resultStr += temp;
	}
	// return the string that has been stripped from white spaces
	return resultStr;
}

General::Qualifier::Qualifier(QDomNode& qualifierNode): isAddress_(false),
		isData_(false), isClock_(false), isReset_(false) {
	for (int i = 0; i < qualifierNode.childNodes().count(); ++i) {
		QDomNode tempNode = qualifierNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:isAddress")) {
			QString isAddress = tempNode.childNodes().at(0).nodeValue();
			isAddress_ = General::str2Bool(isAddress, false);
		}

		else if (tempNode.nodeName() == QString("spirit:isData")) {
			QString isData = tempNode.childNodes().at(0).nodeValue();
			isData_ = General::str2Bool(isData, false);
		}

		else if (tempNode.nodeName() == QString("spirit:isClock")) {
			QString isClock = tempNode.childNodes().at(0).nodeValue();
			isClock_ = General::str2Bool(isClock, false);
		}

		else if (tempNode.nodeName() == QString("spirit:isReset")) {
			QString isReset = tempNode.childNodes().at(0).nodeValue();
			isReset_ = General::str2Bool(isReset, false);
		}
	}
	return;
}

General::Qualifier::Qualifier( const Qualifier& other ):
isAddress_(other.isAddress_),
isData_(other.isData_),
isClock_(other.isClock_),
isReset_(other.isReset_) {
}

General::Qualifier::Qualifier():
isAddress_(false),
isData_(false),
isClock_(false),
isReset_(false) {
}

General::Qualifier& General::Qualifier::operator=( const Qualifier& other ) {
	if (this != &other) {
		isAddress_ = other.isAddress_;
		isData_ = other.isData_;
		isClock_ = other.isClock_;
		isReset_ = other.isReset_;
	}
	return *this;
}

General::LibraryFilePair::LibraryFilePair(const QString filePath,
		const QString libraryName): filePath_(filePath),
		libraryName_(libraryName) {
}

General::LibraryFilePair::LibraryFilePair( const LibraryFilePair& other ):
filePath_(other.filePath_),
libraryName_(other.libraryName_) {
}

bool General::LibraryFilePair::operator==( const LibraryFilePair& other ) const {
	return other.filePath_ == this->filePath_ && 
		other.libraryName_ == this->libraryName_;
}

General::LibraryFilePair& General::LibraryFilePair::operator=( const LibraryFilePair& other ) {
	if (this != &other) {
		filePath_ = other.filePath_;
		libraryName_ = other.libraryName_;
	}
	return *this;
}

void General::writeVLNVAttributes(QXmlStreamWriter& writer,
		const VLNV* vlnv) {
	writer.writeAttribute(SPIRIT_VENDOR, vlnv->getVendor());
	writer.writeAttribute(SPIRIT_LIBRARY, vlnv->getLibrary());
	writer.writeAttribute(SPIRIT_NAME, vlnv->getName());
	writer.writeAttribute(SPIRIT_VERSION, vlnv->getVersion());
	return;
}

QString General::getRelativePath(const QString from, const QString to) {
	if (from.isEmpty() || to.isEmpty()) {
		return QString();
	}

	// create file info instance to make sure that only the directory of the
	// from parameter is used
	QFileInfo fromInfo(from);

    QString fromPath = fromInfo.absolutePath();

    if (fromInfo.isDir())
    {
        fromPath = fromInfo.absoluteFilePath();
    }

	// if the directory does not exist
	QDir ipXactDir(fromPath);

	if (!ipXactDir.exists()) {
		return QString();
	}

	// create file info instance to make sure the target file exists and to
	// use an absolute file path to calculate the relative path.
	QFileInfo toInfo(to);
	if (!toInfo.exists()) {
		return QString();
	}

	// calculate the relative path and return it.
	QString relPath = ipXactDir.relativeFilePath(toInfo.absoluteFilePath());

    // Strip the ending slash if found.
    if (relPath.size() > 0 && relPath.at(relPath.size() - 1) == '/')
    {
        relPath = relPath.left(relPath.size() - 1);
    }
    else if (relPath.isEmpty())
    {
        relPath = ".";
    }

    return relPath;
}

QString General::getRelativeSavePath( const QString& from, const QString& to ) {
	if (from.isEmpty() || to.isEmpty()) {
		return QString();
	}

	// create file info instance to make sure that only the directory of the
	// from parameter is used
	QFileInfo fromInfo(from);

	QString fromPath = fromInfo.absolutePath();

	if (fromInfo.isDir())
	{
		fromPath = fromInfo.absoluteFilePath();
	}

	// if the directory does not exist
	QDir ipXactDir(fromPath);

	if (!ipXactDir.exists()) {
		return QString();
	}

	// create file info instance to make sure the target file exists and to
	// use an absolute file path to calculate the relative path.
	QFileInfo toInfo(to);

	// calculate the relative path and return it.
	return ipXactDir.relativeFilePath(toInfo.absoluteFilePath());
}

QString General::getAbsolutePath(const QString originalPath,
		const QString relativePath) {

	// if one of the parameters is empty
	if (originalPath.isEmpty() || relativePath.isEmpty()) {
		return QString();
	}

	QFileInfo relativeInfo(relativePath);
	if (relativeInfo.isAbsolute()) {
		return relativePath;
	}

	// get the directory path of the original path
	QFileInfo original(originalPath);
	QDir originalDir(original.absolutePath());

	// make sure the path exists
	QFileInfo file(originalDir.absoluteFilePath(relativePath));

	return file.canonicalFilePath();
}

bool General::isIpXactFileType( const QString& fileType ) {

	// check all the pre-specified file types
	for (unsigned int i = 0; i < General::FILE_TYPE_COUNT; ++i) {
		
		// if the file type is one of the specified ones
		if (QString(General::FILE_TYPES[i]) == fileType) {
			return true;
		}
	}
	return false;
}

QStringList General::getFileTypes( QSettings& settings, const QString& fileSuffix ) {
	
	settings.beginGroup("FileTypes");
	QStringList typeNames = settings.childKeys();
	settings.endGroup();

	QStringList types;

	// check each file type
	foreach (QString const& typeName, typeNames)	{
		
		// get the extensions associated with the file type
		QString extensionsStr = settings.value("FileTypes/" + typeName).toString();

		// there may be several extensions for the file type
		QStringList extensions = extensionsStr.split(";", QString::SkipEmptyParts);

		// check each extension of the file type
		foreach (QString extension, extensions) {

			// if the file type has the searched extension defined
			if (!extension.isEmpty() && extension.compare(fileSuffix, Qt::CaseInsensitive) == 0) {
				types.append(typeName);
			}
		}
	}

	return types;
}

QStringList General::getFileTypes( QSettings& settings, const QFileInfo& file ) {
	return General::getFileTypes(settings, file.suffix());
}

General::ModifiedWrite General::str2ModifiedWrite( const QString& str ) {
	
	// check all defined strings
	for (unsigned int i = 0; i < General::MODIFIED_WRITE_COUNT; ++i) {
		
		// if a match is found
		if (str.compare(General::MODIFIED_WRITE_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::ModifiedWrite>(i);
		}
	}

	// if none of the defined strings matched 
	return General::MODIFIED_WRITE_COUNT;
}

QString General::modifiedWrite2Str( const General::ModifiedWrite modWrite ) {
	return General::MODIFIED_WRITE_STRINGS[modWrite];
}

QString General::readAction2Str( const General::ReadAction readAction ) {
	return General::READ_ACTION_STRINGS[readAction];
}

General::ReadAction General::str2ReadAction( const QString& str ) {
	// check all defined strings
	for (unsigned int i = 0; i < General::READ_ACTION_COUNT; ++i) {
		
		// if a match is found
		if (str.compare(General::READ_ACTION_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::ReadAction>(i);
		}
	}

	// if none of the defined strings matched
	return General::READ_ACTION_COUNT;
}

QString General::testConstraint2Str( const General::TestConstraint testConstraint ) {
	return General::TEST_CONSTRAINT_STRINGS[testConstraint];
}

General::TestConstraint General::str2TestConstraint( const QString& str ) {
	// check all defined strings
	for (unsigned int i = 0; i < General::TESTCONSTRAINT_COUNT; ++i) {

		// if a match is found
		if (str.compare(General::TEST_CONSTRAINT_STRINGS[i], Qt::CaseInsensitive) == 0) {
			return static_cast<General::TestConstraint>(i);
		}
	}

	// if none of the defined strings matched
	return General::TESTCONSTRAINT_COUNT;
}

General::NameGroup::NameGroup(): 
name_(),
displayName_(),
description_() {
}

General::NameGroup::NameGroup(QDomNode& node): name_(), displayName_(),
description_() {

	// search all the node's childNodes and find the right ones.
	for (int i = 0; i < node.childNodes().count(); ++i) {
		QDomNode tempNode = node.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:name")) {
			name_ = tempNode.childNodes().at(0).nodeValue();
			name_ = General::removeWhiteSpace(name_);
		}
		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ = tempNode.childNodes().at(0).nodeValue();
		}
		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}
	}
}

General::NameGroup::NameGroup( const QString name ):
name_(name), displayName_(), description_() {
}

General::NameGroup::NameGroup( const NameGroup& other ):
name_(other.name_),
displayName_(other.displayName_),
description_(other.description_) {
}

General::NameGroup& General::NameGroup::operator=( const NameGroup& other ) {
	if (this != &other) {
		name_ = other.name_;
		displayName_ = other.displayName_;
		description_ = other.description_;
	}
	return *this;
}

