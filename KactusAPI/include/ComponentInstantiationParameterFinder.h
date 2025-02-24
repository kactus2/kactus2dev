//-----------------------------------------------------------------------------
// File: ComponentInstantiationParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 05.05.2017
//
// Description:
// For finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATIONPARAMETERFINDER_H
#define COMPONENTINSTANTIATIONPARAMETERFINDER_H

#include <KactusAPI/KactusAPIGlobal.h>

#include "ParameterFinder.h"


class AbstractParameterModel;
class ComponentInstantiation;

//-----------------------------------------------------------------------------
//! The implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------
class KACTUS2_API ComponentInstantiationParameterFinder : public ParameterFinder
{

public:

    /*!
     *  Constructor.
     *
	 *    @param [in] componentInstantiation  The element which parameters are being searched for.
     */
    explicit ComponentInstantiationParameterFinder(QSharedPointer<ComponentInstantiation const> componentInstantiation);

    /*!
     *  Destructor.
     */
    ~ComponentInstantiationParameterFinder() override = default;

    //! No copying
    ComponentInstantiationParameterFinder(const ComponentInstantiationParameterFinder& other) = delete;
    //! No assignment
    ComponentInstantiationParameterFinder& operator=(const ComponentInstantiationParameterFinder& other) = delete;

    /*!
     *  Get the parameter with the given id.
     *
     *    @param [in] parameterId     The id of the parameter being searched for.
     */
    QSharedPointer<Parameter> getParameterWithID(QStringView parameterId) const final;

    /*!
     *  Checks if a parameter with the given id exists.
     *
     *    @param [in] id      The id to search for.
     *
     *    @return True, if the parameter with the given id exists, otherwise false.
     */
    bool hasId(QStringView id) const final;

    /*!
     *  Finds the name of the parameter with the given id.
     *
     *    @param [in] id      The id to search for.
     *
     *    @return The name of the parameter.
     */
    QString nameForId(QStringView id) const final;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *    @param [in] id      The id of the parameter to search for.
     *
     *    @return The value of the parameter.
     */
    QString valueForId(QStringView id) const final;

    /*!
     *  Gets all of the ids of components instantiation parameters.
     *
     *    @return A list containing all of the ids.
     */
    QStringList getAllParameterIds() const final;

    /*!
     *  Gets the number of parameters in the component instantiation.
     *
     *    @return The number of parameters in the component instantiation.
     */
    int getNumberOfParameters() const final;

    /*!
     *  Sets a new component instantiation for the parameter finder.
     */
    virtual void setComponentInstantiation(QSharedPointer<ComponentInstantiation const> componentInstantiation);
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *    @param [in] model   The model to register.
     */
    void registerParameterModel(QAbstractItemModel const* model) final;

private:

	/*!
	 *  Returns a parameter corresponding given id, if any exists.
	*/
	QSharedPointer<Parameter> searchParameter(QStringView parameterId) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameters are searched from this component instantiation.
    QSharedPointer<ComponentInstantiation const> componentInstantiation_;
};

#endif // COMPONENTINSTANTIATIONPARAMETERFINDER_H
