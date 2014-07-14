//-----------------------------------------------------------------------------
// File: HDLObject.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for objects used in HDL generation.
//-----------------------------------------------------------------------------

#include "HDLObject.h"

//-----------------------------------------------------------------------------
// Function: HDLObject::HDLObject()
//-----------------------------------------------------------------------------
HDLObject::HDLObject( QObject* parent, 
					   QString const& name, 
					   QString const& type, 
					   QString const& defaultValue, 
					   QString const& description ):
QObject(parent),
name_(name),
type_(type),
description_(description),
defaultValue_(defaultValue) 
{

}

//-----------------------------------------------------------------------------
// Function: HDLObject::~HDLObject()
//-----------------------------------------------------------------------------
HDLObject::~HDLObject()
{

}

//-----------------------------------------------------------------------------
// Function: HDLObject::name()
//-----------------------------------------------------------------------------
QString HDLObject::name() const
{
	return name_;
}

//-----------------------------------------------------------------------------
// Function: HDLObject::setName()
//-----------------------------------------------------------------------------
void HDLObject::setName( QString const& name )
{
	name_ = name;
}

//-----------------------------------------------------------------------------
// Function: HDLObject::type()
//-----------------------------------------------------------------------------
QString HDLObject::type() const
{
	return type_;
}

//-----------------------------------------------------------------------------
// Function: HDLObject::setType()
//-----------------------------------------------------------------------------
void HDLObject::setType( QString const& type )
{
	type_ = type;
}

//-----------------------------------------------------------------------------
// Function: HDLObject::description()
//-----------------------------------------------------------------------------
QString HDLObject::description() const
{
	return description_;
}

//-----------------------------------------------------------------------------
// Function: HDLObject::setDescription()
//-----------------------------------------------------------------------------
void HDLObject::setDescription( QString const& description )
{
	description_ = description;
}

//-----------------------------------------------------------------------------
// Function: HDLObject::defaultValue()
//-----------------------------------------------------------------------------
QString HDLObject::defaultValue() const
{
	return defaultValue_;
}

//-----------------------------------------------------------------------------
// Function: HDLObject::setDefaultValue()
//-----------------------------------------------------------------------------
void HDLObject::setDefaultValue( QString const& defaultValue )
{
	defaultValue_ = defaultValue;
}
