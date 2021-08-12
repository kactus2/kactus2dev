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
QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > ConnectivityGraphUtilities::getDefaultCPUs(
    LibraryInterface* library, QSharedPointer<Component> component, QString const& viewName)
{
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > defaultCPUs;

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
            QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> checkInterface =
                getMatchingCpuContainer(master, defaultCPUs);
            checkInterface->routes_.append(masterRoute);
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
                    QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> cpuInterface(
                        new ConnectivityGraphUtilities::cpuDetailRoutes());
                    cpuInterface->cpuName_ = checkBoxText;
                    cpuInterface->cpuID_ = checkBoxText;
                    cpuInterface->cpuInterface_ = master;
                    cpuInterface->routes_.append(masterRoute);

                    cpuInterface->createFile_ = true;
                    cpuInterface->revision_ = QString();
                    cpuInterface->endian_ = QString();
                    cpuInterface->mpuPresent_ = false;
                    cpuInterface->fpuPresent_ = false;
                    cpuInterface->nvicPrioBits_ = QString();
                    cpuInterface->vendorSystickConfig_ = false;

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
bool ConnectivityGraphUtilities::interfacedCpuExists(QSharedPointer<const ConnectivityInterface> master,
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes>> cpuList)
{
    for (auto checkInterface : cpuList)
    {
        if (checkInterface->cpuInterface_ == master)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphUtilities::getMatchingCheckInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> ConnectivityGraphUtilities::getMatchingCpuContainer(
    QSharedPointer<const ConnectivityInterface> master,
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes>> cpuList)
{
    for (auto checkInterface : cpuList)
    {
        if (checkInterface->cpuInterface_ == master)
        {
            return checkInterface;
        }
    }

    return QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes>();
}
