//-----------------------------------------------------------------------------
// File: SVDCpuRoutesContainer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// Container for routes connected to a master interface for SVD generator.
//-----------------------------------------------------------------------------

#include "SVDCpuRoutesContainer.h"

#include <Plugins/SVDGenerator/CPUDialog/SVDUtilities.h>

#include <QJsonObject>

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::SVDCpuRoutesContainer()
//-----------------------------------------------------------------------------
SVDCpuRoutesContainer::SVDCpuRoutesContainer(QSharedPointer<Cpu> cpu):
SingleCpuRoutesContainer(cpu),
revision_(QString("")),
endian_(QString("")),
mpuPresent_(false),
fpuPresent_(false),
nvicPrioBits_(QString("")),
vendorSystickConfig_(false)
{

}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::SVDCpuRoutesContainer()
//-----------------------------------------------------------------------------
SVDCpuRoutesContainer::SVDCpuRoutesContainer(const SingleCpuRoutesContainer& other) :
SingleCpuRoutesContainer(other),
revision_(QString()),
endian_(QString()),
mpuPresent_(false),
fpuPresent_(false),
nvicPrioBits_(QString()),
vendorSystickConfig_(false)
{

}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::setupConfiguration()
//-----------------------------------------------------------------------------
void SVDCpuRoutesContainer::setupConfiguration(QJsonObject const& configurationObject)
{
    setCreateFileFlag(configurationObject.value(SVDConstants::CREATESVDFLAG).toBool(false));
    setEndian(configurationObject.value(SVDConstants::CPUENDIAN).toString(QString("")));
    setFPUPresence(configurationObject.value(SVDConstants::CPUFPUPRESENT).toBool(false));
    setMPUPresence(configurationObject.value(SVDConstants::CPUMPUPRESENT).toBool(false));
    setNVICPrioBits(configurationObject.value(SVDConstants::CPPUNVICPRIOBITS).toString(QString("")));
    setRevision(configurationObject.value(SVDConstants::CPUREVISION).toString(QString("")));
    setVendorSystickConfig(configurationObject.value(SVDConstants::CPUVENDORSYSTICKCONFIG).toBool(false));
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::getRevision()
//-----------------------------------------------------------------------------
QString SVDCpuRoutesContainer::getRevision() const
{
    return revision_;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::setRevision()
//-----------------------------------------------------------------------------
void SVDCpuRoutesContainer::setRevision(QString const& newRevision)
{
    revision_ = newRevision;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::getEndian()
//-----------------------------------------------------------------------------
QString SVDCpuRoutesContainer::getEndian() const
{
    return endian_;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::setEndian()
//-----------------------------------------------------------------------------
void SVDCpuRoutesContainer::setEndian(QString const& newEndian)
{
    endian_ = newEndian;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::isMPUPresent()
//-----------------------------------------------------------------------------
bool SVDCpuRoutesContainer::isMPUPresent() const
{
    return mpuPresent_;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::setMPUPresence()
//-----------------------------------------------------------------------------
void SVDCpuRoutesContainer::setMPUPresence(bool newPresence)
{
    mpuPresent_ = newPresence;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::isFPUPresent()
//-----------------------------------------------------------------------------
bool SVDCpuRoutesContainer::isFPUPresent() const
{
    return fpuPresent_;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::setFPUPresence()
//-----------------------------------------------------------------------------
void SVDCpuRoutesContainer::setFPUPresence(bool newPresence)
{
    fpuPresent_ = newPresence;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::getNVICPrioBits()
//-----------------------------------------------------------------------------
QString SVDCpuRoutesContainer::getNVICPrioBits() const
{
    return nvicPrioBits_;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::setNVICPrioBits()
//-----------------------------------------------------------------------------
void SVDCpuRoutesContainer::setNVICPrioBits(QString const& newBits)
{
    nvicPrioBits_ = newBits;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::isVendorSystickConfig()
//-----------------------------------------------------------------------------
bool SVDCpuRoutesContainer::isVendorSystickConfig() const
{
    return vendorSystickConfig_;
}

//-----------------------------------------------------------------------------
// Function: SVDCpuRoutesContainer::setVendorSystickConfig()
//-----------------------------------------------------------------------------
void SVDCpuRoutesContainer::setVendorSystickConfig(bool newConfig)
{
    vendorSystickConfig_ = newConfig;
}
