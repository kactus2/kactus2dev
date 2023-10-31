//-----------------------------------------------------------------------------
// File: ListParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.03.2015
//
// Description:
// The implementation for finding parameters from a list with the correct ID.
//-----------------------------------------------------------------------------

#ifndef LISTPARAMETERFINDER_H
#define LISTPARAMETERFINDER_H

#include "ParameterFinder.h"

#include <IPXACTmodels/common/Parameter.h>

#include "KactusAPI/KactusAPIGlobal.h"

#include <QSharedPointer>
#include <QList>

//-----------------------------------------------------------------------------
//! The implementation for finding parameters from a list with the correct ID.
//-----------------------------------------------------------------------------
class KACTUS2_API ListParameterFinder : public ParameterFinder
{

public:

    /*!
     *  The constructor.
     */
    ListParameterFinder();

    /*!
     *  The Destructor.
     */
    ~ListParameterFinder() override = default;

    //! No copying
    ListParameterFinder(const ListParameterFinder& other) = delete;
    //! No assignment
    ListParameterFinder& operator=(const ListParameterFinder& other) = delete;

    /*!
     *  Get the parameter with the given id.
     *
     *      @param [in] parameterId     The id of the parameter being searched for.
     */
    QSharedPointer<Parameter> getParameterWithID(QStringView parameterId) const override;

    /*!
     *  Checks if a parameter with the given id exists.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return True, if the parameter with the given id exists, otherwise false.
     */
    bool hasId(QStringView id) const override;

    /*!
     *  Finds the name of the parameter with the given id.
     *
     *      @param [in] id      The id to search for.
     *
     *      @return The name of the parameter.
     */
    QString nameForId(QStringView id) const override;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter.
     */
    QString valueForId(QStringView id) const override;

    /*!
     *  Gets all of the ids of parameters in the list.
     *
     *      @return A list containing all of the ids.
     */
    QStringList getAllParameterIds() const override;

    /*!
     *  Gets the number of parameters in the list.
     *
     *      @return The number of parameters in the list.
     */
    int getNumberOfParameters() const noexcept override;

    /*!
     *  Set the parameter list.
     *
     *      @param [in] parameterList   [Description].
     */
    void setParameterList(QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList) noexcept;
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
     */
    void registerParameterModel(QAbstractItemModel const* model) final;

private:


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameters are searched from this list.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList_ =
        QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >());
};

#endif // LISTPARAMETERFINDER_H
