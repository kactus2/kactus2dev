//-----------------------------------------------------------------------------
// File: ComponentInstanceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.09.2015
//
// Description:
// Writer class for IP-XACT component instance element.
//-----------------------------------------------------------------------------

#include "ComponentInstanceWriter.h"

//-----------------------------------------------------------------------------
// Function: ComponentInstanceWriter::ComponentInstanceWriter()
//-----------------------------------------------------------------------------
ComponentInstanceWriter::ComponentInstanceWriter() : CommonItemsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceWriter::writeComponentInstance()
//-----------------------------------------------------------------------------
void ComponentInstanceWriter::writeComponentInstance(QXmlStreamWriter& writer,
    QSharedPointer<ComponentInstance> instance, Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:componentInstance"));
    writer.writeTextElement(QStringLiteral("ipxact:instanceName"), instance->name());

    CommonItemsWriter::writeDisplayName(writer, instance->displayName());

    if (docRevision == Document::Revision::Std22)
    {
        CommonItemsWriter::writeShortDescription(writer, instance->shortDescription());
    }

    CommonItemsWriter::writeDescription(writer, instance->description());

    CommonItemsWriter::writeIsPresent(writer, instance->getIsPresent());

    writeConfigurableVLNVReference(writer, instance->getComponentRef(), QStringLiteral("ipxact:componentRef"));

    if (docRevision == Document::Revision::Std22)
    {
        writePowerDomainLinks(writer, instance);
    }

    writeVendorExtensions(writer, instance);

    writer.writeEndElement(); // ipxact:componentInstance
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceWriter::writeConfigurableVLNVReference()
//-----------------------------------------------------------------------------
void ComponentInstanceWriter::writeConfigurableVLNVReference(QXmlStreamWriter& writer,
    QSharedPointer<ConfigurableVLNVReference> VLNVreference, QString const& xmlElementName) const
{
    writer.writeStartElement(xmlElementName);

    writeVLNVAttributes(writer, *VLNVreference);

    if (!VLNVreference->getConfigurableElementValues()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:configurableElementValues"));

        for (QSharedPointer<ConfigurableElementValue> configurableElement :
            *VLNVreference->getConfigurableElementValues())
        {
            writer.writeStartElement(QStringLiteral("ipxact:configurableElementValue"));
            writer.writeAttribute(QStringLiteral("referenceId"), configurableElement->getReferenceId());
            writer.writeCharacters(configurableElement->getConfigurableValue());
            writer.writeEndElement(); // ipxact:configurableElementValue
        }
        writer.writeEndElement(); // ipxact:configurableElementValues
    }

    writer.writeEndElement(); // xmlElementName
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceWriter::writePowerDomainLinks()
//-----------------------------------------------------------------------------
void ComponentInstanceWriter::writePowerDomainLinks(QXmlStreamWriter& writer, 
    QSharedPointer<ComponentInstance> instance) const
{
    if (instance->getPowerDomainLinks()->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:powerDomainLinks"));
    for (auto const& link : *instance->getPowerDomainLinks())
    {
        writer.writeStartElement(QStringLiteral("ipxact:powerDomainLink"));

        writer.writeTextElement(QStringLiteral("ipxact:externalPowerDomainReference"), 
            QString::fromStdString(link->externalReference_));
        writer.writeTextElement(QStringLiteral("ipxact:internalPowerDomainReference"), 
            QString::fromStdString(link->internalReference_));

        writer.writeEndElement(); // ipxact:powerDomainLink
    }

    writer.writeEndElement(); // ipxact:powerDomainLinks
}
