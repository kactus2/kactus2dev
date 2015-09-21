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
ChannelReader::ChannelReader(QObject* parent /* = 0 */) :
QObject(parent), CommonItemsReader(parent)
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
	// Create the new CPU.
	QSharedPointer<Channel> newChannel (new Channel());

	// Parse presence, name group, and vendor extensions with pre-existing parsers.
	// CPU has no vendor extensions supported by Kactus2.
	parseIsPresent(channelNode, newChannel);
	parseNameGroup(channelNode, newChannel);

	// The intermediate list for parsed child nodes.
	QStringList busInterfaces;

	// Go through all the childnodes.
	for (int i = 0; i < channelNode.childNodes().count(); ++i)
	{
		QDomNode tempNode = channelNode.childNodes().at(i);

		// Get the bus interface refs.
		if (tempNode.nodeName() == QString("ipxact:busInterfaceRef"))
		{
			// Go through the child nodes of the ref.
			for (int j = 0; j < tempNode.childNodes().count(); ++j)
			{
				QDomNode nameNode = tempNode.childNodes().at(j);

				// Find the name of the reference.
				if (nameNode.nodeName() == QString("ipxact:localName"))
				{
					busInterfaces.append(nameNode.childNodes().at(0).nodeValue());
				}
			}
		}
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