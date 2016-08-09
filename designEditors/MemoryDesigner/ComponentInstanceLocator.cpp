//-----------------------------------------------------------------------------
// File: ComponentInstanceLocator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Locator for component instances within a selected design.
//-----------------------------------------------------------------------------

#include "ComponentInstanceLocator.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::ComponentInstanceLocator()
//-----------------------------------------------------------------------------
ComponentInstanceLocator::ComponentInstanceLocator(LibraryInterface* library):
libraryHandler_(library)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::~ComponentInstanceLocator()
//-----------------------------------------------------------------------------
ComponentInstanceLocator::~ComponentInstanceLocator()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::getMatchingInstances()
//-----------------------------------------------------------------------------
QList<ComponentInstanceLocator::MatchingInstance> ComponentInstanceLocator::getMatchingInstances(
    QSharedPointer<Design> design, QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    QList<ComponentInstanceLocator::MatchingInstance> foundInstances;

    if (design)
    {
        foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
        {
            if (!instance->isDraft())
            {
                QSharedPointer<ConfigurableVLNVReference> componentVLNV = instance->getComponentRef();
                if (componentVLNV)
                {
                    QSharedPointer<const Document> componentDocument =
                        libraryHandler_->getModelReadOnly(*componentVLNV.data());

                    QSharedPointer<const Component> instancedComponent =
                        componentDocument.dynamicCast<const Component>();
                    if (instancedComponent)
                    {
                        foundInstances.append(getInstanceData(instance, designConfiguration));
                    }

                    if (designConfiguration)
                    {
                        QList<ComponentInstanceLocator::MatchingInstance> childInstances =
                            getChildInstancesFromComponent(instance, instancedComponent, designConfiguration);
                        if (!childInstances.isEmpty())
                        {
                            foundInstances.append(childInstances);
                        }
                    }
                }
            }
        }
    }

    return foundInstances;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::getInstanceData()
//-----------------------------------------------------------------------------
ComponentInstanceLocator::MatchingInstance ComponentInstanceLocator::getInstanceData(
    QSharedPointer<ComponentInstance> instance, QSharedPointer<const DesignConfiguration> designConfiguration)
    const
{
    MatchingInstance newInstance;

    newInstance.instanceName_ = instance->getInstanceName();
    newInstance.instanceUUID_ = instance->getUuid();
    newInstance.componentVLNV_ = instance->getComponentRef();

    newInstance.activeView_ = designConfiguration->getActiveView(instance->getInstanceName());

    return newInstance;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::getChildInstancesFromComponent()
//-----------------------------------------------------------------------------
QList<ComponentInstanceLocator::MatchingInstance> ComponentInstanceLocator::getChildInstancesFromComponent(
    QSharedPointer<ComponentInstance> instance, QSharedPointer<const Component> instancedComponent,
    QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    QString viewName = designConfiguration->getActiveView(instance->getInstanceName());
    if (!viewName.isEmpty())
    {
        foreach (QSharedPointer<View> componentView, *instancedComponent->getViews())
        {
            if (componentView->name() == viewName)
            {
                if (componentView->isHierarchical())
                {
                    QSharedPointer<const DesignConfiguration> hierarchicalConfiguration =
                        getHierarchicalDesignConfiguration(instancedComponent, componentView);
                    QSharedPointer<Design> hierarchicalDesign = getHierarchicalDesign(instancedComponent,
                        componentView, hierarchicalConfiguration);

                    return getMatchingInstances(hierarchicalDesign, hierarchicalConfiguration);
                }

                break;
            }
        }
    }

    QList<ComponentInstanceLocator::MatchingInstance> noInstances;
    return noInstances;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::getHierarchicalDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<const DesignConfiguration> ComponentInstanceLocator::getHierarchicalDesignConfiguration(
    QSharedPointer<const Component> component, QSharedPointer<View> hierarchicalView) const
{
    QString configurationReference = hierarchicalView->getDesignConfigurationInstantiationRef();
    if (!configurationReference.isEmpty())
    {
        foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
            *component->getDesignConfigurationInstantiations())
        {
            if (instantiation->name() == configurationReference)
            {
                QSharedPointer<ConfigurableVLNVReference> configurationVLNV =
                    instantiation->getDesignConfigurationReference();

                QSharedPointer<const Document> configurationDocument =
                    libraryHandler_->getModelReadOnly(*configurationVLNV.data());
                QSharedPointer<const DesignConfiguration> configuration =
                    configurationDocument.dynamicCast<const DesignConfiguration>();

                return configuration;
            }
        }
    }

    return QSharedPointer<const DesignConfiguration>();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::getHierarchicalDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> ComponentInstanceLocator::getHierarchicalDesign(QSharedPointer<const Component> component,
    QSharedPointer<View> hierarchicalView, QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    VLNV designVLNV = getHierarchicalDesignVLNV(component, hierarchicalView, designConfiguration);

    QSharedPointer<Design> design;

    if (designVLNV.isValid())
    {
        design = libraryHandler_->getDesign(designVLNV);
    }

    return design;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceLocator::getHierarchicalDesignVLNV()
//-----------------------------------------------------------------------------
VLNV ComponentInstanceLocator::getHierarchicalDesignVLNV(QSharedPointer<const Component> component,
    QSharedPointer<View> hierarchicalView, QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    QString referencedInstantiation = hierarchicalView->getDesignInstantiationRef();

    if (!referencedInstantiation.isEmpty())
    {
        foreach (QSharedPointer<DesignInstantiation> instantiation, *component->getDesignInstantiations())
        {
            if (instantiation->name() == referencedInstantiation)
            {
                QSharedPointer<ConfigurableVLNVReference> designConfigurableVLNV =
                    instantiation->getDesignReference();
                return *designConfigurableVLNV.data();
            }
        }
    }
    else
    {
        return designConfiguration->getDesignRef();
    }

    return VLNV();
}
