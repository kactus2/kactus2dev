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
#include "XmlUtils.h"

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
ApiInterface::ApiInterface()
    : nameGroup_(),
      apiType_(),
      dependencyDir_(DEPENDENCY_PROVIDER),
      defaultPos_()
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(ApiInterface const& rhs)
    : nameGroup_(rhs.nameGroup_),
      apiType_(rhs.apiType_),
      dependencyDir_(rhs.dependencyDir_),
      defaultPos_(rhs.defaultPos_)
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(QDomNode& node)
    : nameGroup_(node),
      apiType_(),
      dependencyDir_(DEPENDENCY_PROVIDER),
      defaultPos_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "kactus2:apiType")
        {
            apiType_ = VLNV::createVLNV(childNode, VLNV::APIDEFINITION);
        }
        else if (childNode.nodeName() == "kactus2:dependencyDirection")
        {
            dependencyDir_ = str2DependencyDirection(childNode.childNodes().at(0).nodeValue(), DEPENDENCY_PROVIDER);
        }
        else if (childNode.nodeName() == "kactus2:position")
        {
            defaultPos_.setX(childNode.attributes().namedItem("x").nodeValue().toInt());
            defaultPos_.setY(childNode.attributes().namedItem("y").nodeValue().toInt());
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

    writer.writeTextElement("spirit:name", nameGroup_.name_);
    writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
    writer.writeTextElement("spirit:description", nameGroup_.description_);

    writer.writeEmptyElement("kactus2:apiType");
    apiType_.writeAsAttributes(writer);

    writer.writeTextElement("kactus2:dependencyDirection", dependencyDirection2Str(dependencyDir_));

    if (!defaultPos_.isNull())
    {
        XmlUtils::writePosition(writer, defaultPos_);
    }

    writer.writeEndElement(); // kactus2:apiInterface
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::isValid()
//-----------------------------------------------------------------------------
bool ApiInterface::isValid(QStringList& errorList, QString const& parentId) const
{
    QString const thisId = QObject::tr("API interface '%1'").arg(nameGroup_.name_);
    bool valid = true;

    if (nameGroup_.name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for an API interface in %1").arg(parentId));
        valid = false;
    }

    if (!apiType_.isEmpty() && !apiType_.isValid())
    {
        errorList.append(QObject::tr("Invalid API type set in %1").arg(thisId));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::isValid()
//-----------------------------------------------------------------------------
bool ApiInterface::isValid() const
{
    return (!nameGroup_.name_.isEmpty() && (apiType_.isEmpty() || apiType_.isValid()));
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setName()
//-----------------------------------------------------------------------------
void ApiInterface::setName(QString const& name)
{
    nameGroup_.name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDisplayName()
//-----------------------------------------------------------------------------
void ApiInterface::setDisplayName(QString const& displayName)
{
    nameGroup_.displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDescription()
//-----------------------------------------------------------------------------
void ApiInterface::setDescription(QString const& desc)
{
    nameGroup_.description_ = desc;
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
    return nameGroup_.name_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ApiInterface::getDisplayName() const
{
    return nameGroup_.displayName_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDescription()
//-----------------------------------------------------------------------------
QString const& ApiInterface::getDescription() const
{
    return nameGroup_.description_;
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
        nameGroup_.name_ = rhs.nameGroup_.name_;
        nameGroup_.displayName_ = rhs.nameGroup_.displayName_;
        nameGroup_.description_ = rhs.nameGroup_.description_;
        apiType_ = rhs.apiType_;
        dependencyDir_ = rhs.dependencyDir_;
    }

    return *this;
}

General::NameGroup& ApiInterface::getNameGroup() {
	return nameGroup_;
}

const General::NameGroup& ApiInterface::getNameGroup() const {
	return nameGroup_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDefaultPos()
//-----------------------------------------------------------------------------
void ApiInterface::setDefaultPos(QPointF const& pos)
{
    defaultPos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF const& ApiInterface::getDefaultPos() const
{
    return defaultPos_;
}
