//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUDetails.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.10.2021
//
// Description:
// Details for Linux device tree CPU declarations.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeCPUDetails.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryConnectionAddressCalculator.h>

#include <KactusAPI/include/LibraryInterface.h>


//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDetails::getCPUContainers()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > LinuxDeviceTreeCPUDetails::getCPUContainers(
    QString const& fileName, QVector<QSharedPointer<ConnectivityInterface>> roots, LibraryInterface* library)
{
    QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > containers;

    QVector<QSharedPointer<ConnectivityInterface>> visitedInterfaces;
    QMultiMap<QSharedPointer<const Cpu>, QSharedPointer<ConnectivityInterface> > cpuNodes =
        getCPUInterfacePairs(roots, visitedInterfaces, library);

    QMultiMapIterator<QSharedPointer<const Cpu>, QSharedPointer<ConnectivityInterface> > cpuNodeIterator(cpuNodes);
    while (cpuNodeIterator.hasNext())
    {
        cpuNodeIterator.next();

        QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> containerCPU =
            getContainerForCPU(cpuNodeIterator.key(), containers);
        if (!containerCPU)
        {
            containerCPU = QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer>(
                new LinuxDeviceTreeCPUDetails::CPUContainer());
            containerCPU->createFile_ = true;
            containerCPU->fileName_ = "";
            containerCPU->filePath_ = "";
            containerCPU->interfacedCPUs_.append(cpuNodeIterator.key());

            containers.append(containerCPU);
        }

        containerCPU->rootInterfaces_.append(cpuNodeIterator.value());
    }

    for (int i = 0; i < containers.size(); ++i)
    {
        QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> singleContainer = containers.at(i);
        singleContainer->fileName_ = fileName + "_" + QString::number(i);

        int comparisonIterator = i + 1;
        while (comparisonIterator < containers.size())
        {
            QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> comparisonContainer =
                containers.at(comparisonIterator);
            if (singleContainer->rootInterfaces_ == comparisonContainer->rootInterfaces_)
            {
                singleContainer->interfacedCPUs_.append(comparisonContainer->interfacedCPUs_);
                containers.remove(comparisonIterator);
            }
            else
            {
                comparisonIterator++;
            }
        }
    }

    return containers;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDetails::getCPUInterfacePairs()
//-----------------------------------------------------------------------------
QMultiMap<QSharedPointer<const Cpu>, QSharedPointer<ConnectivityInterface> > LinuxDeviceTreeCPUDetails::
getCPUInterfacePairs(QVector<QSharedPointer<ConnectivityInterface>> roots,
    QVector<QSharedPointer<ConnectivityInterface>>& visitedInterfaces, LibraryInterface* library)
{
    QMultiMap<QSharedPointer<const Cpu>, QSharedPointer<ConnectivityInterface> > cpuNodes;

    for (auto interfaceNode : roots)
    {
        if (!visitedInterfaces.contains(interfaceNode))
        {
            visitedInterfaces.append(interfaceNode);

            QSharedPointer<const Component> component = getComponentContainingInterface(interfaceNode, library);
            if (component)
            {
                QVector<QSharedPointer<const Cpu> > cpus = getInterfacedCPUs(component, interfaceNode);

                for (auto singleCPU : cpus)
                {
                    cpuNodes.insert(singleCPU, interfaceNode);
                }

            }

            QMultiMap<QSharedPointer<const Cpu>, QSharedPointer<ConnectivityInterface> > childCPUNodes =
                getCPUInterfacePairs(interfaceNode->getChildInterfaceNodes(), visitedInterfaces, library);

            QMultiMapIterator<QSharedPointer<const Cpu>, QSharedPointer<ConnectivityInterface> >
                childIterator(childCPUNodes);
            while (childIterator.hasNext())
            {
                childIterator.next();
                cpuNodes.insert(childIterator.key(), childIterator.value());
            }
        }
    }

    return cpuNodes;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDetails::getComponentContainingInterface()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> LinuxDeviceTreeCPUDetails::getComponentContainingInterface(
    QSharedPointer<const ConnectivityInterface> interfaceNode, LibraryInterface* library)
{
    VLNV componentVLNV(VLNV::COMPONENT, interfaceNode->getInstance()->getVlnv());

    QSharedPointer<Document const> componentDocument = library->getModelReadOnly(componentVLNV);
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
// Function: LinuxDeviceTreeCPUDetails::getInterfacedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<const Cpu> > LinuxDeviceTreeCPUDetails::getInterfacedCPUs(
    QSharedPointer<const Component> containingComponent, QSharedPointer<const ConnectivityInterface> interfaceNode)
{
    QVector<QSharedPointer<const Cpu > > interfacedCPUs;

    QSharedPointer<MemoryItem> connectedMemory = interfaceNode->getConnectedMemory();
    if (connectedMemory && connectedMemory->getType().compare("addressSpace", Qt::CaseInsensitive) == 0)
    {
        QString spaceName = connectedMemory->getName();
        if (containingComponent && containingComponent->getAddressSpaceNames().contains(spaceName))
        {
            for (auto singleCPU : *containingComponent->getCpus())
            {
                if (singleCPU->getAddressSpaceRefs().contains(spaceName))
                {
                    interfacedCPUs.append(singleCPU);
                }
            }
        }
    }

    return interfacedCPUs;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDetails::getContainerForCPU()
//-----------------------------------------------------------------------------
QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> LinuxDeviceTreeCPUDetails::getContainerForCPU(
    QSharedPointer<const Cpu> singleCPU,
    QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > containers)
{
    for (auto cpuContainer : containers)
    {
        if (cpuContainer->interfacedCPUs_.contains(singleCPU))
        {
            return cpuContainer;
        }
    }

    return QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDetails::getMemories()
//-----------------------------------------------------------------------------
QVector<LinuxDeviceTreeCPUDetails::CpuMemory> LinuxDeviceTreeCPUDetails::getMemories(
    QSharedPointer<const ConnectivityInterface> previousInterface,
    QSharedPointer<ConnectivityInterface> interfaceNode, quint64 baseAddress, quint64 const& memoryItemRange)
{
    QVector<LinuxDeviceTreeCPUDetails::CpuMemory> newCpuMemories;
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

            LinuxDeviceTreeCPUDetails::CpuMemory newMemory{
                interfaceNode, memoryBase, newBaseAddress, newMemoryRange };
            newCpuMemories.append(newMemory);
            return newCpuMemories;
        }
    }

    for (int i = interfaceNode->getChildInterfaceNodes().size() - 1; i >= 0; i--)
    {
        QSharedPointer<ConnectivityInterface> pathNode = interfaceNode->getChildInterfaceNodes().at(i);
        if (pathNode != previousInterface)
        {
            QVector<LinuxDeviceTreeCPUDetails::CpuMemory> nodeMemories =
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
// Function: LinuxDeviceTreeCPUDetails::interfacedItemIsMemory()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeCPUDetails::interfacedItemIsMemory(QSharedPointer<const ConnectivityInterface> memoryInterface)
{
    QSharedPointer<MemoryItem> interfacedMemoryItem = memoryInterface->getConnectedMemory();
    if (interfacedMemoryItem && interfacedMemoryItem->getUsage() == General::MEMORY)
    {
        return true;
    }

    return false;
}
