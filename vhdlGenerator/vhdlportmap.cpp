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
right_(-1) {
}


VhdlPortMap::VhdlPortMap( const QString& name, unsigned int size /*= 1*/ ):
name_(name),
left_(-1),
right_(-1) {

	if (size > 1) {
		left_ = size -1;
		right_ = 0;
	}
}

VhdlPortMap::VhdlPortMap( const QString& name, int leftBound, int rightBound ):
name_(name),
left_(leftBound),
right_(rightBound) {
}

VhdlPortMap::VhdlPortMap( const VhdlPortMap& other ):
name_(other.name_),
left_(other.left_),
right_(other.right_) {
}

VhdlPortMap& VhdlPortMap::operator=( const VhdlPortMap& other ) {
	if (this != &other) {
		name_ = other.name_;
		left_ = other.left_;
		right_ = other.right_;
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
	if (name_.localeAwareCompare(other.name_) == 0) {
		if (left_ == other.left_) {
			return right_ < other.right_;
		}
		else {
			return other.left_ < other.left_;
		}
	}
	else {
		return name_.localeAwareCompare(other.name_) < 0;
	}
}

bool VhdlPortMap::operator>( const VhdlPortMap& other ) const {
	if (name_.localeAwareCompare(other.name_) == 0) {
		if (left_ == other.left_) {
			return right_ > other.right_;
		}
		else {
			return other.left_ > other.left_;
		}
	}
	else {
		return name_.localeAwareCompare(other.name_) > 0;
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

	if (left_ >= 0) {

		result += QString("(%1 downto %2)").arg(left_).arg(right_);
	}

	return result;
}

QString VhdlPortMap::name() const {
	return name_;
}
