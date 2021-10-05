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
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <library/LibraryInterface.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

#include <QTextStream>

namespace
{
    QString const TABPREFIX = QLatin1String("\t");
};

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
    bool writeAddressBlocks, QString const& outputPath)
{
    QSharedPointer<ConnectivityGraph> graph = graphFactory_.createConnectivityGraph(topComponent, activeView);
    MasterSlavePathSearch searchAlgorithm;

    writeFile(outputPath, topComponent, activeView, writeAddressBlocks,
        getMasterRoots(searchAlgorithm.findMasterSlaveRoots(graph)));
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getMasterRoots()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityInterface> > LinuxDeviceTreeGenerator::getMasterRoots(
    QVector<QSharedPointer<ConnectivityInterface>> roots)
{
    QVector<QSharedPointer<ConnectivityInterface> > masterRoots;
    while (!roots.isEmpty())
    {
        QSharedPointer<ConnectivityInterface> rootNode = roots.takeFirst();

        LinuxDeviceTreeGenerator::ComponentCPUContainer rootCPU = getCPUsForInterface(rootNode);
        if (rootCPU.interfacedCPUs_.isEmpty())
        {
            masterRoots.append(rootNode);
        }
        else
        {
            QVector<QSharedPointer<ConnectivityInterface> > rootsSharingCPU;

            for (int i = roots.size() - 1; i >= 0; --i)
            {
                QSharedPointer<ConnectivityInterface> comparisonRoot = roots.at(i);
                LinuxDeviceTreeGenerator::ComponentCPUContainer comparisonCPU =
                    getCPUsForInterface(comparisonRoot);
                if (cpuContainersMatch(rootCPU, comparisonCPU))
                {
                    rootsSharingCPU.prepend(comparisonRoot);
                    roots.remove(i);
                }
            }

            if (rootsSharingCPU.isEmpty())
            {
                masterRoots.append(rootNode);
            }
            else
            {
                rootsSharingCPU.prepend(rootNode);

                QSharedPointer<ConnectivityInterface> virtualMasterRoot(
                    new ConnectivityInterface(QLatin1String("virtualInterface")));
                
                virtualMasterRoot->setConnectedMemory(rootNode->getConnectedMemory());
                virtualMasterRoot->setInstance(rootNode->getInstance());
                virtualMasterRoot->setMode(rootNode->getMode());

                for (auto cpuRoot : rootsSharingCPU)
                {
                    virtualMasterRoot->addChildInterfaceNode(cpuRoot);
                }

                masterRoots.append(virtualMasterRoot);
            }
        }
    }

    return masterRoots;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::cpuContainersMatch()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeGenerator::cpuContainersMatch(ComponentCPUContainer const& firstContainer,
    ComponentCPUContainer const& secondContainer) const
{
    QVector<QSharedPointer<const Cpu> > firstCPUs = firstContainer.interfacedCPUs_;
    QVector<QSharedPointer<const Cpu> > secondCPUs = secondContainer.interfacedCPUs_;

    if (firstCPUs.size() != secondCPUs.size())
    {
        return false;
    }

    for (int i = 0; i < firstCPUs.size(); ++i)
    {
        if (firstCPUs.at(i) != secondCPUs.at(i))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeFile()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeFile(QString const& outputPath, QSharedPointer<Component> topComponent,
    QString const& activeView, bool writeAddressBlocks, QVector<QSharedPointer<ConnectivityInterface>> masterRoots)
{
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    int pathNumber = 0;
    QVector<QSharedPointer<const Cpu> > connectedCPUs;
    writePaths(outputStream, topComponent, activeView, masterRoots, pathNumber, connectedCPUs, writeAddressBlocks);

    writeUnconnectedCPUs(outputStream, pathNumber, connectedCPUs, topComponent, activeView);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePaths()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePaths(QTextStream& outputStream, QSharedPointer<Component> topComponent,
    QString const& activeView, QVector<QSharedPointer<ConnectivityInterface>> interfaceNodes, int pathNumber,
    QVector<QSharedPointer<const Cpu>>& connectedCPUs, bool writeAddressBlocks)
{
    for (auto node : interfaceNodes)
    {
        LinuxDeviceTreeGenerator::ComponentCPUContainer cpuContainer = getCPUsForInterface(node);
        if (cpuContainer.interfacedCPUs_.isEmpty())
        {
            writePaths(outputStream, topComponent, activeView, node->getChildInterfaceNodes(), pathNumber,
                connectedCPUs, writeAddressBlocks);
        }
        else
        {
            bool writePathOk = true;
            foreach(QSharedPointer<const Cpu> currentCPU, cpuContainer.interfacedCPUs_)
            {
                if (!connectedCPUs.contains(currentCPU))
                {
                    connectedCPUs.append(currentCPU);
                }
                else
                {
                    writePathOk = false;
                }
            }

            if (writePathOk)
            {
                startPathWriting(outputStream, node, cpuContainer, pathNumber, writeAddressBlocks);
                outputStream << endl;
                pathNumber++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getCPUsForInterface()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::ComponentCPUContainer LinuxDeviceTreeGenerator::getCPUsForInterface(
    QSharedPointer<ConnectivityInterface> startInterface) const
{
    LinuxDeviceTreeGenerator::ComponentCPUContainer cpuContainer;

    if (startInterface && startInterface->getInstance())
    {
        QSharedPointer<const Component> component = getComponentContainingInterface(startInterface);
        if (component)
        {
            cpuContainer = getComponentCPUsForInterface(startInterface, component);

            if (!cpuContainer.interfacedCPUs_.isEmpty())
            {
                quint64 memoryItemBaseAddress = 0;
                quint64 memoryItemRange = 0;
                cpuContainer.memory_ = getMemories(QSharedPointer<ConnectivityInterface>(), startInterface,
                    memoryItemBaseAddress, memoryItemRange);
            }
        }
    }

    return cpuContainer;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getComponentContainingInterface()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> LinuxDeviceTreeGenerator::getComponentContainingInterface(
    QSharedPointer<const ConnectivityInterface> interfaceNode) const
{
    VLNV componentVLNV(VLNV::COMPONENT, interfaceNode->getInstance()->getVlnv());

    QSharedPointer<Document const> componentDocument = library_->getModelReadOnly(componentVLNV);
    if (componentDocument)
    {
        QSharedPointer<Component const> component = componentDocument.dynamicCast<Component const>();
        if (component)
        {
            return component;
        }
    }

    return QSharedPointer<const Component>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getMemories()
//-----------------------------------------------------------------------------
QVector<LinuxDeviceTreeGenerator::CpuMemory> LinuxDeviceTreeGenerator::getMemories(
    QSharedPointer<const ConnectivityInterface> previousInterface,
    QSharedPointer<ConnectivityInterface> interfaceNode, quint64 baseAddress, quint64 const& memoryItemRange) const
{
    QVector<LinuxDeviceTreeGenerator::CpuMemory> newCpuMemories;
    quint64 newBaseAddress = baseAddress;
    quint64 newMemoryRange = memoryItemRange;

    if (interfaceNode->getMode().compare(QLatin1String("master"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += interfaceNode->getBaseAddress().toULongLong();
    }
    else if (interfaceNode->getMode().compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += interfaceNode->getRemapAddress().toULongLong();
        newMemoryRange = interfaceNode->getRemapRange().toULongLong();
    }
    else if (interfaceNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0)
    {
        if (interfacedItemIsMemory(interfaceNode))
        {
            QPair<quint64, quint64> memoryAddress = MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(
                interfaceNode->getConnectedMemory());

            quint64 memoryBase = newBaseAddress;
            newBaseAddress += memoryAddress.first;
            if (newMemoryRange == 0)
            {
                newMemoryRange = memoryAddress.second + 1;
            }

            LinuxDeviceTreeGenerator::CpuMemory newMemory{ interfaceNode, memoryBase, newBaseAddress, newMemoryRange };
            newCpuMemories.append(newMemory);
            return newCpuMemories;
        }
    }

    for (int i = interfaceNode->getChildInterfaceNodes().size() - 1; i >= 0; i--)
    {
        QSharedPointer<ConnectivityInterface> pathNode = interfaceNode->getChildInterfaceNodes().at(i);
        if (pathNode != previousInterface)
        {
            QVector<LinuxDeviceTreeGenerator::CpuMemory> nodeMemories =
                getMemories(interfaceNode, pathNode, newBaseAddress, newMemoryRange);
            if (!nodeMemories.isEmpty())
            {
                newCpuMemories.append(nodeMemories);
            }
        }
    }

    return newCpuMemories;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::interfacedItemIsMemory()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeGenerator::interfacedItemIsMemory(QSharedPointer<const ConnectivityInterface> memoryInterface)
    const
{
    QSharedPointer<MemoryItem> interfacedMemoryItem = memoryInterface->getConnectedMemory();
    if (interfacedMemoryItem && interfacedMemoryItem->getUsage() == General::MEMORY)
    {
        return true;
    }

    return false;
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
// Function: LinuxDeviceTreeGenerator::startPathWriting()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::startPathWriting(QTextStream& outputStream,
    QSharedPointer<const ConnectivityInterface> pathRootNode, ComponentCPUContainer cpuContainer, int pathNumber,
    bool writeAddressBlocks)
{
    writeTreeStart(outputStream, pathNumber);

    QString prefix = TABPREFIX;

    writeIntroductionToCPUs(outputStream, prefix);

    for (int i = 0; i < cpuContainer.interfacedCPUs_.size(); ++i)
    {
        QSharedPointer<Cpu const> currentCPU = cpuContainer.interfacedCPUs_.at(i);
        writeCPU(outputStream, currentCPU->name(), cpuContainer.containingComponent_->getVlnv(), i, prefix);
    }

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
    outputStream << endl;

    quint64 memoryItemBaseAddress = 0;
    quint64 memoryItemRange = 0;
    writePathNode(outputStream, QSharedPointer<ConnectivityInterface>(), pathRootNode, memoryItemBaseAddress,
        memoryItemRange, prefix, writeAddressBlocks);

    writeMemories(outputStream, cpuContainer, prefix, writeAddressBlocks);

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePathNode()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePathNode(QTextStream& outputStream,
    QSharedPointer<const ConnectivityInterface> previousInterface,
    QSharedPointer<const ConnectivityInterface> interfaceNode, quint64 const& baseAddress,
    quint64 const& memoryItemRange, QString& prefix, bool writeAddressBlocks)
{
    if (!canWriteNode(interfaceNode, previousInterface))
    {
        return;
    }

    quint64 newBaseAddress = baseAddress;
    quint64 newMemoryRange = memoryItemRange;

    if (interfaceNode->getMode().compare(QLatin1String("master"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += interfaceNode->getBaseAddress().toULongLong();
    }
    else if (interfaceNode->getMode().compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += interfaceNode->getRemapAddress().toULongLong();
        newMemoryRange = interfaceNode->getRemapRange().toULongLong();
    }
    else if (interfaceNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0)
    {
        if (interfaceNode->isBridged())
        {
            writeBridge(outputStream, interfaceNode, prefix);
        }
        else if (interfaceNode->getConnectedMemory())
        {
            QPair<quint64, quint64> memoryAddress =
                MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(
                    interfaceNode->getConnectedMemory());

            quint64 mapBaseAddress = newBaseAddress + memoryAddress.first;
            if (newMemoryRange == 0)
            {
                newMemoryRange = memoryAddress.second + 1;
            }

            writeMemoryMapItem(outputStream, interfaceNode, newBaseAddress, mapBaseAddress, newMemoryRange,
                prefix, writeAddressBlocks);
            outputStream << endl;

            return;
        }
    }

    for (auto pathNode : interfaceNode->getChildInterfaceNodes())
    {
        if (pathNode != previousInterface)
        {
            writePathNode(
                outputStream, interfaceNode, pathNode, newBaseAddress, newMemoryRange, prefix, writeAddressBlocks);

            if (pathNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0 &&
                pathNode->isBridged())
            {
                outputStream << endl;
            }
        }
    }

    if (interfaceNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0 &&
        interfaceNode->isBridged())
    {
        prefix.remove(0, 1);
        writeLineEnding(outputStream, prefix);
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::canWriteNode()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeGenerator::canWriteNode(QSharedPointer<const ConnectivityInterface> interfaceNode,
    QSharedPointer<const ConnectivityInterface> previousNode) const
{
    if (interfacedItemIsMemory(interfaceNode))
    {
        return false;
    }
    else if (interfaceNode->getChildInterfaceNodes().isEmpty())
    {
        return true;
    }

    for (auto childNode : interfaceNode->getChildInterfaceNodes())
    {
        if (childNode != previousNode && canWriteNode(childNode, interfaceNode))
        {
            return true;
        }
    }

    return false;
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
void LinuxDeviceTreeGenerator::writeLineEnding(QTextStream& outputStream, QString const& prefix) const
{
    outputStream << prefix << "};" << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeIntroductionCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeIntroductionToCPUs(QTextStream& outputStream, QString& prefix)
{
    outputStream << prefix << "cpus {" << endl;

    prefix.append(TABPREFIX);

    outputStream << prefix << "#address-cells = <1>;" << endl;
    outputStream << prefix << "#size-cells = <1>;" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeCPU()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeCPU(QTextStream& outputStream, QString const& CPUName,
    VLNV const& componentVLNV, int cpuNumber, QString& prefix)
{
    outputStream << prefix << "// '" << CPUName << "' in component " << componentVLNV.toString() << endl;
    outputStream << prefix << "cpu@" << QString::number(cpuNumber) << " {" << endl;
    prefix.append(TABPREFIX);

    outputStream << prefix << "reg = <" << QString::number(cpuNumber) << ">;" << endl;

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeBridge()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeBridge(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> interfaceNode, QString& prefix)
{
    QString instanceName = interfaceNode->getInstance()->getName();
    QString componentVLNV = interfaceNode->getInstance()->getVlnv();
    QSharedPointer<const Component> interfacedComponent = getComponentContainingInterface(interfaceNode);

    outputStream << prefix << "// Instance '" << instanceName << "' of bridge component " << componentVLNV << endl;
    outputStream << prefix << interfacedComponent->getVlnv().getName() << " {" << endl;

    prefix.append(TABPREFIX);

    outputStream << prefix << "compatible = \"simple-bus\";" << endl;
    outputStream << prefix << "#address-cells = <0x1>;" << endl;
    outputStream << prefix << "#size-cells = <0x1>;" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMemoryMapItem()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemoryMapItem(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> interfaceNode, quint64 itemBaseAddress, quint64 mapBaseAddress,
    quint64 range, QString& prefix, bool writeAddressBlocks)
{
    QString instanceName = interfaceNode->getInstance()->getName();
    QString componentVLNV = interfaceNode->getInstance()->getVlnv();
    QString memoryItemName = interfaceNode->getConnectedMemory()->getName();

    writeMemoryData(outputStream, memoryItemName, instanceName, componentVLNV, itemBaseAddress, mapBaseAddress,
        range, false, prefix, interfaceNode, writeAddressBlocks);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMapComment()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemoryData(QTextStream& outputStream, QString const& itemName,
    QString const& instanceName, QString const& componentVLNV, quint64 const& itemBaseAddress,
    quint64 const& mapBaseAddress, quint64 const& range, bool isMemory, QString& prefix,
    QSharedPointer<ConnectivityInterface const> memoryNode, bool writeAddressBlocks) const
{
    outputStream << prefix << "// Memory map '" << itemName << "' in instance '" << instanceName <<
        "' of component " << componentVLNV << endl;

    QString writtenName = itemName;

    if (isMemory)
    {
        writtenName = QLatin1String("memory");
    }
    outputStream << prefix << writtenName << "@" << QString::number(mapBaseAddress, 16).toUpper() << " {" << endl;
    prefix.append(TABPREFIX);

    if (isMemory)
    {
        outputStream << prefix << QLatin1String("device_type = \"memory\";") << endl;
    }

    outputStream << prefix << "reg = <0x" << QString::number(mapBaseAddress, 16).toUpper() << " 0x" <<
        QString::number(range, 16).toUpper() << ">;" << endl;

    if (writeAddressBlocks)
    {
        writeAddressBlocksData(outputStream, memoryNode, itemBaseAddress, prefix);
    }

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeAddressBlocksData()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeAddressBlocksData(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> memoryNode, quint64 const& itemBaseAddress, QString& prefix) const
{
    for (auto childItem : memoryNode->getConnectedMemory()->getChildItems())
    {
        if (childItem->getType() == "addressBlock")
        {
            writeSingleAddressBlock(outputStream, memoryNode, childItem, itemBaseAddress, prefix);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeSingleAddressBlock()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeSingleAddressBlock(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> containingMemoryNode, QSharedPointer<MemoryItem> blockItem,
    quint64 const& itemBaseAddress, QString& prefix) const
{
    QString blockName = blockItem->getName();

    outputStream << endl;
    outputStream << prefix << "// Address block '" << blockName << "'" << endl;

    quint64 blockAddress = blockItem->getAddress().toULongLong() + itemBaseAddress;
    QString blockAddressString = QString::number(blockAddress, 16);

    outputStream << prefix << blockItem->getName() << "@" << blockAddressString << " {" <<  endl;
    prefix.append(TABPREFIX);

    QStringList vlnvList = containingMemoryNode->getInstance()->getVlnv().split(":");
    QString vendor = "";
    if (vlnvList.size() > 0)
    {
        vendor = vlnvList.first();
    }

    QString blockStatus = "disabled";
    bool canConvertPresence = false;
    int blockPresence = blockItem->getIsPresent().toInt(&canConvertPresence);
    if (canConvertPresence & blockPresence != 0)
    {
        blockStatus = "okay";
    }

    outputStream << prefix << "compatible = \"" << vendor << ", " << blockName << "\";" << endl;
    outputStream << prefix << "status = \"" << blockStatus << "\";" << endl;
    outputStream << prefix << "#address-cells = <2>;" << endl;
    outputStream << prefix << "#size-cells = <1>;" << endl;

    General::Usage blockUsage = blockItem->getUsage();
    if (blockUsage == General::REGISTER)
    {
        quint64 blockRange = blockItem->getRange().toULongLong();
        QString blockRangeString = QString::number(blockRange, 16);

        outputStream << prefix << "reg = <0x" << blockAddressString << " 0x" << blockRangeString << ">;" << endl;
    }
    else if (blockUsage == General::MEMORY)
    {
        outputStream << prefix << "device_type = \"memory\"" << endl;
    }

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMemories()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemories(QTextStream& outputStream, ComponentCPUContainer const& cpuContainer,
    QString& prefix, bool writeAddressBlocks) const
{
    for (auto memory : cpuContainer.memory_)
    {
        QString mapName = memory.memoryInterface_->getConnectedMemory()->getName();
        QString instanceName = memory.memoryInterface_->getInstance()->getName();
        QString componentVLNV = memory.memoryInterface_->getInstance()->getVlnv();

        writeMemoryData(outputStream, mapName, instanceName, componentVLNV, memory.itemBaseAddress_,
            memory.mapBaseAddress_, memory.range_, true, prefix, memory.memoryInterface_, writeAddressBlocks);

        outputStream << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeUnconnectedCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeUnconnectedCPUs(QTextStream& outputStream, int& pathNumber,
    QVector<QSharedPointer<Cpu const>>& connectedCPUs, QSharedPointer<Component const> topComponent,
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
    QSharedPointer<Component const> component, QVector<QSharedPointer<Cpu const>>& connectedCPUs)
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
        QString prefix = TABPREFIX;
        writeTreeStart(outputStream, pathNumber);
        writeIntroductionToCPUs(outputStream, prefix);

        for (int i = 0; i < unconnectedComponentCPUs.size(); ++i)
        {
            QSharedPointer<Cpu> currentCPU = unconnectedComponentCPUs.at(i);
            writeCPU(outputStream, currentCPU->name(), component->getVlnv(), i, prefix);
        }

        prefix.remove(0, 1);
        writeLineEnding(outputStream, prefix);
        prefix.remove(0, 1);
        writeLineEnding(outputStream, prefix);
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
    QVector<QSharedPointer<Cpu const>>& connectedCPUs, QSharedPointer<const Design> design,
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
