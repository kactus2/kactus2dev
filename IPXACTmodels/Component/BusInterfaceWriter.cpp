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

#include "../XmlUtils.h"

#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::BusInterfaceWriter()
//-----------------------------------------------------------------------------
BusInterfaceWriter::BusInterfaceWriter(QObject* parent) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::~BusInterfaceWriter()
//-----------------------------------------------------------------------------
BusInterfaceWriter::~BusInterfaceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writebusinterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writebusinterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const
{
	writer.writeStartElement("ipxact:busInterface");

	// Write attributes to the starting element.
	if (!businterface->getAttributes().isEmpty())
	{
		XmlUtils::writeAttributes(writer, businterface->getAttributes());
	}
    
	// Start the element, write name group, presence and, vendor extensions with pre-existing writers.
	writeNameGroup(writer, businterface);
	writeIsPresent(writer, businterface);

    // Write always bus type, connection requirement the bits in lau, and endianess.
    writer.writeEmptyElement("ipxact:busType");
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
	if (!businterface->getIsPresent().isEmpty())
	{
		writer.writeTextElement("ipxact:isPresent", businterface->getIsPresent());
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeConnectionRequired()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeConnectionRequired(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> busInterface) const
{
    if (!busInterface->getConnectionRequired().isEmpty())
    {
        writer.writeTextElement("ipxact:connectionRequired", busInterface->getConnectionRequired());
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeBitsInLau()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeBitsInLau(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const
{
    if (!busInterface->getBitsInLau().isEmpty())
    {
        writer.writeTextElement("ipxact:bitsInLau", busInterface->getBitsInLau());
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeEndianness()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeEndianness(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const
{
    BusInterface::Endianness endianess = busInterface->getEndianness();
    if (endianess == BusInterface::BIG)
    {
        writer.writeTextElement("ipxact:endianness", "big");
    }
    else if (endianess == BusInterface::LITTLE)
    {
        writer.writeTextElement("ipxact:endianness", "little");
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
		writer.writeStartElement("ipxact:bitSteering");

		// Write the attributes for the element-
		XmlUtils::writeAttributes(writer, businterface->getBitSteeringAttributes());

		// Write the value of the element and close the tag.
        if (bitSteering == BusInterface::BITSTEERING_ON)
        {
            writer.writeCharacters("on");
        }
        else if (bitSteering == BusInterface::BITSTEERING_OFF)
        {
            writer.writeCharacters("off");
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
		writer.writeStartElement("ipxact:abstractionTypes");

		foreach (QSharedPointer<AbstractionType> abstractionType, *businterface->getAbstractionTypes())
		{
			writer.writeStartElement("ipxact:abstractionType");

			if (!abstractionType->getViewRef().isEmpty() )
			{
				writer.writeTextElement("ipxact:viewRef", abstractionType->getViewRef());
			}

            writer.writeEmptyElement("ipxact:abstractionRef");
            writeVLNVAttributes(writer, *abstractionType->getAbstractionRef());

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
        writer.writeStartElement("ipxact:portMaps");

        foreach (QSharedPointer<PortMap> portMap, *abstractionType->getPortMaps())
        {
            writer.writeStartElement("ipxact:portMap");

            if (!portMap->getInvert().toString().isEmpty())
            {
                writer.writeAttribute("invert", portMap->getInvert().toString());
            }

            if (!portMap->getIsPresent().isEmpty())
            {
                writer.writeTextElement("ipxact:isPresent", portMap->getIsPresent());
            }

            // Write ports for the port map.
            if (portMap->getLogicalPort())
            {
                writeLogicalPort(writer, portMap->getLogicalPort());
            }
            if (portMap->getPhysicalPort())
            {
                writePhysicalPort(writer, portMap->getPhysicalPort());
            }

            if (!portMap->getLogicalTieOff().isEmpty() )
            {
                writer.writeTextElement("ipxact:logicalTieOff", portMap->getLogicalTieOff());
            }

            if (!portMap->getIsInformative().toString().isEmpty())
            {
                writer.writeTextElement("ipxact:isInformative", portMap->getIsInformative().toString());
            }

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
    writer.writeStartElement("ipxact:logicalPort");
    writer.writeTextElement("ipxact:name", logicalPort->name_);

    // Write range of the logical port.
    if (logicalPort->range_)
    {
        writer.writeStartElement("ipxact:range");
        writer.writeTextElement("ipxact:left", logicalPort->range_->getLeft());
        writer.writeTextElement("ipxact:right", logicalPort->range_->getRight());
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
    writer.writeStartElement("ipxact:physicalPort");
    writer.writeTextElement("ipxact:name", physicalPort->name_);

    QSharedPointer<PartSelect> partSelect = physicalPort->partSelect_;

    // Part select exists: Write its range.
    if (partSelect)
    {
        writer.writeStartElement("ipxact:partSelect");

        writer.writeStartElement("ipxact:range");
        writer.writeTextElement("ipxact:left", partSelect->getLeftRange());
        writer.writeTextElement("ipxact:right", partSelect->getRightRange());
        writer.writeEndElement();

        // Write all indices of the part select.
        if (!partSelect->getIndices()->isEmpty())
        {
            writer.writeStartElement("ipxact:indices");

            foreach (QString const& index, *partSelect->getIndices())
            {
                writer.writeTextElement("ipxact:index", index);
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

	// Select the correct writer function for the interface mode.
    if (interfaceMode == General::MASTER)
    {
        writeMasterInterface(writer, businterface->getMaster(), false);
    }   
    
    else if(interfaceMode == General::MIRROREDMASTER)
    {
        writeMasterInterface(writer, businterface->getMaster(), true);
    }

    else if(interfaceMode == General::SLAVE)
    {
        writeSlaveInterface(writer, businterface->getSlave());
    }

    else if(interfaceMode == General::MIRROREDSLAVE)
    {
        writeMirroredSlaveInterface(writer, businterface->getMirroredSlave());
    }

    else if(interfaceMode == General::SYSTEM && !businterface->getSystem().isEmpty())
    {
        writer.writeStartElement("ipxact:system");
        writer.writeTextElement("ipxact:group", businterface->getSystem());
        writer.writeEndElement();
    }

    else if(interfaceMode == General::MIRROREDSYSTEM && !businterface->getSystem().isEmpty())
    {
        writer.writeStartElement("ipxact:mirroredSystem");
        writer.writeTextElement("ipxact:group", businterface->getSystem());
        writer.writeEndElement(); 
    }

    else if(interfaceMode == General::MONITOR)
    {
        writeMonitorInterface(writer, businterface);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMasterInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeMasterInterface(QXmlStreamWriter& writer,
    QSharedPointer<MasterInterface> masterInterface, bool mirrored) const
{
	if (masterInterface)
	{
		// Write interface type based on parameter.
		if (mirrored)
		{
			writer.writeStartElement("ipxact:mirroredMaster");
		}
		else
		{
			writer.writeStartElement("ipxact:master");
		}

        if (!masterInterface->getAddressSpaceRef().isEmpty() ||
            !masterInterface->getBaseAddress().isEmpty())
        {
            writer.writeStartElement("ipxact:addressSpaceRef");

            // Write address space reference if it exists.
            if (!masterInterface->getAddressSpaceRef().isEmpty())
            {			
                writer.writeAttribute("ipxact:addressSpaceRef", masterInterface->getAddressSpaceRef());
            }

            if (!masterInterface->getIsPresent().isEmpty())
            {
                writer.writeTextElement("ipxact:isPresent", masterInterface->getIsPresent());
            }

            // Write base address if it exists.
            if (!masterInterface->getBaseAddress().isEmpty())
            {
                writer.writeStartElement("ipxact:baseAddress");

                // Write prompt attribute if it exist.
                if (!masterInterface->getPrompt().isEmpty())
                {
                    writer.writeAttribute("ipxact:prompt", masterInterface->getPrompt());
                }

                // Write the rest of the attributes.
                XmlUtils::writeAttributes(writer, masterInterface->getBaseAttributes());

                // Write the value of the baseAddress element and close the tag.
                writer.writeCharacters(masterInterface->getBaseAddress());
                writer.writeEndElement(); // ipxact:baseAddress
            }

            writer.writeEndElement(); // ipxact:addressSpaceRef
        }
        
		writer.writeEndElement(); // ipxact:(mirrored)Master
	}
}


//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeSlaveInterface(QXmlStreamWriter& writer, QSharedPointer<SlaveInterface> slave) const
{
	if (slave)
	{
		writer.writeStartElement("ipxact:slave");

		// Write memory map reference, along its attributes.
		if (!slave->getMemoryMapRef().isEmpty())
		{
			writer.writeEmptyElement("ipxact:memoryMapRef");
			writer.writeAttribute("memoryMapRef", slave->getMemoryMapRef());
		}

		// Write all bridges.
		foreach (QSharedPointer<SlaveInterface::Bridge> bridge, *slave->getBridges())
		{
			// Bridge has a master reference.
			writer.writeStartElement("ipxact:transparentBridge");
			writer.writeAttribute("masterRef", bridge->masterRef_);

			// Bridge has present.
			if (!bridge->isPresent_.isEmpty())
			{
				writer.writeTextElement("ipxact:isPresent", bridge->isPresent_);
			}

			writer.writeEndElement();
		}

		// Write file set reference groups.
		foreach (QSharedPointer<SlaveInterface::FileSetRefGroup> filesetRef, *slave->getFileSetRefGroup())
		{
			writer.writeStartElement("ipxact:fileSetRefGroup");

			// Write the name of the group.
			writer.writeTextElement("ipxact:group", filesetRef->group_);

			// Write all the fileSetRefs for this group.
			for (int j = 0; j < filesetRef->fileSetRefs_.size(); ++j)
			{
				writer.writeTextElement("ipxact:fileSetRef", filesetRef->fileSetRefs_.at(j));
			}

			writer.writeEndElement(); // ipxact:fileSetRefGroup
		}

		writer.writeEndElement(); // ipaxct:slave
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
        writer.writeStartElement("ipxact:mirroredSlave");

        if (!mirroredSlave->getRemapAddresses()->isEmpty() || !mirroredSlave->getRange().isEmpty())
        {
            writer.writeStartElement("ipxact:baseAddresses");

            // Write all remap addresses.
            foreach (QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress, 
                *mirroredSlave->getRemapAddresses())
            {
                writer.writeStartElement("ipxact:remapAddress");

                // Write state if it exists.
                if (!remapAddress->state_.isEmpty())
                {
                    writer.writeAttribute("state", remapAddress->state_);
                }

                // Write the rest of the attributes.
                XmlUtils::writeAttributes(writer, remapAddress->remapAttributes_);

                writer.writeCharacters(remapAddress->remapAddress_);
                writer.writeEndElement();
            }

            // Write range if it exists.
            if (!mirroredSlave->getRange().isEmpty())
            {
                writer.writeTextElement("ipxact:range", mirroredSlave->getRange());
            }

            writer.writeEndElement(); // ipxact:baseAddresses
        }

        writer.writeEndElement(); // ipxact:mirroredSlave
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
		writer.writeStartElement("ipxact:monitor");

		// Monitor has its interface mode as attribute.
		writer.writeAttribute("interfaceMode",
            General::interfaceMode2Str(businterface->getMonitor()->interfaceMode_));

		// Write group if defined.
		if (!businterface->getMonitor()->group_.isEmpty())
		{
			writer.writeTextElement("ipxact:group", businterface->getMonitor()->group_);
		}

		writer.writeEndElement();
	}
}
