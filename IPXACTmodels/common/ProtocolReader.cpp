//-----------------------------------------------------------------------------
// File: ProtocolReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.09.2015
//
// Description:
// Reader class for ipxact:protocl within component.
//-----------------------------------------------------------------------------

#include "ProtocolReader.h"
#include "Protocol.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: ProtocolReader::ProtocolReader()
//-----------------------------------------------------------------------------
ProtocolReader::ProtocolReader(QObject* parent) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ProtocolReader::~ProtocolReader()
//-----------------------------------------------------------------------------
ProtocolReader::~ProtocolReader()
{

}

//-----------------------------------------------------------------------------
// Function: ProtocolReader::createProtocolFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Protocol> ProtocolReader::createProtocolFrom(QDomNode const& protocolNode) const
{
    QSharedPointer<Protocol> protocol(new Protocol());

    if (!protocolNode.isNull())
    {
        QString protocolType = protocolNode.firstChildElement(
            QStringLiteral("ipxact:protocolType")).firstChild().nodeValue();
        if (protocolType == QLatin1String("custom"))
        {
            QDomElement protocolElement = protocolNode.firstChildElement(QStringLiteral("ipxact:protocolType"));
            protocolType = protocolElement.attribute(QStringLiteral("custom"));
        }
        protocol->setProtocolType(protocolType);

        QDomNode payloadNode = protocolNode.firstChildElement(QStringLiteral("ipxact:payload"));

        protocol->setPayloadName(payloadNode.firstChildElement(
            QStringLiteral("ipxact:name")).firstChild().nodeValue());
        protocol->setPayloadType(payloadNode.firstChildElement(
            QStringLiteral("ipxact:type")).firstChild().nodeValue());

        bool mandatoryExtension = payloadNode.firstChildElement(QStringLiteral("ipxact:extension")).attribute(
            QStringLiteral("mandatory")) == QLatin1String("true");
        protocol->setPayloadExtension(payloadNode.firstChildElement(
            QStringLiteral("ipxact:extension")).firstChild().nodeValue(), mandatoryExtension);

        parseVendorExtensions(payloadNode, protocol);
    }

    return protocol;
}


//-----------------------------------------------------------------------------
// Function: ProtocolReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void ProtocolReader::parseVendorExtensions(QDomNode const& payloadNode, QSharedPointer<Protocol> protocol) const
{
    QDomNodeList extensionNodes = payloadNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions")).childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNodes.at(i)));
        protocol->getVendorExtensions()->append(extension);
    }
}
