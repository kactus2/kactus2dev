//-----------------------------------------------------------------------------
// File: MultipleParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.01.2015
//
// Description:
// Finds parameters from multiple components.
//-----------------------------------------------------------------------------

#ifndef MULTIPLEPARAMETERFINDER_H
#define MULTIPLEPARAMETERFINDER_H

#include "ParameterFinder.h"

#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
//! Parameter finder of multiple components.
//-----------------------------------------------------------------------------
class MultipleParameterFinder : public ParameterFinder
{

public:

    /*!
     *  Constructor.
     *
     *      @param [in] component   The component which parameters are being searched for.
     */
    MultipleParameterFinder(QSharedPointer<Component> component);

    /*!
     *  Destructor.
     */
    ~MultipleParameterFinder();

    /*!
     *  Get the parameter with the given id.
     *
     *      @param [in] parameterId     The id of the parameter being searched for.
     */
    virtual QSharedPointer<Parameter> getParameterWithID(QString const& parameterId) const;

    /*!
     *  Checks if a parameter with the given id exists.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return True, if the parameter with the given id exists, otherwise false.
     */
    virtual bool hasId(QString const& id) const;

    /*!
     *  Finds the name of the parameter with the given id.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return The name of the parameter.
     */
    virtual QString nameForId(QString const& id) const;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter.
     */
    virtual QString valueForId(QString const& id) const;

    /*!
     *  Gets all of the ids of components parameters.
     *
     *      @return A list containing all of the ids.
     */
    virtual QStringList getAllParameterIds() const;

    /*!
     *  Gets the number of parameters in the component.
     *
     *      @return The number of parameters in the component.
     */
    virtual int getNumberOfParameters() const;

    /*!
     *  Set a new component for the parameter finder.
     *
     *      @param [in] component   The new component.
     */
    virtual void setComponent(QSharedPointer<Component> component);

private:

	//! No copying
    MultipleParameterFinder(const MultipleParameterFinder& other);
	//! No assignment
    MultipleParameterFinder& operator=(const MultipleParameterFinder& other);

    //! The old component.
    QSharedPointer<Component> oldComponent_;

    //! The new component.
    QSharedPointer<Component> newComponent_;
};

#endif // MULTIPLEPARAMETERFINDER_H
