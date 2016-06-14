//-----------------------------------------------------------------------------
// File: BusDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Writer class for IP-XACT BusDefinition element.
//-----------------------------------------------------------------------------

#include "BusDefinitionWriter.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::BusDefinitionWriter()
//-----------------------------------------------------------------------------
BusDefinitionWriter::BusDefinitionWriter(QObject *parent): DocumentWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::~BusDefinitionWriter()
//-----------------------------------------------------------------------------
BusDefinitionWriter::~BusDefinitionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeBusDefinition()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeBusDefinition(QXmlStreamWriter& writer, 
    QSharedPointer<BusDefinition> busDefinition) const
{
    writer.writeStartDocument();
    
    writeTopComments(writer, busDefinition);

    writeXmlProcessingInstructions(writer, busDefinition);

    writer.writeStartElement("ipxact:busDefinition");
    writeNamespaceDeclarations(writer);


    writeVLNVElements(writer, busDefinition->getVlnv());

    writer.writeTextElement("ipxact:directConnection", bool2Str(busDefinition->getDirectConnection()));

    writeBroadcast(writer, busDefinition);

    writer.writeTextElement("ipxact:isAddressable", bool2Str(busDefinition->getIsAddressable()));

    writeExtends(writer, busDefinition);

    writeMaximumMasters(writer, busDefinition);
    writeMaximumSlaves(writer, busDefinition);

    writeSystemGroupNames(writer, busDefinition);

    writeDescription(writer, busDefinition);

    writeParameters(writer, busDefinition);

    writeAssertions(writer, busDefinition);

    writeVendorExtensions(writer, busDefinition);

    writer.writeEndElement(); // "ipxact:busDefinition"
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeBroadcast()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeBroadcast(QXmlStreamWriter& writer, 
    QSharedPointer<BusDefinition> busDefinition) const
{
    BooleanValue broadcastValue = busDefinition->getBroadcast();
    if (!broadcastValue.toString().isEmpty())
    {
        writer.writeTextElement("ipxact:broadcast", busDefinition->getBroadcast().toString());
    }
}


//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeExtends()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeExtends(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const
{
    if (!busDefinition->getExtends().isEmpty())
    {
        writer.writeStartElement("ipxact:extends");
        writeVLNVAttributes(writer, busDefinition->getExtends());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeMaximumMasters()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeMaximumMasters(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition) const
{
    if (!busDefinition->getMaxMasters().isEmpty())
    {
        writer.writeTextElement("ipxact:maxMasters", busDefinition->getMaxMasters());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeMaximumSlaves()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeMaximumSlaves(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition) const
{
    if (!busDefinition->getMaxSlaves().isEmpty())
    {
        writer.writeTextElement("ipxact:maxSlaves", busDefinition->getMaxSlaves());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeSystemGroupNames()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeSystemGroupNames(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QStringList systemGroupNames = busDefinition->getSystemGroupNames();
    if (!systemGroupNames.isEmpty())
    {
        writer.writeStartElement("ipxact:systemGroupNames");
        foreach (QString name, systemGroupNames)
        {
            writer.writeTextElement("ipxact:systemGroupName", name);
        }
        writer.writeEndElement();
    }
}


//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::bool2Str()
//-----------------------------------------------------------------------------
QString BusDefinitionWriter::bool2Str(bool value) const
{
    if (value)
    {
        return QString("true");
    }
    else
    {
        return QString("false");
    }
}


