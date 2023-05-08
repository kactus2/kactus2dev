//-----------------------------------------------------------------------------
// File: CPUDetailRoutes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// Container for routes connected to a master interface.
//-----------------------------------------------------------------------------

#include "CPUDetailRoutes.h"

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::CPUDetailRoutes()
//-----------------------------------------------------------------------------
CPUDetailRoutes::CPUDetailRoutes(QSharedPointer<Cpu> cpu):
cpu_(cpu),
cpuName_(),
cpuID_(),
createFile_(false),
routes_()
{

}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::CPUDetailRoutes()
//-----------------------------------------------------------------------------
CPUDetailRoutes::CPUDetailRoutes(const CPUDetailRoutes& other) :
cpu_(other.getCpu()),
cpuName_(other.getCPUName()),
cpuID_(other.getCPUID()),
createFile_(other.shouldCreateFile()),
routes_()
{
    for (auto route : other.getRoutes())
    {
        QSharedPointer<CpuRouteStructs::CpuRoute> newRoute(new CpuRouteStructs::CpuRoute());
        newRoute->cpuInterface_ = route->cpuInterface_;
        newRoute->routes_ = route->routes_;

        routes_.append(newRoute);
    }
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::getCpu()
//-----------------------------------------------------------------------------
QSharedPointer<Cpu> CPUDetailRoutes::getCpu() const
{
    return cpu_;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::setCpu()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::setCpu(QSharedPointer<Cpu> newCpu)
{
    cpu_ = newCpu;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::getCPUName()
//-----------------------------------------------------------------------------
QString CPUDetailRoutes::getCPUName() const
{
    return cpuName_;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::setCPUName()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::setCPUName(QString const& newName)
{
    cpuName_ = newName;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::getCPUID()
//-----------------------------------------------------------------------------
QString CPUDetailRoutes::getCPUID() const
{
    return cpuID_;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::setCPUID()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::setCPUID(QString const& newID)
{
    cpuID_ = newID;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::shouldCreateFile()
//-----------------------------------------------------------------------------
bool CPUDetailRoutes::shouldCreateFile() const
{
    return createFile_;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::setCreateFileFlag()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::setCreateFileFlag(bool newFlag)
{
    createFile_ = newFlag;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::getRoutes()
//-----------------------------------------------------------------------------
QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > CPUDetailRoutes::getRoutes() const
{
    return routes_;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::setRoutes()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::setRoutes(QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > newRoutes)
{
    routes_ = newRoutes;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::addRoute()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::addRoute(QSharedPointer<CpuRouteStructs::CpuRoute> newRoute)
{
    routes_.append(newRoute);
}
