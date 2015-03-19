//-----------------------------------------------------------------------------
// File: TopComponentParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.03.2015
//
// Description:
// Parameter finder for design top component.
//-----------------------------------------------------------------------------

#ifndef TOPCOMPONENTPARAMETERFINDER_H
#define TOPCOMPONENTPARAMETERFINDER_H

#include "ParameterFinder.h"

class Component;

//-----------------------------------------------------------------------------
//! Parameter finder for design top component.
//-----------------------------------------------------------------------------
class TopComponentParameterFinder : public ParameterFinder
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] component   The component which parameters are being searched for.
     */
    TopComponentParameterFinder(QSharedPointer<Component> component);

    /*!
     *  Destructor.
     */
    ~TopComponentParameterFinder();

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
    void setComponent(QSharedPointer<Component> component);

    /*!
     *  Sets the active view to use for the component.
     *
     *      @param [in] view   The name of the active view.
     */
    void setActiveView(QString const& view);

private:

	//! No copying
    TopComponentParameterFinder(const TopComponentParameterFinder& other);
	
    //! No assignment
    TopComponentParameterFinder& operator=(const TopComponentParameterFinder& other);

    /*!
     *  Finds all the parameters in the active view.
     *
     *      @return The parameters in active view.
     */
    QList<QSharedPointer<Parameter> > activeViewParameters() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameters are searched from this component.
    QSharedPointer<Component> component_;

    //! The active view of the component.
    QString activeView_;
};

#endif // TOPCOMPONENTPARAMETERFINDER_H
