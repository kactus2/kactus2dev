//-----------------------------------------------------------------------------
// File: MultipleParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.01.2015
//
// Description:
// Finds parameters from multiple components.
//-----------------------------------------------------------------------------

#ifndef MULTIPLEPARAMETERFINDER_H
#define MULTIPLEPARAMETERFINDER_H

#include "ParameterFinder.h"

#include <IPXACTmodels/common/Parameter.h>

#include "KactusAPI/KactusAPIGlobal.h"

//-----------------------------------------------------------------------------
//! Parameter finder of multiple components.
//-----------------------------------------------------------------------------
class KACTUS2_API MultipleParameterFinder : public ParameterFinder
{

public:

    /*!
     *  Constructor.
     *
     *      @param [in] component   The component which parameters are being searched for.
     */
    MultipleParameterFinder();

    /*!
     *  Destructor.
     */
    ~MultipleParameterFinder() final = default;


    //! No copying
    MultipleParameterFinder(const MultipleParameterFinder& other) = delete;
    //! No assignment
    MultipleParameterFinder& operator=(const MultipleParameterFinder& other) = delete;

     /*!
      *  Adds a finder to be used in searches.
      *
      *      @param [in] finder   The finder to add.
      */
     void addFinder(QSharedPointer<ParameterFinder> finder);

     /*!
      *  Removes a finder from searches.
      *
      *      @param [in] finder   The finder to remove.
      */
     void removeFinder(QSharedPointer<ParameterFinder> finder);

     /*!
      *  Remove all attached parameter finders.
      */
     void removeAllFinders();

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
    int getNumberOfParameters() const noexcept final;
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
     */
    void registerParameterModel(QAbstractItemModel const* model) final;

private:

    //! The finders to use in searches.
    QList<QSharedPointer<ParameterFinder> > finders_;
};

#endif // MULTIPLEPARAMETERFINDER_H
