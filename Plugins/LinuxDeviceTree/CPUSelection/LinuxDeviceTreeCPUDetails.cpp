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

#include <Plugins/common/SingleCpuRoutesContainer.h>
#include <Plugins/common/ConnectivityGraphUtilities.h>

#include <KactusAPI/include/LibraryInterface.h>

namespace
{
    //-----------------------------------------------------------------------------
    // Function: routeExistsInList()
    //-----------------------------------------------------------------------------
    bool routeExistsInList(QSharedPointer<CpuRouteStructs::CpuRoute> route,
        QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > routeData)
    {
        for (auto comparisonRoute : routeData)
        {
            if (comparisonRoute->cpuInterface_ == route->cpuInterface_)
            {
                return true;
            }
        }

        return false;
    }

    //-----------------------------------------------------------------------------
    // Function: containerContainsSameRoutes()
    //-----------------------------------------------------------------------------
    bool containerContainsSameRoutes(QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> container,
        QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > routeData)
    {
        if (routeData.size() == container->getRoutes().size())
        {
            for (auto comparisonRoute : container->getRoutes())
            {
                if (!routeExistsInList(comparisonRoute, routeData))
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    //-----------------------------------------------------------------------------
    // Function: getContainerForRouteData()
    //-----------------------------------------------------------------------------
    QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> getContainerForRouteData(
        QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > routeData,
        QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > containers)
    {
        for (auto singleCpuContainer : containers)
        {
            if (containerContainsSameRoutes(singleCpuContainer, routeData))
            {
                return singleCpuContainer;
            }
        }

        return QSharedPointer<LinuxDeviceTreeCpuRoutesContainer>();
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDetails::getCPUContainers()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > LinuxDeviceTreeCPUDetails::getCPUContainers(
    QString const& fileName, QSharedPointer<Component> topComponent, QString const& viewName, LibraryInterface* library)
{
    QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > dtsCpuContainers;

    for (auto container : ConnectivityGraphUtilities::getDefaultCPUs(library, topComponent, viewName))
    {
        QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> singleDtsContainer =
            getContainerForRouteData(container->getRoutes(), dtsCpuContainers);
        if (!singleDtsContainer)
        {
            singleDtsContainer = QSharedPointer<LinuxDeviceTreeCpuRoutesContainer>(new LinuxDeviceTreeCpuRoutesContainer(*container.data()));

            dtsCpuContainers.append(singleDtsContainer);
        }

        singleDtsContainer->addCpu(container->getCpu());
    }

    for (int i = 0; i < dtsCpuContainers.size(); ++i)
    {
        QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> singleContainer = dtsCpuContainers.at(i);
        singleContainer->setFileName(fileName + "_" + QString::number(i));
    }

    return dtsCpuContainers;
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
// Function: LinuxDeviceTreeCPUDetails::getMemories()
//-----------------------------------------------------------------------------
QVector<LinuxDeviceTreeCPUDetails::CpuMemory> LinuxDeviceTreeCPUDetails::getMemories(
    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> deviceTreeContainer, quint64 baseAddress, quint64 const& memoryItemRange)
{
    QVector<LinuxDeviceTreeCPUDetails::CpuMemory> newCpuMemories;
    quint64 newBaseAddress = baseAddress;
    quint64 newMemoryRange = memoryItemRange;

    QSharedPointer<const ConnectivityInterface> containerInterface = deviceTreeContainer->interface_;
    if (General::InterfaceMode containerMode = containerInterface->getMode();
        containerMode == General::MASTER || containerMode == General::INITIATOR)
    {
        newBaseAddress += containerInterface->getBaseAddress().toULongLong();
    }
    else if (containerMode == General::MIRRORED_SLAVE || containerMode == General::MIRRORED_TARGET)
    {
        newBaseAddress += containerInterface->getRemapAddress().toULongLong();
        newMemoryRange = containerInterface->getRemapRange().toULongLong();
    }
    else if (containerMode == General::SLAVE || containerMode == General::TARGET)
    {
        if (interfacedItemIsMemory(containerInterface))
        {
            QPair<quint64, quint64> memoryAddress =
                MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(containerInterface->getConnectedMemory());

            quint64 memoryBase = newBaseAddress;
            newBaseAddress += memoryAddress.first;
            if (newMemoryRange == 0)
            {
                newMemoryRange = memoryAddress.second + 1;
            }

            LinuxDeviceTreeCPUDetails::CpuMemory newMemory{containerInterface, memoryBase, newBaseAddress, newMemoryRange };
            newCpuMemories.append(newMemory);
            return newCpuMemories;
        }
    }

    for (auto leafContainer : deviceTreeContainer->childContainers_)
    {
        QVector<LinuxDeviceTreeCPUDetails::CpuMemory> nodeMemories = getMemories(leafContainer, newBaseAddress, newMemoryRange);
        if (!nodeMemories.isEmpty())
        {
            newCpuMemories.append(nodeMemories);
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
