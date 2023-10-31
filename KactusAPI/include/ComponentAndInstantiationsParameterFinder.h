//-----------------------------------------------------------------------------
// File: ComponentAndInstantiationsParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.04.2020
//
// Description:
// The implementation for finding parameters from the full component with the correct ID.
//-----------------------------------------------------------------------------

#ifndef COMPONENTANDINSTANTIATIONSPARAMETERFINDER_H
#define COMPONENTANDINSTANTIATIONSPARAMETERFINDER_H

#include <KactusAPI/KactusAPIGlobal.h>

#include "ComponentParameterFinder.h"

class Component;

//-----------------------------------------------------------------------------
//! The implementation for finding parameters from the full component with the correct ID.
//-----------------------------------------------------------------------------
class KACTUS2_API ComponentAndInstantiationsParameterFinder : public ComponentParameterFinder
{

public:

    /*!
     *  Constructor.
     *
	 *      @param [in] component		The component which parameters are being searched for.
     */
    explicit ComponentAndInstantiationsParameterFinder(QSharedPointer<Component const> component) noexcept;

    //! No copying. No assignment.
    ComponentAndInstantiationsParameterFinder(const ComponentAndInstantiationsParameterFinder& other) = delete;
    ComponentAndInstantiationsParameterFinder& operator=(const ComponentAndInstantiationsParameterFinder& other)
        = delete;

    /*!
     *  Destructor.
     */
    ~ComponentAndInstantiationsParameterFinder() final = default;

    /*!
     *  Gets all of the ids of components parameters.
     *
     *      @return A list containing all of the ids.
     */
    QStringList getAllParameterIds() const final;

    /*!
     *  Gets the number of parameters in the component.
     *
     *      @return The number of parameters in the component.
     */
    int getNumberOfParameters() const final;

    /*!
     *  Get all the instantiation parameters.
     *
     *      @return The instantiation parameters.
     */
    QList<QSharedPointer<Parameter> > allInstantiationsParameters() const;

private:

	/*!
	 *  Get the selected parameter.
     *
     *      @param [in] parameterId     ID of the selected parameter.
     *
     *      @return The selected parameter.
     */
	QSharedPointer<Parameter> searchParameter(QStringView parameterId) const final;
};

#endif // COMPONENTPARAMETERFINDER_H
