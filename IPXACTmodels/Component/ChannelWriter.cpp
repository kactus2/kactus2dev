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

//-----------------------------------------------------------------------------
// Function: ChannelWriter::ChannelWriter()
//-----------------------------------------------------------------------------
ChannelWriter::ChannelWriter(QObject* parent /* = 0 */) :
QObject(parent), CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ChannelWriter::~ChannelWriter()
//-----------------------------------------------------------------------------
ChannelWriter::~ChannelWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ChannelWriter::createChannelFrom()
//-----------------------------------------------------------------------------
void ChannelWriter::writeChannel(QXmlStreamWriter& writer, QSharedPointer<Channel> channel) const
{
	// Start the element, write name group, presence and, vendor extensions with pre-existing writers.
	writer.writeStartElement("ipxact:channel");
	writeNameGroup(writer, channel);
	writeIsPresent(writer, channel);

	// Acquire reference for bus interface reference list.
	const QStringList& busIfRefs = channel->getInterfaces();

	// Write all address spaces.
	for (int i = 0; i < busIfRefs.size(); ++i)
	{
		writer.writeStartElement("ipxact:busInterfaceRef");
		writer.writeTextElement("ipxact:localName", busIfRefs.at(i));
		writer.writeEndElement();
	}

	writer.writeEndElement();
	return;
}

//-----------------------------------------------------------------------------
// Function: ChannelWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void ChannelWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Channel> channel) const
{
	NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, channel);
}

//-----------------------------------------------------------------------------
// Function: ChannelWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void ChannelWriter::writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<Channel> channel) const
{
	if (!channel->getIsPresent().isEmpty())
	{
		writer.writeTextElement("ipxact:isPresent", channel->getIsPresent());
	}
}