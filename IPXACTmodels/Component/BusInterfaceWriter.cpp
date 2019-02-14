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

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

#include <IPXACTmodels/utilities/XmlUtils.h>

#include <QStringBuilder>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::BusInterfaceWriter()
//-----------------------------------------------------------------------------
BusInterfaceWriter::BusInterfaceWriter() : CommonItemsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writebusinterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writebusinterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const
{
	writer.writeStartElement(QStringLiteral("ipxact:busInterface"));

	// Write attributes to the starting element.	
	XmlUtils::writeAttributes(writer, businterface->getAttributes());
	    
	// Start the element, write name group, presence and, vendor extensions with pre-existing writers.
	writeNameGroup(writer, businterface);
	writeIsPresent(writer, businterface);

    // Write always bus type, connection requirement the bits in lau, and endianess.
    writer.writeEmptyElement(QStringLiteral("ipxact:busType"));
    writeVLNVAttributes(writer, businterface->getBusType());

    writeAbstractionTypes(writer, businterface);

    writeInterfaceMode(writer, businterface);

    writeConnectionRequired(writer, businterface);

    writeBitsInLau(writer, businterface);

    writeBitSteering(writer, businterface);

    writeEndianness(writer, businterface);

	writeParameters(writer, businterface->getParameters());

    writeVendorExtensions(writer, businterface);

	writer.writeEndElement(); // ipxact:busInterface
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const
{
	NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, businterface);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const
{
    XmlUtils::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), businterface->getIsPresent());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeConnectionRequired()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeConnectionRequired(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> busInterface) const
{
    XmlUtils::writeNonEmptyElement(writer,
        QStringLiteral("ipxact:connectionRequired"), busInterface->getConnectionRequired());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeBitsInLau()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeBitsInLau(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const
{
    XmlUtils::writeNonEmptyElement(writer, QStringLiteral("ipxact:bitsInLau"), busInterface->getBitsInLau());    
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeEndianness()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeEndianness(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const
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
void BusInterfaceWriter::writeBitSteering(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface) const
{
    BusInterface::BitSteering bitSteering = businterface->getBitSteering();

	// If the bitSteering has been defined.
	if (bitSteering != BusInterface::BITSTEERING_UNSPECIFIED)
	{
		// Start the ipxact:bitSteering tag.
		writer.writeStartElement(QStringLiteral("ipxact:bitSteering"));

		// Write the attributes for the element-
		XmlUtils::writeAttributes(writer, businterface->getBitSteeringAttributes());

		// Write the value of the element and close the tag.
        if (bitSteering == BusInterface::BITSTEERING_ON)
        {
            writer.writeCharacters(QStringLiteral("on"));
        }
        else if (bitSteering == BusInterface::BITSTEERING_OFF)
        {
            writer.writeCharacters(QStringLiteral("off"));
        }

		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeAbstractionTypes()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeAbstractionTypes(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface) const
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
                writeVLNVAttributes(writer, *abstractionType->getAbstractionRef());
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
void BusInterfaceWriter::writePortMaps(QXmlStreamWriter& writer, 
    QSharedPointer<AbstractionType> abstractionType) const
{
    if (!abstractionType->getPortMaps()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:portMaps"));

        for (QSharedPointer<PortMap> const& portMap : *abstractionType->getPortMaps())
        {
            writer.writeStartElement(QStringLiteral("ipxact:portMap"));

            XmlUtils::writeNonEmptyAttribute(writer, QStringLiteral("invert"), portMap->getInvert().toString());

            XmlUtils::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), portMap->getIsPresent());

            // Write ports for the port map.
            if (portMap->getLogicalPort())
            {
                writeLogicalPort(writer, portMap->getLogicalPort());
            }
            if (portMap->getPhysicalPort())
            {
                writePhysicalPort(writer, portMap->getPhysicalPort());
            }

            XmlUtils::writeNonEmptyElement(writer,
                QStringLiteral("ipxact:logicalTieOff"), portMap->getLogicalTieOff());
            XmlUtils::writeNonEmptyElement(writer,
                QStringLiteral("ipxact:isInformative"), portMap->getIsInformative().toString());

            writer.writeEndElement();
        }

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeLogicalPort()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeLogicalPort(QXmlStreamWriter& writer, 
    QSharedPointer<PortMap::LogicalPort> logicalPort) const
{
    writer.writeStartElement(QStringLiteral("ipxact:logicalPort"));
    writer.writeTextElement(QStringLiteral("ipxact:name"), logicalPort->name_);

    // Write range of the logical port.
    if (logicalPort->range_)
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
void BusInterfaceWriter::writePhysicalPort(QXmlStreamWriter& writer, 
    QSharedPointer<PortMap::PhysicalPort> physicalPort) const
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
        if (!partSelect->getIndices()->isEmpty())
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
void BusInterfaceWriter::writeInterfaceMode(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface) const
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
void BusInterfaceWriter::writeMasterInterface(QXmlStreamWriter& writer,
    QSharedPointer<MasterInterface> masterInterface) const
{
    if (masterInterface && (!masterInterface->getAddressSpaceRef().isEmpty() ||
        !masterInterface->getBaseAddress().isEmpty()))
    {
        writer.writeStartElement(QStringLiteral("ipxact:addressSpaceRef"));

        // Write address space reference if it exists.            
        XmlUtils::writeNonEmptyAttribute(writer,
            QStringLiteral("addressSpaceRef"), masterInterface->getAddressSpaceRef());

        XmlUtils::writeNonEmptyElement(writer,
            QStringLiteral("ipxact:isPresent"), masterInterface->getIsPresent());

        // Write base address if it exists.
        if (!masterInterface->getBaseAddress().isEmpty())
        {
            writer.writeStartElement(QStringLiteral("ipxact:baseAddress"));

            // Write prompt attribute if it exist.
            XmlUtils::writeNonEmptyAttribute(writer,
                QStringLiteral("ipxact::prompt"), masterInterface->getPrompt());

            // Write the value of the baseAddress element and close the tag.
            writer.writeCharacters(masterInterface->getBaseAddress());
            writer.writeEndElement(); // ipxact:baseAddress
        }

        writer.writeEndElement(); // ipxact:addressSpaceRef
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeSlaveInterface(QXmlStreamWriter& writer, QSharedPointer<SlaveInterface> slave) const
{
	if (slave)
	{
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

            XmlUtils::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), bridge->getIsPresent());
            
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
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeMirroredSlaveInterface(QXmlStreamWriter& writer,
    QSharedPointer<MirroredSlaveInterface> mirroredSlave) const
{
    if (mirroredSlave)
    {
        if (!mirroredSlave->getRemapAddresses()->isEmpty() || !mirroredSlave->getRange().isEmpty())
        {
            writer.writeStartElement(QStringLiteral("ipxact:baseAddresses"));

            // Write all remap addresses.
            for (QSharedPointer<MirroredSlaveInterface::RemapAddress> const& remapAddress : 
                *mirroredSlave->getRemapAddresses())
            {
                writer.writeStartElement(QStringLiteral("ipxact:remapAddress"));

                // Write state if it exists.               
                XmlUtils::writeNonEmptyAttribute(writer, QStringLiteral("state"), remapAddress->state_);

                // Write the rest of the attributes.
                XmlUtils::writeAttributes(writer, remapAddress->remapAttributes_);

                writer.writeCharacters(remapAddress->remapAddress_);
                writer.writeEndElement();
            }

            XmlUtils::writeNonEmptyElement(writer, QStringLiteral("ipxact:range"), mirroredSlave->getRange());

            writer.writeEndElement(); // ipxact:baseAddresses
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMonitorInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeMonitorInterface(QXmlStreamWriter& writer, 
    QSharedPointer<BusInterface> businterface) const
{
	if (businterface->getMonitor())
	{
		// Monitor has its interface mode as attribute.
		writer.writeAttribute(QStringLiteral("interfaceMode"),
            General::interfaceMode2Str(businterface->getMonitor()->interfaceMode_));

        XmlUtils::writeNonEmptyElement(writer, QStringLiteral("ipxact:group"), businterface->getMonitor()->group_);		
	}
}
