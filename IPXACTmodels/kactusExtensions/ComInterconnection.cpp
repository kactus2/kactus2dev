//-----------------------------------------------------------------------------
// File: ComInterconnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#include "ComInterconnection.h"

//-----------------------------------------------------------------------------
// Function: ComInterconnection::ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::ComInterconnection() :
Interconnection()
{

}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::ComInterconnection(QString const& name, QString const& displayName, QString const& description,
                                       QSharedPointer<HierInterface> ref1, QSharedPointer<HierInterface> ref2):
Interconnection()
{
    setName(name);
    setDisplayName(displayName);
    setDescription(description);

    setInterface(ref1);
    setInterface(ref2);
}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::ComInterconnection(ComInterconnection const& rhs) :
Interconnection(rhs)
{

}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::ComInterconnection(QDomNode& connectionNode) :
Interconnection()
{
    QDomElement connectionElement = connectionNode.toElement();
    if (!connectionElement.isNull())
    {
        setName(connectionElement.firstChildElement("ipxact:name").firstChild().nodeValue());
        setDisplayName(connectionElement.firstChildElement("ipxact:displayName").firstChild().nodeValue());
        setDescription(connectionElement.firstChildElement("ipxact:description").firstChild().nodeValue());

        QDomNodeList interfaceList = connectionElement.elementsByTagName("kactus2:activeComInterface");
        for (int interfaceIndex = 0; interfaceIndex < interfaceList.count(); ++interfaceIndex)
        {
            QDomNode activeInterfaceNode = interfaceList.at(interfaceIndex);
            QDomNamedNodeMap startInterfaceAttributes = activeInterfaceNode.attributes();
            QString componentReference = startInterfaceAttributes.namedItem("componentRef").nodeValue();
            QString comReference = startInterfaceAttributes.namedItem("comRef").nodeValue();

            QSharedPointer<ActiveInterface> comInterface (new ActiveInterface(componentReference, comReference));
            setInterface(comInterface);
        }

        QDomElement hierInterfaceElement = connectionElement.firstChildElement("kactus2:hierComInterface");
        if (!hierInterfaceElement.isNull())
        {
            QString comReference = hierInterfaceElement.attribute("comRef");
            QSharedPointer<HierInterface> hierarchicalInterface (new HierInterface(comReference));
            setInterface(hierarchicalInterface);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::~ComInterconnection()
//-----------------------------------------------------------------------------
ComInterconnection::~ComInterconnection()
{

}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::clone()
//-----------------------------------------------------------------------------
ComInterconnection* ComInterconnection::clone() const
{
    return new ComInterconnection(*this);
}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::type()
//-----------------------------------------------------------------------------
QString ComInterconnection::type() const
{
    return QString("kactus2:comConnection");
}

//-----------------------------------------------------------------------------
// Function: ComConnection::write()
//-----------------------------------------------------------------------------
void ComInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:comConnection");

    writer.writeTextElement("ipxact:name", name());
    if (!displayName().isEmpty())
    {
        writer.writeTextElement("ipxact:displayName", displayName());
    }
    if (!description().isEmpty())
    {
        writer.writeTextElement("ipxact:description", description());
    }

    writer.writeEmptyElement("kactus2:activeComInterface");
    writer.writeAttribute("componentRef", getStartInterface()->getComponentReference());
    writer.writeAttribute("comRef", getStartInterface()->getBusReference());

    writeEndInterface(writer);

    writer.writeEndElement(); // kactus2:comConnection
}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::writeEndInterface()
//-----------------------------------------------------------------------------
void ComInterconnection::writeEndInterface(QXmlStreamWriter& writer) const
{
    QSharedPointer<ActiveInterface> activeEndInterface = getEndInterface().dynamicCast<ActiveInterface>();
    if (activeEndInterface)
    {
        writer.writeEmptyElement("kactus2:activeComInterface");
        writer.writeAttribute("componentRef", activeEndInterface->getComponentReference());
    }
    else
    {
        writer.writeEmptyElement("kactus2:hierComInterface");
    }

    writer.writeAttribute("comRef", getEndInterface()->getBusReference());
}

//-----------------------------------------------------------------------------
// Function: ComInterconnection::setInterface()
//-----------------------------------------------------------------------------
void ComInterconnection::setInterface(QSharedPointer<HierInterface> interfaceReference)
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
// Function: ComConnection::getInterface2()
//-----------------------------------------------------------------------------
QSharedPointer<HierInterface> ComInterconnection::getEndInterface() const
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
