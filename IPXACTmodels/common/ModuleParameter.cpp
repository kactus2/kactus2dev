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
Parameter()
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
    return getAttribute(QStringLiteral("dataType"));
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::setDataType()
//-----------------------------------------------------------------------------
void ModuleParameter::setDataType(QString const& newDataType)
{
    setAttribute(QStringLiteral("dataType"), newDataType);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::getUsageType()
//-----------------------------------------------------------------------------
QString ModuleParameter::getUsageType() const
{
    return getAttribute(QStringLiteral("usageType"));
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::setUsageType()
//-----------------------------------------------------------------------------
void ModuleParameter::setUsageType(QString const& newUsageType)
{
    setAttribute(QStringLiteral("usageType"), newUsageType);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::getDataTypeDefinition()
//-----------------------------------------------------------------------------
QString ModuleParameter::getDataTypeDefinition() const
{
    return getAttribute(QStringLiteral("dataTypeDefinition"));
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::setDataTypeDefinition()
//-----------------------------------------------------------------------------
void ModuleParameter::setDataTypeDefinition(QString const& dataTypeDef)
{
    setAttribute(QStringLiteral("dataTypeDefinition"), dataTypeDef);
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
