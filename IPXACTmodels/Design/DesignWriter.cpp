//-----------------------------------------------------------------------------
// File: DesignWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Writer class for IP-XACT Design element.
//-----------------------------------------------------------------------------

#include "DesignWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Design/ComponentInstanceWriter.h>

//-----------------------------------------------------------------------------
// Function: DesignWriter::DesignWriter()
//-----------------------------------------------------------------------------
DesignWriter::DesignWriter() : DocumentWriter()
{

}


//-----------------------------------------------------------------------------
// Function: DesignWriter::writeDesign()
//-----------------------------------------------------------------------------
void DesignWriter::writeDesign(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    writer.writeStartDocument();

    writeDesignStart(writer, design);

    DocumentWriter::writeNamespaceDeclarations(writer, design);

    DocumentWriter::writeDocumentNameGroup(writer, design);

    writeComponentInstances(writer, design);

    writeInterconnections(writer, design);

    writeAdHocConnections(writer, design);
    
    if (design->getRevision() == Document::Revision::Std14)
    {
        CommonItemsWriter::writeDescription(writer, design->getDescription());
    }

    writeParameters(writer, design);

    writeAssertions(writer, design);

    CommonItemsWriter::writeVendorExtensions(writer, design);

    writer.writeEndElement(); // ipxact:design
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeDesignStart()
//-----------------------------------------------------------------------------
void DesignWriter::writeDesignStart(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    writeTopComments(writer, design);
    writeXmlProcessingInstructions(writer, design);
    writer.writeStartElement(QStringLiteral("ipxact:design"));
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeComponentInstances()
//-----------------------------------------------------------------------------
void DesignWriter::writeComponentInstances(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    if (design->getComponentInstances()->size() == 0)
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:componentInstances"));

    ComponentInstanceWriter instanceWriter;

    for (auto const& instance : *design->getComponentInstances())
    {
        instanceWriter.writeComponentInstance(writer, instance, design->getRevision());
    }

    writer.writeEndElement(); // ipxact:componentInstances
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeInterconnections()
//-----------------------------------------------------------------------------
void DesignWriter::writeInterconnections(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    if (design->getInterconnections()->isEmpty() && design->getMonitorInterconnecions()->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:interconnections"));

    for (auto const& currentInterconnection : *design->getInterconnections())
    {
        writeSingleInterconncetion(writer, currentInterconnection, design->getRevision());
    }

    for (auto const& connection : *design->getMonitorInterconnecions())
    {
        writeMonitorInterconnection(writer, connection, design->getRevision());
    }

    writer.writeEndElement(); // ipxact:interconnections
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeSingleInterconncetion()
//-----------------------------------------------------------------------------
void DesignWriter::writeSingleInterconncetion(QXmlStreamWriter& writer, 
    QSharedPointer<Interconnection> currentInterconnection, Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:interconnection"));

    NameGroupWriter::writeNameGroup(writer, currentInterconnection);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, currentInterconnection->getIsPresent());
    }

    writeActiveInterface(writer, currentInterconnection->getStartInterface(), docRevision);

    for (auto const& currentInterface: *currentInterconnection->getActiveInterfaces())
    {
        writeActiveInterface(writer, currentInterface, docRevision);
    }

    for (auto const& currentInterface : *currentInterconnection->getHierInterfaces())
    {
        writeHierInterface(writer, currentInterface);
    }

    CommonItemsWriter::writeVendorExtensions(writer, currentInterconnection);

    writer.writeEndElement(); // ipxact:interconnection
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeActiveInterface()
//-----------------------------------------------------------------------------
void DesignWriter::writeActiveInterface(QXmlStreamWriter& writer, QSharedPointer<ActiveInterface> activeInterface, 
    Document::Revision docRevision)
    const
{
    writer.writeStartElement(QStringLiteral("ipxact:activeInterface"));

    writer.writeAttribute(componentReferenceAttribute(docRevision), activeInterface->getComponentReference());
    writer.writeAttribute(QStringLiteral("busRef"), activeInterface->getBusReference());
    
    if (docRevision == Document::Revision::Std14)
    {
        writeIsPresent(writer, activeInterface->getIsPresent());
    }

    CommonItemsWriter::writeDescription(writer, activeInterface->getDescription());

    if (!activeInterface->getExcludePorts()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:excludePorts"));

        for (auto const& portName : *activeInterface->getExcludePorts())
        {
            writer.writeTextElement(QStringLiteral("ipxact:excludePort"), portName);
        }

        writer.writeEndElement(); // ipxact:excludePorts
    }

    CommonItemsWriter::writeVendorExtensions(writer, activeInterface);

    writer.writeEndElement(); //ipxact:activeInterface
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::componentReferenceAttribute()
//-----------------------------------------------------------------------------
QString DesignWriter::componentReferenceAttribute(Document::Revision docRevision) const
{
    auto referenceAttribute = QString();
    if (docRevision == Document::Revision::Std14)
    {
        referenceAttribute = QStringLiteral("componentRef");
    }
    else if (docRevision == Document::Revision::Std22)
    {
        referenceAttribute = QStringLiteral("componentInstanceRef");
    }

    return referenceAttribute;
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeHierInterface()
//-----------------------------------------------------------------------------
void DesignWriter::writeHierInterface(QXmlStreamWriter& writer, QSharedPointer<HierInterface> hierInterface) const
{
    writer.writeStartElement(QStringLiteral("ipxact:hierInterface"));

    writer.writeAttribute(QStringLiteral("busRef"), hierInterface->getBusReference());

    CommonItemsWriter::writeIsPresent(writer, hierInterface->getIsPresent());

    CommonItemsWriter::writeDescription(writer, hierInterface->getDescription());

    CommonItemsWriter::writeVendorExtensions(writer, hierInterface);

    writer.writeEndElement(); //ipxact:hierInterface
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeMonitorInterconnection()
//-----------------------------------------------------------------------------
void DesignWriter::writeMonitorInterconnection(QXmlStreamWriter& writer,
    QSharedPointer<MonitorInterconnection> monitorConnection, Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:monitorInterconnection"));

    NameGroupWriter::writeNameGroup(writer, monitorConnection);

    CommonItemsWriter::writeIsPresent(writer, monitorConnection->getIsPresent());

    writer.writeStartElement(QStringLiteral("ipxact:monitoredActiveInterface"));

    writeMonitorInterface(writer, monitorConnection->getMonitoredActiveInterface(), docRevision);

    writer.writeEndElement(); // ipxact:monitoredActiveInterface

    for (auto const& monitorInterface : *monitorConnection->getMonitorInterfaces())
    {
        writer.writeStartElement(QStringLiteral("ipxact:monitorInterface"));

        writeMonitorInterface(writer, monitorInterface, docRevision);

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeIsPresent(writer, monitorInterface->getIsPresent());
        }

        writer.writeEndElement(); // ipxact:monitorInterface
    }

    writer.writeEndElement(); // ipxact:monitorInterconnection
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeMonitorInterface()
//-----------------------------------------------------------------------------
void DesignWriter::writeMonitorInterface(QXmlStreamWriter& writer,
    QSharedPointer<MonitorInterface> monitorInterface, Document::Revision docRevision) const
{
    writer.writeAttribute(componentReferenceAttribute(docRevision), monitorInterface->getComponentReference());

    writer.writeAttribute(QStringLiteral("busRef"), monitorInterface->getBusReference());

    if (!monitorInterface->getPath().isEmpty())
    {
        writer.writeAttribute(QStringLiteral("path"), monitorInterface->getPath());
    }

    CommonItemsWriter::writeDescription(writer, monitorInterface->getDescription());

    CommonItemsWriter::writeVendorExtensions(writer, monitorInterface);
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeAdHocConnections()
//-----------------------------------------------------------------------------
void DesignWriter::writeAdHocConnections(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    if (design->getAdHocConnections()->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:adHocConnections"));

    for (auto const& connection : *design->getAdHocConnections())
    {
        writeSingleAdHocConnection(writer, connection, design->getRevision());
    }

    writer.writeEndElement(); // ipxact:adHocConnections
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeSingleAdHocConnection()
//-----------------------------------------------------------------------------
void DesignWriter::writeSingleAdHocConnection(QXmlStreamWriter& writer,
    QSharedPointer<AdHocConnection> adHocConnection, Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:adHocConnection"));

    NameGroupWriter::writeNameGroup(writer, adHocConnection);

    CommonItemsWriter::writeIsPresent(writer, adHocConnection->getIsPresent());

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:tiedValue"), adHocConnection->getTiedValue());

    writeAdHocPortReferences(writer, adHocConnection, docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, adHocConnection);

    writer.writeEndElement(); // ipxact:adHocConnection
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeAdHocPortReferences()
//-----------------------------------------------------------------------------
void DesignWriter::writeAdHocPortReferences(QXmlStreamWriter& writer,
    QSharedPointer<AdHocConnection> adHocConnection, Document::Revision docRevision) const
{
    if (adHocConnection->getInternalPortReferences()->isEmpty() &&
        adHocConnection->getExternalPortReferences()->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:portReferences"));

    for (auto const& internalRef : *adHocConnection->getInternalPortReferences())
    {
        writer.writeStartElement(QStringLiteral("ipxact:internalPortReference"));

        writer.writeAttribute(componentReferenceAttribute(docRevision), internalRef->getComponentRef());

        writePortReference(writer, internalRef, docRevision);

        writer.writeEndElement(); // ipxact:internalPortReference
    }

    for (auto const& externalRef : *adHocConnection->getExternalPortReferences())
    {
        writer.writeStartElement(QStringLiteral("ipxact:externalPortReference"));

        writePortReference(writer, externalRef, docRevision);

        writer.writeEndElement(); // ipxact:externalPortReference
    }

    writer.writeEndElement(); // ipxact:portReferences
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writePortReference()
//-----------------------------------------------------------------------------
void DesignWriter::writePortReference(QXmlStreamWriter& writer, QSharedPointer<PortReference> portReference,
    Document::Revision docRevision) const
{
    writer.writeAttribute(QStringLiteral("portRef"), portReference->getPortRef());

    if (docRevision == Document::Revision::Std14)
    {
        writeIsPresent(writer, portReference->getIsPresent());
    }

    if (docRevision == Document::Revision::Std22)
    {
        for (auto const& subPortReference : *portReference->getSubPortReferences())
        {
            writer.writeStartElement(QStringLiteral("ipxact:subPortReference"));
            writer.writeAttribute(QStringLiteral("subPortRef"), subPortReference->getPortRef());

            writePartSelect(writer, subPortReference->getPartSelect());

            writer.writeEndElement(); //ipxact:subPortReference
        }
    }

    writePartSelect(writer, portReference->getPartSelect());

}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writePartSelect()
//-----------------------------------------------------------------------------
void DesignWriter::writePartSelect(QXmlStreamWriter& writer, QSharedPointer<PartSelect> partSelect) const
{
    if (partSelect == nullptr)
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:partSelect"));

    writer.writeStartElement(QStringLiteral("ipxact:range"));

    writer.writeTextElement(QStringLiteral("ipxact:left"), partSelect->getLeftRange());
    writer.writeTextElement(QStringLiteral("ipxact:right"), partSelect->getRightRange());

    writer.writeEndElement(); //ipxact:range

    if (!partSelect->getIndices()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:indices"));

        for (auto const& index : *partSelect->getIndices())
        {
            writer.writeTextElement(QStringLiteral("ipxact:index"), index);
        }

        writer.writeEndElement(); // ipxact:indices
    }

    writer.writeEndElement(); // ipxact:partSelect
}