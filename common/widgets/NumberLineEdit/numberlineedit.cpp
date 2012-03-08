/* 
 *  	Created on: 29.2.2012
 *      Author: Antti Kamppi
 * 		filename: numberlineedit.cpp
 *		Project: Kactus 2
 */

#include "numberlineedit.h"

#include <QRegExp>
#include <QRegExpValidator>

NumberLineEdit::NumberLineEdit(QWidget *parent):
QLineEdit(parent) {

	// set validator to accept only strings that begin with number and may
	// contain one multiple letter in the end
	QRegExp regExp("[0-9]+[kKMGTP]?", Qt::CaseSensitive, QRegExp::W3CXmlSchema11);
	QRegExpValidator* validator = new QRegExpValidator(regExp, this);
	setValidator(validator);

	setPlaceholderText(tr("Insert number i.e 4G"));
}

NumberLineEdit::~NumberLineEdit() {
}
