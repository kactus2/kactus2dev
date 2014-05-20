//-----------------------------------------------------------------------------
// File: Kactus2Extension.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.5.2014
//
// Description:
// Kactus2 vendor extension for name-value pairs.
//-----------------------------------------------------------------------------

#include "Kactus2Extension.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::Kactus2Extension()
//-----------------------------------------------------------------------------
Kactus2Extension::Kactus2Extension(QString name, QString value):
    name_(name),
    value_(value)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::~Kactus2Extension()
//-----------------------------------------------------------------------------
Kactus2Extension::~Kactus2Extension()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::clone()
//-----------------------------------------------------------------------------
Kactus2Extension* Kactus2Extension::clone() const
{
    return new Kactus2Extension(name_, value_);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::type()
//-----------------------------------------------------------------------------
QString Kactus2Extension::type() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::write()
//-----------------------------------------------------------------------------
void Kactus2Extension::write(QXmlStreamWriter& writer) const
{
    writer.writeTextElement(name_, value_);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::value()
//-----------------------------------------------------------------------------
QString Kactus2Extension::value() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::setValue()
//-----------------------------------------------------------------------------
void Kactus2Extension::setValue(QString const& newValue)
{
    value_ = newValue;
}

