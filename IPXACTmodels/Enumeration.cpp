//-----------------------------------------------------------------------------
// File: Enumeration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.09.2014
//
// Description:
// Enumeration matches the spirit:enumeration element in IP-XACT.
//-----------------------------------------------------------------------------

#include "Enumeration.h"

//-----------------------------------------------------------------------------
// Function: Enumeration::Enumeration()
//-----------------------------------------------------------------------------
Enumeration::Enumeration():
value_(), 
    text_(),
    help_()
{

}

//-----------------------------------------------------------------------------
// Function: Enumeration::Enumeration()
//-----------------------------------------------------------------------------
Enumeration::Enumeration(QDomNode const& enumerationNode):
value_(), 
    text_(),
    help_()
{
    // get the name of the enumeration
    value_ = enumerationNode.childNodes().at(0).nodeValue();

    QDomNamedNodeMap attributeMap = enumerationNode.attributes();
    text_ = attributeMap.namedItem("spirit:text").nodeValue();
    help_ = attributeMap.namedItem("spirit:help").nodeValue();
}

//-----------------------------------------------------------------------------
// Function: Enumeration::Enumeration()
//-----------------------------------------------------------------------------
Enumeration::Enumeration(Enumeration const& other):
value_(other.value_),
    text_(other.text_),
    help_(other.help_)
{

}


//-----------------------------------------------------------------------------
// Function: Enumeration::~Enumeration()
//-----------------------------------------------------------------------------
Enumeration::~Enumeration()
{

}

//-----------------------------------------------------------------------------
// Function: Enumeration::write()
//-----------------------------------------------------------------------------
void Enumeration::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("spirit:enumeration");

    if (!text_.isEmpty())
    {
        writer.writeAttribute("spirit:text", text_);
    }

    if (!help_.isEmpty())
    {
        writer.writeAttribute("spirit:help", help_);
    }

    writer.writeCharacters(value_);

    writer.writeEndElement(); // spirit:enumeration
}

//-----------------------------------------------------------------------------
// Function: Enumeration::setValue()
//-----------------------------------------------------------------------------
void Enumeration::setValue(QString const& value)
{
    value_ = value;
}

//-----------------------------------------------------------------------------
// Function: Enumeration::getValue()
//-----------------------------------------------------------------------------
QString Enumeration::getValue() const
{
    return value_;
}
//-----------------------------------------------------------------------------
// Function: Enumeration::setText()
//-----------------------------------------------------------------------------
void Enumeration::setText(QString const& text)
{
    text_ = text;
}

//-----------------------------------------------------------------------------
// Function: Enumeration::getText()
//-----------------------------------------------------------------------------
QString Enumeration::getText() const
{
    return text_;
}

//-----------------------------------------------------------------------------
// Function: Enumeration::setHelp()
//-----------------------------------------------------------------------------
void Enumeration::setHelp(QString const& help)
{
    help_ = help;
}

//-----------------------------------------------------------------------------
// Function: Enumeration::getHelp()
//-----------------------------------------------------------------------------
QString Enumeration::getHelp() const
{
    return help_;
}
