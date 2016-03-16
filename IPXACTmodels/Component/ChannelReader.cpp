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

//-----------------------------------------------------------------------------
// Function: ChannelReader::ChannelReader()
//-----------------------------------------------------------------------------
ChannelReader::ChannelReader(QObject* parent) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ChannelReader::~ChannelReader()
//-----------------------------------------------------------------------------
ChannelReader::~ChannelReader()
{

}

//-----------------------------------------------------------------------------
// Function: ChannelReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Channel> ChannelReader::createChannelFrom(QDomNode const& channelNode) const
{
	// Create the new channel.
	QSharedPointer<Channel> newChannel (new Channel());

	// Parse presence and name group with pre-existing parsers.
	// Channel has no vendor extensions supported by Kactus2.
	parseIsPresent(channelNode, newChannel);
	parseNameGroup(channelNode, newChannel);

	// The intermediate list for parsed child nodes.
	QStringList busInterfaces;

    QDomNodeList interfaceReferenceNodes = channelNode.toElement().elementsByTagName("ipxact:busInterfaceRef");
    for (int i = 0; i < interfaceReferenceNodes.count(); i++)
    {
        QDomNode nameNode = interfaceReferenceNodes.at(i).toElement().firstChildElement("ipxact:localName");
            
        busInterfaces.append(nameNode.firstChild().nodeValue());
    }

	// Finally, set the parsed data as the channel data.
	newChannel->setInterfaces( busInterfaces );

    return newChannel;
}

//-----------------------------------------------------------------------------
// Function: ChannelReader::parseIsPresent()
//-----------------------------------------------------------------------------
void ChannelReader::parseIsPresent(QDomNode const& channelNode, QSharedPointer<Channel> newChannel) const
{
	QString newIsPresent = channelNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue();
	if (!newIsPresent.isEmpty())
	{
		newChannel->setIsPresent(newIsPresent);
	}
}

//-----------------------------------------------------------------------------
// Function: ChannelReader::parseNameGroup()
//-----------------------------------------------------------------------------
void ChannelReader::parseNameGroup(QDomNode const& channelNode, QSharedPointer<Channel> newChannel) const
{
    NameGroupReader nameReader;
    nameReader.parseNameGroup(channelNode, newChannel);
}
