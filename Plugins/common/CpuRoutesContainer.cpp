//-----------------------------------------------------------------------------
// File: CpuRoutesContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// Container for routes connected to a master interface.
//-----------------------------------------------------------------------------

#include "CpuRoutesContainer.h"

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::CpuRoutesContainer()
//-----------------------------------------------------------------------------
CpuRoutesContainer::CpuRoutesContainer():
fileName_(),
fileID_(),
createFile_(false),
routeContainers_()
{

}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::CpuRoutesContainer()
//-----------------------------------------------------------------------------
CpuRoutesContainer::CpuRoutesContainer(const CpuRoutesContainer& other) :
fileName_(other.getFileName()),
fileID_(other.getFileID()),
createFile_(other.shouldCreateFile()),
routeContainers_()
{
    for (auto route : other.getRoutes())
    {
        QSharedPointer<CpuRouteStructs::CpuRoute> newRoute(new CpuRouteStructs::CpuRoute());
        newRoute->cpuInterface_ = route->cpuInterface_;
        newRoute->routes_ = route->routes_;

        routeContainers_.append(newRoute);
    }
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::getFileName()
//-----------------------------------------------------------------------------
QString CpuRoutesContainer::getFileName() const
{
    return fileName_;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::setFileName()
//-----------------------------------------------------------------------------
void CpuRoutesContainer::setFileName(QString const& newName)
{
    fileName_ = newName;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::getFileID()
//-----------------------------------------------------------------------------
QString CpuRoutesContainer::getFileID() const
{
    return fileID_;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::setCPUID()
//-----------------------------------------------------------------------------
void CpuRoutesContainer::setFileID(QString const& newID)
{
    fileID_ = newID;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::shouldCreateFile()
//-----------------------------------------------------------------------------
bool CpuRoutesContainer::shouldCreateFile() const
{
    return createFile_;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::setCreateFileFlag()
//-----------------------------------------------------------------------------
void CpuRoutesContainer::setCreateFileFlag(bool newFlag)
{
    createFile_ = newFlag;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::getRoutes()
//-----------------------------------------------------------------------------
QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > CpuRoutesContainer::getRoutes() const
{
    return routeContainers_;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::setRoutes()
//-----------------------------------------------------------------------------
void CpuRoutesContainer::setRoutes(QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > newRoutes)
{
    routeContainers_ = newRoutes;
}

//-----------------------------------------------------------------------------
// Function: CpuRoutesContainer::addRoute()
//-----------------------------------------------------------------------------
void CpuRoutesContainer::addRoute(QSharedPointer<CpuRouteStructs::CpuRoute> newRoute)
{
    routeContainers_.append(newRoute);
}
