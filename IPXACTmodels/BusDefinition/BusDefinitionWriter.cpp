//-----------------------------------------------------------------------------
// File: BusDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Writer for IP-XACT BusDefinition element.
//-----------------------------------------------------------------------------

#include "BusDefinitionWriter.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/common/Choice.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeBusDefinition()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::writeBusDefinition(QXmlStreamWriter& writer, 
    QSharedPointer<BusDefinition> busDefinition)
{
    writer.writeStartDocument();
    
    DocumentWriter::writeTopComments(writer, busDefinition);

    DocumentWriter::writeXmlProcessingInstructions(writer, busDefinition);

    writer.writeStartElement(QStringLiteral("ipxact:busDefinition"));
    DocumentWriter::writeNamespaceDeclarations(writer, busDefinition);

    DocumentWriter::writeDocumentNameGroup(writer, busDefinition);

    writer.writeTextElement(QStringLiteral("ipxact:directConnection"), Details::bool2Str(busDefinition->getDirectConnection()));

    Details::writeBroadcast(writer, busDefinition);

    writer.writeTextElement(QStringLiteral("ipxact:isAddressable"), Details::bool2Str(busDefinition->getIsAddressable()));

    Details::writeExtends(writer, busDefinition);

    Details::writeMaximumInitiators(writer, busDefinition);
    Details::writeMaximumTargets(writer, busDefinition);

    Details::writeSystemGroupNames(writer, busDefinition);

    Details::writeDescription(writer, busDefinition);

    Details::writeChoices(writer, busDefinition);

    DocumentWriter::writeParameters(writer, busDefinition);

    DocumentWriter::writeAssertions(writer, busDefinition);

    DocumentWriter::writeVendorExtensions(writer, busDefinition);

    writer.writeEndElement(); // QStringLiteral("ipxact:busDefinition"
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeBroadcast()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::Details::writeBroadcast(QXmlStreamWriter& writer, 
    QSharedPointer<BusDefinition> busDefinition)
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
void BusDefinitionWriter::Details::writeExtends(QXmlStreamWriter& writer, QSharedPointer<BusDefinition> busDefinition)
{
    if (!busDefinition->getExtends().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:extends"));
        CommonItemsWriter::writeVLNVAttributes(writer, busDefinition->getExtends());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeMaximumInitiators()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::Details::writeMaximumInitiators(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition)
{
    auto const& maxInitiators = busDefinition->getMaxInitiators();

    if (maxInitiators.empty())
    {
        return;
    }

    if (busDefinition->getRevision() == Document::Revision::Std14)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxMasters"), QString::fromStdString(maxInitiators));
    }
    else if (busDefinition->getRevision() == Document::Revision::Std22)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxInitiators"), QString::fromStdString(maxInitiators));
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeMaximumTargets()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::Details::writeMaximumTargets(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition)
{
    auto const& maxTargets = busDefinition->getMaxTargets();

    if (maxTargets.empty())
    {
        return;
    }

    if (busDefinition->getRevision() == Document::Revision::Std14)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxSlaves"), QString::fromStdString(maxTargets));
    }
    else if (busDefinition->getRevision() == Document::Revision::Std22)
    {
        writer.writeTextElement(QStringLiteral("ipxact:maxTargets"), QString::fromStdString(maxTargets));
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeSystemGroupNames()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::Details::writeSystemGroupNames(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition)
{
    QStringList systemGroupNames = busDefinition->getSystemGroupNames();
    if (!systemGroupNames.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:systemGroupNames"));
        for (auto const& name : systemGroupNames)
        {
            writer.writeTextElement(QStringLiteral("ipxact:systemGroupName"), name);
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeChoices()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::Details::writeChoices(QXmlStreamWriter& writer, 
    QSharedPointer<BusDefinition> busDefinition)
{
    if (busDefinition->getRevision() != Document::Revision::Std14)
    {
        CommonItemsWriter::writeChoices(writer, busDefinition->getChoices());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::writeDescription()
//-----------------------------------------------------------------------------
void BusDefinitionWriter::Details::writeDescription(QXmlStreamWriter& writer,
    QSharedPointer<BusDefinition> busDefinition)
{
    if (busDefinition->getRevision() == Document::Revision::Std14)
    {
        CommonItemsWriter::writeDescription(writer, busDefinition->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionWriter::bool2Str()
//-----------------------------------------------------------------------------
QString BusDefinitionWriter::Details::bool2Str(bool value)
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


