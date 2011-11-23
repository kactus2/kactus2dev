/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlportsorter.cpp
 *		Project: Kactus 2
 */

#include "vhdlportsorter.h"

#include <QDebug>

VhdlPortSorter::VhdlPortSorter(const QString& interface,
							   const QString& name, 
							   General::Direction direction ):
interface_(interface),
name_(name),
direction_(direction) {

}

VhdlPortSorter::VhdlPortSorter( const VhdlPortSorter& other ):
interface_(other.interface_),
name_(other.name_),
direction_(other.direction_) {
}

VhdlPortSorter::~VhdlPortSorter() {
}

VhdlPortSorter& VhdlPortSorter::operator=( const VhdlPortSorter& other ) {

	if (this != &other) {
		interface_ = other.interface_;
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

	// first check the interface name
	if (interface_.compare(other.interface_, Qt::CaseInsensitive) == 0) {
		
		// compare the direction and then the name
		if (direction_ == other.direction_) {
			return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
		}
		else {
			return direction_ < other.direction_;
		}
	}
	else {
		return interface_.compare(other.interface_, Qt::CaseInsensitive) < 0;
	}

}

bool VhdlPortSorter::operator>( const VhdlPortSorter& other ) const {

	// first check the interface name
	if (interface_.compare(other.interface_, Qt::CaseInsensitive) == 0) {

		// compare the direction and then the name
		if (direction_ == other.direction_) {
			return name_.compare(other.name_, Qt::CaseInsensitive) > 0;
		}
		else {
			return direction_ > other.direction_;
		}
	}
	else {
		return interface_.compare(other.interface_, Qt::CaseInsensitive) > 0;
	}
}

QString VhdlPortSorter::interface() const {
	return interface_;
}
