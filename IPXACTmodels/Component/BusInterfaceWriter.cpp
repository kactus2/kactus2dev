//-----------------------------------------------------------------------------
// File: businterfaceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Writer class for IP-XACT businterface element.
//-----------------------------------------------------------------------------

#include "BusInterfaceWriter.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

#include <QStringBuilder>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writebusinterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeBusInterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface)
{
	writer.writeStartElement(QStringLiteral("ipxact:busInterface"));

	// Write attributes to the starting element.	
    CommonItemsWriter::writeAttributes(writer, businterface->getAttributes());
	    
	// Start the element, write name group, presence and, vendor extensions with pre-existing writers.
    NameGroupWriter::writeNameGroup(writer, businterface);
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), businterface->getIsPresent());

    // Write always bus type, connection requirement the bits in lau, and endianess.
    writer.writeEmptyElement(QStringLiteral("ipxact:busType"));
    CommonItemsWriter::writeVLNVAttributes(writer, businterface->getBusType());

    Details::writeAbstractionTypes(writer, businterface);

    Details::writeInterfaceMode(writer, businterface);

    Details::writeConnectionRequired(writer, businterface);

    Details::writeBitsInLau(writer, businterface);

    Details::writeBitSteering(writer, businterface);

    Details::writeEndianness(writer, businterface);

	CommonItemsWriter::writeParameters(writer, businterface->getParameters());

    CommonItemsWriter::writeVendorExtensions(writer, businterface);

	writer.writeEndElement(); // ipxact:busInterface
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeConnectionRequired()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeConnectionRequired(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> busInterface) 
{
    CommonItemsWriter::writeNonEmptyElement(writer,
        QStringLiteral("ipxact:connectionRequired"), busInterface->getConnectionRequired());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeBitsInLau()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeBitsInLau(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> busInterface)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:bitsInLau"),
        busInterface->getBitsInLau());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeEndianness()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeEndianness(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface)
{
    BusInterface::Endianness endianess = busInterface->getEndianness();
    if (endianess == BusInterface::BIG)
    {
        writer.writeTextElement(QStringLiteral("ipxact:endianness"), QStringLiteral("big"));
    }
    else if (endianess == BusInterface::LITTLE)
    {
        writer.writeTextElement(QStringLiteral("ipxact:endianness"), QStringLiteral("little"));
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeBitSteering()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeBitSteering(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface) 
{
    BusInterface::BitSteering bitSteering = businterface->getBitSteering();

	// If the bitSteering has been defined.
	if (bitSteering != BusInterface::BITSTEERING_UNSPECIFIED)
	{
		writer.writeStartElement(QStringLiteral("ipxact:bitSteering"));

		CommonItemsWriter::writeAttributes(writer, businterface->getBitSteeringAttributes());

		// Write the value of the element and close the tag.
        if (bitSteering == BusInterface::BITSTEERING_ON)
        {
            writer.writeCharacters(QStringLiteral("on"));
        }
        else if (bitSteering == BusInterface::BITSTEERING_OFF)
        {
            writer.writeCharacters(QStringLiteral("off"));
        }

		writer.writeEndElement(); // ipxact:bitSteering
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeAbstractionTypes()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeAbstractionTypes(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface)
{
    if (!businterface->getAbstractionTypes()->isEmpty())
	{
		writer.writeStartElement(QStringLiteral("ipxact:abstractionTypes"));

		for (QSharedPointer<AbstractionType> const& abstractionType : *businterface->getAbstractionTypes())
		{
			writer.writeStartElement(QStringLiteral("ipxact:abstractionType"));

            for (QString const& viewReference : *abstractionType->getViewReferences())
            {
                writer.writeTextElement(QStringLiteral("ipxact:viewRef"), viewReference);
            }

            if (abstractionType->getAbstractionRef())
            {
                writer.writeEmptyElement(QStringLiteral("ipxact:abstractionRef"));
                CommonItemsWriter::writeVLNVAttributes(writer, *abstractionType->getAbstractionRef());
            }

            writePortMaps(writer, abstractionType);

			writer.writeEndElement(); // ipxact:abstractionType
		}

		writer.writeEndElement(); // ipxact:abstractionTypes
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writePortMaps()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writePortMaps(QXmlStreamWriter& writer,
    QSharedPointer<AbstractionType> abstractionType)
{
    if (!abstractionType->getPortMaps()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:portMaps"));

        for (QSharedPointer<PortMap> const& portMap : *abstractionType->getPortMaps())
        {
            writer.writeStartElement(QStringLiteral("ipxact:portMap"));

            CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("invert"), portMap->getInvert().toString());

            CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), portMap->getIsPresent());

            // Write ports for the port map.
            if (portMap->getLogicalPort())
            {
                writeLogicalPort(writer, portMap->getLogicalPort());
            }
            if (portMap->getPhysicalPort())
            {
                writePhysicalPort(writer, portMap->getPhysicalPort());
            }

            CommonItemsWriter::writeNonEmptyElement(writer,
                QStringLiteral("ipxact:logicalTieOff"), portMap->getLogicalTieOff());
            CommonItemsWriter::writeNonEmptyElement(writer,
                QStringLiteral("ipxact:isInformative"), portMap->getIsInformative().toString());

            writer.writeEndElement();
        }

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeLogicalPort()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeLogicalPort(QXmlStreamWriter& writer,
    QSharedPointer<PortMap::LogicalPort> logicalPort)
{
    writer.writeStartElement(QStringLiteral("ipxact:logicalPort"));
    writer.writeTextElement(QStringLiteral("ipxact:name"), logicalPort->name_);

    // Write range of the logical port.
    QSharedPointer<Range> logicalRange = logicalPort->range_;
    if (logicalRange && (logicalRange->getLeft().isEmpty() == false || logicalRange->getRight().isEmpty() == false))
    {
        writer.writeStartElement(QStringLiteral("ipxact:range"));
        writer.writeTextElement(QStringLiteral("ipxact:left"), logicalPort->range_->getLeft());
        writer.writeTextElement(QStringLiteral("ipxact:right"), logicalPort->range_->getRight());
        writer.writeEndElement();
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writePhysicalPort()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writePhysicalPort(QXmlStreamWriter& writer,
    QSharedPointer<PortMap::PhysicalPort> physicalPort)
{
    writer.writeStartElement(QStringLiteral("ipxact:physicalPort"));
    writer.writeTextElement(QStringLiteral("ipxact:name"), physicalPort->name_);

    QSharedPointer<PartSelect> partSelect = physicalPort->partSelect_;

    // Part select exists: Write its range.
    if (partSelect)
    {
        writer.writeStartElement(QStringLiteral("ipxact:partSelect"));

        writer.writeStartElement(QStringLiteral("ipxact:range"));
        writer.writeTextElement(QStringLiteral("ipxact:left"), partSelect->getLeftRange());
        writer.writeTextElement(QStringLiteral("ipxact:right"), partSelect->getRightRange());
        writer.writeEndElement();

        // Write all indices of the part select.
        if (partSelect->getIndices()->isEmpty() == false)
        {
            writer.writeStartElement(QStringLiteral("ipxact:indices"));

            for (QString const& index : *partSelect->getIndices())
            {
                writer.writeTextElement(QStringLiteral("ipxact:index"), index);
            }

            writer.writeEndElement(); // ipxact:indices
        }

        writer.writeEndElement(); // ipxact:partSelect
    }

    writer.writeEndElement(); // ipxact:physicalPort
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeInterfaceMode()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeInterfaceMode(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface)
{
    General::InterfaceMode interfaceMode = businterface->getInterfaceMode();
    if (interfaceMode == General::INTERFACE_MODE_COUNT)
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:") % General::interfaceMode2Str(interfaceMode));

	// Select the correct writer function for the interface mode.
    if (interfaceMode == General::MASTER)
    {
        writeMasterInterface(writer, businterface->getMaster());
    }       
    // Mirrored master has only the element, no content.
    else if (interfaceMode == General::SLAVE)
    {
        writeSlaveInterface(writer, businterface->getSlave());
    }
    else if (interfaceMode == General::MIRROREDSLAVE)
    {
        writeMirroredSlaveInterface(writer, businterface->getMirroredSlave());
    }
    else if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRROREDSYSTEM)
    {        
        writer.writeTextElement(QStringLiteral("ipxact:group"), businterface->getSystem());        
    }
    else if (interfaceMode == General::MONITOR)
    {
        writeMonitorInterface(writer, businterface);
    }

    writer.writeEndElement(); // ipxact:<mode>
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMasterInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeMasterInterface(QXmlStreamWriter& writer,
    QSharedPointer<MasterInterface> masterInterface)
{
    if (masterInterface == nullptr || 
        (masterInterface->getAddressSpaceRef().isEmpty() && masterInterface->getBaseAddress().isEmpty()))
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:addressSpaceRef"));

    // Write address space reference if it exists.            
    CommonItemsWriter::writeNonEmptyAttribute(writer,
        QStringLiteral("addressSpaceRef"), masterInterface->getAddressSpaceRef());

    CommonItemsWriter::writeNonEmptyElement(writer,
        QStringLiteral("ipxact:isPresent"), masterInterface->getIsPresent());

    // Write base address if it exists.
    if (!masterInterface->getBaseAddress().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:baseAddress"));

        // Write prompt attribute if it exist.
        CommonItemsWriter::writeNonEmptyAttribute(writer,
            QStringLiteral("ipxact::prompt"), masterInterface->getPrompt());

        // Write the value of the baseAddress element and close the tag.
        writer.writeCharacters(masterInterface->getBaseAddress());
        writer.writeEndElement(); // ipxact:baseAddress
    }

    writer.writeEndElement(); // ipxact:addressSpaceRef
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeSlaveInterface(QXmlStreamWriter& writer, QSharedPointer<SlaveInterface> slave)
{
    if (slave == nullptr)
    {
        return;
    }

    // Write memory map reference, along its attributes.
    if (!slave->getMemoryMapRef().isEmpty())
    {
        writer.writeEmptyElement(QStringLiteral("ipxact:memoryMapRef"));
        writer.writeAttribute(QStringLiteral("memoryMapRef"), slave->getMemoryMapRef());
    }

    // Write all bridges.
    for (QSharedPointer<TransparentBridge> const& bridge : *slave->getBridges())
    {
        // Bridge has a master reference.
        writer.writeStartElement(QStringLiteral("ipxact:transparentBridge"));
        writer.writeAttribute(QStringLiteral("masterRef"), bridge->getMasterRef());

        CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), bridge->getIsPresent());

        writer.writeEndElement();
    }

    // Write file set reference groups.
    for (QSharedPointer<SlaveInterface::FileSetRefGroup> const& group : *slave->getFileSetRefGroup())
    {
        writer.writeStartElement(QStringLiteral("ipxact:fileSetRefGroup"));

        // Write the name of the group.
        writer.writeTextElement(QStringLiteral("ipxact:group"), group->group_);

        // Write all the fileSetRefs for this group.
        for (QString const& filesetRef : group->fileSetRefs_)
        {
            writer.writeTextElement(QStringLiteral("ipxact:fileSetRef"), filesetRef);
        }

        writer.writeEndElement(); // ipxact:fileSetRefGroup
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeMirroredSlaveInterface(QXmlStreamWriter& writer,
    QSharedPointer<MirroredSlaveInterface> mirroredSlave)
{
    if (mirroredSlave == nullptr || mirroredSlave->getRange().isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:baseAddresses"));

    // Write all remap addresses.
    for (QSharedPointer<MirroredSlaveInterface::RemapAddress> const& remapAddress :
        *mirroredSlave->getRemapAddresses())
    {
        writer.writeStartElement(QStringLiteral("ipxact:remapAddress"));

        // Write state if it exists.               
        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("state"), remapAddress->state_);

        // Write the rest of the attributes.
        CommonItemsWriter::writeAttributes(writer, remapAddress->remapAttributes_);

        writer.writeCharacters(remapAddress->remapAddress_);
        writer.writeEndElement();
    }

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:range"), mirroredSlave->getRange());

    writer.writeEndElement(); // ipxact:baseAddresses
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMonitorInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeMonitorInterface(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface)
{
    if (businterface->getMonitor() == nullptr)
    {
        return;
    }

    // Monitor has its interface mode as attribute.
    writer.writeAttribute(QStringLiteral("interfaceMode"),
        General::interfaceMode2Str(businterface->getMonitor()->interfaceMode_));

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:group"),
        businterface->getMonitor()->group_);
}
