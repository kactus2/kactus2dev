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
	 *      @param [in] componentInstantiation  The element which parameters are being searched for.
     */
    ComponentInstantiationParameterFinder(QSharedPointer<ComponentInstantiation const> componentInstantiation);

    /*!
     *  Destructor.
     */
    ~ComponentInstantiationParameterFinder();

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
     *  Gets all of the ids of components instantiation parameters.
     *
     *      @return A list containing all of the ids.
     */
    virtual QStringList getAllParameterIds() const;

    /*!
     *  Gets the number of parameters in the component instantiation.
     *
     *      @return The number of parameters in the component instantiation.
     */
    virtual int getNumberOfParameters() const;

    /*!
     *  Sets a new component instantiation for the parameter finder.
     */
    virtual void setComponentInstantiation(QSharedPointer<ComponentInstantiation const> componentInstantiation);
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
     */
    virtual void registerParameterModel(QAbstractItemModel const* model);

private:

	//! No copying
    ComponentInstantiationParameterFinder(const ComponentInstantiationParameterFinder& other);
	//! No assignment
    ComponentInstantiationParameterFinder& operator=(const ComponentInstantiationParameterFinder& other);

	/*!
	 *  Returns a parameter corresponding given id, if any exists.
	*/
	QSharedPointer<Parameter> searchParameter(QString const& parameterId) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameters are searched from this component instantiation.
    QSharedPointer<ComponentInstantiation const> componentInstantiation_;
};

#endif // COMPONENTINSTANTIATIONPARAMETERFINDER_H
