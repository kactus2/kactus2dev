/* 
 *
 *  Created on: 3.11.2010
 *      Author: Antti Kamppi
 */

#include "vhdl_error.h"

#include <QString>

// the constructor
Vhdl_error::Vhdl_error(const QString errorMsg) throw():
runtime_error("Vhdl generating error"), errorMsg_(errorMsg) {

}

// the destructor
Vhdl_error::~Vhdl_error() throw() {
}

// Assignment operator
Vhdl_error & Vhdl_error::operator =(const Vhdl_error & other) throw() {
	if (this != &other) {
		runtime_error::operator=(other);
		errorMsg_ = other.errorMsg_;
	}
	return *this;
}

// Copy constructor
Vhdl_error::Vhdl_error(const Vhdl_error & other) throw():
		runtime_error(other), errorMsg_(other.errorMsg_){
}

// Get the error message
QString& Vhdl_error::errorMsg() throw() {
	return errorMsg_;
}





