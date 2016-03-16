//-----------------------------------------------------------------------------
// File: ApiInterconnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating API connection data.
//-----------------------------------------------------------------------------

#include "ApiInterconnection.h"

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection() :
Interconnection(),
imported_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection(QString const& name, QString const& displayName, QString const& description,
                                       QSharedPointer<HierInterface> ref1, QSharedPointer<HierInterface> ref2,
                                       bool imported):
Interconnection(),
imported_(imported)
{
    setName(name);
    setDisplayName(displayName);
    setDescription(description);

    setInterface(ref1);
    setInterface(ref2);
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection(ApiInterconnection const& other) :
Interconnection(other),
imported_(other.imported_)
{

}

//-----------------------------------------------------------------------------
// Function: ApiDependency::ApiDependency()
//-----------------------------------------------------------------------------
ApiInterconnection::ApiInterconnection(QDomNode& node) :
Interconnection(),
imported_()
{
    setName(node.firstChildElement("ipxact:name").firstChild().nodeValue());
    setDisplayName(node.firstChildElement("ipxact:displayName").firstChild().nodeValue());
    setDescription(node.firstChildElement("ipxact:description").firstChild().nodeValue());

    QDomElement apiConnectionElement = node.toElement();

    if (!apiConnectionElement.isNull())
    {
        QDomNodeList apiInterfaceNodes = apiConnectionElement.elementsByTagName("kactus2:activeApiInterface");

        for (int connectionIndex = 0; connectionIndex < apiInterfaceNodes.count(); ++connectionIndex)
        {
            QDomNode activeInterfaceNode = apiInterfaceNodes.at(connectionIndex);
            QDomNamedNodeMap activeAttributes = activeInterfaceNode.attributes();
            QString componentReference = activeAttributes.namedItem("componentRef").nodeValue();
            QString apiReference = activeAttributes.namedItem("apiRef").nodeValue();

            QSharedPointer<ActiveInterface> apiInterface(new ActiveInterface(componentReference, apiReference));
            setInterface(apiInterface);
        }

        QDomElement hierInterfaceElement = apiConnectionElement.firstChildElement("kactus2:hierApiInterface");
        if (!hierInterfaceElement.isNull())
        {
            QString apiReference = hierInterfaceElement.attribute("apiRef");
            QSharedPointer<HierInterface> hierarchicalInterface (new HierInterface(apiReference));
            setInterface(hierarchicalInterface);
        }
    }

    QDomNode importedNode = node.firstChildElement("kactus2:imported");
    imported_ = !importedNode.isNull();
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::~ApiInterconnection()
//-----------------------------------------------------------------------------
ApiInterconnection::~ApiInterconnection()
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::clone()
//-----------------------------------------------------------------------------
ApiInterconnection* ApiInterconnection::clone() const
{
    return new ApiInterconnection(*this);
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::type()
//-----------------------------------------------------------------------------
QString ApiInterconnection::type() const
{
    return QString("kactus2:apiConnection");
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::write()
//-----------------------------------------------------------------------------
void ApiInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:apiConnection");

    writer.writeTextElement("ipxact:name", name());
    if (!displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", displayName());
    }
    if (!description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", description());
    }

    writer.writeEmptyElement("kactus2:activeApiInterface");
    writer.writeAttribute("componentRef", getStartInterface()->getComponentReference());
    writer.writeAttribute("apiRef", getStartInterface()->getBusReference());

    writeEndInterface(writer);

    if (imported_)
    {
        writer.writeEmptyElement("kactus2:imported");
    }

    writer.writeEndElement(); // kactus2:apiConnection
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::writeEndInterface()
//-----------------------------------------------------------------------------
void ApiInterconnection::writeEndInterface(QXmlStreamWriter& writer) const
{
    QSharedPointer<ActiveInterface> activeEndInterface = getEndInterface().dynamicCast<ActiveInterface>();
    if (activeEndInterface)
    {
        writer.writeEmptyElement("kactus2:activeApiInterface");
        writer.writeAttribute("componentRef", activeEndInterface->getComponentReference());
    }
    else
    {
        writer.writeEmptyElement("kactus2:hierApiInterface");
    }

    writer.writeAttribute("apiRef", getEndInterface()->getBusReference());
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::setInterface()
//-----------------------------------------------------------------------------
void ApiInterconnection::setInterface(QSharedPointer<HierInterface> interfaceReference)
{
    QSharedPointer<ActiveInterface> activeInterface = interfaceReference.dynamicCast<ActiveInterface>();
    if (activeInterface)
    {
        if (!getStartInterface())
        {
            setStartInterface(activeInterface);
        }
        else
        {
            if (!getActiveInterfaces()->isEmpty())
            {
                getActiveInterfaces()->clear();
            }

            getActiveInterfaces()->append(activeInterface);
        }
    }
    else
    {
        if (!getHierInterfaces()->isEmpty())
        {
            getHierInterfaces()->clear();
        }

        getHierInterfaces()->append(interfaceReference);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::setImported()
//-----------------------------------------------------------------------------
void ApiInterconnection::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::getEndInterface()
//-----------------------------------------------------------------------------
QSharedPointer<HierInterface> ApiInterconnection::getEndInterface() const
{
    if (!getActiveInterfaces()->isEmpty())
    {
        return getActiveInterfaces()->first();
    }
    else if (!getHierInterfaces()->isEmpty())
    {
        return getHierInterfaces()->first();
    }
    else
    {
        return QSharedPointer<ActiveInterface>(new ActiveInterface());
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::isImported()
//-----------------------------------------------------------------------------
bool ApiInterconnection::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterconnection::operator=()
//-----------------------------------------------------------------------------
ApiInterconnection& ApiInterconnection::operator=(ApiInterconnection const& other)
{
    if (&other != this)
    {
        Interconnection::operator=(other);
        imported_ = other.imported_;
    }

    return *this;
}
