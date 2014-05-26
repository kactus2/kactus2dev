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
// Function: Kactus2Flag::Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Placeholder::Kactus2Placeholder(QString name):
    name_(name),
    attributes_()
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
// Function: Kactus2Flag::~Kactus2Flag()
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
QString Kactus2Placeholder::type() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::write()
//-----------------------------------------------------------------------------
void Kactus2Placeholder::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(name_);

    for (QMapIterator<QString, QString> attributeIterator(attributes_); attributeIterator.hasNext(); 
        attributeIterator.next())
    {
        writer.writeAttribute(attributeIterator.key(), attributeIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::setAttribute()
//-----------------------------------------------------------------------------
void Kactus2Placeholder::setAttribute(QString const& attributeName, QString const& attributeValue)
{
    attributes_.insert(attributeName, attributeValue);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Placeholder::getAttributeValue()
//-----------------------------------------------------------------------------
QString Kactus2Placeholder::getAttributeValue(QString const& attributeName)
{
    return attributes_.value(attributeName);    
}
