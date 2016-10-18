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
DesignWriter::DesignWriter(QObject* parent /* = 0 */) :
DocumentWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: DesignWriter::~DesignWriter()
//-----------------------------------------------------------------------------
DesignWriter::~DesignWriter()
{

}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeDesign()
//-----------------------------------------------------------------------------
void DesignWriter::writeDesign(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    writer.writeStartDocument();

    writeDesignStart(writer, design);

    writeNamespaceDeclarations(writer);

    writeVLNVElements(writer, design->getVlnv());

    writeComponentInstances(writer, design);

    writeInterconnections(writer, design);

    writeAdHocConnections(writer, design);

    writeDescription(writer, design);

    writeParameters(writer, design);

    writeAssertions(writer, design);

    writeVendorExtensions(writer, design);

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
    if (design->getComponentInstances()->size() > 0)
    {
        writer.writeStartElement(QStringLiteral("ipxact:componentInstances"));

        ComponentInstanceWriter instanceWriter;

        foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
        {
            instanceWriter.writeComponentInstance(writer, instance);
        }

        writer.writeEndElement(); // ipxact:componentInstances
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeInterconnections()
//-----------------------------------------------------------------------------
void DesignWriter::writeInterconnections(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    if (!design->getInterconnections()->isEmpty() || !design->getMonitorInterconnecions()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:interconnections"));

        foreach (QSharedPointer<Interconnection> currentInterconnection, *design->getInterconnections())
        {
            writeSingleInterconncetion(writer, currentInterconnection);
        }

        foreach (QSharedPointer<MonitorInterconnection> connection, *design->getMonitorInterconnecions())
        {
            writeMonitorInterconnection(writer, connection);
        }

        writer.writeEndElement(); // ipxact:interconnections
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeSingleInterconncetion()
//-----------------------------------------------------------------------------
void DesignWriter::writeSingleInterconncetion(
    QXmlStreamWriter& writer, QSharedPointer<Interconnection> currentInterconnection) const
{
    writer.writeStartElement(QStringLiteral("ipxact:interconnection"));

    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, currentInterconnection);

    writeIsPresent(writer, currentInterconnection->getIsPresent());

    writeActiveInterface(writer, currentInterconnection->getStartInterface());

    foreach (QSharedPointer<ActiveInterface> currentInterface,
        *currentInterconnection->getActiveInterfaces())
    {
        writeActiveInterface(writer, currentInterface);
    }

    foreach (QSharedPointer<HierInterface> currentInterface, *currentInterconnection->getHierInterfaces())
    {
        writeHierInterface(writer, currentInterface);
    }

    writeVendorExtensions(writer, currentInterconnection);

    writer.writeEndElement(); // ipxact:interconnection
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeActiveInterface()
//-----------------------------------------------------------------------------
void DesignWriter::writeActiveInterface(QXmlStreamWriter& writer, QSharedPointer<ActiveInterface> activeInterface)
    const
{
    writer.writeStartElement(QStringLiteral("ipxact:activeInterface"));

    writer.writeAttribute(QStringLiteral("componentRef"), activeInterface->getComponentReference());
    writer.writeAttribute(QStringLiteral("busRef"), activeInterface->getBusReference());
    writeIsPresent(writer, activeInterface->getIsPresent());

    if (!activeInterface->getDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), activeInterface->getDescription());
    }

    if (!activeInterface->getExcludePorts()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:excludePorts"));

        foreach (QString portName, *activeInterface->getExcludePorts())
        {
            writer.writeTextElement(QStringLiteral("ipxact:excludePort"), portName);
        }

        writer.writeEndElement(); // ipxact:excludePorts
    }

    writeVendorExtensions(writer, activeInterface);

    writer.writeEndElement(); //ipxact:activeInterface
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeHierInterface()
//-----------------------------------------------------------------------------
void DesignWriter::writeHierInterface(QXmlStreamWriter& writer, QSharedPointer<HierInterface> hierInterface) const
{
    writer.writeStartElement(QStringLiteral("ipxact:hierInterface"));

    writer.writeAttribute(QStringLiteral("busRef"), hierInterface->getBusReference());

    writeIsPresent(writer, hierInterface->getIsPresent());

    if (!hierInterface->getDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), hierInterface->getDescription());
    }

    writeVendorExtensions(writer, hierInterface);

    writer.writeEndElement(); //ipxact:hierInterface
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeMonitorInterconnection()
//-----------------------------------------------------------------------------
void DesignWriter::writeMonitorInterconnection(QXmlStreamWriter& writer,
    QSharedPointer<MonitorInterconnection> monitorConnection) const
{
    writer.writeStartElement(QStringLiteral("ipxact:monitorInterconnection"));

    NameGroupWriter monitorNamegroupWriter;
    monitorNamegroupWriter.writeNameGroup(writer, monitorConnection);

    writeIsPresent(writer, monitorConnection->getIsPresent());

    writer.writeStartElement(QStringLiteral("ipxact:monitoredActiveInterface"));

    writeMonitorInterface(writer, monitorConnection->getMonitoredActiveInterface());

    writer.writeEndElement(); // ipxact:monitoredActiveInterface

    foreach (QSharedPointer<MonitorInterface> monitorInterface, *monitorConnection->getMonitorInterfaces())
    {
        writer.writeStartElement(QStringLiteral("ipxact:monitorInterface"));

        writeMonitorInterface(writer, monitorInterface);

        writeIsPresent(writer, monitorInterface->getIsPresent());

        writer.writeEndElement(); // ipxact:monitorInterface
    }

    writer.writeEndElement(); // ipxact:monitorInterconnection
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeMonitorInterface()
//-----------------------------------------------------------------------------
void DesignWriter::writeMonitorInterface(QXmlStreamWriter& writer,
    QSharedPointer<MonitorInterface> monitorInterface) const
{
    writer.writeAttribute(QStringLiteral("componentRef"), monitorInterface->getComponentReference());
    writer.writeAttribute(QStringLiteral("busRef"), monitorInterface->getBusReference());

    if (!monitorInterface->getPath().isEmpty())
    {
        writer.writeAttribute(QStringLiteral("path"), monitorInterface->getPath());
    }

    if (!monitorInterface->getDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), monitorInterface->getDescription());
    }

    writeVendorExtensions(writer, monitorInterface);
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeAdHocConnections()
//-----------------------------------------------------------------------------
void DesignWriter::writeAdHocConnections(QXmlStreamWriter& writer, QSharedPointer<Design> design) const
{
    if (!design->getAdHocConnections()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:adHocConnections"));

        foreach (QSharedPointer<AdHocConnection> connection, *design->getAdHocConnections())
        {
            writeSingleAdHocConnection(writer, connection);
        }

        writer.writeEndElement(); // ipxact:adHocConnections
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeSingleAdHocConnection()
//-----------------------------------------------------------------------------
void DesignWriter::writeSingleAdHocConnection(QXmlStreamWriter& writer,
    QSharedPointer<AdHocConnection> adHocConnection) const
{
    writer.writeStartElement(QStringLiteral("ipxact:adHocConnection"));

    NameGroupWriter adHocNameWriter;
    adHocNameWriter.writeNameGroup(writer, adHocConnection);

    writeIsPresent(writer, adHocConnection->getIsPresent());

    if (!adHocConnection->getTiedValue().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:tiedValue"), adHocConnection->getTiedValue());
    }

    writeAdHocPortReferences(writer, adHocConnection);

    writeVendorExtensions(writer, adHocConnection);

    writer.writeEndElement(); // ipxact:adHocConnection
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writeAdHocPortReferences()
//-----------------------------------------------------------------------------
void DesignWriter::writeAdHocPortReferences(QXmlStreamWriter& writer,
    QSharedPointer<AdHocConnection> adHocConnection) const
{
    if (!adHocConnection->getInternalPortReferences()->isEmpty() || 
        !adHocConnection->getExternalPortReferences()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:portReferences"));

        foreach (QSharedPointer<PortReference> internalRef, *adHocConnection->getInternalPortReferences())
        {
            writer.writeStartElement(QStringLiteral("ipxact:internalPortReference"));

            writer.writeAttribute(QStringLiteral("componentRef"), internalRef->getComponentRef());
            writePortReference(writer, internalRef);

            writer.writeEndElement(); // ipxact:internalPortReference
        }

        foreach (QSharedPointer<PortReference> externalRef, *adHocConnection->getExternalPortReferences())
        {
            writer.writeStartElement(QStringLiteral("ipxact:externalPortReference"));

            writePortReference(writer, externalRef);

            writer.writeEndElement(); // ipxact:externalPortReference
        }

        writer.writeEndElement(); // ipxact:portReferences
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writePortReference()
//-----------------------------------------------------------------------------
void DesignWriter::writePortReference(QXmlStreamWriter& writer, QSharedPointer<PortReference> portRefernce) const
{
    writer.writeAttribute(QStringLiteral("portRef"), portRefernce->getPortRef());

    writeIsPresent(writer, portRefernce->getIsPresent());

    if (portRefernce->getPartSelect())
    {
        writePartSelect(writer, portRefernce->getPartSelect());
    }
}

//-----------------------------------------------------------------------------
// Function: DesignWriter::writePartSelect()
//-----------------------------------------------------------------------------
void DesignWriter::writePartSelect(QXmlStreamWriter& writer, QSharedPointer<PartSelect> partSelect) const
{
    writer.writeStartElement(QStringLiteral("ipxact:partSelect"));

    writer.writeStartElement(QStringLiteral("ipxact:range"));

    writer.writeTextElement(QStringLiteral("ipxact:left"), partSelect->getLeftRange());
    writer.writeTextElement(QStringLiteral("ipxact:right"), partSelect->getRightRange());

    writer.writeEndElement(); //ipxact:range

    if (!partSelect->getIndices()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:indices"));

        foreach (QString index, *partSelect->getIndices())
        {
            writer.writeTextElement(QStringLiteral("ipxact:index"), index);
        }

        writer.writeEndElement(); // ipxact:indices
    }

    writer.writeEndElement(); // ipxact:partSelect
}