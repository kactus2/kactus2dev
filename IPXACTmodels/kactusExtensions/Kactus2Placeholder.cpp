//-----------------------------------------------------------------------------
// File: Kactus2Placeholder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.5.2014
//
// Description:
// Kactus2 vendor extension for single values.
//-----------------------------------------------------------------------------

#include "Kactus2Placeholder.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::Kactus2Placeholder()
//-----------------------------------------------------------------------------
Kactus2Placeholder::Kactus2Placeholder(std::string const& name):
    name_(name)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::Kactus2Placeholder()
//-----------------------------------------------------------------------------
Kactus2Placeholder::Kactus2Placeholder(Kactus2Placeholder const& other):
    name_(other.name_),
    attributes_(other.attributes_)
{

}


//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::~Kactus2Placeholder()
//-----------------------------------------------------------------------------
Kactus2Placeholder::~Kactus2Placeholder()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::clone()
//-----------------------------------------------------------------------------
Kactus2Placeholder* Kactus2Placeholder::clone() const
{
    Kactus2Placeholder* cloned = new Kactus2Placeholder(name_);
    cloned->attributes_ = attributes_;
    return cloned;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::type()
//-----------------------------------------------------------------------------
std::string Kactus2Placeholder::type() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::write()
//-----------------------------------------------------------------------------
void Kactus2Placeholder::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(QString::fromStdString(name_));

    for (auto const& attribute : attributes_.keys())
    {
        writer.writeAttribute(QString::fromStdString(attribute), 
            QString::fromStdString(attributes_.value(attribute)));
    }
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::setAttribute()
//-----------------------------------------------------------------------------
void Kactus2Placeholder::setAttribute(QString const& attributeName, QString const& attributeValue)
{
    attributes_.insert(attributeName.toStdString(), attributeValue.toStdString());
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::setAttribute()
//-----------------------------------------------------------------------------
void Kactus2Placeholder::setAttribute(std::string const& attributeName, std::string const& attributeValue)
{
    attributes_.insert(attributeName, attributeValue);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::getAttributeValue()
//-----------------------------------------------------------------------------
std::string Kactus2Placeholder::getAttributeValue(QString const& attributeName)
{
    return attributes_.value(attributeName.toStdString());    
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::getAttributeValue()
//-----------------------------------------------------------------------------
std::string Kactus2Placeholder::getAttributeValue(std::string const& attributeName)
{
    return attributes_.value(attributeName);
}
