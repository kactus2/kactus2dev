////-----------------------------------------------------------------------------
// File: ComponentSearch.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.06.2018
//
// Description:
// Utilities for searching items in Component.
//-----------------------------------------------------------------------------

#include "ComponentSearch.h"

#include <IPXACTmodels/Component/Model.h>

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findDesignReference()
//-----------------------------------------------------------------------------
VLNV ComponentSearch::findDesignReference(QSharedPointer<Component> component, QString const& viewName)
{
    return findDesignReference(component, findByName(viewName, *component->getViews()));
}

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findDesignReference()
//-----------------------------------------------------------------------------
VLNV ComponentSearch::findDesignReference(QSharedPointer<Component> component, QSharedPointer<View> view)
{
    if (view.isNull() == false)
    {
        auto designInstantion = findByName(view->getDesignInstantiationRef(),
            *component->getDesignInstantiations());

        if (designInstantion)
        {
            return VLNV(*designInstantion->getDesignReference());
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findDesignConfigurationReference()
//-----------------------------------------------------------------------------
VLNV ComponentSearch::findDesignConfigurationReference(QSharedPointer<Component> component, QSharedPointer<View> view)
{
    if (view.isNull() == false)
    {
        auto configurationInstantiation = findByName(view->getDesignConfigurationInstantiationRef(),
            *component->getDesignConfigurationInstantiations());

        if (configurationInstantiation)
        {
            return  VLNV(*configurationInstantiation->getDesignConfigurationReference());
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findView()
//-----------------------------------------------------------------------------
QSharedPointer<View> ComponentSearch::findView(QSharedPointer<Component> component, QString const& viewName)
{
    return findByName(viewName, *component->getViews());
}
