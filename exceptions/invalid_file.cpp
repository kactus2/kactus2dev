/* 
 *
 *  Created on: 20.8.2010
 *      Author: Antti Kamppi
 */

#include "invalid_file.h"

#include <stdexcept>

// the constructor
Invalid_file::Invalid_file(QString errorMsg, QString filePath) throw():
runtime_error("Invalid file"), errorMsg_(errorMsg), filePath_(filePath) {
}

// copy constructor
Invalid_file::Invalid_file(const Invalid_file& other) throw():
                runtime_error(other), errorMsg_(other.errorMsg_),
		filePath_(other.filePath_) {
}

// the detructor
Invalid_file::~Invalid_file() throw() {
}

QString& Invalid_file::filePath() throw() {
	return filePath_;
}

QString& Invalid_file::errorMsg() throw() {
	return errorMsg_;
}

void Invalid_file::addFilePath(QString filePath) throw() {
	filePath_ = filePath;
}

// assignment operator
Invalid_file & Invalid_file::operator =(const Invalid_file& other) throw() {
	if (this != &other) {
		runtime_error::operator=(other);
		errorMsg_ = other.errorMsg_;
		filePath_ = other.filePath_;
	}
	return *this;
}
