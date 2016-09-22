//-----------------------------------------------------------------------------
// File: ParameterCache.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.09.2016
//
// Description:
// The cached implementation for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#include "ParameterCache.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QAbstractItemModel>

//-----------------------------------------------------------------------------
// Function: ParameterCache::ParameterCache()
//-----------------------------------------------------------------------------
ParameterCache::ParameterCache(QSharedPointer<Component> component) : QObject(0),
ComponentParameterFinder(component), availableParameters_()
{
    resetCache();
}

//-----------------------------------------------------------------------------
// Function: ParameterCache::~ParameterCache()
//-----------------------------------------------------------------------------
ParameterCache::~ParameterCache()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterCache::hasId()
//-----------------------------------------------------------------------------
bool ParameterCache::hasId(QString const& id) const
{
    return availableParameters_.contains(id);
}

//-----------------------------------------------------------------------------
// Function: ParameterCache::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ParameterCache::getAllParameterIds() const
{
    return availableParameters_.keys();
}

//-----------------------------------------------------------------------------
// Function: ParameterCache::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ParameterCache::getParameterWithID(QString const& parameterId) const
{
	return availableParameters_.value(parameterId);
}

//-----------------------------------------------------------------------------
// Function: ParameterCache::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ParameterCache::getNumberOfParameters() const
{
	return availableParameters_.count();
}

//-----------------------------------------------------------------------------
// Function: ParameterCache::setComponent()
//-----------------------------------------------------------------------------
void ParameterCache::setComponent(QSharedPointer<Component> component)
{
	ComponentParameterFinder::setComponent(component);
	
    resetCache();
}

//-----------------------------------------------------------------------------
// Function: ParameterCache::registerParameterModel()
//-----------------------------------------------------------------------------
void ParameterCache::registerParameterModel(QAbstractItemModel const* model)
{
    ComponentParameterFinder::registerParameterModel(model);

    connect(model, SIGNAL(modelReset()), this, SLOT(resetCache()), Qt::UniqueConnection);
    connect(model, SIGNAL(rowsRemoved(QModelIndex const&, int, int)), this, SLOT(resetCache()), Qt::UniqueConnection);
    connect(model, SIGNAL(rowsInserted(QModelIndex const&, int, int)), this, SLOT(resetCache()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ParameterCache::resetCache()
//-----------------------------------------------------------------------------
void ParameterCache::resetCache()
{
    availableParameters_.clear();
    foreach(QString const& id, ComponentParameterFinder::getAllParameterIds())
    {
        availableParameters_.insert(id, ComponentParameterFinder::getParameterWithID(id));
    }
}
