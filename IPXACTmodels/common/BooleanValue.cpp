//-----------------------------------------------------------------------------
// File: BooleanValue.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "BooleanValue.h"

//-----------------------------------------------------------------------------
// Function: BooleanValue::BooleanValue()
//-----------------------------------------------------------------------------
BooleanValue::BooleanValue() : value_(BOOL_UNSPECIFIED)
{

}

//-----------------------------------------------------------------------------
// Function: BooleanValue::BooleanValue()
//-----------------------------------------------------------------------------
BooleanValue::BooleanValue(BooleanValue const& other) :
    value_(other.value_)
{

}

//-----------------------------------------------------------------------------
// Function: BooleanValue::~BooleanValue()
//-----------------------------------------------------------------------------
BooleanValue::~BooleanValue()
{

}

//-----------------------------------------------------------------------------
// Function: BooleanValue::operator=()
//-----------------------------------------------------------------------------
BooleanValue& BooleanValue::operator=(BooleanValue const& other)
{
    if (this != &other)
    {
        value_ = other.value_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: BooleanValue::setValue()
//-----------------------------------------------------------------------------
void BooleanValue::setValue(bool value)
{
    if (value)
    {
        value_ = BOOL_TRUE;
    }
    else
    {
        value_ = BOOL_FALSE;
    }
}

//-----------------------------------------------------------------------------
// Function: BooleanValue::toBool()
//-----------------------------------------------------------------------------
bool BooleanValue::toBool() const
{
    if (value_ == BOOL_TRUE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: BooleanValue::toString()
//-----------------------------------------------------------------------------
QString BooleanValue::toString() const
{
    if (value_ == BOOL_TRUE)
    {
        return QString("true");
    }
    else if (value_ == BOOL_FALSE)
    {
        return QString("false");
    }
    else
    {
        return QString();
    }    
}

//-----------------------------------------------------------------------------
// Function: BooleanValue::setUnspecified()
//-----------------------------------------------------------------------------
void BooleanValue::setUnspecified()
{
    value_ = BOOL_UNSPECIFIED;
}