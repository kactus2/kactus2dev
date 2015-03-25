//-----------------------------------------------------------------------------
// File: ListParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.03.2015
//
// Description:
// The implementation for finding parameters from a list with the correct ID.
//-----------------------------------------------------------------------------

#ifndef LISTPARAMETERFINDER_H
#define LISTPARAMETERFINDER_H

#include "ParameterFinder.h"

#include <IPXACTmodels/parameter.h>

#include <QSharedPointer>
#include <QList>

class Component;

//-----------------------------------------------------------------------------
//! The implementation for finding parameters from a list with the correct ID.
//-----------------------------------------------------------------------------
//class ComponentParameterFinder : public ParameterFinder
class ListParameterFinder : public ParameterFinder
{

public:

    /*!
     *  The constructor.
     */
    ListParameterFinder();

    /*!
     *  The Destructor.
     */
    ~ListParameterFinder();

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
     *  Gets all of the ids of parameters in the list.
     *
     *      @return A list containing all of the ids.
     */
    virtual QStringList getAllParameterIds() const;

    /*!
     *  Gets the number of parameters in the list.
     *
     *      @return The number of parameters in the component.
     */
    virtual int getNumberOfParameters() const;

    /*!
     *  Set the parameter list.
     *
     *      @param [in] parameterList   [Description].
     */
    void setParameterList(QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList);

private:

	//! No copying
    ListParameterFinder(const ListParameterFinder& other);
	//! No assignment
    ListParameterFinder& operator=(const ListParameterFinder& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameters are searched from this list.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList_;
};

#endif // LISTPARAMETERFINDER_H
