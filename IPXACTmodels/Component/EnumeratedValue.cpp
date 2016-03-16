//-----------------------------------------------------------------------------
// File: EnumeratedValue.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Describes the ipxact:enumeratedValue element of a field.
//-----------------------------------------------------------------------------

#include "EnumeratedValue.h"

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::EnumeratedValue()
//-----------------------------------------------------------------------------
EnumeratedValue::EnumeratedValue(QString const& enumerationName, QString const& value) :
NameGroup(enumerationName),
Extendable(),
usage_(EnumeratedValue::UNKNOWNUSAGE),
value_(value)
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::EnumeratedValue()
//-----------------------------------------------------------------------------
EnumeratedValue::EnumeratedValue(const EnumeratedValue& other) :
NameGroup(other),
Extendable(other),
usage_(other.usage_),
value_(other.value_)
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::operator=()
//-----------------------------------------------------------------------------
EnumeratedValue& EnumeratedValue::operator=(const EnumeratedValue& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        usage_ = other.usage_;
        value_ = other.value_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::~EnumeratedValue()
//-----------------------------------------------------------------------------
EnumeratedValue::~EnumeratedValue()
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::getUsage()
//-----------------------------------------------------------------------------
EnumeratedValue::EnumeratedUsage EnumeratedValue::getUsage() const
{
    return usage_;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::setUsage()
//-----------------------------------------------------------------------------
void EnumeratedValue::setUsage(EnumeratedValue::EnumeratedUsage usage)
{
    usage_ = usage;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::getValue()
//-----------------------------------------------------------------------------
QString EnumeratedValue::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::setValue()
//-----------------------------------------------------------------------------
void EnumeratedValue::setValue( QString const& value )
{
    value_ = value;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::usage2Str()
//-----------------------------------------------------------------------------
QString EnumeratedValue::usage2Str(EnumeratedValue::EnumeratedUsage usage)
{
    if (usage == EnumeratedValue::READ)
    {
        return QString("read");
    }
    else if (usage == EnumeratedValue::WRITE)
    {
        return QString("write");
    }
    else if (usage == EnumeratedValue::READWRITE)
    {
        return QString("read-write");
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValue::str2Usage()
//-----------------------------------------------------------------------------
EnumeratedValue::EnumeratedUsage EnumeratedValue::str2Usage(const QString& usage, EnumeratedUsage defaultValue)
{
    if (usage == QString("read"))
    {
        return EnumeratedValue::READ;
    }
    else if (usage == QString("write"))
    {
        return EnumeratedValue::WRITE;
    }
    else if (usage == QString("read-write"))
    {
        return EnumeratedValue::READWRITE;
    }
    else
    {
        return defaultValue;
    }
}
