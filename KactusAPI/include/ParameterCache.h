//-----------------------------------------------------------------------------
// File: ParameterCache.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 06.09.2016
//
// Description:
// The cached implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#ifndef PARAMETERCACHE_H
#define PARAMETERCACHE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include "ParameterFinder.h"
#include "ComponentParameterFinder.h"

#include <QHash>
#include <QObject>

class Component;
class Parameter;

//-----------------------------------------------------------------------------
//! The cached implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------
class KACTUS2_API ParameterCache : public QObject, public ComponentParameterFinder
{
    Q_OBJECT
public:

    /*!
     *  Constructor.
     *
	 *    @param [in] component		The component which parameters are being searched for.
     */
    ParameterCache(QSharedPointer<const Component> component);

    /*!
     *  Destructor.
     */
    ~ParameterCache() override = default;

    //! No copying
    ParameterCache(const ParameterCache& other) = delete;
    //! No assignment
    ParameterCache& operator=(const ParameterCache& other) = delete;

    /*!
     *  Get the parameter corresponding the id.
     *
     *    @param [in] parameterId     The id of the parameter being searched for.
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
     *  Gets all of the ids of components parameters.
     *
     *    @return A list containing all of the ids.
     */
    QStringList getAllParameterIds() const final;

	/*!
     *  Gets the number of parameters in the component.
     *
     *    @return The number of parameters in the component.
     */
    int getNumberOfParameters() const final;

    /*!
     *  Set a new component for the parameter finder.
     *
     *    @param [in] component   The new component.
     */
    void setComponent(QSharedPointer<Component const> component) final;
    
    /*!
     *  Registers a parameter model that can modify parameters for the finder.
     *
     *    @param [in] model   The model to register.
     */
    void registerParameterModel(QAbstractItemModel const* model) final;

private slots:

    void resetCache(); 

private:


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Parameters available in the cache. Key is the parameterID.
	QHash<QString, QSharedPointer<Parameter> > availableParameters_;

};

#endif // PARAMETERCACHE_H
