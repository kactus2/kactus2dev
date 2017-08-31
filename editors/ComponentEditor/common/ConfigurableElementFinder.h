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
    ~ConfigurableElementFinder();

    /*!
     *  Get the parameter with the given id.
     *
     *      @param [in] parameterId     The id of the parameter being searched for.
     *
     *      @return The parameter with the selected ID.
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
     *  Gets all of the ids of parameters in the list.
     *
     *      @return A list containing all of the ids.
     */
    virtual QStringList getAllParameterIds() const;

    /*!
     *  Gets the number of parameters in the list.
     *
     *      @return The number of parameters in the list.
     */
    virtual int getNumberOfParameters() const;

    /*!
     *  Set the configurable element list.
     *
     *      @param [in] newElementList  List containing the configurable elements.
     */
    void setConfigurableElementList(QList<QSharedPointer<EditorConfigurableElement> > newElementList);

    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
     */
    virtual void registerParameterModel(QAbstractItemModel const* model);

private:

    //! No copying. No assignment.
    ConfigurableElementFinder(const ConfigurableElementFinder& other);
    ConfigurableElementFinder& operator=(const ConfigurableElementFinder& other);

    /*!
     *  Get the configurable element referenced the selected parameter ID.
     *
     *      @param [in] id  ID of the selected parameter reference.
     *
     *      @return Configurable element containing the selected parameter reference.
     */
    QSharedPointer<EditorConfigurableElement> getElementForID(QString const& id) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The configurable elements are searched from this list.
    QList<QSharedPointer<EditorConfigurableElement> > elementList_;
};

#endif // LISTPARAMETERFINDER_H
