//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 05.05.2017
//
// Description:
// For finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONINSTANTIATIONPARAMETERFINDER_H
#define DESIGNCONFIGURATIONINSTANTIATIONPARAMETERFINDER_H

#include "ParameterFinder.h"

class AbstractParameterModel;
class DesignConfigurationInstantiation;

//-----------------------------------------------------------------------------
//! The implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------
class DesignConfigurationInstantiationParameterFinder : public ParameterFinder
{

public:

    /*!
     *  Constructor.
     *
	 *      @param [in] designConfigurationInstantiation		The element which parameters are being searched for.
     */
    explicit DesignConfigurationInstantiationParameterFinder
        (QSharedPointer<DesignConfigurationInstantiation const> designConfigurationInstantiation);

    /*!
     *  Destructor.
     */
    ~DesignConfigurationInstantiationParameterFinder() final = default;

    //! No copying
    DesignConfigurationInstantiationParameterFinder(const DesignConfigurationInstantiationParameterFinder& other)
        = delete;
    //! No assignment
    DesignConfigurationInstantiationParameterFinder& operator=(
        const DesignConfigurationInstantiationParameterFinder& other) = delete;

    /*!
     *  Get the parameter with the given id.
     *
     *      @param [in] parameterId     The id of the parameter being searched for.
     */
    QSharedPointer<Parameter> getParameterWithID(QStringView parameterId) const final;

    /*!
     *  Checks if a parameter with the given id exists.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return True, if the parameter with the given id exists, otherwise false.
     */
    bool hasId(QStringView id) const final;

    /*!
     *  Finds the name of the parameter with the given id.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return The name of the parameter.
     */
    QString nameForId(QStringView id) const final;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter.
     */
    QString valueForId(QStringView id) const final;

    /*!
     *  Gets all of the ids of design configuration instantiation parameters.
     *
     *      @return A list containing all of the ids.
     */
    QStringList getAllParameterIds() const final;

    /*!
     *  Gets the number of design configuration instantiation in the component.
     *
     *      @return The number of design configuration instantiation in the component.
     */
    int getNumberOfParameters() const final;

    /*!
     *  Sets a new component for the parameter finder.
     */
    virtual void setDesignConfigurationInstantiation
        (QSharedPointer<DesignConfigurationInstantiation const> DesignConfigurationInstantiation);
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
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
    QSharedPointer<DesignConfigurationInstantiation const> designConfigurationInstantiation_;
};

#endif // DESIGNCONFIGURATIONINSTANTIATIONPARAMETERFINDER_H
