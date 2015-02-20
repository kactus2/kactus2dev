//-----------------------------------------------------------------------------
// File: ModelParameterFactor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.02.2015
//
// Description:
// Interface for ModelParameter factory.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERFACTORY_H
#define MODELPARAMETERFACTORY_H

class ModelParameter;

//-----------------------------------------------------------------------------
//! Interface for ModelParameter factory.
//-----------------------------------------------------------------------------
class ModelParameterFactory
{
public:

    //! The destructor.
    virtual ~ModelParameterFactory(){};

    /*!
     *  Creates a new model parameter.
     *
     *      @return The created model parameter.
     */
    virtual ModelParameter* make() const = 0;

};

#endif // MODELPARAMETERFACTORY_H
