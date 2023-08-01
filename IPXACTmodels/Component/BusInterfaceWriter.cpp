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

#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/MirroredTargetInterface.h>
#include <IPXACTmodels/Component/TargetInterface.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

#include <QStringBuilder>
#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writebusinterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::writeBusInterface(QXmlStreamWriter& writer, QSharedPointer<BusInterface> businterface,
    Document::Revision docRevision)
{
	writer.writeStartElement(QStringLiteral("ipxact:busInterface"));

	// Write attributes to the starting element.	
    CommonItemsWriter::writeAttributes(writer, businterface->getAttributes());
	    
	// Start the element, write name group, presence and, vendor extensions with pre-existing writers.
    NameGroupWriter::writeNameGroup(writer, businterface, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), businterface->getIsPresent());
    }

    // Write always bus type, connection requirement the bits in lau, and endianess.
    writer.writeEmptyElement(QStringLiteral("ipxact:busType"));
    CommonItemsWriter::writeVLNVAttributes(writer, businterface->getBusType());

    Details::writeAbstractionTypes(writer, businterface, docRevision);

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
    auto bitSteering = businterface->getBitSteering();

	// If the bitSteering has been defined.
    if (bitSteering.isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:bitSteering"));

        CommonItemsWriter::writeAttributes(writer, businterface->getBitSteeringAttributes());

        writer.writeCharacters(bitSteering);

        writer.writeEndElement(); // ipxact:bitSteering
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeAbstractionTypes()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeAbstractionTypes(QXmlStreamWriter& writer,
    QSharedPointer<BusInterface> businterface, Document::Revision docRevision)
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

            writePortMaps(writer, abstractionType, docRevision);

			writer.writeEndElement(); // ipxact:abstractionType
		}

		writer.writeEndElement(); // ipxact:abstractionTypes
	}
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writePortMaps()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writePortMaps(QXmlStreamWriter& writer,
    QSharedPointer<AbstractionType> abstractionType, Document::Revision docRevision)
{
    if (!abstractionType->getPortMaps()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:portMaps"));

        for (QSharedPointer<PortMap> const& portMap : *abstractionType->getPortMaps())
        {
            writer.writeStartElement(QStringLiteral("ipxact:portMap"));

            CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("invert"), portMap->getInvert().toString());

            if (docRevision == Document::Revision::Std14)
            {
                CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), portMap->getIsPresent());
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

            CommonItemsWriter::writeNonEmptyElement(writer,
                QStringLiteral("ipxact:logicalTieOff"), portMap->getLogicalTieOff());
            CommonItemsWriter::writeNonEmptyElement(writer,
                QStringLiteral("ipxact:isInformative"), portMap->getIsInformative().toString());

            if (docRevision == Document::Revision::Std22)
            {
                CommonItemsWriter::writeVendorExtensions(writer, portMap);
            }

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
    else if (interfaceMode == General::MIRRORED_SLAVE)
    {
        writeMirroredSlaveInterface(writer, businterface->getMirroredSlave());
    }
    else if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRRORED_SYSTEM)
    {        
        writer.writeTextElement(QStringLiteral("ipxact:group"), businterface->getSystem());        
    }
    else if (interfaceMode == General::MONITOR)
    {
        writeMonitorInterface(writer, businterface);
    }
    else if (interfaceMode == General::INITIATOR)
    {
        writeInitiatorInterface(writer, businterface->getInitiator(), Document::Revision::Std22);
    }
    else if (interfaceMode == General::TARGET)
    {
        writeTargetInterface(writer, businterface->getTarget(), Document::Revision::Std22);
    }
    else if (interfaceMode == General::MIRRORED_TARGET)
    {
        writeMirroredTargetInterface(writer, businterface->getMirroredTarget(), Document::Revision::Std22);
    }

    writer.writeEndElement(); // ipxact:<mode>
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMasterInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeMasterInterface(QXmlStreamWriter& writer,
    QSharedPointer<InitiatorInterface> masterInterface)
{
    writeInitiatorInterface(writer, masterInterface, Document::Revision::Std14);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeSlaveInterface(QXmlStreamWriter& writer, QSharedPointer<TargetInterface> slave)
{
    writeTargetInterface(writer, slave, Document::Revision::Std14);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeMirroredSlaveInterface(QXmlStreamWriter& writer,
    QSharedPointer<MirroredTargetInterface> mirroredSlave)
{
    writeMirroredTargetInterface(writer, mirroredSlave, Document::Revision::Std14);
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

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeInitiatorInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeInitiatorInterface(QXmlStreamWriter& writer,
    QSharedPointer<InitiatorInterface> initiatorInterface, Document::Revision docRevsion)
{
    if (initiatorInterface == nullptr ||
        (initiatorInterface->getAddressSpaceRef().isEmpty() && initiatorInterface->getBaseAddress().isEmpty()))
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:addressSpaceRef"));

    // Write address space reference if it exists.            
    CommonItemsWriter::writeNonEmptyAttribute(writer,
        QStringLiteral("addressSpaceRef"), initiatorInterface->getAddressSpaceRef());

    if (docRevsion == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, initiatorInterface->getIsPresent());
    }

    // Write base address if it exists.
    if (!initiatorInterface->getBaseAddress().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:baseAddress"));

        // Write the value of the baseAddress element and close the tag.
        writer.writeCharacters(initiatorInterface->getBaseAddress());
        writer.writeEndElement(); // ipxact:baseAddress
    }

    writer.writeEndElement(); // ipxact:addressSpaceRef
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWriter::writeTargetInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeTargetInterface(QXmlStreamWriter& writer,
    QSharedPointer<TargetInterface> target, Document::Revision docRevision)
{
    if (target == nullptr)
    {
        return;
    }

    // Write memory map reference, along its attributes.
    if (!target->getMemoryMapRef().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:memoryMapRef"));
        writer.writeAttribute(QStringLiteral("memoryMapRef"), target->getMemoryMapRef());

        if (docRevision == Document::Revision::Std22)
        {
            for (auto const& mode : target->getModeRefs())
            {
                writer.writeTextElement(QStringLiteral("ipxact:modeRef"), mode);
            }
        }

        writer.writeEndElement(); // ipxact:memoryMapRef
    }

    // Write all bridges.
    for (QSharedPointer<TransparentBridge> const& bridge : *target->getBridges())
    {
        // Bridge has a master reference.
        writer.writeStartElement(QStringLiteral("ipxact:transparentBridge"));

        if (docRevision == Document::Revision::Std14)
        {
            writer.writeAttribute(QStringLiteral("masterRef"), bridge->getMasterRef());
        }
        else if (docRevision == Document::Revision::Std22)
        {
            writer.writeAttribute(QStringLiteral("initiatorRef"), bridge->getInitiatorRef());
        }

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"),
                bridge->getIsPresent());
        }

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeVendorExtensions(writer, bridge);
        }
        writer.writeEndElement();
    }

    // Write file set reference groups.
    for (QSharedPointer<TargetInterface::FileSetRefGroup> const& group : *target->getFileSetRefGroup())
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
// Function: BusInterfaceWriter::writeMirroredTargetInterface()
//-----------------------------------------------------------------------------
void BusInterfaceWriter::Details::writeMirroredTargetInterface(QXmlStreamWriter& writer,
    QSharedPointer<MirroredTargetInterface> mirroredTarget, Document::Revision docRevision)
{
    if (mirroredTarget == nullptr || mirroredTarget->getRange().isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:baseAddresses"));

    // Write all remap addresses.
    for (QSharedPointer<MirroredTargetInterface::RemapAddress> const& remapAddress :
        *mirroredTarget->getRemapAddresses())
    {
        if (docRevision == Document::Revision::Std22)
        {
            writer.writeStartElement(QStringLiteral("ipxact:remapAddresses"));
        }

        writer.writeStartElement(QStringLiteral("ipxact:remapAddress"));

        if (docRevision == Document::Revision::Std14)
        {
            // Write state if it exists.               
            CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("state"), remapAddress->state_);

            // Write the rest of the attributes.
            CommonItemsWriter::writeAttributes(writer, remapAddress->remapAttributes_);
        }

        writer.writeCharacters(remapAddress->remapAddress_);

        if (docRevision == Document::Revision::Std22)
        {
            for (auto const& modeRef : remapAddress->modeRefs_)
            {
                writer.writeTextElement(QStringLiteral("ipxact:modeRef"), modeRef);
                if (remapAddress->priorities_.contains(modeRef))
                {
                    writer.writeAttribute(QStringLiteral("priority"),
                        QString::number(remapAddress->priorities_.value(modeRef)));
                }

            }
        }

        writer.writeEndElement(); // ipxact:remapAddress

        if (docRevision == Document::Revision::Std22)
        {
            writer.writeEndElement(); // ipxact:remapAddresses
        }
    }

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:range"), mirroredTarget->getRange());

    writer.writeEndElement(); // ipxact:baseAddresses
}
