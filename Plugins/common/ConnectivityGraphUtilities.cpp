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
// Function: ConnectivityGraphUtilities::getInterfacedCPU()
//-----------------------------------------------------------------------------
QSharedPointer<Cpu> ConnectivityGraphUtilities::getInterfacedCPU(QSharedPointer<const Component> containingComponent,
    QSharedPointer<const ConnectivityInterface> routeInterface)
{
    QSharedPointer<MemoryItem> interfacedMemory = routeInterface->getConnectedMemory();
    if (interfacedMemory && interfacedMemory->getType() == MemoryDesignerConstants::ADDRESSSPACE_TYPE)
    {
        for (auto componentCPU : *containingComponent->getCpus())
        {
            if (componentCPU->getAddressSpaceRefs().contains(interfacedMemory->getName()))
            {
                return componentCPU;
            }
        }
    }

    return QSharedPointer<Cpu>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getDefaultCPUs()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<CPUDetailRoutes> > ConnectivityGraphUtilities::getDefaultCPUs(LibraryInterface* library, QSharedPointer<Component> component, QString const& viewName)
{
    QVector<QSharedPointer<CPUDetailRoutes> > defaultCPUs;

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
            QSharedPointer<CPUDetailRoutes> checkInterface = getMatchingCpuContainer(master, defaultCPUs);
            checkInterface->getRoutes()->append(masterRoute);
        }
        else
        {
            QSharedPointer<const ConnectivityComponent> routeComponent = master->getInstance();
            QSharedPointer<const Component> interfaceComponent =
                ConnectivityGraphUtilities::getInterfacedComponent(library, routeComponent);
            if (interfaceComponent)
            {
                QSharedPointer<Cpu> interfaceCPU =
                    ConnectivityGraphUtilities::getInterfacedCPU(interfaceComponent, master);
                if (interfaceCPU)
                {
                    QString checkBoxText = interfaceCPU->name() + " [" + routeComponent->getName() + "]";
                    QSharedPointer<CPUDetailRoutes> cpuInterface(new CPUDetailRoutes());
                    cpuInterface->setCPUName(checkBoxText);
                    cpuInterface->setCPUID(checkBoxText);
                    cpuInterface->setCPUInterface(master);
                    cpuInterface->getRoutes()->append(masterRoute);
                    cpuInterface->setCreateFileFlag(true);

                    defaultCPUs.append(cpuInterface);
                }
            }
        }
    }

    return defaultCPUs;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::interfacedCpuExists()
//-----------------------------------------------------------------------------
bool ConnectivityGraphUtilities::interfacedCpuExists(QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<CPUDetailRoutes>> cpuList)
{
    for (auto checkInterface : cpuList)
    {
        if (checkInterface->getCPUInterface() == master)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getMatchingCheckInterface()
//-----------------------------------------------------------------------------
QSharedPointer<CPUDetailRoutes> ConnectivityGraphUtilities::getMatchingCpuContainer(QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<CPUDetailRoutes> > cpuList)
{
    for (auto checkInterface : cpuList)
    {
        if (checkInterface->getCPUInterface() == master)
        {
            return checkInterface;
        }
    }

    return QSharedPointer<CPUDetailRoutes>();
}
