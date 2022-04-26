//-----------------------------------------------------------------------------
// File: ComponentInstanceParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 13.01.2017
//
// Description:
// The implementation for finding parameters for component instance.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCEPARAMETERFINDER_H
#define COMPONENTINSTANCEPARAMETERFINDER_H

#include <editors/ComponentEditor/common/ParameterCache.h>

#include <KactusAPI/KactusAPIGlobal.h>

#include <QSharedPointer>

class Component;
class ComponentInstance;
class Parameter;

//-----------------------------------------------------------------------------
//! The implementation for finding parameters for component instance.
//-----------------------------------------------------------------------------
class KACTUS2_API ComponentInstanceParameterFinder : public ParameterCache
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     *
     *      @param [in] componentInstance	The component instance whose parameters are being searched for.
	 *      @param [in] component		    The instantiated component.
     */
    ComponentInstanceParameterFinder(QSharedPointer<const ComponentInstance> componentInstance,
        QSharedPointer<const Component> component);

    /*!
     *  Destructor.
     */
    virtual ~ComponentInstanceParameterFinder();
    
    /*!
     *  Finds the value of the parameter with the given id.
     *
     *      @param [in] id      The id of the parameter to search for.
     *
     *      @return The value of the parameter.
     */
    virtual QString valueForId(QString const& id) const;

private:

	//! No copying
    ComponentInstanceParameterFinder(const ParameterCache& other);
	//! No assignment
    ComponentInstanceParameterFinder& operator=(const ParameterCache& other);

    // The component instance whose parameters are being searched for.
    QSharedPointer<const ComponentInstance> componentInstance_;
};

#endif // COMPONENTINSTANCEPARAMETERFINDER_H
