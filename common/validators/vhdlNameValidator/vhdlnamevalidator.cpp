/* 
 *  	Created on: 17.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlnamevalidator.cpp
 *		Project: Kactus 2
 */

#include "vhdlnamevalidator.h"

#include <QRegExp>

VhdlNameValidator::VhdlNameValidator(QObject *parent):
QRegExpValidator(parent) {

	QRegExp exp;

	exp.setPattern("^[a-zA-Z]+[a-zA-Z0-9_]*$");

	exp.setPatternSyntax(QRegExp::W3CXmlSchema11);

	setRegExp(exp);
}

VhdlNameValidator::~VhdlNameValidator() {
}
