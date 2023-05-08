//-----------------------------------------------------------------------------
// File: SVDCPUDetailRoutes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// Container for routes connected to a master interface for SVD generator.
//-----------------------------------------------------------------------------

#include "SVDCPUDetailRoutes.h"

#include <Plugins/SVDGenerator/CPUDialog/SVDUtilities.h>

#include <QJsonObject>

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::SVDCPUDetailRoutes()
//-----------------------------------------------------------------------------
SVDCPUDetailRoutes::SVDCPUDetailRoutes(QSharedPointer<Cpu> cpu):
CPUDetailRoutes(cpu),
revision_(QString("")),
endian_(QString("")),
mpuPresent_(false),
fpuPresent_(false),
nvicPrioBits_(QString("")),
vendorSystickConfig_(false)
{

}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::SVDCPUDetailRoutes()
//-----------------------------------------------------------------------------
SVDCPUDetailRoutes::SVDCPUDetailRoutes(const CPUDetailRoutes& other) :
CPUDetailRoutes(other),
revision_(QString()),
endian_(QString()),
mpuPresent_(false),
fpuPresent_(false),
nvicPrioBits_(QString()),
vendorSystickConfig_(false)
{

}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::setupConfiguration()
//-----------------------------------------------------------------------------
void SVDCPUDetailRoutes::setupConfiguration(QJsonObject const& configurationObject)
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
// Function: SVDCPUDetailRoutes::getRevision()
//-----------------------------------------------------------------------------
QString SVDCPUDetailRoutes::getRevision() const
{
    return revision_;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::setRevision()
//-----------------------------------------------------------------------------
void SVDCPUDetailRoutes::setRevision(QString const& newRevision)
{
    revision_ = newRevision;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::getEndian()
//-----------------------------------------------------------------------------
QString SVDCPUDetailRoutes::getEndian() const
{
    return endian_;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::setEndian()
//-----------------------------------------------------------------------------
void SVDCPUDetailRoutes::setEndian(QString const& newEndian)
{
    endian_ = newEndian;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::isMPUPresent()
//-----------------------------------------------------------------------------
bool SVDCPUDetailRoutes::isMPUPresent() const
{
    return mpuPresent_;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::setMPUPresence()
//-----------------------------------------------------------------------------
void SVDCPUDetailRoutes::setMPUPresence(bool newPresence)
{
    mpuPresent_ = newPresence;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::isFPUPresent()
//-----------------------------------------------------------------------------
bool SVDCPUDetailRoutes::isFPUPresent() const
{
    return fpuPresent_;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::setFPUPresence()
//-----------------------------------------------------------------------------
void SVDCPUDetailRoutes::setFPUPresence(bool newPresence)
{
    fpuPresent_ = newPresence;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::getNVICPrioBits()
//-----------------------------------------------------------------------------
QString SVDCPUDetailRoutes::getNVICPrioBits() const
{
    return nvicPrioBits_;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::setNVICPrioBits()
//-----------------------------------------------------------------------------
void SVDCPUDetailRoutes::setNVICPrioBits(QString const& newBits)
{
    nvicPrioBits_ = newBits;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::isVendorSystickConfig()
//-----------------------------------------------------------------------------
bool SVDCPUDetailRoutes::isVendorSystickConfig() const
{
    return vendorSystickConfig_;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDetailRoutes::setVendorSystickConfig()
//-----------------------------------------------------------------------------
void SVDCPUDetailRoutes::setVendorSystickConfig(bool newConfig)
{
    vendorSystickConfig_ = newConfig;
}
