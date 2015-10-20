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

#include "BusinterfaceWriter.h"
#include "../masterinterface.h"
#include "../XmlUtils.h"
#include "../masterinterface.h"
#include "../mirroredslaveinterface.h"
#include "../slaveinterface.h"
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>
#include <IPXACTmodels/common/ParameterWriter.h>

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::businterfaceWriter()
//-----------------------------------------------------------------------------
businterfaceWriter::businterfaceWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::~businterfaceWriter()
//-----------------------------------------------------------------------------
businterfaceWriter::~businterfaceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writebusinterface()
//-----------------------------------------------------------------------------
void businterfaceWriter::writebusinterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const
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

    // Write always bus type, conection requirement the bits in lau, and endianess.
    writer.writeEmptyElement("ipxact:busType");
    businterface->getBusType().writeAsAttributes(writer);

    writeAbstractionTypes(businterface, writer);

    writeInterfaceMode(businterface, writer);

    writeConnectionRequired(writer, businterface);

    writeBitsInLau(writer, businterface);

    writeBitSteering(businterface, writer);

    writeEndianness(writer, businterface);

	writeParameters(businterface, writer);

    writeVendorExtensions( writer, businterface );

	writer.writeEndElement(); // ipxact:busInterface
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const
{
	NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, businterface);
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface) const
{
	if (!businterface->getIsPresent().isEmpty())
	{
		writer.writeTextElement("ipxact:isPresent", businterface->getIsPresent());
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeConnectionRequired()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeConnectionRequired(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> busInterface) const
{
    if (!busInterface->getConnectionRequired().isEmpty())
    {
        writer.writeTextElement("ipxact:connectionRequired", busInterface->getConnectionRequired());
    }
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeBitsInLau()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeBitsInLau(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const
{
    //writer.writeTextElement("ipxact:bitsInLau", QString::number(businterface->getBitsInLau()));
    if (!busInterface->getBitsInLau().isEmpty())
    {
        writer.writeTextElement("ipxact:bitsInLau", busInterface->getBitsInLau());
    }
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeEndianness()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeEndianness(QXmlStreamWriter& writer, QSharedPointer<BusInterface> busInterface) const
{
    if (busInterface->getEndianness() != General::ENDIANNESS_UNSPECIFIED)
    {
        QString endiannessString = General::endianness2Str(busInterface->getEndianness());
        writer.writeTextElement("ipxact:endianness", endiannessString);
    }
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeParameters()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeParameters(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const
{
	if (businterface->getParameters()->size() != 0)
	{
		writer.writeStartElement("ipxact:parameters");

		// Write each parameter with the parameter writer.
		ParameterWriter parameterWriter;

		for (int i = 0; i < businterface->getParameters()->size(); ++i)
		{
			parameterWriter.writeParameter(writer, businterface->getParameters()->at(i));
		}

		writer.writeEndElement(); 
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeBitSteering()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeBitSteering(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const
{
	// If the bitSteering has been defined.
	if (businterface->getBitSteering() != General::BITSTEERING_UNSPECIFIED)
	{
		// Start the ipxact:bitSteering tag.
		writer.writeStartElement("ipxact:bitSteering");

		// Write the attributes for the element-
		XmlUtils::writeAttributes(writer, businterface->getBitSteeringAttributes());

		// Write the value of the element and close the tag.
		writer.writeCharacters(General::bitSteering2Str(businterface->getBitSteering()));
		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeAbstractionTypes()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeAbstractionTypes(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const
{
	// Write each abstraction type.
    if (!businterface->getAbstractionTypes()->isEmpty())
	{
		writer.writeStartElement("ipxact:abstractionTypes");

		foreach ( QSharedPointer<BusInterface::AbstractionType> abstractionType,
            *businterface->getAbstractionTypes() )
		{
			writer.writeStartElement("ipxact:abstractionType");

			// Write view reference.
			if ( !abstractionType->viewRef_.isEmpty() )
			{
				writer.writeTextElement("ipxact:viewRef", abstractionType->viewRef_);
			}

			// Write abstraction reference.
			if ( abstractionType->abstractionRef_ )
			{
				writer.writeEmptyElement("ipxact:abstractionRef");

				abstractionType->abstractionRef_->writeAsAttributes(writer);
			}

			// Write each port map.
            if (!abstractionType->portMaps_->isEmpty())
			{
				writer.writeStartElement("ipxact:portMaps");

				foreach ( QSharedPointer<PortMap> portMap, *abstractionType->portMaps_ )
				{
					writer.writeStartElement("ipxact:portMap");

					// Write logical tie off for the port map.
					if ( !portMap->getLogicalTieOff().isEmpty() )
					{
						writer.writeTextElement("ipxact:logicalTieOff", portMap->getLogicalTieOff());
					}

					// Write ports for the port map.
					writeLogicalPort(portMap, writer);

					writePhysicalPort(portMap, writer);

					writer.writeEndElement();
				}

				writer.writeEndElement();
			}

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeLogicalPort()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeLogicalPort(QSharedPointer<PortMap> portMap, QXmlStreamWriter &writer) const
{
	QSharedPointer<PortMap::LogicalPort> lport = portMap->getLogicalPort();

	if ( lport )
	{
		// Logical port exists: Write it, including its name.
		writer.writeStartElement("ipxact:logicalPort");
		writer.writeTextElement("ipxact:name", lport->name_);

		// Write range of the logical port.
		if ( lport->range_ )
		{
			writer.writeStartElement("ipxact:range");

			writer.writeTextElement("ipxact:left", lport->range_->getLeft());
			writer.writeTextElement("ipxact:right", lport->range_->getRight());

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writePhysicalPort()
//-----------------------------------------------------------------------------
void businterfaceWriter::writePhysicalPort(QSharedPointer<PortMap> portMap, QXmlStreamWriter &writer) const
{
	QSharedPointer<PortMap::PhysicalPort> pport = portMap->getPhysicalPort();

	if ( pport )
	{
		// Physical port exists: Write it, including its name.
		writer.writeStartElement("ipxact:physicalPort");
		writer.writeTextElement("ipxact:name", pport->name_);

		QSharedPointer<PartSelect> partSelect = pport->partSelect_;

		// Part select exists: Write its range.
		if ( partSelect )
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

				foreach (QString index, *partSelect->getIndices())
				{
					writer.writeTextElement("ipxact:index", index);
				}

				writer.writeEndElement();
			}

			writer.writeEndElement(); 
		}

		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeInterfaceMode()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeInterfaceMode(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const
{
	// Select the correct writer function for the interface mode.
	switch (businterface->getInterfaceMode())
	{
	case General::MASTER:
		{
			writeMasterInterface(businterface, writer, false);

			break;
		}

	case General::SLAVE:
		{
			writeSlaveInterface(businterface, writer);

			break;
		}

	case General::SYSTEM:
		{
			if (!businterface->getSystem().isEmpty())
			{
				writer.writeStartElement("ipxact:system");
				writer.writeTextElement("ipxact:group", businterface->getSystem());
				writer.writeEndElement();
			}
			break;
		}

	case General::MIRROREDSLAVE:
		{
			writeMirroredSlaveInterface(businterface, writer);

			break;
		}

	case General::MIRROREDSYSTEM:
		{
			if (!businterface->getSystem().isEmpty())
			{
				writer.writeStartElement("ipxact:mirroredSystem");
				writer.writeTextElement("ipxact:group", businterface->getSystem());
				writer.writeEndElement(); 
			}
			break;
		}

	case General::MIRROREDMASTER:
		{
			writeMasterInterface(businterface, writer, true);

			break;
		}

	case General::MONITOR:
		{
			writeMonitorInterface(businterface, writer);

			break;
		}

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeMasterInterface()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeMasterInterface(QSharedPointer<BusInterface> businterface,
	QXmlStreamWriter &writer, bool mirrored) const
{
	QSharedPointer<MasterInterface> master = businterface->getMaster();

	if (master)
	{
		// Write interface type based on parameter.
		if ( mirrored )
		{
			writer.writeStartElement("ipxact:mirroredMaster");
		}
		else
		{
			writer.writeStartElement("ipxact:master");
		}

		// Write address space reference if it exists.
		if ( !master->getAddressSpaceRef().isEmpty() )
		{
			writer.writeStartElement("ipxact:addressSpaceRef");
			writer.writeAttribute("ipxact:addressSpaceRef", master->getAddressSpaceRef());
		}

		// Write base address if it exists.
		if (!master->getBaseAddress().isEmpty())
		{
			writer.writeStartElement("ipxact:baseAddress");

			// Write prompt attribute if it exist.
			if (!master->getPrompt().isEmpty())
			{
				writer.writeAttribute("ipxact:prompt", master->getPrompt());
			}

			// Write the rest of the attributes.
			XmlUtils::writeAttributes(writer, master->getBaseAttributes());

			// Write the value of the baseAddress element and close the tag.
			writer.writeCharacters(master->getBaseAddress());
			writer.writeEndElement();
		}

		writer.writeEndElement();

		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeMirroredSlaveInterface(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const
{
	QSharedPointer<MirroredSlaveInterface> slave = businterface->getMirroredSlave();

	if (slave)
	{
		writer.writeStartElement("ipxact:mirroredSlave");
		writer.writeStartElement("ipxact:baseAddresses");

		// Write all remap addresses.
		for (int i = 0; i < slave->getRemapAddresses().size(); ++i)
		{
			QSharedPointer<MirroredSlaveInterface::RemapAddress> ra = slave->getRemapAddresses().at(i);

			writer.writeStartElement("ipxact:remapAddress");

			// Write state if it exists.
			if (!ra->state_.isEmpty())
			{
				writer.writeAttribute("state", ra->state_);
			}

			// Write the rest of the attributes.
			XmlUtils::writeAttributes(writer, ra->remapAttributes_);

			writer.writeCharacters(ra->remapAddress_);
			writer.writeEndElement();
		}

		// Write range if it exists.
		if (!slave->getRange().isEmpty())
		{
			writer.writeStartElement("ipxact:range");

			writer.writeCharacters(slave->getRange());
			writer.writeEndElement();
		}

		writer.writeEndElement();
		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeSlaveInterface()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeSlaveInterface(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const
{
	QSharedPointer<SlaveInterface> slave = businterface->getSlave();

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
		for (int i = 0; i < slave->getBridges().size(); ++i)
		{
			// Bridge has a master reference.
			writer.writeStartElement("ipxact:transparentBridge");
			writer.writeAttribute("masterRef", slave->getBridges().at(i)->masterRef_);

			// Bridge has present.
			if (!slave->getBridges().at(i)->isPresent_.isEmpty())
			{
				writer.writeTextElement("ipxact:isPresent", slave->getBridges().at(i)->isPresent_);
			}

			writer.writeEndElement();
		}

		// Write file set refrence groups.
		for (int i = 0; i < slave->getFileSetRefGroup().size(); ++i)
		{
			QSharedPointer<SlaveInterface::FileSetRefGroup> fsrg =
				slave->getFileSetRefGroup().at(i);

			writer.writeStartElement("ipxact:fileSetRefGroup");

			// Write the name of the group.
			writer.writeTextElement("ipxact:group", fsrg->group_);

			// Write all the fileSetRefs for this group.
			for (int j = 0; j < fsrg->fileSetRefs_.size(); ++j)
			{
				writer.writeTextElement("ipxact:fileSetRef", fsrg->fileSetRefs_.at(j));
			}

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceWriter::writeMonitorInterface()
//-----------------------------------------------------------------------------
void businterfaceWriter::writeMonitorInterface(QSharedPointer<BusInterface> businterface, QXmlStreamWriter &writer) const
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