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

#include "Search.h"

#include <IPXACTmodels/Component/Model.h>

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findDesignReference()
//-----------------------------------------------------------------------------
VLNV ComponentSearch::findDesignReference(QSharedPointer<Component> component, QString const& viewName)
{
    return findDesignReference(component, Search::findByName(viewName, *component->getViews()));
}

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findDesignReference()
//-----------------------------------------------------------------------------
VLNV ComponentSearch::findDesignReference(QSharedPointer<Component> component, QSharedPointer<View> view)
{
    if (view.isNull() == false)
    {
        auto designInstantion = Search::findByName(view->getDesignInstantiationRef(),
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
        auto configurationInstantiation = Search::findByName(view->getDesignConfigurationInstantiationRef(),
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
    return Search::findByName(viewName, *component->getViews());
}

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> ComponentSearch::findComponentInstantiation(
    QSharedPointer<Component> component, QString const& viewName)
{
    QSharedPointer<View> view = findView(component, viewName);

    if (view)
    {
        foreach(QSharedPointer<ComponentInstantiation> instantiation, *component->getComponentInstantiations())
        {
            if (instantiation->name() == view->getComponentInstantiationRef())
            {
                return instantiation;
            }
        }
    }

    return QSharedPointer<ComponentInstantiation>();
}