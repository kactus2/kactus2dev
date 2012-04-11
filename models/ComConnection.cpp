//-----------------------------------------------------------------------------
// File: ComConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#include "ComConnection.h"

//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection() : name_(), displayName_(), desc_(),
                                 interface1_(), interface2_()
{
}

//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection(ComConnection const& rhs) : name_(rhs.name_),
                                                         displayName_(rhs.displayName_),
                                                         desc_(rhs.desc_),
                                                         interface1_(rhs.interface1_),
                                                         interface2_(rhs.interface2_)
{
}

//-----------------------------------------------------------------------------
// Function: ComConnection::ComConnection()
//-----------------------------------------------------------------------------
ComConnection::ComConnection(QDomNode& node) : name_(), displayName_(), desc_(),
                                               interface1_(), interface2_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "spirit:name")
        {
            name_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "spirit:displayName")
        {
            displayName_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "spirit:description")
        {
            desc_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:activeComInterface")
        {
            if (interface1_.componentRef.isNull())
            {
                interface1_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                interface1_.comRef = childNode.attributes().namedItem("kactus2:comRef").nodeValue();
            }
            else
            {
                interface2_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
                interface2_.comRef = childNode.attributes().namedItem("kactus2:comRef").nodeValue();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComConnection::~ComConnection()
//-----------------------------------------------------------------------------
ComConnection::~ComConnection()
{
}

//-----------------------------------------------------------------------------
// Function: ComConnection::write()
//-----------------------------------------------------------------------------
void ComConnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:comConnection");

    writer.writeTextElement("spirit:name", name_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("kactus2:componentRef", interface1_.componentRef);
    writer.writeAttribute("kactus2:comRef", interface1_.comRef);

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("kactus2:componentRef", interface2_.componentRef);
    writer.writeAttribute("kactus2:comRef", interface2_.comRef);

    writer.writeEndElement(); // kactus2:comConnection
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setName()
//-----------------------------------------------------------------------------
void ComConnection::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setDisplayName()
//-----------------------------------------------------------------------------
void ComConnection::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::setDescription()
//-----------------------------------------------------------------------------
void ComConnection::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getName()
//-----------------------------------------------------------------------------
QString const& ComConnection::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ComConnection::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::getDescription()
//-----------------------------------------------------------------------------
QString const& ComConnection::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ComConnection::operator=()
//-----------------------------------------------------------------------------
ComConnection& ComConnection::operator=(ComConnection const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        interface1_ = rhs.interface1_;
        interface2_ = rhs.interface2_;
    }

    return *this;
}
