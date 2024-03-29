//-----------------------------------------------------------------------------
// File: ApiInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 3.4.2012
//
// Description:
// API interface class for defining properties of a specific API interface
// in a SW component.
//-----------------------------------------------------------------------------

#include "ApiInterface.h"

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: str2DependencyDirection()
//-----------------------------------------------------------------------------
DependencyDirection str2DependencyDirection(QString const& str, DependencyDirection defaultValue)
{
    if (str == QLatin1String("provider"))
    {
        return DEPENDENCY_PROVIDER;
    }
    else if (str == QLatin1String("requester"))
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
    if (dir == DEPENDENCY_PROVIDER)
    {
        return QStringLiteral("provider");
    }
    else
    {
        return QStringLiteral("requester");
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface() :
NameGroup()
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(ApiInterface const& rhs) :
NameGroup(rhs),
apiType_(rhs.apiType_),
dependencyDir_(rhs.dependencyDir_),
defaultPos_(rhs.getDefaultPos())
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(QDomNode& node) :
NameGroup()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == QLatin1String("ipxact:name"))
        {
            setName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:displayName"))
        {
            setDisplayName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:shortDescription"))
        {
            setShortDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:description"))
        {
            setDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:apiType"))
        {
            apiType_ = CommonItemsReader::parseVLNVAttributes(childNode, VLNV::APIDEFINITION);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:dependencyDirection"))
        {
            dependencyDir_ = str2DependencyDirection(childNode.childNodes().at(0).nodeValue(), DEPENDENCY_PROVIDER);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:position"))
        {
            defaultPos_.parsePosition(childNode);
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
// Function: ApiInterface::clone()
//-----------------------------------------------------------------------------
ApiInterface* ApiInterface::clone() const
{
    return new ApiInterface(*this);
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::type()
//-----------------------------------------------------------------------------
QString ApiInterface::type() const
{
    return QStringLiteral("kactus2:apiInterface");
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::write()
//-----------------------------------------------------------------------------
void ApiInterface::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(QStringLiteral("kactus2:apiInterface"));

    writer.writeTextElement(QStringLiteral("ipxact:name"), name());

    CommonItemsWriter::writeDisplayName(writer, displayName());

    CommonItemsWriter::writeShortDescription(writer, shortDescription());

    CommonItemsWriter::writeDescription(writer, description());

    writer.writeEmptyElement(QStringLiteral("kactus2:apiType"));
    CommonItemsWriter::writeVLNVAttributes(writer, apiType_);

    writer.writeTextElement(QStringLiteral("kactus2:dependencyDirection"), dependencyDirection2Str(dependencyDir_));

    if (!defaultPos_.position().isNull())
    {
        defaultPos_.write(writer);
    }

    writer.writeEndElement(); // kactus2:apiInterface
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::isValid()
//-----------------------------------------------------------------------------
bool ApiInterface::isValid(QStringList& errorList, QString const& parentId) const
{
    QString const thisId = QObject::tr("API interface '%1'").arg(name());
    bool valid = true;

    if (name().isEmpty())
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
    return (!name().isEmpty() && (apiType_.isEmpty() || apiType_.isValid()));
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
        NameGroup::operator=(rhs);
        apiType_ = rhs.apiType_;
        dependencyDir_ = rhs.dependencyDir_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDefaultPos()
//-----------------------------------------------------------------------------
void ApiInterface::setDefaultPos(QPointF const& pos)
{
    defaultPos_.setPosition(pos);
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF ApiInterface::getDefaultPos() const
{
    return defaultPos_.position();
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::writePosition()
//-----------------------------------------------------------------------------
void ApiInterface::writePosition(QXmlStreamWriter& writer, QPointF const& pos) const
{
    writer.writeEmptyElement(QStringLiteral("kactus2:position"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(pos.x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(pos.y())));
}