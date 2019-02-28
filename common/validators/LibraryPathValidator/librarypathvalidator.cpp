/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypathvalidator.cpp
 *		Project: Kactus 2
 */

#include "librarypathvalidator.h"

#include <QRegularExpressionValidator>

//-----------------------------------------------------------------------------
// Function: LibraryPathValidator()
//-----------------------------------------------------------------------------
LibraryPathValidator::LibraryPathValidator(QObject *parent): QRegularExpressionValidator(parent)
{

}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
QValidator::State LibraryPathValidator::validate(QString& input, int& pos) const
{
    State state = QRegularExpressionValidator::validate(input,pos);

    // Partial library path is considered invalid.
    if (state == QValidator::Intermediate)
    {
        return QValidator::Invalid;
    }

    return state;
}

//-----------------------------------------------------------------------------
// Function: LibraryPathValidator::setUnmodifiablePath()
//-----------------------------------------------------------------------------
void LibraryPathValidator::setUnmodifiablePath( const QString& path )
{
    QRegularExpression regExp(path + QString(".*"), 
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	setRegularExpression(regExp);
}