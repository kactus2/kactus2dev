//-----------------------------------------------------------------------------
// File: LinuxDeviceTreePlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Copyright: Tampere University of Technology
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Creates a Linux Device Tree from the selected document.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeGenerator.h"

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <library/LibraryInterface.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::LinuxDeviceTreeGenerator()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::LinuxDeviceTreeGenerator(LibraryInterface* library):
library_(library),
graphFactory_(library)
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::~LinuxDeviceTreeGenerator()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::~LinuxDeviceTreeGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::generate()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::generate(QSharedPointer<Component> topComponent, QString const& activeView,
    QString const& outputPath)
{
    QSharedPointer<ConnectivityGraph> graph = graphFactory_.createConnectivityGraph(topComponent, activeView);
    MasterSlavePathSearch searchAlgorithm;

    writeFile(outputPath, topComponent, activeView, searchAlgorithm.findMasterSlavePaths(graph));
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeFile()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeFile(QString const& outputPath, QSharedPointer<Component> topComponent,
    QString const& activeView, QVector<QVector<QSharedPointer<const ConnectivityInterface> > > masterRoutes)
{
    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);
    
    writePaths(outputStream, topComponent, activeView, masterRoutes);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePaths()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePaths(QTextStream& outputStream, QSharedPointer<Component> topComponent,
    QString const& activeView, QVector<QVector<QSharedPointer<const ConnectivityInterface> > > masterPaths)
{
    int pathNumber = 0;

    QVector<QSharedPointer<Cpu> > connectedCPUs;

	foreach (auto path, masterPaths)
	{
        while (path.size() > 0)
        {
            QSharedPointer<const ConnectivityInterface> firstInterface = path.first();
            LinuxDeviceTreeGenerator::ComponentCPUContainer cpuContainer = getCPUsForInterface(firstInterface);
            if (cpuContainer.interfacedCPUs_.isEmpty())
            {
                path.removeFirst();
            }
            else
            {
                foreach(QSharedPointer<Cpu> currentCPU, cpuContainer.interfacedCPUs_)
                {
                    if (!connectedCPUs.contains(currentCPU))
                    {
                        connectedCPUs.append(currentCPU);
                    }
                }

                writePath(outputStream, path, cpuContainer, pathNumber);
                outputStream << endl;
                pathNumber++;

                break;
            }
        }
	}

    writeUnconnectedCPUs(outputStream, pathNumber, connectedCPUs, topComponent, activeView);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getCPUsForInterface()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::ComponentCPUContainer LinuxDeviceTreeGenerator::getCPUsForInterface(
    QSharedPointer<const ConnectivityInterface> startInterface) const
{
    LinuxDeviceTreeGenerator::ComponentCPUContainer cpuContainer;

    if (startInterface && startInterface->getInstance())
    {
        VLNV::IPXactType vlnvType = VLNV::COMPONENT;
        QString vlnvString = startInterface->getInstance()->getVlnv();
        VLNV componentVLNV(vlnvType, vlnvString);

        QSharedPointer<Document const> componentDocument = library_->getModelReadOnly(componentVLNV);
        if (componentDocument)
        {
            QSharedPointer<Component const> component = componentDocument.dynamicCast<Component const>();
            if (component)
            {
                cpuContainer = getComponentCPUsForInterface(startInterface, component);
            }
        }
    }

    return cpuContainer;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getComponentCPUsForInterface()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::ComponentCPUContainer LinuxDeviceTreeGenerator::getComponentCPUsForInterface(
    QSharedPointer<const ConnectivityInterface> connectionInterface,
    QSharedPointer<Component const> containingComponent) const
{
    LinuxDeviceTreeGenerator::ComponentCPUContainer cpuContainer;
    cpuContainer.containingComponent_ = containingComponent;

    QSharedPointer<MemoryItem> connectedMemory = connectionInterface->getConnectedMemory();
    if (connectedMemory && connectedMemory->getType().compare("addressSpace", Qt::CaseInsensitive) == 0)
    {
        QString spaceName = connectedMemory->getName();
        if (containingComponent && containingComponent->getAddressSpaceNames().contains(spaceName))
        {
            foreach(QSharedPointer<Cpu> singleCPU, *containingComponent->getCpus())
            {
                if (singleCPU->getAddressSpaceRefs().contains(spaceName))
                {
                    cpuContainer.interfacedCPUs_.append(singleCPU);
                }
            }
        }
    }

    return cpuContainer;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePath()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePath(QTextStream& outputStream,
    QVector<QSharedPointer<const ConnectivityInterface> > path, ComponentCPUContainer cpuContainer, int pathNumber)
{
    writeTreeStart(outputStream, pathNumber);

    QSharedPointer<const ConnectivityInterface> startInterface = path.first();
    QSharedPointer<const ConnectivityInterface> endInterface = path.last();

    MemoryConnectionAddressCalculator::ConnectionPathVariables pathVariables =
        MemoryConnectionAddressCalculator::calculatePathAddresses(startInterface, endInterface, path);

    writeIntroductionToCPUs(outputStream);
    
    for (int i = 0; i < cpuContainer.interfacedCPUs_.size(); ++i)
    {
        QSharedPointer<Cpu> currentCPU = cpuContainer.interfacedCPUs_.at(i);
        writeCPU(outputStream, currentCPU->name(), cpuContainer.containingComponent_->getVlnv(), i);
    }

    writeLineEnding(outputStream, QString("\t"));
    outputStream << endl;

    writeMultipleItems(outputStream, startInterface, endInterface, pathVariables);


    QSharedPointer<MemoryItem> endMemoryItem = endInterface->getConnectedMemory();
    if (endMemoryItem)
    {
        QString endItemName = endMemoryItem->getName();
        quint64 endItemBaseAddress = pathVariables.remappedAddress_;
        quint64 endItemLastAddress = pathVariables.remappedEndAddress_;
        writeItem(outputStream, endItemName, endItemBaseAddress, endItemLastAddress);
    }

    writeLineEnding(outputStream, QString());
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeTreeStart()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeTreeStart(QTextStream& outputStream, int pathNumber)
{
    outputStream << "/dts-v" << QString::number(pathNumber) << "/;" << endl << endl << "/ {" << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeLineEnding()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeLineEnding(QTextStream& outputStream, QString const& prefix)
{
    outputStream << prefix << "};" << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeIntroductionCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeIntroductionToCPUs(QTextStream& outputStream)
{
    outputStream << "\tcpus {" << endl;

    outputStream << "\t\t#address-cells = <1>;" << endl;
    outputStream << "\t\t#size-cells = <1>;" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeCPU()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeCPU(QTextStream& outputStream, QString const& CPUName,
    VLNV const& componentVLNV, int cpuNumber)
{
    outputStream << "\t\t// '" << CPUName << "' in component " << componentVLNV.toString() << endl;
    outputStream << "\t\tcpu@" << QString::number(cpuNumber) << " {" << endl;
    outputStream << "\t\t\treg = <" << QString::number(cpuNumber) << ">;" << endl;
    outputStream << "\t\t};" << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMultipleItems()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMultipleItems(QTextStream& outputStream,
    QSharedPointer<const ConnectivityInterface> startInterface,
    QSharedPointer<const ConnectivityInterface> endInterface,
    MemoryConnectionAddressCalculator::ConnectionPathVariables pathVariables)
{
    foreach (auto spaceChain, pathVariables.spaceChain_)
    {
        QSharedPointer<const ConnectivityInterface> spaceInterface = spaceChain.spaceInterface_;
        if (spaceInterface && spaceInterface != startInterface && spaceInterface->getConnectedMemory())
        {
            QSharedPointer<MemoryItem> spaceMemory = spaceInterface->getConnectedMemory();
            QString spaceName = spaceMemory->getName();

            quint64 offset = spaceChain.spaceConnectionBaseAddress_;
            quint64 spaceEndAddress = spaceMemory->getRange().toULongLong() + offset - 1;

            writeItem(outputStream, spaceName, offset, spaceEndAddress);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeItem()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeItem(QTextStream& outputStream, QString const& itemName, quint64 baseAddress,
    quint64 lastAddress)
{
    outputStream << "\t" << itemName << "@" << QString::number(baseAddress, 16) << " {" << endl;
    outputStream << "\t\treg = <0x" << QString::number(baseAddress, 16) << " 0x" <<
        QString::number(lastAddress, 16) << ">;" << endl;

    writeLineEnding(outputStream, QString("\t"));
    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeUnconnectedCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeUnconnectedCPUs(QTextStream& outputStream, int& pathNumber,
    QVector<QSharedPointer<Cpu> >& connectedCPUs, QSharedPointer<Component const> topComponent,
    QString const& activeView)
{
    writeUnconnectedCPUsOfComponent(outputStream, pathNumber, topComponent, connectedCPUs);

    QSharedPointer<View> designView = getView(topComponent, activeView);
    if (designView)
    {
        QSharedPointer<const DesignConfiguration> hierarchicalConfiguration =
            getDesignConfiguration(topComponent, designView);
        QSharedPointer<const Design> hierarchicalDesign =
            getHierarchicalDesign(topComponent, designView, hierarchicalConfiguration);

        if (hierarchicalDesign)
        {
            analyzeDesignForUnconnectedCPUs(
                outputStream, pathNumber, connectedCPUs, hierarchicalDesign, hierarchicalConfiguration);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeUnconnectedCPUsOfComponent()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeUnconnectedCPUsOfComponent(QTextStream& outputStream, int& pathNumber,
    QSharedPointer<Component const> component, QVector<QSharedPointer<Cpu> >& connectedCPUs)
{
    QVector<QSharedPointer<Cpu> > unconnectedComponentCPUs;
    foreach(QSharedPointer<Cpu> componentCPU, *component->getCpus())
    {
        if (!connectedCPUs.contains(componentCPU))
        {
            unconnectedComponentCPUs.append(componentCPU);
        }
    }

    if (!unconnectedComponentCPUs.isEmpty())
    {
        writeTreeStart(outputStream, pathNumber);
        writeIntroductionToCPUs(outputStream);

        for (int i = 0; i < unconnectedComponentCPUs.size(); ++i)
        {
            QSharedPointer<Cpu> currentCPU = unconnectedComponentCPUs.at(i);
            writeCPU(outputStream, currentCPU->name(), component->getVlnv(), i);
        }

        writeLineEnding(outputStream, QString("\t"));
        writeLineEnding(outputStream, QString());
        outputStream << endl;

        pathNumber++;
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getView()
//-----------------------------------------------------------------------------
QSharedPointer<View> LinuxDeviceTreeGenerator::getView(QSharedPointer<Component const> containingComponent,
    QString const& viewName) const
{
    foreach(QSharedPointer<View> view, *containingComponent->getViews())
    {
        if (view->name() == viewName)
        {
            return view;
        }
    }

    return QSharedPointer<View>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<const DesignConfiguration> LinuxDeviceTreeGenerator::getDesignConfiguration(
    QSharedPointer<Component const> containingComponent, QSharedPointer<View> referencingView) const
{
    QString configurationReference = referencingView->getDesignConfigurationInstantiationRef();
    if (!configurationReference.isEmpty())
    {
        foreach(QSharedPointer<DesignConfigurationInstantiation> instantiation,
            *containingComponent->getDesignConfigurationInstantiations())
        {
            if (instantiation->name().compare(configurationReference) == 0)
            {
                QSharedPointer<ConfigurableVLNVReference> configurationVLNV =
                    instantiation->getDesignConfigurationReference();

                QSharedPointer<const Document> configurationDocument =
                    library_->getModelReadOnly(*configurationVLNV);

                return configurationDocument.dynamicCast<const DesignConfiguration>();
            }
        }
    }

    return QSharedPointer<const DesignConfiguration>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<const Design> LinuxDeviceTreeGenerator::getHierarchicalDesign(
    QSharedPointer<Component const> containingComponent, QSharedPointer<View> referencingView,
    QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    VLNV designVLNV = getHierarchicalDesignVLNV(containingComponent, referencingView);

    if (!designVLNV.isValid() && !designConfiguration.isNull())
    {
        designVLNV = designConfiguration->getDesignRef();
    }

    if (designVLNV.isValid())
    {
        return library_->getModelReadOnly(designVLNV).dynamicCast<const Design>();
    }

    return QSharedPointer<const Design>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getHierarchicalDesignVLNV()
//-----------------------------------------------------------------------------
VLNV LinuxDeviceTreeGenerator::getHierarchicalDesignVLNV(QSharedPointer<Component const> containingComponent,
    QSharedPointer<View> referencingView) const
{
    QString referencedInstantiation = referencingView->getDesignInstantiationRef();

    if (!referencedInstantiation.isEmpty())
    {
        foreach(QSharedPointer<DesignInstantiation> instantiation, *containingComponent->getDesignInstantiations())
        {
            if (instantiation->name().compare(referencedInstantiation) == 0)
            {
                return *instantiation->getDesignReference();
            }
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::analyzeDesignForUnconnectedCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::analyzeDesignForUnconnectedCPUs(QTextStream& outputStream, int& pathNumber,
    QVector<QSharedPointer<Cpu>>& connectedCPUs, QSharedPointer<const Design> design,
    QSharedPointer<const DesignConfiguration> configuration)
{
    foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
    {
        QSharedPointer<ConfigurableVLNVReference> componentReference = instance->getComponentRef();
        QSharedPointer<Document const> componentDocument = library_->getModelReadOnly(*componentReference.data());
        if (componentDocument)
        {
            QSharedPointer<Component const> instancedComponent = componentDocument.dynamicCast<Component const>();
            if (instancedComponent)
            {
                QString componentActiveView("");
                if (configuration)
                {
                    componentActiveView = configuration->getActiveView(instance->getInstanceName());
                }

                writeUnconnectedCPUs(
                    outputStream, pathNumber, connectedCPUs, instancedComponent, componentActiveView);
            }
        }
    }
}
