//-----------------------------------------------------------------------------
// File: SingleCpuRoutesContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.05.2023
//
// Description:
// Container for routes connected to a single CPU.
//-----------------------------------------------------------------------------

#include "SingleCpuRoutesContainer.h"

//-----------------------------------------------------------------------------
// Function: SingleCpuRoutesContainer::SingleCpuRoutesContainer()
//-----------------------------------------------------------------------------
SingleCpuRoutesContainer::SingleCpuRoutesContainer(QSharedPointer<Cpu> cpu):
CpuRoutesContainer(),
cpu_(cpu)
{

}

//-----------------------------------------------------------------------------
// Function: SingleCpuRoutesContainer::SingleCpuRoutesContainer()
//-----------------------------------------------------------------------------
SingleCpuRoutesContainer::SingleCpuRoutesContainer(const SingleCpuRoutesContainer& other) :
CpuRoutesContainer(other),
cpu_(other.getCpu())
{

}

//-----------------------------------------------------------------------------
// Function: SingleCpuRoutesContainer::getCpu()
//-----------------------------------------------------------------------------
QSharedPointer<Cpu> SingleCpuRoutesContainer::getCpu() const
{
    return cpu_;
}

//-----------------------------------------------------------------------------
// Function: SingleCpuRoutesContainer::setCpu()
//-----------------------------------------------------------------------------
void SingleCpuRoutesContainer::setCpu(QSharedPointer<Cpu> newCpu)
{
    cpu_ = newCpu;
}
