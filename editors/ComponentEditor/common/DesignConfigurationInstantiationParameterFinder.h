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
    DesignConfigurationInstantiationParameterFinder
        (QSharedPointer<DesignConfigurationInstantiation const> designConfigurationInstantiation);

    /*!
     *  Destructor.
     */
    ~DesignConfigurationInstantiationParameterFinder();

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
     *  Gets all of the ids of design configuration instantiation parameters.
     *
     *      @return A list containing all of the ids.
     */
    virtual QStringList getAllParameterIds() const;

    /*!
     *  Gets the number of design configuration instantiation in the component.
     *
     *      @return The number of design configuration instantiation in the component.
     */
    virtual int getNumberOfParameters() const;

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
    virtual void registerParameterModel(QAbstractItemModel const* model);

private:

	//! No copying
    DesignConfigurationInstantiationParameterFinder(const DesignConfigurationInstantiationParameterFinder& other);
	//! No assignment
    DesignConfigurationInstantiationParameterFinder& operator=(const DesignConfigurationInstantiationParameterFinder& other);

	/*!
	 *  Returns a parameter corresponding given id, if any exists.
	*/
	QSharedPointer<Parameter> searchParameter(QString const& parameterId) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameters are searched from this component instantiation.
    QSharedPointer<DesignConfigurationInstantiation const> designConfigurationInstantiation_;
};

#endif // DESIGNCONFIGURATIONINSTANTIATIONPARAMETERFINDER_H
