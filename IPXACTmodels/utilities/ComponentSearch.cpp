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
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Design/Design.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findDesignReference()
//-----------------------------------------------------------------------------
VLNV ComponentSearch::findDesignReference(QSharedPointer<Component> component, LibraryInterface* libraryHandler, 
    QString const& viewName)
{
    return findDesignReference(component, libraryHandler, Search::findByName(viewName, *component->getViews()));
}

//-----------------------------------------------------------------------------
// Function: ComponentSearch::findDesignReference()
//-----------------------------------------------------------------------------
VLNV ComponentSearch::findDesignReference(QSharedPointer<Component> component, LibraryInterface* libraryHandler, 
    QSharedPointer<View> view)
{
    if (view.isNull() == false)
    {
        if (auto designInstantion = Search::findByName(view->getDesignInstantiationRef(),
            *component->getDesignInstantiations()))
        {
            return VLNV(*designInstantion->getDesignReference());
        }

        // Look for design reference in design configuration if no design instantiation.
        auto designConfigRef = findDesignConfigurationReference(component, view);

        if (auto designCfg = libraryHandler->getModelReadOnly<DesignConfiguration>(designConfigRef))
        {
            return designCfg->getDesignRef();
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
    if (QSharedPointer<View> view = findView(component, viewName))
    {
        for (auto const& instantiation : *component->getComponentInstantiations())
        {
            if (instantiation->name() == view->getComponentInstantiationRef())
            {
                return instantiation;
            }
        }
    }

    return QSharedPointer<ComponentInstantiation>();
}