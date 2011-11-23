/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportsorter.cpp
 *		Project: Kactus 2
 */

#include "vhdlportsorter.h"

#include <QDebug>

VhdlPortSorter::VhdlPortSorter( const QString& name, General::Direction direction ):
name_(name),
direction_(direction) {

}

VhdlPortSorter::VhdlPortSorter( const VhdlPortSorter& other ):
name_(other.name_),
direction_(other.direction_) {
}

VhdlPortSorter::~VhdlPortSorter() {
}

VhdlPortSorter& VhdlPortSorter::operator=( const VhdlPortSorter& other ) {

	if (this != &other) {
		name_ = other.name_;
		direction_ = other.direction_;
	}
	return *this;
}

bool VhdlPortSorter::operator==( const VhdlPortSorter& other ) const {
	return (0 == name_.compare(other.name_, Qt::CaseInsensitive));
}

bool VhdlPortSorter::operator!=( const VhdlPortSorter& other ) const {
	return (0 != name_.compare(other.name_, Qt::CaseInsensitive));
}

bool VhdlPortSorter::operator<( const VhdlPortSorter& other ) const {

	// if the direction of one is invalid then just compare the name
// 	if (other.direction_ == General::DIRECTION_INVALID ||
// 		direction_ == General::DIRECTION_INVALID) {
// 		return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
// 	}
// 
// 	// normally first compare the direction and then the name
// 	else if (direction_ == other.direction_) {
// 		return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
// 	}
// 	else {
// 		return direction_ < other.direction_;
// 	}

	return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
}

bool VhdlPortSorter::operator>( const VhdlPortSorter& other ) const {
	// if the direction of other is invalid then just compare the name
// 	if (other.direction_ == General::DIRECTION_INVALID ||
// 		direction_ == General::DIRECTION_INVALID) {
// 		return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
// 	}
// 
// 	// normally first compare the direction and then the name
// 	else if (direction_ == other.direction_) {
// 		return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
// 	}
// 	else {
// 		return direction_ > other.direction_;
// 	}
// 	
	return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
}
