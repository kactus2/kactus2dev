/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlport.cpp
 *		Project: Kactus 2
 */

#include "vhdlport.h"

#include "vhdlgeneral.h"

#include <models/vector.h>
#include <models/port.h>

VhdlPort::VhdlPort( VhdlGenerator2* parent, Port* port ):
VhdlObject(parent, port->getName(), 
		   port->getTypeName(),
		   port->getDefaultValue(), 
		   port->getDescription()),
direction_(port->getDirection()),
left_(port->getLeftBound()),
right_(port->getRightBound()),
commentOut_(true) {

	Q_ASSERT(parent);
	Q_ASSERT(port);

	// if type is not set then use the defaults
	if (type_.isEmpty()) {

		int size = left_ - right_ + 1;

		// if port is scalar
		if (size == 1) {
			type_ = QString("std_logic");
		}
		else {
			type_ = QString("std_logic_vector");
		}
	}
}

VhdlPort::VhdlPort( VhdlComponentDeclaration* parent, Port* port ):
VhdlObject(parent, port->getName(), 
		   port->getTypeName(),
		   port->getDefaultValue(), 
		   port->getDescription()),
direction_(port->getDirection()),
left_(port->getLeftBound()),
right_(port->getRightBound()),
commentOut_(true) {

	Q_ASSERT(parent);
	Q_ASSERT(port);

	// if type is not set then use the defaults
	if (type_.isEmpty()) {

		int size = left_ - right_ + 1;

		// if port is scalar
		if (size == 1) {
			type_ = QString("std_logic");
		}
		else {
			type_ = QString("std_logic_vector");
		}
	}
}

VhdlPort::~VhdlPort() {
}

void VhdlPort::write( QTextStream& stream ) const {
	if (commentOut_) {
		stream << "-- ";
	}
	stream << name_ << " : " << General::direction2Str(direction_) << " ";

	stream << VhdlGeneral::vhdlType2String(type_, left_, right_);
}

int VhdlPort::size() const {
	return left_ - right_ + 1;
}

int VhdlPort::left() const {
	return left_;
}

int VhdlPort::right() const {
	return right_;
}

void VhdlPort::setCommented( bool commentOut ) {
	commentOut_ = commentOut;
}

bool VhdlPort::isCommented() const {
	return commentOut_;
}

