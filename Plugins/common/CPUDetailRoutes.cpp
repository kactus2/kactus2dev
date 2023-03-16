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
CPUDetailRoutes::CPUDetailRoutes():
cpuName_(),
cpuID_(),
createFile_(false),
cpuInterface_(),
routes_(new QVector < QVector<QSharedPointer<const ConnectivityInterface> > >())
{

}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::CPUDetailRoutes()
//-----------------------------------------------------------------------------
CPUDetailRoutes::CPUDetailRoutes(const CPUDetailRoutes& other) :
cpuName_(other.getCPUName()),
cpuID_(other.getCPUID()),
createFile_(other.shouldCreateFile()),
cpuInterface_(other.getCPUInterface()),
routes_(new QVector < QVector<QSharedPointer<const ConnectivityInterface> > >())
{
    for (auto route : *other.getRoutes())
    {
        routes_->append(route);
    }
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
// Function: CPUDetailRoutes::getCPUInterface()
//-----------------------------------------------------------------------------
QSharedPointer<const ConnectivityInterface> CPUDetailRoutes::getCPUInterface() const
{
    return cpuInterface_;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::setCPUInterface()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::setCPUInterface(QSharedPointer<const ConnectivityInterface> newCPUInterface)
{
    cpuInterface_ = newCPUInterface;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::getRoutes()
//-----------------------------------------------------------------------------
QSharedPointer< QVector < QVector<QSharedPointer<const ConnectivityInterface> > > > CPUDetailRoutes::getRoutes() const
{
    return routes_;
}

//-----------------------------------------------------------------------------
// Function: CPUDetailRoutes::setRoutes()
//-----------------------------------------------------------------------------
void CPUDetailRoutes::setRoutes(QVector < QVector<QSharedPointer<const ConnectivityInterface> > > newRoutes)
{
    routes_->clear();

    for (auto singleRoute : newRoutes)
    {
        routes_->append(singleRoute);
    }
}
