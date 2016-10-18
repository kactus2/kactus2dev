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
        setName(connectionElement.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue());
        setDisplayName(connectionElement.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
        setDescription(connectionElement.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());

        QDomNodeList interfaceList = connectionElement.elementsByTagName(QStringLiteral("kactus2:activeComInterface"));
        for (int interfaceIndex = 0; interfaceIndex < interfaceList.count(); ++interfaceIndex)
        {
            QDomNode activeInterfaceNode = interfaceList.at(interfaceIndex);
            QDomNamedNodeMap startInterfaceAttributes = activeInterfaceNode.attributes();
            QString componentReference = startInterfaceAttributes.namedItem(QStringLiteral("componentRef")).nodeValue();
            QString comReference = startInterfaceAttributes.namedItem(QStringLiteral("comRef")).nodeValue();

            QSharedPointer<ActiveInterface> comInterface (new ActiveInterface(componentReference, comReference));
            setInterface(comInterface);
        }

        QDomElement hierInterfaceElement = connectionElement.firstChildElement(QStringLiteral("kactus2:hierComInterface"));
        if (!hierInterfaceElement.isNull())
        {
            QString comReference = hierInterfaceElement.attribute(QStringLiteral("comRef"));
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
    return QStringLiteral("kactus2:comConnection");
}

//-----------------------------------------------------------------------------
// Function: ComConnection::write()
//-----------------------------------------------------------------------------
void ComInterconnection::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(QStringLiteral("kactus2:comConnection"));

    writer.writeTextElement(QStringLiteral("ipxact:name"), name());
    if (!displayName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:displayName"), displayName());
    }
    if (!description().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), description());
    }

    writer.writeEmptyElement(QStringLiteral("kactus2:activeComInterface"));
    writer.writeAttribute(QStringLiteral("componentRef"), getStartInterface()->getComponentReference());
    writer.writeAttribute(QStringLiteral("comRef"), getStartInterface()->getBusReference());

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
        writer.writeEmptyElement(QStringLiteral("kactus2:activeComInterface"));
        writer.writeAttribute(QStringLiteral("componentRef"), activeEndInterface->getComponentReference());
    }
    else
    {
        writer.writeEmptyElement(QStringLiteral("kactus2:hierComInterface"));
    }

    writer.writeAttribute(QStringLiteral("comRef"), getEndInterface()->getBusReference());
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
