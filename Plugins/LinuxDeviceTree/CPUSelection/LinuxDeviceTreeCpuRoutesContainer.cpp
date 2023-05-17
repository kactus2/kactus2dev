//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCpuRoutesContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.05.2023
//
// Description:
// Container for routes connected to CPUs in Linux Device Tree.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeCpuRoutesContainer.h"

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::LinuxDeviceTreeCpuRoutesContainer()
//-----------------------------------------------------------------------------
LinuxDeviceTreeCpuRoutesContainer::LinuxDeviceTreeCpuRoutesContainer() :
CpuRoutesContainer(),
cpus_(),
filePath_(""),
deviceTreeRoutes_()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::LinuxDeviceTreeCpuRoutesContainer()
//-----------------------------------------------------------------------------
LinuxDeviceTreeCpuRoutesContainer::LinuxDeviceTreeCpuRoutesContainer(const CpuRoutesContainer& other) :
CpuRoutesContainer(other),
cpus_(),
filePath_(""),
deviceTreeRoutes_()
{
    setupDeviceTreeRoutes(other.getRoutes());
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::getCpus()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<const Cpu> > LinuxDeviceTreeCpuRoutesContainer::getCpus() const
{
    return cpus_;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::setCpus()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCpuRoutesContainer::setCpus(QVector<QSharedPointer<Cpu> > newCpus)
{
    cpus_.clear();

    for (auto singleCpu : newCpus)
    {
        cpus_.append(singleCpu);
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::addCpu()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCpuRoutesContainer::addCpu(QSharedPointer<Cpu> newCpu)
{
    cpus_.append(newCpu);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::getFilePath()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreeCpuRoutesContainer::getFilePath() const
{
    return filePath_;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::setFilePath()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCpuRoutesContainer::setFilePath(QString const& newFilePath)
{
    filePath_ = newFilePath;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::getDeviceTreeRoutes()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> > LinuxDeviceTreeCpuRoutesContainer::getDeviceTreeRoutes() const
{
    return deviceTreeRoutes_;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::setRoutes()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCpuRoutesContainer::setRoutes(QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > newContainers)
{
    CpuRoutesContainer::setRoutes(newContainers);
    setupDeviceTreeRoutes(newContainers);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::setupDeviceTreeRoutes()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCpuRoutesContainer::setupDeviceTreeRoutes(QVector<QSharedPointer<CpuRouteStructs::CpuRoute>> newRouteContainers)
{
    for (auto routeContainer : newRouteContainers)
    {
        setupSingleDeviceTreeRoute(routeContainer);
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::setupSingleDeviceTreeRoute()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCpuRoutesContainer::setupSingleDeviceTreeRoute(QSharedPointer<CpuRouteStructs::CpuRoute> routeContainer)
{
    for (auto route : routeContainer->routes_)
    {
        QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> cpuDeviceTree = getDeviceTreeForRoute(route, deviceTreeRoutes_);
        if (cpuDeviceTree && !deviceTreeRoutes_.contains(cpuDeviceTree))
        {
            deviceTreeRoutes_.append(cpuDeviceTree);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::getDeviceTreeForRoute()
//-----------------------------------------------------------------------------
QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> LinuxDeviceTreeCpuRoutesContainer::getDeviceTreeForRoute(
    QVector<QSharedPointer<const ConnectivityInterface> > route,
    QVector<QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> > treeRoutes)
{
    if (route.isEmpty())
    {
        return QSharedPointer<DeviceTreeUtilities::CpuDeviceTree>();
    }

    QVector<QSharedPointer<const ConnectivityInterface> > shortenedRoute = route;
    QSharedPointer<const ConnectivityInterface> startingInterface = shortenedRoute.takeFirst();

    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> treeContainer = getDeviceTreeContainerForInterface(startingInterface, treeRoutes);
    if (!treeContainer)
    {
        treeContainer = QSharedPointer<DeviceTreeUtilities::CpuDeviceTree>(new DeviceTreeUtilities::CpuDeviceTree());
        treeContainer->interface_ = startingInterface;
    }

    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> childContainer = getDeviceTreeForRoute(shortenedRoute, treeContainer->childContainers_);
    if (childContainer && !treeContainer->childContainers_.contains(childContainer))
    {
        treeContainer->childContainers_.append(childContainer);
    }

    return treeContainer;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::getDeviceTreeContainerForInterface()
//-----------------------------------------------------------------------------
QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> LinuxDeviceTreeCpuRoutesContainer::getDeviceTreeContainerForInterface(
    QSharedPointer<const ConnectivityInterface> interfaceNode,
    QVector<QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> > treeContainers) const
{
    for (auto container : treeContainers)
    {
        if (container->interface_ == interfaceNode)
        {
            return container;
        }
    }

    return QSharedPointer<DeviceTreeUtilities::CpuDeviceTree>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCpuRoutesContainer::addRoute()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCpuRoutesContainer::addRoute(QSharedPointer<CpuRouteStructs::CpuRoute> newContainer)
{
    CpuRoutesContainer::addRoute(newContainer);

    for (auto route : newContainer->routes_)
    {
        QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> cpuDeviceTree = getDeviceTreeForRoute(route, deviceTreeRoutes_);
        if (cpuDeviceTree && !deviceTreeRoutes_.contains(cpuDeviceTree))
        {
            deviceTreeRoutes_.append(cpuDeviceTree);
        }
    }
}