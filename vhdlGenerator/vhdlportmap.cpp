/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportmap.cpp
 *		Project: Kactus 2
 */

#include "vhdlportmap.h"

#include "vhdlcomponentinstance.h"


VhdlPortMap::VhdlPortMap():
name_(),
left_(-1),
right_(-1),
type_("std_logic") {
}


VhdlPortMap::VhdlPortMap( const QString& name, unsigned int size /*= 1*/ ):
name_(name),
left_(-1),
right_(-1),
type_("std_logic") {

	if (size > 1) {
		left_ = size -1;
		right_ = 0;
		type_ = "std_logic_vector";
	}
}

VhdlPortMap::VhdlPortMap( const QString& name,
						 int leftBound, 
						 int rightBound,
						 const QString& type):
name_(name),
left_(leftBound),
right_(rightBound),
type_(type) {

	// if type was not defined then use defaults
	if (type_.isEmpty()) {
		
		// if the port size is 1
		if (left_ - right_ > 0) {
			type_ = "std_logic";
		}
		// if port size if larger than 1
		else {
			type_ = "std_logic_vector";
		}
	}
}

VhdlPortMap::VhdlPortMap( const VhdlPortMap& other ):
name_(other.name_),
left_(other.left_),
right_(other.right_),
type_(other.type_) {
}

VhdlPortMap& VhdlPortMap::operator=( const VhdlPortMap& other ) {
	if (this != &other) {
		name_ = other.name_;
		left_ = other.left_;
		right_ = other.right_;
		type_ = other.type_;
	}
	return *this;
}

bool VhdlPortMap::operator==( const VhdlPortMap& other ) const {
	if (name_ == other.name_ &&
		left_ == other.left_ &&
		right_ == other.right_) {
			return true;
	}
	return false;
}

bool VhdlPortMap::operator!=( const VhdlPortMap& other ) const {
	if (name_ != other.name_) {
		return true;
	}
	else if (left_ != other.left_) {
		return true;
	}
	else if (right_ != other.right_) {
		return true;
	}
	return false;
}

bool VhdlPortMap::operator<( const VhdlPortMap& other ) const {
	if (name_.compare(other.name_, Qt::CaseInsensitive) == 0) {
		if (left_ == other.left_) {
			return right_ < other.right_;
		}
		else {
			return left_ < other.left_;
		}
	}
	else {
		return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
	}
}

bool VhdlPortMap::operator>( const VhdlPortMap& other ) const {
	if (name_.compare(other.name_, Qt::CaseInsensitive) == 0) {
		if (left_ == other.left_) {
			return right_ > other.right_;
		}
		else {
			return left_ > other.left_;
		}
	}
	else {
		return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
	}
}

VhdlPortMap::~VhdlPortMap() {
}

void VhdlPortMap::write( QTextStream& stream ) const {
	stream << toString();
}

QString VhdlPortMap::toString() const {
	QString result(name_);

	// make sure the if one bound is defined then both are
	Q_ASSERT((left_ >= 0 && right_ >= 0) || (left_ < 0 && right_ < 0));
	Q_ASSERT(left_ >= right_);

	// if there are no indexes specified then don't write indexing
	if (left_ < 0) {
		return result;
	}

	int size = left_ - right_ + 1;

	// if the type is vectored then the indexing depends on the size
	if (type_ == QString("std_logic_vector")) {

		// if size is one bit 
		if (size == 1) {
			result += QString("(%1)").arg(left_);
		}
		// if size is larger then 
		else {
			result += QString("(%1 downto %2)").arg(left_).arg(right_);
		}
	}

	// for other vectored ports use the downto in indexing
	else if (size > 1) {
		result += QString("(%1 downto %2)").arg(left_).arg(right_);
	}

	return result;
}

QString VhdlPortMap::name() const {
	return name_;
}
