//-----------------------------------------------------------------------------
// File: ComponentParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.01.2015
//
// Description:
// The implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef COMPONENTPARAMETERFINDER_H
#define COMPONENTPARAMETERFINDER_H

#include "ParameterFinder.h"

class AbstractParameterModel;
class Component;

//-----------------------------------------------------------------------------
//! The implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------
class ComponentParameterFinder : public ParameterFinder
{

public:

    /*!
     *  Constructor.
     *
	 *      @param [in] component		The component which parameters are being searched for.
     */
    explicit ComponentParameterFinder(QSharedPointer<Component const> component);

    //! No copying
    ComponentParameterFinder(const ComponentParameterFinder& other) = delete;

    //! No assignment
    ComponentParameterFinder& operator=(const ComponentParameterFinder& other) = delete;

    /*!
     *  Destructor.
     */
    virtual ~ComponentParameterFinder() = default;

    /*!
     *  Get the parameter with the given id.
     *
     *      @param [in] parameterId     The id of the parameter being searched for.
     */
    virtual QSharedPointer<Parameter> getParameterWithID(QString const& parameterId) const override;

    /*!
     *  Checks if a parameter with the given id exists.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return True, if the parameter with the given id exists, otherwise false.
     */
    virtual bool hasId(QString const& id) const override;

    /*!
     *  Finds the name of the parameter with the given id.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return The name of the parameter.
     */
    virtual QString nameForId(QString const& id) const override;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter.
     */
    virtual QString valueForId(QString const& id) const override;

    /*!
     *  Gets all of the ids of components parameters.
     *
     *      @return A list containing all of the ids.
     */
    virtual QStringList getAllParameterIds() const override;

    /*!
     *  Gets the number of parameters in the component.
     *
     *      @return The number of parameters in the component.
     */
    virtual int getNumberOfParameters() const override;

    /*!
    *  Registers a parameter model that can modify parameters for the finder.
    *
    *      @param [in] model   The model to register.
    */
    virtual void registerParameterModel(QAbstractItemModel const* model) override;

    /*!
     *  Set a new component for the parameter finder.
     *
     *      @param [in] component   The new component.
     */
    virtual void setComponent(QSharedPointer<Component const> component);

    /*!
     *  Returns all the parameters in component address spaces.
     */
    QList<QSharedPointer<Parameter> > allAddressSpaceParameters() const;

    /*!
     *  Finds all the parameters in component bus interfaces.
     *
     *      @return The parameters in all bus interfaces.
     */
    QList<QSharedPointer<Parameter> > allBusInterfaceParameters() const;
    
    /*!
     *  Returns all the parameters in component cpus.
     */
    QList<QSharedPointer<Parameter> > allCpuParameters() const;
    
    /*!
     *  Returns all the parameters in component generators.
     */
    QList<QSharedPointer<Parameter> > allGeneratorParameters() const;

    /*!
     *  Finds all the parameters in component registers.
     *
     *      @return The parameters in all registers.
     */
    QList<QSharedPointer<Parameter> > allRegisterParameters() const;

    /*!
     *  Get all the parameters in indirect interfaces.
     *
     *      @return The parameters in indirect interfaces.
     */
    QList<QSharedPointer<Parameter> > allIndirectInterfacesParameters() const;

protected:

    /*!
     *  Returns a parameter corresponding given id, if any exists.
     */
    virtual QSharedPointer<Parameter> searchParameter(QString const& parameterId) const;

    /*!
     *  Get the component.
     *
     *      @return The contained component.
     */
    QSharedPointer<const Component> getComponent() const;

private:
    
    /*!
     *  Returns number of parameters in bus interfaces.
     */
    int busInterfaceParameterCount() const;
    
    /*!
     *  Returns number of parameters in registers.
     */
    int registerParameterCount() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameters are searched from this component.
    QSharedPointer<Component const> component_;
};

#endif // COMPONENTPARAMETERFINDER_H
