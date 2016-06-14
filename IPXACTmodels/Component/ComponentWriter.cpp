//-----------------------------------------------------------------------------
// File: ComponentWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.10.2015
//
// Description:
// Writer class for ipxact:component element.
//-----------------------------------------------------------------------------

#include "ComponentWriter.h"
#include "Component.h"
#include "Model.h"

#include "BusInterfaceWriter.h"
#include "ChannelWriter.h"
#include "RemapStateWriter.h"
#include "AddressSpaceWriter.h"
#include "MemoryMapWriter.h"
#include "ViewWriter.h"
#include "InstantiationsWriter.h"
#include "PortWriter.h"
#include "ComponentGeneratorWriter.h"
#include "ChoiceWriter.h"
#include "FileSetWriter.h"
#include "CPUWriter.h"
#include "OtherClockDriverWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: ComponentWriter::ComponentWriter()
//-----------------------------------------------------------------------------
ComponentWriter::ComponentWriter(QObject* parent /* = 0 */) :
DocumentWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::~ComponentWriter()
//-----------------------------------------------------------------------------
ComponentWriter::~ComponentWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeComponent()
//-----------------------------------------------------------------------------
void ComponentWriter::writeComponent(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    writer.writeStartDocument();

    writeTopComments(writer, component);

    writeXmlProcessingInstructions(writer, component);

    writer.writeStartElement("ipxact:component");

    writeNamespaceDeclarations(writer);

    writeVLNVElements(writer, component->getVlnv());

    writeBusInterfaces(writer, component);

    writeChannels(writer, component);

    writeRemapStates(writer, component);

    writeAddressSpaces(writer, component);

    writeMemoryMaps(writer, component);

    writeModel(writer, component);
    
    writeComponentGenerators(writer, component);

    writeChoices(writer, component);

    writeFileSets(writer, component);

    writeCPUs(writer, component);

    writeOtherClockDrivers(writer, component);

    writeDescription(writer, component);

    writeParameters(writer, component);

    writeAssertions(writer, component);

    writeComponentExtensions(writer, component);

    writer.writeEndElement(); // ipxact:component

    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeBusInterfaces()
//-----------------------------------------------------------------------------
void ComponentWriter::writeBusInterfaces(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getBusInterfaces()->isEmpty())
    {
        BusInterfaceWriter busWriter;

        writer.writeStartElement("ipxact:busInterfaces");

        foreach (QSharedPointer<BusInterface> busInterface, *component->getBusInterfaces())
        {
            busWriter.writebusinterface(writer, busInterface);
        }

        writer.writeEndElement(); // ipxact:busInterfaces
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeChannels()
//-----------------------------------------------------------------------------
void ComponentWriter::writeChannels(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getChannels()->isEmpty())
    {
        ChannelWriter channelWriter;

        writer.writeStartElement("ipxact:channels");

        foreach (QSharedPointer<Channel> channel, *component->getChannels())
        {
            channelWriter.writeChannel(writer, channel);
        }

        writer.writeEndElement(); // ipxact:channels
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeRemapStates()
//-----------------------------------------------------------------------------
void ComponentWriter::writeRemapStates(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getRemapStates()->isEmpty())
    {
        RemapStateWriter remapStateWriter;

        writer.writeStartElement("ipxact:remapStates");

        foreach (QSharedPointer<RemapState> remapState, *component->getRemapStates())
        {
            remapStateWriter.writeRemapState(writer, remapState);
        }

        writer.writeEndElement(); // ipxact:remapStates
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeAddressSpaces()
//-----------------------------------------------------------------------------
void ComponentWriter::writeAddressSpaces(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getAddressSpaces()->isEmpty())
    {
        AddressSpaceWriter spaceWriter;

        writer.writeStartElement("ipxact:addressSpaces");

        foreach (QSharedPointer<AddressSpace> addressSpace, *component->getAddressSpaces())
        {
            spaceWriter.writeAddressSpace(writer, addressSpace);
        }

        writer.writeEndElement(); // ipxact:addressSpaces
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeMemoryMaps()
//-----------------------------------------------------------------------------
void ComponentWriter::writeMemoryMaps(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getMemoryMaps()->isEmpty())
    {
        MemoryMapWriter mapWriter;

        writer.writeStartElement("ipxact:memoryMaps");

        foreach (QSharedPointer<MemoryMap> memoryMap, *component->getMemoryMaps())
        {
            mapWriter.writeMemoryMap(writer, memoryMap);
        }

        writer.writeEndElement(); // ipxact:memoryMaps
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeModel()
//-----------------------------------------------------------------------------
void ComponentWriter::writeModel(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (component->getModel()->hasContents())
    {
        writer.writeStartElement("ipxact:model");

        writeViews(writer, component);

        writeInstantiations(writer, component);

        writePorts(writer, component);

        writer.writeEndElement(); // ipxact:model
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeViews()
//-----------------------------------------------------------------------------
void ComponentWriter::writeViews(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getViews()->isEmpty())
    {
        ViewWriter viewWriter;

        writer.writeStartElement("ipxact:views");

        foreach (QSharedPointer<View> view, *component->getViews())
        {
            viewWriter.writeView(writer, view);
        }

        writer.writeEndElement(); // ipxact:views
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeInstantiations()
//-----------------------------------------------------------------------------
void ComponentWriter::writeInstantiations(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getComponentInstantiations()->isEmpty() || !component->getDesignInstantiations()->isEmpty() ||
        !component->getDesignConfigurationInstantiations()->isEmpty())
    {
        InstantiationsWriter instantiationsWriter;

        writer.writeStartElement("ipxact:instantiations");

        foreach (QSharedPointer<ComponentInstantiation> instantiation, *component->getComponentInstantiations())
        {
            instantiationsWriter.writeComponentInstantiation(writer, instantiation);
        }
        foreach (QSharedPointer<DesignInstantiation> instantiation, *component->getDesignInstantiations())
        {
            instantiationsWriter.writeDesignInstantiation(writer, instantiation);
        }
        foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
            *component->getDesignConfigurationInstantiations())
        {
            instantiationsWriter.writeDesignConfigurationInstantiation(writer, instantiation);
        }

        writer.writeEndElement(); // ipxact:instantiations
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writePorts()
//-----------------------------------------------------------------------------
void ComponentWriter::writePorts(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getPorts()->isEmpty())
    {
        PortWriter portWriter;

        writer.writeStartElement("ipxact:ports");

        foreach (QSharedPointer<Port> port, *component->getPorts())
        {
            portWriter.writePort(writer, port);
        }

        writer.writeEndElement(); // ipxact:ports
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeComponentGenerators()
//-----------------------------------------------------------------------------
void ComponentWriter::writeComponentGenerators(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getComponentGenerators()->isEmpty())
    {
        ComponentGeneratorWriter generatorWriter;

        writer.writeStartElement("ipxact:componentGenerators");

        foreach (QSharedPointer<ComponentGenerator> generator, *component->getComponentGenerators())
        {
            generatorWriter.writeComponentGenerator(writer, generator);
        }

        writer.writeEndElement(); // ipxact:componentGenerators
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeChoices()
//-----------------------------------------------------------------------------
void ComponentWriter::writeChoices(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getChoices()->isEmpty())
    {
        ChoiceWriter choiceWriter;

        writer.writeStartElement("ipxact:choices");

        foreach (QSharedPointer<Choice> choice, *component->getChoices())
        {
            choiceWriter.writeChoice(writer, choice);
        }

        writer.writeEndElement(); // ipxact:choices
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeFileSets()
//-----------------------------------------------------------------------------
void ComponentWriter::writeFileSets(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getFileSets()->isEmpty())
    {
        FileSetWriter setWriter;

        writer.writeStartElement("ipxact:fileSets");

        foreach (QSharedPointer<FileSet> fileSet, *component->getFileSets())
        {
            setWriter.writeFileSet(writer, fileSet);
        }

        writer.writeEndElement(); // ipxact:fileSets
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeCPUs()
//-----------------------------------------------------------------------------
void ComponentWriter::writeCPUs(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getCpus()->isEmpty())
    {
        CPUWriter cpuWriter;

        writer.writeStartElement("ipxact:cpus");

        foreach (QSharedPointer<Cpu> cpu, *component->getCpus())
        {
            cpuWriter.writeCPU(writer, cpu);
        }

        writer.writeEndElement(); // ipxact:cpus
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeOtherClockDrivers()
//-----------------------------------------------------------------------------
void ComponentWriter::writeOtherClockDrivers(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getOtherClockDrivers()->isEmpty())
    {
        OtherClockDriverWriter clockWriter;

        writer.writeStartElement("ipxact:otherClockDrivers");

        foreach (QSharedPointer<OtherClockDriver> driver, *component->getOtherClockDrivers())
        {
            clockWriter.writeOtherClockDriver(writer, driver);
        }

        writer.writeEndElement(); // ipxact:otherClockDrivers
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWriter::writeComponentExtensions()
//-----------------------------------------------------------------------------
void ComponentWriter::writeComponentExtensions(QXmlStreamWriter& writer, QSharedPointer<Component> component) const
{
    if (!component->getPendingFileDependencies().isEmpty())
    {
        component->setFileDependendencies(component->getPendingFileDependencies());
    }

    writeVendorExtensions(writer, component);
}
