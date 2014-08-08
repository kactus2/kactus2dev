//-----------------------------------------------------------------------------
// File: VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#include "VerilogGenerator.h"

#include <IPXACTmodels/businterface.h>

#include <kactusGenerators/HDLGenerator/HDLUtils.h>

#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortSorter/InterfaceDirectionNameSorter.h>

#include <QDate>

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(LibraryInterface* library): headerWriter_(0), topWriter_(0), 
topComponent_(), sorter_(new InterfaceDirectionNameSorter()), library_(library)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::~VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::~VerilogGenerator()
{
    delete headerWriter_;
    headerWriter_ = 0;

    delete topWriter_;
    topWriter_ = 0;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::generate()
//-----------------------------------------------------------------------------
void VerilogGenerator::generate(QString const& outputPath) const
{
    if (nothingToWrite())
    {
        return;
    }

    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    QString fileName = outputPath.mid(outputPath.lastIndexOf("/") + 1);

    headerWriter_->write(outputStream, fileName, topComponent_->getDescription(), QDateTime::currentDateTime());
    topWriter_->write(outputStream);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parse()
//-----------------------------------------------------------------------------
void VerilogGenerator::parse(QSharedPointer<Component> component, QSharedPointer<Design> design)
{
    topComponent_ = component;

    QString mockUsername = "user";
    QString mockPath = "C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml";

    delete headerWriter_;
    headerWriter_ = new VerilogHeaderWriter(*topComponent_->getVlnv(), mockPath, mockUsername);

    delete topWriter_;
    topWriter_ = new ComponentVerilogWriter(topComponent_, sorter_);

    parseComponentInstances(design);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogGenerator::nothingToWrite() const
{
    return topWriter_ == 0;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseComponentInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseComponentInstances(QSharedPointer<Design> design)
{
    if (!design)
    {
        return;
    }

    foreach(ComponentInstance instance, design->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComp = library_->getModel(instance.getComponentRef());
        QSharedPointer<Component> instanceComponent = libComp.dynamicCast<Component>();
        if (instanceComponent)
        {
            ComponentInstanceVerilogWriter* instanceWriter = new ComponentInstanceVerilogWriter(instance, instanceComponent, sorter_);
            parseHierarchicalConnectionsForInstance(instanceComponent, instanceWriter, design);
            topWriter_->add(instanceWriter);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseHierarchicalConnectionsForInstance()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseHierarchicalConnectionsForInstance(QSharedPointer<Component> instanceComponent, 
    ComponentInstanceVerilogWriter* instanceWriter, 
    QSharedPointer<Design> design)
{
    foreach(HierConnection connection, design->getHierarchicalConnections())
    {
        QSharedPointer<BusInterface> topIf = topComponent_->getBusInterface(connection.getInterfaceRef());
        QSharedPointer<BusInterface> instanceIf = instanceComponent->getBusInterface(connection.getInterface().getBusRef());

        if (!topIf.isNull() && !instanceIf.isNull())
        {
            mapInterfacesInInstance(topIf, instanceIf, instanceComponent, instanceWriter);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addHierarchicalPortMapsForInstance()
//-----------------------------------------------------------------------------
void VerilogGenerator::mapInterfacesInInstance(QSharedPointer<BusInterface> topIf,
    QSharedPointer<BusInterface> instanceIf, 
    QSharedPointer<Component> instanceComponent, 
    ComponentInstanceVerilogWriter* instanceWriter)
{
    foreach(QSharedPointer<General::PortMap> topMap, topIf->getPortMaps())
    {
        foreach(QSharedPointer<General::PortMap> instanceMap, instanceIf->getPortMaps())
        {
            if (instanceMap->logicalPort_ == topMap->logicalPort_)
            {
                QString port = instanceMap->physicalPort_;
                QString signal = topMap->physicalPort_;

                General::PortAlignment alignment = General::calculatePortAlignment(instanceMap.data(), 
                    instanceComponent->getPortLeftBound(port),instanceComponent->getPortRightBound(port),
                    topMap.data(), topComponent_->getPortLeftBound(signal), topComponent_->getPortRightBound(port));                

                if (instanceComponent->getPortWidth(port) == topComponent_->getPortWidth(signal) &&
                    alignment.port1Left_ == alignment.port2Left_ && alignment.port1Right_ == alignment.port2Right_)
                {
                    instanceWriter->assignPortForFullWidth(port, signal);
                }
                else
                {
                    instanceWriter->assignPortForRange(port, signal, alignment.port2Left_, alignment.port2Right_);    
                }           
            }
        }
    }
}
