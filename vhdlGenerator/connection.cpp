/* 
 *
 *  Created on: 3.11.2010
 *      Author: Antti Kamppi
 */

#include "connection.h"

#include <models/generaldeclarations.h>

#include <exceptions/vhdl_error.h>

#include <QObject>
#include <QString>
#include <QTextStream>

#include <QDebug>

Connection::Connection(const QString name, QObject* parent): 
QObject(parent),
name_(name), 
direction_(General::DIRECTION_INVALID),
type_(Connection::STD_LOGIC),
left_(0), 
right_(0), 
defaultValue_(),
description_() {
}

Connection::Connection(const QString name, General::Direction direction,
					   int left, int right, const QString& description, 
					   QObject* parent):
QObject(parent),
name_(name),
direction_(direction),
type_(Connection::STD_LOGIC), 
left_(left),
right_(right),
defaultValue_(),
description_(description) {

	if (left != right)
		type_ = Connection::STD_LOGIC_VECTOR;
}


Connection::Connection(const QString& name, Vector* vector, QObject* parent):
QObject(parent), 
name_(name), 
direction_(General::DIRECTION_INVALID),
type_(Connection::STD_LOGIC), 
left_(0), 
right_(0), 
defaultValue_(),
description_() {

	// if the vector is not null then the connection is std_logic_vector
	if (vector) {
		type_ = Connection::STD_LOGIC_VECTOR;
		left_ = vector->getLeft();
		right_ = vector->getRight();
	}
}

Connection::~Connection() {
}

void Connection::writeSignalVhdl(QTextStream& stream) {

	// write the signal to the stream
	stream << "\tsignal " << name_ << " : ";

	// write the signal type (std_logic or std_logic_vector)
	writeType(stream);

	// write the default value if it has been defined
	writeDefault(stream);

	stream << ";" << endl;
}

void Connection::writeDefault(QTextStream& stream) {
	// if a default value has been assigned
	if (!defaultValue_.isEmpty()) {

		// print the assignment operator to the stream
		stream << " := ";

		bool ok = false;

		// the print format depends on the type of signal
		switch (type_) {

		// if signal is std_logic
		case Connection::STD_LOGIC: {
			// convert the value to int
			int value = str2Int(ok, defaultValue_);

			// if conversion was succesful
			if (ok)
				stream << "'" << value << "'";

			// conversion was unsuccesful
			else
                                stream << "'" << defaultValue_ << "'";
			break;
		}

		// if signal is std_logic_vector
		default: {

			int value = str2Int(ok, defaultValue_);

			// print the conversion function into vhdl
			stream << "std_logic_vector(to_signed(";

			// if conversion was successful
			if (ok) {
				// print the converted value
				stream << value;
			}
			// if conversion failed
			else {
				// print the value that was in the IP-Xact metadata as it was.
				stream << defaultValue_;
			}

			// print the rest of the conversion function into vhdl
			stream << ", " << (left_ - right_ + 1) << "))";
			break;
		}
		}
	}
}

int Connection::str2Int(bool& ok, const QString& value) {

	// try to convert from hexadecimal format
	int temp = value.toInt(&ok, 16);

	// if the conversion was unsuccessful
	if (!ok) {
		// try to convert from 10 base format
		temp = value.toInt(&ok, 10);
	}

	// if the conversion was unsuccessful
	if (!ok) {
		// try to convert from binary format
		temp = value.toInt(&ok, 2);
	}

	// if conversion was unsuccessful
	if (!ok) {
		// try to convert from 8-base format
		temp = value.toInt(&ok, 8);
	}
	return temp;
}

void Connection::writePortMap(QTextStream& stream) {
	// write the name
	stream << name_;

	// if the connection is type std_logic_vector
	if (type_ == Connection::STD_LOGIC_VECTOR){

		// if the slice is only one bit then no "downto"
		if (left_ == right_) {
			stream << "(" << left_ << ")";
		}
		// if slice is more than one bit
		else {
			stream << "(" << left_ << " downto " << right_ << ")";
		}
	}
	return;
}

void Connection::writePortDeclaration(QTextStream& stream) {

	// write the name and direction or the port
	stream << "\t\t\t" << name_ << " : " << General::direction2Str(direction_)
	<< " ";

	// write the signal type
	writeType(stream);

	writeDefault(stream);

	return;
}

