//-----------------------------------------------------------------------------
// File: ChannelReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Reader class for IP-XACT Channel element.
//-----------------------------------------------------------------------------

#include "ChannelReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: ChannelReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Channel> ChannelReader::createChannelFrom(QDomNode const& channelNode,
    Document::Revision docRevision)
{
	QSharedPointer<Channel> newChannel (new Channel());

	NameGroupReader::parseNameGroup(channelNode, newChannel);

    if (docRevision == Document::Revision::Std14)
    {
        newChannel->setIsPresent(CommonItemsReader::parseIsPresent(
            channelNode.firstChildElement(QStringLiteral("ipxact:isPresent"))));
    }

	Details::parseInterfaceReferences(channelNode, newChannel, docRevision);

    if (docRevision == Document::Revision::Std22)
    {
        CommonItemsReader::parseVendorExtensions(channelNode, newChannel);
    }

    return newChannel;
}

//-----------------------------------------------------------------------------
// Function: ChannelReader::Details::parseInterfaceReferences()
//-----------------------------------------------------------------------------
void ChannelReader::Details::parseInterfaceReferences(QDomNode const& channelNode, 
    QSharedPointer<Channel> newChannel, Document::Revision docRevision)
{
    QDomNodeList interfaceReferenceNodes = 
        channelNode.toElement().elementsByTagName(QStringLiteral("ipxact:busInterfaceRef"));

    const int REFERENCE_COUNT = interfaceReferenceNodes.count();
    for (int i = 0; i < REFERENCE_COUNT; i++)
    {
        QDomNode referenceNode = interfaceReferenceNodes.at(i);
        QDomNode nameNode = referenceNode.toElement().firstChildElement(QStringLiteral("ipxact:localName"));
        auto localName = nameNode.firstChild().nodeValue();

        QSharedPointer<Channel::BusInterfaceRef> interfaceRef(new Channel::BusInterfaceRef(localName));

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(referenceNode, interfaceRef);
        }

        newChannel->getInterfaces()->append(interfaceRef);
    }
}
