//-----------------------------------------------------------------------------
// File: ComponentParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2015
//
// Description:
// The interface for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef COMPONENTPARAMETERFINDER_H
#define COMPONENTPARAMETERFINDER_H

#include "ParameterFinder.h"

#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
//! Component parameter finder.
//-----------------------------------------------------------------------------
class ComponentParameterFinder : public ParameterFinder
{

public:

    /*!
     *  Constructor.
     *
     *      @param [in] component   The component which parameters are being searched for.
     */
    ComponentParameterFinder(QSharedPointer<Component> component);

    /*!
     *  Destructor.
     */
    ~ComponentParameterFinder();

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

private:

	//! No copying
    ComponentParameterFinder(const ComponentParameterFinder& other);
	//! No assignment
    ComponentParameterFinder& operator=(const ComponentParameterFinder& other);

    //! The parameters are searched from this component.
    QSharedPointer<Component> component_;
};

#endif // COMPONENTPARAMETERFINDER_H
