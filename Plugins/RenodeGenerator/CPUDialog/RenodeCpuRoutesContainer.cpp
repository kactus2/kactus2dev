//-----------------------------------------------------------------------------
// File: RenodeCpuRoutesContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.03.2023
//
// Description:
// Container for routes connected to a master interface for Renode generator.
//-----------------------------------------------------------------------------

#include "RenodeCpuRoutesContainer.h"

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::RenodeCpuRoutesContainer()
//-----------------------------------------------------------------------------
RenodeCpuRoutesContainer::RenodeCpuRoutesContainer(QSharedPointer<Cpu> renodeCpu):
SingleCpuRoutesContainer(renodeCpu)
{
    initializeCpuData();
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::RenodeCpuRoutesContainer()
//-----------------------------------------------------------------------------
RenodeCpuRoutesContainer::RenodeCpuRoutesContainer(const SingleCpuRoutesContainer& other) :
SingleCpuRoutesContainer(other)
{
    initializeCpuData();
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::initializeCpuData()
//-----------------------------------------------------------------------------
void RenodeCpuRoutesContainer::initializeCpuData()
{
    className_ = "IbexRiscV32";
    cpuType_ = "rv64gc";
    timeProvider_ = "empty";
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::setPeripherals()
//-----------------------------------------------------------------------------
void RenodeCpuRoutesContainer::setPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > newPeripherals)
{
    peripherals_ = newPeripherals;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::getPeripherals()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > RenodeCpuRoutesContainer::getPeripherals() const
{
    return peripherals_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::setMemories()
//-----------------------------------------------------------------------------
void RenodeCpuRoutesContainer::setMemories(QVector<QSharedPointer<RenodeStructs::cpuMemories> > newMemories)
{
    memories_ = newMemories;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::getMemories()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::cpuMemories> > RenodeCpuRoutesContainer::getMemories() const
{
    return memories_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::setClassName()
//-----------------------------------------------------------------------------
void RenodeCpuRoutesContainer::setClassName(QString const& newClass)
{
    className_ = newClass;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::getClassName()
//-----------------------------------------------------------------------------
QString RenodeCpuRoutesContainer::getClassName()
{
    return className_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::setCpuType()
//-----------------------------------------------------------------------------
void RenodeCpuRoutesContainer::setCpuType(QString const& newType)
{
    cpuType_ = newType;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::getCpuType()
//-----------------------------------------------------------------------------
QString RenodeCpuRoutesContainer::getCpuType()
{
    return cpuType_;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::setTimeProvider()
//-----------------------------------------------------------------------------
void RenodeCpuRoutesContainer::setTimeProvider(QString const& newTimeProvider)
{
    timeProvider_ = newTimeProvider;
}

//-----------------------------------------------------------------------------
// Function: RenodeCpuRoutesContainer::getTimeProvider()
//-----------------------------------------------------------------------------
QString RenodeCpuRoutesContainer::getTimeProvider()
{
    return timeProvider_;
}
