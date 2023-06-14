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
// Function: DesignWriter::writeDesign()
//-----------------------------------------------------------------------------
void DesignWriter::writeDesign(QXmlStreamWriter& writer, QSharedPointer<Design> design)
{
    writer.writeStartDocument();

    Details::writeDesignStart(writer, design);

    DocumentWriter::writeNamespaceDeclarations(writer, design);

    DocumentWriter::writeDocumentNameGroup(writer, design);

    Details::writeComponentInstances(writer, design);

    Details::writeInterconnections(writer, design);

    Details::writeAdHocConnections(writer, design);
    
    if (design->getRevision() == Document::Revision::Std14)
    {
        CommonItemsWriter::writeDescription(writer, design->getDescription());
    }

    DocumentWriter::writeParameters(writer, design);

    DocumentWriter::writeAssertions(writer, design);

    CommonItemsWriter::writeVendorExtensions(writer, design);

    writer.writeEndElement(); // ipxact:design
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeDesignStart()
//-----------------------------------------------------------------------------
void DesignWriter::Details::writeDesignStart(QXmlStreamWriter& writer, QSharedPointer<Design> design)
{
    DocumentWriter::writeTopComments(writer, design);
    DocumentWriter::writeXmlProcessingInstructions(writer, design);
    writer.writeStartElement(QStringLiteral("ipxact:design"));
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeComponentInstances()
//-----------------------------------------------------------------------------
void DesignWriter::Details::writeComponentInstances(QXmlStreamWriter& writer, QSharedPointer<Design> design)
{
    if (design->getComponentInstances()->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:componentInstances"));

    for (auto const& instance : *design->getComponentInstances())
    {
        ComponentInstanceWriter::writeComponentInstance(writer, instance, design->getRevision());
    }

    writer.writeEndElement(); // ipxact:componentInstances
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeInterconnections()
//-----------------------------------------------------------------------------
void DesignWriter::Details::writeInterconnections(QXmlStreamWriter& writer, QSharedPointer<Design> design)
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
void DesignWriter::Details::writeSingleInterconncetion(QXmlStreamWriter& writer,
    QSharedPointer<Interconnection> currentInterconnection, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:interconnection"));

    NameGroupWriter::writeNameGroup(writer, currentInterconnection, docRevision);

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
void DesignWriter::Details::writeActiveInterface(QXmlStreamWriter& writer,
    QSharedPointer<ActiveInterface> activeInterface, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:activeInterface"));

    writer.writeAttribute(componentReferenceAttribute(docRevision), activeInterface->getComponentReference());
    writer.writeAttribute(QStringLiteral("busRef"), activeInterface->getBusReference());
    
    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, activeInterface->getIsPresent());
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
QString DesignWriter::Details::componentReferenceAttribute(Document::Revision docRevision)
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
void DesignWriter::Details::writeHierInterface(QXmlStreamWriter& writer, 
    QSharedPointer<HierInterface> hierInterface)
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
void DesignWriter::Details::writeMonitorInterconnection(QXmlStreamWriter& writer,
    QSharedPointer<MonitorInterconnection> monitorConnection, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:monitorInterconnection"));

    NameGroupWriter::writeNameGroup(writer, monitorConnection, docRevision);

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
void DesignWriter::Details::writeMonitorInterface(QXmlStreamWriter& writer,
    QSharedPointer<MonitorInterface> monitorInterface, Document::Revision docRevision)
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
void DesignWriter::Details::writeAdHocConnections(QXmlStreamWriter& writer, QSharedPointer<Design> design)
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
void DesignWriter::Details::writeSingleAdHocConnection(QXmlStreamWriter& writer,
    QSharedPointer<AdHocConnection> adHocConnection, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:adHocConnection"));

    NameGroupWriter::writeNameGroup(writer, adHocConnection, docRevision);

    CommonItemsWriter::writeIsPresent(writer, adHocConnection->getIsPresent());

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:tiedValue"), adHocConnection->getTiedValue());

    writeAdHocPortReferences(writer, adHocConnection, docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, adHocConnection);

    writer.writeEndElement(); // ipxact:adHocConnection
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeAdHocPortReferences()
//-----------------------------------------------------------------------------
void DesignWriter::Details::writeAdHocPortReferences(QXmlStreamWriter& writer,
    QSharedPointer<AdHocConnection> adHocConnection, Document::Revision docRevision)
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
void DesignWriter::Details::writePortReference(QXmlStreamWriter& writer, QSharedPointer<PortReference> portReference,
    Document::Revision docRevision)
{
    writer.writeAttribute(QStringLiteral("portRef"), portReference->getPortRef());

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, portReference->getIsPresent());
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
void DesignWriter::Details::writePartSelect(QXmlStreamWriter& writer, QSharedPointer<PartSelect> partSelect)
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