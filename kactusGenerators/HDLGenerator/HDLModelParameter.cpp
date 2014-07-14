//-----------------------------------------------------------------------------
// File: HDLModelParameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for all HDL representations of model parameters.
//-----------------------------------------------------------------------------

#include "HDLModelParameter.h"

#include <IPXACTmodels/modelparameter.h>

//-----------------------------------------------------------------------------
// Function: HDLModelParameter::HDLModelParameter()
//-----------------------------------------------------------------------------
HDLModelParameter::HDLModelParameter(ModelParameter* modelPararameter, QObject* parent):
HDLObject(parent,
		   modelPararameter->getName(),
		   modelPararameter->getDataType(),
		   modelPararameter->getValue(),
		   modelPararameter->getDescription()) 
{
	Q_ASSERT(modelPararameter);
}

//-----------------------------------------------------------------------------
// Function: HDLModelParameter::~HDLModelParameter()
//-----------------------------------------------------------------------------
HDLModelParameter::~HDLModelParameter()
{

}
