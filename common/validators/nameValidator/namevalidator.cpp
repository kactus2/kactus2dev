/* 
 *
 *  Created on: 14.2.2011
 *      Author: Antti Kamppi
 * 		filename: namevalidator.cpp
 */

#include "namevalidator.h"

#include <QRegExp>

NameValidator::NameValidator(QObject *parent, bool startNumber):
QRegExpValidator(parent) {

	QRegExp exp;

	// set the validator for name-edit to allow only strings with no
	// embedded whitespace characters
	if (startNumber) {
		exp.setPattern("^[a-zA-Z:_0-9]+[a-zA-Z0-9-:_-.]*$");
	}
	else {
		exp.setPattern("^[a-zA-Z:_]+[a-zA-Z0-9-:_-.]*$");
	}
	exp.setPatternSyntax(QRegExp::W3CXmlSchema11);

	setRegExp(exp);
}

NameValidator::~NameValidator() {
}