void Connection::writeType(QTextStream& stream) {

	switch (type_) {
	case Connection::STD_LOGIC: {
		stream << "std_logic";
		if (left_ != right_) {
			emit errorMessage(tr("Signal %1 type did not match width.").arg(name_));
		}
		return;
	}
	case Connection::STD_LOGIC_VECTOR: {
		stream << "std_logic_vector(" << left_ << " downto " << right_;
		stream << ")";
		return;
	}
	// signals should be std_logic or std_logic_vector
	default: {
		emit errorMessage(tr("Invalid signal/port type defined for %1").arg(name_));
		return;
	}
	}
}
QString Connection::getName() const {
	return name_;
}

/*
void Connection::setName(const QString name) {
	name_ = name;
}*/

void Connection::setType(const Connection::PortType type) {
	type_ = type;
}

void Connection::setBounds(const int left, const int right) {
	left_ = left;
	right_ = right;

	// if left != right it means that signal has to be vectored
	if (left_ != right_) {
		type_ = Connection::STD_LOGIC_VECTOR;
	}
	else {
		type_ = Connection::STD_LOGIC;
	}
}

bool Connection::operator<(const Connection &other) const {
	return (this->name_.toLower() < other.getName().toLower());
}

// the copy constructor
Connection::Connection(const Connection& other): name_(other.name_),
		type_(other.type_), left_(other.left_), right_(other.right_) {
}

// assignment operator
Connection& Connection::operator=(const Connection& other) {
	// if we are not assigning to self
	if (this != &other) {
		name_ = other.name_;
		type_ = other.type_;
		left_ = other.left_;
		right_ = other.right_;
	}
	return *this;
}

General::Direction Connection::getDirection() const {
	return direction_;
}

unsigned int Connection::getLeft() const {
	return left_;
}

unsigned int Connection::getRight() const {
	return right_;
}

unsigned int Connection::getSize() const {
	return (left_ - right_ + 1);
}

void Connection::setDefaultValue(const QString value) {
	defaultValue_ = value;
}

QString Connection::getDefaultValue() const {
	return defaultValue_;
}

void Connection::setDirection(const General::Direction direction) {
	direction_ = direction;
}

QString Connection::getDescription() const {
	return description_;
}

Connection::EndPoint::EndPoint(): 
instanceName_(), portName_(), signalLeft_(0), signalRight_(0),
portLeft_(0), portRight_(0) {
}

Connection::EndPoint::EndPoint( const QString& instance, const QString& port ) :
instanceName_(instance), portName_(port),
signalLeft_(0), signalRight_(0), portLeft_(0), portRight_(0) {
}

Connection::EndPoint::EndPoint( const EndPoint& other ): 
instanceName_(other.instanceName_), portName_(other.portName_),
signalLeft_(other.signalLeft_), signalRight_(other.signalRight_), 
portLeft_(other.portLeft_), portRight_(other.portRight_) {
}

Connection::EndPoint& Connection::EndPoint::operator=( const EndPoint& other ) {
	if (this != &other) {
		instanceName_ = other.instanceName_;
		portName_ = other.portName_;
		signalLeft_ = other.signalLeft_;
		signalRight_ = other.signalRight_;
		portLeft_ = other.portLeft_;
		portRight_ = other.portRight_;
	}
	return *this;
}

bool Connection::EndPoint::operator==( const EndPoint& other ) const {
	
	if (instanceName_ == other.instanceName_ &&
		portName_ == other.portName_ &&
		portLeft_ == other.portLeft_ &&
		portRight_ == other.portRight_)
		return true;

	return false;
}

bool Connection::EndPoint::operator<( const EndPoint& other ) const {
	if (instanceName_ == other.instanceName_) {
		
		if (portName_ == other.portName_) {

			if (portLeft_ == other.portLeft_) 
				return portRight_ < other.portRight_;
			else 
				return portLeft_ < other.portLeft_;

		}
		else {
			return portName_ < other.portName_;
		}
	}
	else {
		return instanceName_ < other.instanceName_;
	}
}

bool Connection::EndPoint::operator>( const EndPoint& other ) const {
	if (instanceName_ == other.instanceName_) {
		if (portName_ == other.portName_) {
			if (portLeft_ == other.portLeft_)
				return portRight_ > other.portRight_;
			else
				return portLeft_ > other.portLeft_;
		}
		else {
			return portName_ > other.portName_;
		}
	}
	else {
		return instanceName_ > other.instanceName_;
	}
}