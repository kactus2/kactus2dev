/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathvalidator.cpp
 *		Project: Kactus 2
 */

#include "librarypathvalidator.h"

#include <QRegExp>

LibraryPathValidator::LibraryPathValidator(QObject *parent):
QRegExpValidator(parent) {
}

LibraryPathValidator::~LibraryPathValidator() {
}

void LibraryPathValidator::setUnmodifiablePath( const QString& path ) {
	QRegExp regExp(path + QString("*"), Qt::CaseInsensitive, QRegExp::Wildcard);
	setRegExp(regExp);
}
