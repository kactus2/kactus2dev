//-----------------------------------------------------------------------------
// File: ConnectivityGraphUtilities.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.04.2021
//
// Description:
// Utility functions for connectivity graphs.
//-----------------------------------------------------------------------------

#include "ConnectivityGraphUtilities.h"

#include <KactusAPI/include/IPluginUtility.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getInterfacedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<const Component> ConnectivityGraphUtilities::getInterfacedComponent(LibraryInterface* library,
    QSharedPointer<const ConnectivityComponent> masterComponent)
{
    QSharedPointer<const Component> interfaceComponent;

    if (masterComponent)
    {
        QStringList vlnvList = masterComponent->getVlnv().split(":");
        if (vlnvList.size() == 4)
        {
            VLNV componentVLNV(VLNV::COMPONENT, vlnvList.at(0), vlnvList.at(1), vlnvList.at(2), vlnvList.at(3));
            QSharedPointer<const Document> vlnvDocument = library->getModelReadOnly(componentVLNV);
            if (vlnvDocument)
            {
                interfaceComponent = vlnvDocument.dynamicCast<const Component>();
            }
        }
    }

    return interfaceComponent;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getInterfacedCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<Cpu> > ConnectivityGraphUtilities::getInterfacedCPUs(QSharedPointer<const Component> containingComponent, QSharedPointer<const ConnectivityInterface> routeInterface)
{
    QVector<QSharedPointer<Cpu> > cpus;
    QSharedPointer<MemoryItem> interfacedMemory = routeInterface->getConnectedMemory();
    if (interfacedMemory && interfacedMemory->getType() == MemoryDesignerConstants::ADDRESSSPACE_TYPE)
    {
        QString memoryName = interfacedMemory->getName();

        for (auto componentCPU : *containingComponent->getCpus())
        {
            if (componentCPU->getAddressSpaceRefs().contains(memoryName))
            {
                cpus.append(componentCPU);
            }
        }
    }

    return cpus;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getDefaultCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<SingleCpuRoutesContainer> > ConnectivityGraphUtilities::getDefaultCPUs(LibraryInterface* library, QSharedPointer<Component> component, QString const& viewName)
{
    QVector<QSharedPointer<SingleCpuRoutesContainer> > defaultCPUs;

    ConnectivityGraphFactory graphFactory(library);
    QSharedPointer<ConnectivityGraph> graph = graphFactory.createConnectivityGraph(component, viewName);
    MasterSlavePathSearch searchAlgorithm;
    QVector<QVector<QSharedPointer<const ConnectivityInterface>>> masterRoutes =
        searchAlgorithm.findMasterSlavePaths(graph);

    for (auto masterRoute : masterRoutes)
    {
        QSharedPointer<const ConnectivityInterface> master = masterRoute.first();
        if (interfacedCpuExists(master, defaultCPUs))
        {
            for (auto checkInterface : getMatchingCpuContainers(master, defaultCPUs))
            {
                QSharedPointer<CpuRouteStructs::CpuRoute> cpuRouteDetail = getRouteForInterface(master, checkInterface->getRoutes());
                if (cpuRouteDetail)
                {
                    cpuRouteDetail->routes_.append(masterRoute);
                }
                else
                {
                    QSharedPointer<CpuRouteStructs::CpuRoute> newRoute(new CpuRouteStructs::CpuRoute());
                    newRoute->cpuInterface_ = master;
                    newRoute->routes_.append(masterRoute);

                    checkInterface->addRoute(newRoute);
                }
            }
        }
        else
        {
            QSharedPointer<const ConnectivityComponent> routeComponent = master->getInstance();
            QSharedPointer<const Component> interfaceComponent =
                ConnectivityGraphUtilities::getInterfacedComponent(library, routeComponent);
            if (interfaceComponent)
            {
                for (auto interfaceCPU : ConnectivityGraphUtilities::getInterfacedCPUs(interfaceComponent, master))
                {
                    QString checkBoxText = interfaceCPU->name() + " [" + routeComponent->getName() + "]";

                    QSharedPointer<SingleCpuRoutesContainer> existingRoute = getCpuDetailRoute(interfaceCPU, defaultCPUs);
                    if (existingRoute)
                    {
                        QSharedPointer<CpuRouteStructs::CpuRoute> cpuRouteDetail = getRouteForInterface(master, existingRoute->getRoutes());
                        if (!cpuRouteDetail)
                        {
                            QSharedPointer<CpuRouteStructs::CpuRoute> newRoute(new CpuRouteStructs::CpuRoute());
                            newRoute->cpuInterface_ = master;
                            newRoute->routes_.append(masterRoute);

                            existingRoute->addRoute(newRoute);
                        }
                    }
                    else
                    {
                        QSharedPointer<SingleCpuRoutesContainer> cpuInterface(new SingleCpuRoutesContainer(interfaceCPU));
                        cpuInterface->setFileName(checkBoxText);
                        cpuInterface->setFileID(checkBoxText);

                        QSharedPointer<CpuRouteStructs::CpuRoute> newRoute(new CpuRouteStructs::CpuRoute());
                        newRoute->cpuInterface_ = master;
                        newRoute->routes_.append(masterRoute);

                        cpuInterface->addRoute(newRoute);

                        cpuInterface->setCreateFileFlag(true);

                        defaultCPUs.append(cpuInterface);
                    }
                }
            }
        }
    }

    return defaultCPUs;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getRouteForInterface()
//-----------------------------------------------------------------------------
QSharedPointer<CpuRouteStructs::CpuRoute> ConnectivityGraphUtilities::getRouteForInterface(
    QSharedPointer<const ConnectivityInterface> masterInterface, QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > cpuRoutes)
{
    for (auto cpuRouteDetail : cpuRoutes)
    {
        if (cpuRouteDetail->cpuInterface_ == masterInterface)
        {
            return cpuRouteDetail;
        }
    }

    return QSharedPointer<CpuRouteStructs::CpuRoute>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getCpuDetailRoute()
//-----------------------------------------------------------------------------
QSharedPointer<SingleCpuRoutesContainer> ConnectivityGraphUtilities::getCpuDetailRoute(QSharedPointer<Cpu> comparisonCpu,
    QVector<QSharedPointer<SingleCpuRoutesContainer>> existingRoutes)
{
    for (auto currentRoute : existingRoutes)
    {
        if (currentRoute->getCpu() == comparisonCpu)
        {
            return currentRoute;
        }
    }

    return QSharedPointer<SingleCpuRoutesContainer>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::interfacedCpuExists()
//-----------------------------------------------------------------------------
bool ConnectivityGraphUtilities::interfacedCpuExists(QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<SingleCpuRoutesContainer>> cpuList)
{
    for (auto checkInterface : cpuList)
    {
        for (auto cpuRoute : checkInterface->getRoutes())
        {
            if (cpuRoute->cpuInterface_ == master)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getMatchingCheckInterface()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<SingleCpuRoutesContainer> > ConnectivityGraphUtilities::getMatchingCpuContainers(
    QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<SingleCpuRoutesContainer> > cpuList)
{
    QVector<QSharedPointer<SingleCpuRoutesContainer> > cpuContainer;

    for (auto checkInterface : cpuList)
    {
        for (auto cpuRoute : checkInterface->getRoutes())
        {
            if (cpuRoute->cpuInterface_ == master && !cpuContainer.contains(checkInterface))
            {
                cpuContainer.append(checkInterface);
            }
        }
    }

    return cpuContainer;
}
