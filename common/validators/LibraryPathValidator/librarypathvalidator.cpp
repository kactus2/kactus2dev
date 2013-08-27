/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathvalidator.cpp
 *		Project: Kactus 2
 */

#include "librarypathvalidator.h"

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: LibraryPathValidator()
//-----------------------------------------------------------------------------
LibraryPathValidator::LibraryPathValidator(QObject *parent):
QRegExpValidator(parent) {
}

//-----------------------------------------------------------------------------
// Function: ~LibraryPathValidator()
//-----------------------------------------------------------------------------
LibraryPathValidator::~LibraryPathValidator() {
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
QValidator::State LibraryPathValidator::validate(QString& input, int& pos) const
{
    State state = QRegExpValidator::validate(input,pos);

    // Partial libraryp path is considered invalid.
    if (state == QValidator::Intermediate)
    {
        return QValidator::Invalid;
    }
    return state;
}

//-----------------------------------------------------------------------------
// Function: setUnmodifiablePath()
//-----------------------------------------------------------------------------
void LibraryPathValidator::setUnmodifiablePath( const QString& path ) {
	QRegExp regExp(path + QString("*"), Qt::CaseInsensitive, QRegExp::Wildcard);
	setRegExp(regExp);
}