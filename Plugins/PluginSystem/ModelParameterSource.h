//-----------------------------------------------------------------------------
// File: ModelParameterSource.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// Interface for import plugins creating model parameters.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERSOURCE_H
#define MODELPARAMETERSOURCE_H

#include "ModelParameterVisualizer.h"

//-----------------------------------------------------------------------------
//! Interface for import plugins creating model parameters.
//-----------------------------------------------------------------------------
class ModelParameterSource
{
public:

	//! The destructor.
    virtual ~ModelParameterSource() {};
    
    /*!
     *  Sets the given visualizer to be used by the source.
     *
     *      @param [in] visualizer   The visualizer to use.          
     */
    virtual void setModelParameterVisualizer(ModelParameterVisualizer* visualizer) = 0;

};

#endif // MODELPARAMETERSOURCE_H
