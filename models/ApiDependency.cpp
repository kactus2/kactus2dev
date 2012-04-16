//-----------------------------------------------------------------------------
// File: ApiDependency.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------

#include "ApiDependency.h"

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::ApiDependency() : name_(), displayName_(), desc_(),
                                 providerRef_(), requesterRef_()
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::ApiDependency(ApiDependency const& rhs) : name_(rhs.name_),
                                                         displayName_(rhs.displayName_),
                                                         desc_(rhs.desc_),
                                                         providerRef_(rhs.providerRef_),
                                                         requesterRef_(rhs.requesterRef_)
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::ApiDependency(QDomNode& node) : name_(), displayName_(), desc_(),
                                               providerRef_(), requesterRef_()
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
            name_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "spirit:displayName")
        {
            displayName_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "spirit:description")
        {
            desc_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:provider")
        {
            providerRef_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
            providerRef_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:requester")
        {
            requesterRef_.componentRef = childNode.attributes().namedItem("kactus2:componentRef").nodeValue();
            requesterRef_.apiRef = childNode.attributes().namedItem("kactus2:apiRef").nodeValue();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::~ApiDependency()
//-----------------------------------------------------------------------------
ApiDependency::~ApiDependency()
{
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::write()
//-----------------------------------------------------------------------------
void ApiDependency::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:apiDependency");

    writer.writeTextElement("spirit:name", name_);
    writer.writeTextElement("spirit:displayName", displayName_);
    writer.writeTextElement("spirit:description", desc_);

    writer.writeEmptyElement("kactus2:provider");
    writer.writeAttribute("kactus2:componentRef", providerRef_.componentRef);
    writer.writeAttribute("kactus2:apiRef", providerRef_.apiRef);

    writer.writeEmptyElement("kactus2:requester");
    writer.writeAttribute("kactus2:componentRef", requesterRef_.componentRef);
    writer.writeAttribute("kactus2:apiRef", requesterRef_.apiRef);

    writer.writeEndElement(); // kactus2:apiDependency
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setName()
//-----------------------------------------------------------------------------
void ApiDependency::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setDisplayName()
//-----------------------------------------------------------------------------
void ApiDependency::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setDescription()
//-----------------------------------------------------------------------------
void ApiDependency::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setProviderRef()
//-----------------------------------------------------------------------------
void ApiDependency::setProviderRef(ApiInterfaceRef const& providerRef)
{
    providerRef_ = providerRef;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::setRequesterRef()
//-----------------------------------------------------------------------------
void ApiDependency::setRequesterRef(ApiInterfaceRef const& requesterRef)
{
    requesterRef_ = requesterRef;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getName()
//-----------------------------------------------------------------------------
QString const& ApiDependency::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ApiDependency::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getDescription()
//-----------------------------------------------------------------------------
QString const& ApiDependency::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getProviderRef()
//-----------------------------------------------------------------------------
ApiInterfaceRef const& ApiDependency::getProviderRef() const
{
    return providerRef_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::getRequesterRef()
//-----------------------------------------------------------------------------
ApiInterfaceRef const& ApiDependency::getRequesterRef() const
{
    return requesterRef_;
}

//-----------------------------------------------------------------------------
// Function: ApiDependency::operator=()
//-----------------------------------------------------------------------------
ApiDependency& ApiDependency::operator=(ApiDependency const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        providerRef_ = rhs.providerRef_;
        requesterRef_ = rhs.requesterRef_;
    }

    return *this;
}
