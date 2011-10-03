/* 
 *
 *  Created on: 15.10.2010
 *      Author: Antti kamppi
 */

#include "write_error.h"

#include <QString>

// the constructor
Write_error::Write_error(QString errorMsg) throw() :
runtime_error("Writing error"), errorMsg_(errorMsg), filePath_() {
}

// the destructor
Write_error::~Write_error() throw() {
}

// the copy constructor
Write_error::Write_error(const Write_error& other)
throw(): runtime_error(other),
		errorMsg_(other.errorMsg_),
		filePath_(other.filePath_) {
}

// the assignment operator
Write_error& Write_error::operator =(const Write_error& other) throw() {
	if (this != &other) {
		runtime_error::operator=(other);
		errorMsg_ = other.errorMsg_;
		filePath_ = other.filePath_;
	}
	return *this;
}

QString& Write_error::filePath() throw() {
	return filePath_;
}

void Write_error::addFilePath(QString path) throw() {
	filePath_ = path;
}

QString& Write_error::errorMsg() throw() {
	return errorMsg_;
}
