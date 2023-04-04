//-----------------------------------------------------------------------------
// File: RenodeCPUDetailRoutes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.03.2023
//
// Description:
// Container for routes connected to a master interface for Renode generator.
//-----------------------------------------------------------------------------

#include "RenodeCPUDetailRoutes.h"

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::RenodeCPUDetailRoutes()
//-----------------------------------------------------------------------------
RenodeCPUDetailRoutes::RenodeCPUDetailRoutes():
CPUDetailRoutes(),
peripherals_(),
memories_(),
className_(),
cpuType_(),
timeProvider_()
{
    initializeCpuData();
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::RenodeCPUDetailRoutes()
//-----------------------------------------------------------------------------
RenodeCPUDetailRoutes::RenodeCPUDetailRoutes(const CPUDetailRoutes& other) :
CPUDetailRoutes(other),
peripherals_(),
memories_(),
className_(),
cpuType_(),
timeProvider_()
{
    initializeCpuData();
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::initializeCpuData()
//-----------------------------------------------------------------------------
void RenodeCPUDetailRoutes::initializeCpuData()
{
    className_ = "IbexRiscV32";
    cpuType_ = "rv64gc";
    timeProvider_ = "empty";
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::setPeripherals()
//-----------------------------------------------------------------------------
void RenodeCPUDetailRoutes::setPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > newPeripherals)
{
    peripherals_ = newPeripherals;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::getPeripherals()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > RenodeCPUDetailRoutes::getPeripherals() const
{
    return peripherals_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::setMemories()
//-----------------------------------------------------------------------------
void RenodeCPUDetailRoutes::setMemories(QVector<QSharedPointer<RenodeStructs::cpuMemories> > newMemories)
{
    memories_ = newMemories;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::getMemories()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::cpuMemories> > RenodeCPUDetailRoutes::getMemories() const
{
    return memories_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::setClassName()
//-----------------------------------------------------------------------------
void RenodeCPUDetailRoutes::setClassName(QString const& newClass)
{
    className_ = newClass;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::getClassName()
//-----------------------------------------------------------------------------
QString RenodeCPUDetailRoutes::getClassName()
{
    return className_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::setCpuType()
//-----------------------------------------------------------------------------
void RenodeCPUDetailRoutes::setCpuType(QString const& newType)
{
    cpuType_ = newType;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::getCpuType()
//-----------------------------------------------------------------------------
QString RenodeCPUDetailRoutes::getCpuType()
{
    return cpuType_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::setTimeProvider()
//-----------------------------------------------------------------------------
void RenodeCPUDetailRoutes::setTimeProvider(QString const& newTimeProvider)
{
    timeProvider_ = newTimeProvider;
}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::getTimeProvider()
//-----------------------------------------------------------------------------
QString RenodeCPUDetailRoutes::getTimeProvider()
{
    return timeProvider_;
}
