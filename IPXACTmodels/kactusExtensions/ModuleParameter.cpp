//-----------------------------------------------------------------------------
// File: ModuleParameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.02.2015
//
// Description:
// Component instance module parameter element in IP-XACT 2014.
//-----------------------------------------------------------------------------

#include "ModuleParameter.h"

//-----------------------------------------------------------------------------
// Function: ModuleParameter::ModuleParameter()
//-----------------------------------------------------------------------------
ModuleParameter::ModuleParameter(QDomNode& node): ModelParameter(node)
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::~ModuleParameter()
//-----------------------------------------------------------------------------
ModuleParameter::~ModuleParameter()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::clone()
//-----------------------------------------------------------------------------
ModuleParameter* ModuleParameter::clone() const
{
    return new ModuleParameter(*this);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::elementName()
//-----------------------------------------------------------------------------
QString ModuleParameter::elementName() const
{
    return "module parameter";
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::elementIdentifier()
//-----------------------------------------------------------------------------
QString ModuleParameter::elementIdentifier() const
{
    return "kactus2:moduleParameter";
}

//-----------------------------------------------------------------------------
// Function: ModuleParameter::ModuleParameter()
//-----------------------------------------------------------------------------
ModuleParameter::ModuleParameter(ModuleParameter const& other) : ModelParameter(other) 
{

}