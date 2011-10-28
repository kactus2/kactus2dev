/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportsorter.cpp
 *		Project: Kactus 2
 */

#include "vhdlportsorter.h"

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
	if (direction_ == other.direction_) {
		return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
	}
	else {
		return direction_ < other.direction_;
	}
}

bool VhdlPortSorter::operator>( const VhdlPortSorter& other ) const {
	if (direction_ == other.direction_) {
		return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
	}
	else {
		return direction_ > other.direction_;
	}
}
