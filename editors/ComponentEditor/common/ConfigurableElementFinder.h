//-----------------------------------------------------------------------------
// File: ConfigurableElementFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.08.2017
//
// Description:
// The implementation for finding configurable elements from a list with the correct ID.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTFINDER_H
#define CONFIGURABLEELEMENTFINDER_H

#include "ParameterFinder.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QSharedPointer>
#include <QList>

class EditorConfigurableElement;

//-----------------------------------------------------------------------------
//! The implementation for finding configurable elements from a list with the correct ID.
//-----------------------------------------------------------------------------
class ConfigurableElementFinder : public ParameterFinder
{

public:

    /*!
     *  The constructor.
     */
    ConfigurableElementFinder();

    /*!
     *  The Destructor.
     */
    ~ConfigurableElementFinder() override = default;

    //! No copying. No assignment.
    ConfigurableElementFinder(const ConfigurableElementFinder& other) = delete;
    ConfigurableElementFinder& operator=(const ConfigurableElementFinder& other) = delete;

    /*!
     *  Get the parameter with the given id.
     *
     *    @param [in] parameterId     The id of the parameter being searched for.
     *
     *    @return The parameter with the selected ID.
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
     *  Gets all of the ids of parameters in the list.
     *
     *    @return A list containing all of the ids.
     */
    QStringList getAllParameterIds() const final;

    /*!
     *  Gets the number of parameters in the list.
     *
     *    @return The number of parameters in the list.
     */
    int getNumberOfParameters() const final;

    /*!
     *  Set the configurable element list.
     *
     *    @param [in] newElementList  List containing the configurable elements.
     */
    void setConfigurableElementList(QList<QSharedPointer<EditorConfigurableElement> > newElementList);

    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *    @param [in] model   The model to register.
     */
    void registerParameterModel(QAbstractItemModel const* model) final;

private:

    /*!
     *  Get the configurable element referenced the selected parameter ID.
     *
     *    @param [in] id  ID of the selected parameter reference.
     *
     *    @return Configurable element containing the selected parameter reference.
     */
    QSharedPointer<EditorConfigurableElement> getElementForID(QStringView id) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The configurable elements are searched from this list.
    QList<QSharedPointer<EditorConfigurableElement> > elementList_;
};

#endif // LISTPARAMETERFINDER_H
