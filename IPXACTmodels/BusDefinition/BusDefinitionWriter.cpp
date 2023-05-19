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
#include <IPXACTmodels/Component/Choice.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::BusDefinitionWriter()
//-----------------------------------------------------------------------------
BusDefinitionWriter::BusDefinitionWriter(): DocumentWriter()
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

    writer.writeStartElement(QStringLiteral("ipxact:busDefinition"));
    writeNamespaceDeclarations(writer, busDefinition);

    writeDocumentNameGroup(writer, busDefinition);

    writer.writeTextElement(QStringLiteral("ipxact:directConnection"), bool2Str(busDefinition->getDirectConnection()));

    writeBroadcast(writer, busDefinition);

    writer.writeTextElement(QStringLiteral("ipxact:isAddressable"), bool2Str(busDefinition->getIsAddressable()));

    writeExtends(writer, busDefinition);

    writeMaximumInitiators(writer, busDefinition);
    writeMaximumTargets(writer, busDefinition);

    writeSystemGroupNames(writer, busDefinition);

    writeDescription(writer, busDefinition);

    writeChoices(writer, busDefinition);

    writeParameters(writer, busDefinition);

    writeAssertions(writer, busDefinition);

    writeVendorExtensions(writer, busDefinition);

    writer.writeEndElement(); // QStringLiteral("ipxact:busDefinition"
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
        writer.writeTextElement(QStringLiteral("ipxact:broadcast"), busDefinition->getBroadcast().toString());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeExtends()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeExtends(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const
{
    if (!busDefinition->getExtends().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:extends"));
        writeVLNVAttributes(writer, busDefinition->getExtends());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeMaximumInitiators()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeMaximumInitiators(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition) const
{
    auto const& maxInitiators = busDefinition->getMaxInitiators();

    if (maxInitiators.isEmpty())
    {
        return;
    }

    if (busDefinition->getRevision() == Document::Revision::Std14)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxMasters"), maxInitiators);
    }
    else if (busDefinition->getRevision() == Document::Revision::Std22)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxInitiators"), maxInitiators);
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeMaximumTargets()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeMaximumTargets(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition) const
{
    auto const& maxTargets = busDefinition->getMaxTargets();

    if (maxTargets.isEmpty())
    {
        return;
    }

    if (busDefinition->getRevision() == Document::Revision::Std14)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxSlaves"), maxTargets);
    }
    else if (busDefinition->getRevision() == Document::Revision::Std22)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxTargets"), maxTargets);
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
        writer.writeStartElement(QStringLiteral("ipxact:systemGroupNames"));
        foreach (QString name, systemGroupNames)
        {
            writer.writeTextElement(QStringLiteral("ipxact:systemGroupName"), name);
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeChoices()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeChoices(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition) const
{
    if (busDefinition->getRevision() != Document::Revision::Std14)
    {
        CommonItemsWriter::writeChoices(writer, busDefinition->getChoices());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::bool2Str()
//-----------------------------------------------------------------------------
QString BusDefinitionWriter::bool2Str(bool value) const
{
    if (value)
    {
        return QStringLiteral("true");
    }
    else
    {
        return QStringLiteral("false");
    }
}


