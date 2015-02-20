//-----------------------------------------------------------------------------
// File: ModelParameterFactor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Implementation of ModelParameterFactory for Module parameters.
//-----------------------------------------------------------------------------

#ifndef ModuleParameterFactoryImplementation_H
#define ModuleParameterFactoryImplementation_H

#include <editors/ComponentEditor/modelParameters/ModelParameterFactory.h>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Implementation of ModelParameterFactory for Module parameters.
//-----------------------------------------------------------------------------
class ModuleParameterFactoryImplementation : public ModelParameterFactory
{
public:

    //! The constructor.
    ModuleParameterFactoryImplementation();

    //! The destructor.
    virtual ~ModuleParameterFactoryImplementation();

    /*!
     *  Creates a new module parameter.
     *
     *      @return The created module parameter.
     */
    ModelParameter* make() const;

private:

    // Disable copying.
    ModuleParameterFactoryImplementation(ModuleParameterFactoryImplementation const& rhs);
    ModuleParameterFactoryImplementation& operator=(ModuleParameterFactoryImplementation const& rhs);
};

#endif // ModuleParameterFactoryImplementation_H
