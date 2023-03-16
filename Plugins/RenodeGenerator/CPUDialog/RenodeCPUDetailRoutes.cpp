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
peripherals_()
{

}

//-----------------------------------------------------------------------------
// Function: RenodeCPUDetailRoutes::RenodeCPUDetailRoutes()
//-----------------------------------------------------------------------------
RenodeCPUDetailRoutes::RenodeCPUDetailRoutes(const CPUDetailRoutes& other) :
CPUDetailRoutes(other),
peripherals_()
{

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
