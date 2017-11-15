// File: VhdlTypedObject.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.12.2012
//
// Description:
// Class for containing VHDL type and default value.
//-----------------------------------------------------------------------------

#include "VhdlTypedObject.h"

//-----------------------------------------------------------------------------
// Function: VhdlTypedObject::VhdlTypedObject()
//-----------------------------------------------------------------------------
VhdlTypedObject::VhdlTypedObject (  const QString& name /* = QString() */, 
                                    const QString& type /* = QString() */, 
                                    const QString& defaultValue /* = QString() */, 
                                    const QString& description /* = QString() */):
VhdlObject(name, description),
type_(type),
defaultValue_(defaultValue) 
{
}

//-----------------------------------------------------------------------------
// Function: VhdlTypedObject::~VhdlTypedObject()
//-----------------------------------------------------------------------------
VhdlTypedObject::~VhdlTypedObject()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlTypedObject::VhdlTypedObject()
//-----------------------------------------------------------------------------
QString VhdlTypedObject::type() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: VhdlTypedObject::setType()
//-----------------------------------------------------------------------------
void VhdlTypedObject::setType(const QString& type)
{
    type_ = type;
}

//-----------------------------------------------------------------------------
// Function: VhdlTypedObject::defaultValue()
//-----------------------------------------------------------------------------
QString VhdlTypedObject::defaultValue() const
{
    return defaultValue_;
}

//-----------------------------------------------------------------------------
// Function: VhdlTypedObject::setDefaultValue()
//-----------------------------------------------------------------------------
void VhdlTypedObject::setDefaultValue(const QString& defaultValue)
{
    defaultValue_ = defaultValue;
}

/*
//-----------------------------------------------------------------------------
// Function: vhdlobject::getReplacedName()
//-----------------------------------------------------------------------------
QString VhdlObject::getReplacedName() const
{
    QString replacedName = name_;
    replacedName.replace(":","_");
    replacedName.replace(".","_");
    replacedName.replace("-","_");

    return replacedName;
}
*/