//-----------------------------------------------------------------------------
// File: ModelParameterFactoryImplementation.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Implementing class for ModelParameterFactory.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERFACTORYIMPLEMENTATION_H
#define MODELPARAMETERFACTORYIMPLEMENTATION_H

#include "ModelParameterFactory.h"

class ModelParameter;

//-----------------------------------------------------------------------------
//! Implementing class for ModelParameterFactory.
//-----------------------------------------------------------------------------
class ModelParameterFactoryImplementation : public ModelParameterFactory
{
public:

    //! The constructor.
    ModelParameterFactoryImplementation();

    //! The destructor.
    virtual ~ModelParameterFactoryImplementation();

    /*!
     *  Creates a new model parameter.
     *
     *      @return The created model parameter.
     */
    ModelParameter* make() const;

private:
    // Disable copying.
    ModelParameterFactoryImplementation(ModelParameterFactoryImplementation const& rhs);
    ModelParameterFactoryImplementation& operator=(ModelParameterFactoryImplementation const& rhs);
};

#endif // MODELPARAMETERFACTORYIMPLEMENTATION_H
