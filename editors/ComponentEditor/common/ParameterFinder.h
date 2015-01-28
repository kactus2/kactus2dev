//-----------------------------------------------------------------------------
// File: ParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2015
//
// Description:
// The interface for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef PARAMETERFINDER_H
#define PARAMETERFINDER_H

#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
//! Parameter finder.
//-----------------------------------------------------------------------------
class ParameterFinder
{

public:

    /*!
     *  The destructor.
     */
    virtual ~ParameterFinder() {};

    /*!
     *  Get the parameter corresponding the id.
     *
     *      @param [in] parameterId     The id of the parameter being searched for.
     */
    virtual QSharedPointer<Parameter> getParameterWithID(QString const& parameterId) const = 0;

    /*!
     *  Checks if a parameter with the given id exists.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return True, if the parameter with the given id exists, otherwise false.
     */
    virtual bool hasId(QString const& id) const = 0;

    /*!
     *  Finds the name of the parameter with the given id.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return The name of the parameter
     */
    virtual QString nameForId(QString const& id) const = 0;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter
     */
    virtual QString valueForId(QString const& id) const = 0;

    /*!
     *  Gets all of the ids of components parameters.
     *
     *      @return A list containing all of the ids of parameters.
     */
    virtual QStringList getAllParameterIds() const = 0;

    /*!
     *  Gets the number of parameters in the component.
     *
     *      @return The number of parameters.
     */
    virtual int getNumberOfParameters() const = 0;

    /*!
     *  Set a new component for the parameter finder.
     *
     *      @param [in] component   The new component for the finder.
     */
    virtual void setComponent(QSharedPointer<Component> component) = 0;
};

#endif // PARAMETERFINDER_H
