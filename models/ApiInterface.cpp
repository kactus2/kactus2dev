//-----------------------------------------------------------------------------
// File: ApiInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.4.2012
//
// Description:
// API interface class for defining properties of a specific API interface
// in a SW component.
//-----------------------------------------------------------------------------

#include "ApiInterface.h"

#include "generaldeclarations.h"

//-----------------------------------------------------------------------------
// Function: str2DependencyDirection()
//-----------------------------------------------------------------------------
DependencyDirection str2DependencyDirection(QString const& str, DependencyDirection defaultValue)
{
    if (str == "provider")
    {
        return DEPENDENCY_PROVIDER;
    }
    else if (str == "requester")
    {
        return DEPENDENCY_REQUESTER;
    }
    else
    {
        return defaultValue;
    }
}

//-----------------------------------------------------------------------------
// Function: dependencyDirection2Str()
//-----------------------------------------------------------------------------
QString dependencyDirection2Str(DependencyDirection dir)
{
    switch (dir)
    {
    case DEPENDENCY_PROVIDER:
        return "provider";

    case DEPENDENCY_REQUESTER:
    default:
        return "requester";
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface() : name_(), displayName_(), desc_(), apiType_(),
                               dependencyDir_(DEPENDENCY_PROVIDER)
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(ApiInterface const& rhs) : name_(rhs.name_),
                                                      displayName_(rhs.displayName_),
                                                      desc_(rhs.desc_),
                                                      apiType_(rhs.apiType_),
                                                      dependencyDir_(rhs.dependencyDir_)
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(QDomNode& node) : name_(), displayName_(), desc_(), apiType_(),
                                             dependencyDir_(DEPENDENCY_PROVIDER)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "kactus2:name")
        {
            name_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:displayName")
        {
            displayName_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:description")
        {
            desc_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:apiType")
        {
            apiType_ = General::createVLNV(childNode, VLNV::APIDEFINITION);
        }
        else if (childNode.nodeName() == "kactus2:dependencyDirection")
        {
            dependencyDir_ = str2DependencyDirection(childNode.nodeValue(), DEPENDENCY_PROVIDER);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::~ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::~ApiInterface()
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterface::write()
//-----------------------------------------------------------------------------
void ApiInterface::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:apiInterface");

    writer.writeTextElement("kactus2:name", name_);
    writer.writeTextElement("kactus2:displayName", displayName_);
    writer.writeTextElement("kactus2:description", desc_);

    writer.writeEmptyElement("kactus2:apiType");
    General::writeVLNVAttributes(writer, &apiType_);

    writer.writeTextElement("kactus2:dependencyDirection", dependencyDirection2Str(dependencyDir_));

    writer.writeEndElement(); // kactus2:apiInterface
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::isValid()
//-----------------------------------------------------------------------------
bool ApiInterface::isValid(QStringList& errorList) const
{
    bool valid = true;

    if (name_.isEmpty())
    {
        errorList.append(QObject::tr("Mandatory name not found in an API interface"));
        valid = false;
    }

    if (!apiType_.isEmpty() && !apiType_.isValid())
    {
        errorList.append(QObject::tr("Invalid API type set in API interface '%1'").arg(name_));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::isValid()
//-----------------------------------------------------------------------------
bool ApiInterface::isValid() const
{
    return (!name_.isEmpty() && (apiType_.isEmpty() || apiType_.isValid()));
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setName()
//-----------------------------------------------------------------------------
void ApiInterface::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDisplayName()
//-----------------------------------------------------------------------------
void ApiInterface::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDescription()
//-----------------------------------------------------------------------------
void ApiInterface::setDescription(QString const& desc)
{
    desc_ = desc;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setApiType()
//-----------------------------------------------------------------------------
void ApiInterface::setApiType(VLNV const& vlnv)
{
    apiType_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDependencyDirection()
//-----------------------------------------------------------------------------
void ApiInterface::setDependencyDirection(DependencyDirection dir)
{
    dependencyDir_ = dir;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getName()
//-----------------------------------------------------------------------------
QString const& ApiInterface::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ApiInterface::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDescription()
//-----------------------------------------------------------------------------
QString const& ApiInterface::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getApiType()
//-----------------------------------------------------------------------------
VLNV const& ApiInterface::getApiType() const
{
    return apiType_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDependencyDirection()
//-----------------------------------------------------------------------------
DependencyDirection ApiInterface::getDependencyDirection() const
{
    return dependencyDir_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::operator=()
//-----------------------------------------------------------------------------
ApiInterface& ApiInterface::operator=(ApiInterface const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        apiType_ = rhs.apiType_;
        dependencyDir_ = rhs.dependencyDir_;
    }

    return *this;
}
