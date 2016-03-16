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
ProtocolReader::ProtocolReader(QObject* parent /* = 0 */) :
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
        QString protocolType = protocolNode.firstChildElement("ipxact:protocolType").firstChild().nodeValue();
        if (protocolType == "custom")
        {
            QDomElement protocolElement = protocolNode.firstChildElement("ipxact:protocolType");
            protocolType = protocolElement.attribute("custom");
        }
        protocol->setProtocolType(protocolType);

        QDomNode payloadNode = protocolNode.firstChildElement("ipxact:payload");

        protocol->setPayloadName(payloadNode.firstChildElement("ipxact:name").firstChild().nodeValue());
        protocol->setPayloadType(payloadNode.firstChildElement("ipxact:type").firstChild().nodeValue());

        bool mandatoryExtension = payloadNode.firstChildElement("ipxact:extension").attribute("mandatory") == "true";
        protocol->setPayloadExtension(payloadNode.firstChildElement("ipxact:extension").firstChild().nodeValue(),
            mandatoryExtension);

        parseVendorExtensions(payloadNode, protocol);
    }

    return protocol;
}


//-----------------------------------------------------------------------------
// Function: ProtocolReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void ProtocolReader::parseVendorExtensions(QDomNode const& payloadNode, QSharedPointer<Protocol> protocol) const
{
    QDomNodeList extensionNodes = payloadNode.firstChildElement("ipxact:vendorExtensions").childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNodes.at(i)));
        protocol->getVendorExtensions()->append(extension);
    }
}
