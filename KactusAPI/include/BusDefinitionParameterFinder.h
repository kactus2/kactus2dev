//-----------------------------------------------------------------------------
// File: BusDefinitionParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 22.08.2024
//
// Description:
// The implementation for finding bus definition parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONPARAMETERFINDER_H
#define BUSDEFINITIONPARAMETERFINDER_H

#include "ParameterFinder.h"

#include <KactusAPI/KactusAPIGlobal.h>

class BusDefinition;

class KACTUS2_API BusDefinitionParameterFinder : public ParameterFinder
{

public:

    /*!
     *  Constructor.
     *
     *      @param [in] busDef		The bus definition in which parameters are being searched for.
     */
    explicit BusDefinitionParameterFinder(QSharedPointer<BusDefinition const> busDef) noexcept;

    //! No copying
    BusDefinitionParameterFinder(const BusDefinitionParameterFinder& other) = delete;

    //! No assignment
    BusDefinitionParameterFinder& operator=(const BusDefinitionParameterFinder& other) = delete;

    /*!
     *  The destructor.
     */
    virtual ~BusDefinitionParameterFinder() = default;

    void setBusDef(QSharedPointer<BusDefinition> busDef);

    /*!
     *  Get the parameter corresponding the id.
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
     *      @return The name of the parameter
     */
    QString nameForId(QStringView id) const final;

    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter
     */
    QString valueForId(QStringView id) const final;

    /*!
     *  Gets all of the ids of components parameters.
     *
     *      @return A list containing all of the ids of parameters.
     */
    QStringList getAllParameterIds() const final;

    /*!
     *  Gets the number of parameters in the component.
     *
     *      @return The number of parameters.
     */
    int getNumberOfParameters() const final;

    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
     */
    void registerParameterModel(QAbstractItemModel const* model) final;

private:

    //! The bus definition whose parameters are searched.
    QSharedPointer<BusDefinition const> busDef_;
};


#endif // BUSDEFINITIONPARAMETERFINDER_H
