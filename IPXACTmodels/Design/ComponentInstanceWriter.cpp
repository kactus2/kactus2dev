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
ComponentInstanceWriter::ComponentInstanceWriter(QObject* parent) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceWriter::ComponentInstanceWriter()
//-----------------------------------------------------------------------------
ComponentInstanceWriter::~ComponentInstanceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceWriter::writeComponentInstance()
//-----------------------------------------------------------------------------
void ComponentInstanceWriter::writeComponentInstance(QXmlStreamWriter& writer,
    QSharedPointer<ComponentInstance> instance) const
{
    writer.writeStartElement(QStringLiteral("ipxact:componentInstance"));
    writer.writeTextElement(QStringLiteral("ipxact:instanceName"), instance->getInstanceName());

    if (!instance->getDisplayName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:displayName"), instance->getDisplayName());
    }
    if (!instance->getDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), instance->getDescription());
    }

    writeIsPresent(writer, instance->getIsPresent());

    writeConfigurableVLNVReference(writer, instance->getComponentRef(), QStringLiteral("ipxact:componentRef"));

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

        foreach (QSharedPointer<ConfigurableElementValue> configurableElement,
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