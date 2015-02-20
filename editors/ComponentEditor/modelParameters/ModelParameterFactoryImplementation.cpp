//-----------------------------------------------------------------------------
// File: ModelParameterFactoryImplementation.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Implementing class for ModelParameterFactory.
//-----------------------------------------------------------------------------

#include "ModelParameterFactoryImplementation.h"

#include <IPXACTmodels/modelparameter.h>

//-----------------------------------------------------------------------------
// Function: ModelParameterFactoryImplementation::ModelParameterFactoryImplementation()
//-----------------------------------------------------------------------------
ModelParameterFactoryImplementation::ModelParameterFactoryImplementation()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterFactoryImplementation::~ModelParameterFactoryImplementation()
//-----------------------------------------------------------------------------
ModelParameterFactoryImplementation::~ModelParameterFactoryImplementation()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterFactoryImplementation::make()
//-----------------------------------------------------------------------------
ModelParameter* ModelParameterFactoryImplementation::make() const
{
    return new ModelParameter();
}
