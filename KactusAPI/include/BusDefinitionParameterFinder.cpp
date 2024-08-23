//-----------------------------------------------------------------------------
// File: BusDefinitionParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 22.08.2024
//
// Description:
// The implementation for finding bus definition parameters with the correct ID.
//-----------------------------------------------------------------------------

#include "BusDefinitionParameterFinder.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::BusDefinitionParameterFinder()
//-----------------------------------------------------------------------------
BusDefinitionParameterFinder::BusDefinitionParameterFinder(QSharedPointer<BusDefinition const> busDef) noexcept :
    busDef_(busDef)
{
    
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::setBusDef()
//-----------------------------------------------------------------------------
void BusDefinitionParameterFinder::setBusDef(QSharedPointer<BusDefinition> busDef)
{
    busDef_ = busDef;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> BusDefinitionParameterFinder::getParameterWithID(QStringView parameterId) const
{
    if (!busDef_)
    {
        return nullptr;
    }

    auto busParams = busDef_->getParameters();

    auto paramIter = std::find_if(busParams->cbegin(), busParams->cend(), [&parameterId](QSharedPointer<Parameter> param)
        {
            return param->getValueId() == parameterId;
        });

    return paramIter != busParams->cend() ? *paramIter : nullptr;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool BusDefinitionParameterFinder::hasId(QStringView id) const
{
    return busDef_ && getParameterWithID(id);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString BusDefinitionParameterFinder::nameForId(QStringView id) const
{
    if (auto foundParam = getParameterWithID(id))
    {
        return foundParam->name();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString BusDefinitionParameterFinder::valueForId(QStringView id) const
{
    if (auto foundParam = getParameterWithID(id))
    {
        return foundParam->getValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList BusDefinitionParameterFinder::getAllParameterIds() const
{
    QStringList ids;
    ids.reserve(busDef_->getParameters()->size());
    std::transform(busDef_->getParameters()->cbegin(), busDef_->getParameters()->cend(), std::back_inserter(ids),
        [](QSharedPointer<Parameter> param)
        {
            return param->getValueId();
        });

    return ids;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int BusDefinitionParameterFinder::getNumberOfParameters() const
{
    return busDef_->getParameters()->size();
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void BusDefinitionParameterFinder::registerParameterModel(QAbstractItemModel const*)
{
    /* do nothing */
}
