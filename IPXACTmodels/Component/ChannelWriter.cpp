//-----------------------------------------------------------------------------
// File: ChannelWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Writer class for IP-XACT Channel element.
//-----------------------------------------------------------------------------

#include "ChannelWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: ChannelWriter::createChannelFrom()
//-----------------------------------------------------------------------------
void ChannelWriter::writeChannel(QXmlStreamWriter& writer, QSharedPointer<Channel> channel,
	Document::Revision docRevision)
{
	// Start the element, write name group and presence.
    writer.writeStartElement(QStringLiteral("ipxact:channel"));

    NameGroupWriter::writeNameGroup(writer, channel);

	if (docRevision == Document::Revision::Std14)
	{
        CommonItemsWriter::writeIsPresent(writer, channel->getIsPresent());
	}

	for (auto const& busInterfaceReference : *channel->getInterfaces())
	{
		writer.writeStartElement(QStringLiteral("ipxact:busInterfaceRef"));
		writer.writeTextElement(QStringLiteral("ipxact:localName"), busInterfaceReference->localName_);

		if (docRevision == Document::Revision::Std22)
		{
			CommonItemsWriter::writeVendorExtensions(writer, busInterfaceReference);
		}

		writer.writeEndElement();
	}

	if (docRevision == Document::Revision::Std22)
	{
        CommonItemsWriter::writeVendorExtensions(writer, channel);
	}

	writer.writeEndElement();
	return;
}
