/* 
 *
 *  Created on: 13.8.2010
 *      Author: Antti Kamppi
 */

#include "parse_error.h"

#include <stdexcept>
#include <QString>

// the constructor
Parse_error::Parse_error(QString errorMsg) throw() :
runtime_error("Parsing error"), errorMsg_(errorMsg), filePath_(QString()) {
}

// the destructor
Parse_error::~Parse_error() throw() {
}

// the copy constructor
Parse_error::Parse_error(const Parse_error& other) throw() :runtime_error(other),
		errorMsg_(other.errorMsg_),
		filePath_(other.filePath_) {
}

// the assignment operator
Parse_error& Parse_error::operator =(const Parse_error& other) throw() {
	if (this != &other) {
		runtime_error::operator=(other);
		errorMsg_ = other.errorMsg_;
		filePath_ = other.filePath_;
	}
	return *this;
}

QString& Parse_error::filePath() throw() {
	return filePath_;
}

void Parse_error::addFilePath(QString path) throw() {
	filePath_ = path;
}

QString& Parse_error::errorMsg() throw() {
	return errorMsg_;
}
