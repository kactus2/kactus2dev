/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlobject.cpp
 *		Project: Kactus 2
 */

#include "vhdlobject.h"

#include <QRegularExpression>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: vhdlobject::VhdlObject()
//-----------------------------------------------------------------------------
VhdlObject::VhdlObject(const QString& name /*= QString()*/,
					   const QString& description /*= QString()*/ ):
name_(name),
description_(description)
{
}

//-----------------------------------------------------------------------------
// Function: vhdlobject::name()
//-----------------------------------------------------------------------------
QString VhdlObject::name() const {
	return name_;
}

//-----------------------------------------------------------------------------
// Function: vhdlobject::setName()
//-----------------------------------------------------------------------------
void VhdlObject::setName( const QString& name ) {
	name_ = name;
}

//-----------------------------------------------------------------------------
// Function: vhdlobject::description()
//-----------------------------------------------------------------------------
QString VhdlObject::description() const {
	return description_;
}

//-----------------------------------------------------------------------------
// Function: vhdlobject::setDescription()
//-----------------------------------------------------------------------------
void VhdlObject::setDescription( const QString& description ) {
	description_ = description;
}

//-----------------------------------------------------------------------------
// Function: vhdlobject::getVhdlLegalName()
//-----------------------------------------------------------------------------
QString VhdlObject::getVhdlLegalName() const
{
    QString vhdlLegalName = name_;

    QRegularExpression illegalCharacters("[:_.-]+");
    vhdlLegalName = vhdlLegalName.replace(illegalCharacters, "_");

    QRegularExpression leadingUnderScore("^[_]");
    vhdlLegalName.remove(leadingUnderScore);
    QRegularExpression trailingUnderScore("[_]$");
    vhdlLegalName.remove(trailingUnderScore);

    return vhdlLegalName;
}