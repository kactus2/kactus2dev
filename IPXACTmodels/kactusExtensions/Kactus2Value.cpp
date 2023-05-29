//-----------------------------------------------------------------------------
// File: Kactus2Value.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.5.2014
//
// Description:
// Kactus2 vendor extension for name-value pairs.
//-----------------------------------------------------------------------------

#include "Kactus2Value.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Value::Kactus2Value()
//-----------------------------------------------------------------------------
Kactus2Value::Kactus2Value(std::string const& name, std::string const& value):
    name_(name),
    value_(value)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Value::~Kactus2Value()
//-----------------------------------------------------------------------------
Kactus2Value::~Kactus2Value()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Value::clone()
//-----------------------------------------------------------------------------
Kactus2Value* Kactus2Value::clone() const
{
    return new Kactus2Value(name_, value_);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Value::type()
//-----------------------------------------------------------------------------
std::string Kactus2Value::type() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Value::write()
//-----------------------------------------------------------------------------
void Kactus2Value::write(QXmlStreamWriter& writer) const
{    
    writer.writeTextElement(QString::fromStdString(name_), QString::fromStdString(value_));
}

//-----------------------------------------------------------------------------
// Function: Kactus2Value::value()
//-----------------------------------------------------------------------------
std::string Kactus2Value::value() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Value::setValue()
//-----------------------------------------------------------------------------
void Kactus2Value::setValue(std::string const& newValue)
{
    value_ = newValue;
}
