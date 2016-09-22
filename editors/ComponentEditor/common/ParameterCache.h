//-----------------------------------------------------------------------------
// File: ParameterCache.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.09.2016
//
// Description:
// The cached implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef PARAMETERCACHE_H
#define PARAMETERCACHE_H

#include "ParameterFinder.h"
#include "ComponentParameterFinder.h"

#include <QHash>

class Component;
class Parameter;

//-----------------------------------------------------------------------------
//! The cached implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------
class ParameterCache : public QObject, public ComponentParameterFinder
{
    Q_OBJECT
public:

    /*!
     *  Constructor.
     *
	 *      @param [in] component		The component which parameters are being searched for.
     */
    ParameterCache(QSharedPointer<Component> component);

    /*!
     *  Destructor.
     */
    ~ParameterCache();
    
    /*!
     *  Get the parameter corresponding the id.
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
    virtual void setComponent(QSharedPointer<Component> component);
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *      @param [in] model   The model to register.
     */
    virtual void registerParameterModel(QAbstractItemModel const* model);

private slots:

    void resetCache(); 

private:

	//! No copying
    ParameterCache(const ParameterCache& other);
	//! No assignment
    ParameterCache& operator=(const ParameterCache& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Parameters available in the cache. Key is the parameterID.
	QHash<QString, QSharedPointer<Parameter> > availableParameters_;

};

#endif // PARAMETERCACHE_H
