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

int Utils::str2Int( const QString& str ) {
	QString strNumber = str;
	const QChar multiple = strNumber.at(strNumber.size()-1);
	int multiplier = 1;

	// get the correct multiplier and remove the letter from the string
	if (multiple == 'k' || multiple == 'K') {
		multiplier = 2^10;
		strNumber.chop(1);
	}
	else if (multiple == 'M') {
		multiplier = 2^20;
		strNumber.chop(1);
	}
	else if (multiple == 'G') {
		multiplier = 2^30;
		strNumber.chop(1);
	}
	else if (multiple == 'T') {
		multiplier = 2^40;
		strNumber.chop(1);
	}
	else if (multiple == 'P') {
		multiplier = 2^50;
		strNumber.chop(1);
	}

	bool success = true;
	int number = strNumber.toInt(&success);

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
