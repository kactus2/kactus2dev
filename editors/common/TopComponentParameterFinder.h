//-----------------------------------------------------------------------------
// File: TopComponentParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.03.2015
//
// Description:
// Parameter finder for design top component.
//-----------------------------------------------------------------------------

#ifndef TOPCOMPONENTPARAMETERFINDER_H
#define TOPCOMPONENTPARAMETERFINDER_H

#include <KactusAPI/include/ParameterFinder.h>

class Component;
class View;

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
    explicit TopComponentParameterFinder(QSharedPointer<Component> component);

    /*!
     *  Destructor.
     */
    ~TopComponentParameterFinder() final = default;

    //! No copying
    TopComponentParameterFinder(const TopComponentParameterFinder& other) = delete;

    //! No assignment
    TopComponentParameterFinder& operator=(const TopComponentParameterFinder& other) = delete;

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
    void setActiveView(QSharedPointer<View> view);
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
     */
    void registerParameterModel(QAbstractItemModel const* model) final;

private:


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
    QSharedPointer<View> activeView_;
};

#endif // TOPCOMPONENTPARAMETERFINDER_H
