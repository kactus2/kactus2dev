//-----------------------------------------------------------------------------
// File: ModelParameterFactorImplementation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Implementation of ModelParameterFactory for Module parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterFactoryImplementation.h"

#include <IPXACTmodels/common/ModuleParameter.h>

//-----------------------------------------------------------------------------
// Function: ModuleParameterFactoryImplementation::ModuleParameterFactoryImplementation()
//-----------------------------------------------------------------------------
ModuleParameterFactoryImplementation::ModuleParameterFactoryImplementation()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterFactoryImplementation::~ModuleParameterFactoryImplementation()
//-----------------------------------------------------------------------------
ModuleParameterFactoryImplementation::~ModuleParameterFactoryImplementation()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterFactoryImplementation::make()
//-----------------------------------------------------------------------------
ModuleParameter* ModuleParameterFactoryImplementation::make() const
{
    return new ModuleParameter();
}
