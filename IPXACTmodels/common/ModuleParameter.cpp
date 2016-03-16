//-----------------------------------------------------------------------------
// File: ModuleParameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Implementation for ipxact:moduleParameter element.
//-----------------------------------------------------------------------------

#include "ModuleParameter.h"

//-----------------------------------------------------------------------------
// Function: ModuleParameter::ModuleParameter()
//-----------------------------------------------------------------------------
ModuleParameter::ModuleParameter() :
Parameter(),
isPresent_()
{
    
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::ModuleParameter()
//-----------------------------------------------------------------------------
ModuleParameter::ModuleParameter(const ModuleParameter& other) :
Parameter(other),
isPresent_(other.isPresent_)
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::~ModuleParameter()
//-----------------------------------------------------------------------------
ModuleParameter::~ModuleParameter()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::operator =()
//-----------------------------------------------------------------------------
ModuleParameter& ModuleParameter::operator =(const ModuleParameter& other)
{
    if (this != &other)
    {
        Parameter::operator=(other);
        isPresent_ = other.isPresent_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::getDataType()
//-----------------------------------------------------------------------------
QString ModuleParameter::getDataType() const
{
    return getAttribute("dataType");
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::setDataType()
//-----------------------------------------------------------------------------
void ModuleParameter::setDataType(QString const& newDataType)
{
    setAttribute("dataType", newDataType);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::getUsageType()
//-----------------------------------------------------------------------------
QString ModuleParameter::getUsageType() const
{
    return getAttribute("usageType");
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::setUsageType()
//-----------------------------------------------------------------------------
void ModuleParameter::setUsageType(QString const& newUsageType)
{
    setAttribute("usageType", newUsageType);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::getIsPresent()
//-----------------------------------------------------------------------------
QString ModuleParameter::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::setIsPresent()
//-----------------------------------------------------------------------------
void ModuleParameter::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}
