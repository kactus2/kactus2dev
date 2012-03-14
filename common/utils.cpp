/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: utils.cpp
 * 		
 * 		Description: This file contains definitions for general purpose 
 * 		functions that can be used in the whole software.
 */

#include "utils.h"

#include <qmath.h>

Qt::CheckState Utils::bool2CheckState(const bool state) {
	if (state) 
		return Qt::Checked;
	else
		return Qt::Unchecked;

}

bool Utils::checkState2Bool(const Qt::CheckState state) {
	if (state == Qt::Checked)
		return true;
	else
		return false;
}

bool Utils::checkBoxState2Bool(const int state ) {
	switch (state) {
		case 2:
			return true;
		default:
			return false;
	}
}

quint64 Utils::str2Int( const QString& str ) {

	if (str.isEmpty()) {
		return 0;
	}

	// used to detect if the conversion was successful
	bool success = true;
	quint64 number = 0;

	// if starts with "0x" then it is hexadecimal digit
	if (str.startsWith("0x", Qt::CaseInsensitive)) {
		
		number = str.toULongLong(&success, 16);
		if (success) {
			return number;
		}
	}

	// needed because the last letter is chopped if one is found
	QString strNumber = str;

	// the multiple is the last letter if one exists
	const QChar multiple = strNumber.at(strNumber.size()-1);
	quint64 multiplier = 1;

	// get the correct multiplier and remove the letter from the string
	if (multiple == 'k' || multiple == 'K') {
		multiplier = qPow(2, 10);
		strNumber.chop(1);
	}
	else if (multiple == 'M') {
		multiplier = qPow(2, 20);
		strNumber.chop(1);
	}
	else if (multiple == 'G') {
		multiplier = qPow(2, 30);
		strNumber.chop(1);
	}
	else if (multiple == 'T') {
		multiplier = qPow(2, 40);
		strNumber.chop(1);
	}
	else if (multiple == 'P') {
		multiplier = qPow(2, 50);
		strNumber.chop(1);
	}

	// try to convert the number 
	number = strNumber.toULongLong(&success);

	// if the conversion failed
	if (!success) {
		return 0;
	}
	// otherwise return the correct int-format
	else {
		return number * multiplier;
	}
}

Utils::Mapping::Mapping(): portName_(QString("---")), left_(0), right_(0){
}

Utils::Mapping::Mapping( const QString& name ): 
portName_(name), left_(0), right_(0) {
}

Utils::Mapping::Mapping( const QString& name, int left, int right ):
portName_(name), left_(left), right_(right) {
}

bool Utils::Mapping::operator==( const Mapping& other ) const {
	return (portName_ == other.portName_);
}

bool Utils::Mapping::operator!=( const Mapping& other ) const {
	return (portName_ != other.portName_);
}

bool Utils::Mapping::operator<( const Mapping& other ) const {
	return portName_ < other.portName_;
}

bool Utils::Mapping::operator>( const Mapping& other ) const {
	return portName_ > other.portName_;
}

Utils::ImplementationOptions::ImplementationOptions():
hw_(true),
sw_(true),
system_(true) {
}

Utils::HierarchyOptions::HierarchyOptions():
global_(true),
product_(true),
board_(true),
chip_(true),
soc_(true),
ip_(true) {
}

Utils::FirmnessOptions::FirmnessOptions():
templates_(true),
mutable_(true),
parameterizable_(true),
fixed_(true) {
}

Utils::TypeOptions::TypeOptions():
components_(true),
buses_(true),
advanced_(false) {
}
