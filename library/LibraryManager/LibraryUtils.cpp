//-----------------------------------------------------------------------------
// File: LibraryUtils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.6.2011
//
// Description:
// Library utility functions.
//-----------------------------------------------------------------------------

#include "LibraryUtils.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/common/VLNV.h>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: getDesign()
//-----------------------------------------------------------------------------
bool getDesign(LibraryInterface* lh, VLNV& designVLNV,
               QSharedPointer<Design const>& design, QSharedPointer<DesignConfiguration const>& designConf)
{
    designVLNV.setType(lh->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        return false;
    }

    // Check if the component contains a direct reference to a design.
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        design = lh->getModelReadOnly(designVLNV).staticCast<Design const>();
    }
    // Otherwise check if the component had reference to a design configuration.
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        designConf = lh->getModelReadOnly(designVLNV).staticCast<DesignConfiguration const>();

        designVLNV = designConf->getDesignRef();
        if (designVLNV.isValid())
        {
            design = lh->getModelReadOnly(designVLNV).staticCast<Design const>();
        }

        // If design configuration did not contain a reference to a design.
        if (!design)
        {
            return false;
        }
    }
    // If referenced view was not found.
    else
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: getConnectionIndex()
//-----------------------------------------------------------------------------
int getConnectionIndex(QList<Interconnection> const& connections, QString const& name)
{
    // Search for a match in the list.
    for (int i = 0; i < connections.size(); ++i)
    {
        if (connections.at(i).name() == name)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: getInstanceIndex()
//-----------------------------------------------------------------------------
int getInstanceIndex(QList<QSharedPointer<ComponentInstance> > instances, QString const& instanceName)
{
    // Search for a match in the list.
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances.at(i)->getInstanceName() == instanceName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: getInstanceIndexByUUID()
//-----------------------------------------------------------------------------
int getInstanceIndexByUUID(QList<QSharedPointer<ComponentInstance> > instances, QString const& uuid)
{
    // Search for a match in the list.
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances.at(i)->getUuid() == uuid)
        {
            return i;
        }
    }

    return -1;
}


//-----------------------------------------------------------------------------
// Function: getInstanceIndex()
//-----------------------------------------------------------------------------
int getInstanceIndex(QList<QSharedPointer<SWInstance> > instances, QString const& importRef,
                     QString const& mapping)
{
    // Search for a match in the list.
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances[i]->getImportRef() == importRef && instances[i]->getMapping() == mapping)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: parseProgrammableElements()
//-----------------------------------------------------------------------------
void parseProgrammableElementsV2(LibraryInterface* lh, VLNV designVLNV,
                                 QList<QSharedPointer<ComponentInstance> >& elements)
{
    QSharedPointer<Design const> compDesign;
    QSharedPointer<DesignConfiguration const> designConf;

    // The received type is always VLNV::DESIGN so it must be asked from the
    // library handler to make sure the type is correct.
    designVLNV.setType(lh->getDocumentType(designVLNV));

    if (!getDesign(lh, designVLNV, compDesign, designConf))
    {
        return;
    }

    // Go through all component instances and search for programmable elements.
    foreach (QSharedPointer<ComponentInstance> instance, *compDesign->getComponentInstances())
    {
        if (!instance->isDraft())
        {
            QSharedPointer<Document> libComp = lh->getModel(*instance->getComponentRef());
            QSharedPointer<Component> childComp = libComp.staticCast<Component>();

            if (childComp)
            {
                // Add the component to the system design only if it is a leaf component and has a CPU
                // or COM interfaces.
                if ((!childComp->isHierarchical() && childComp->isCpu()) || !childComp->getComInterfaces().isEmpty())
                {
                    QSharedPointer<ComponentInstance> copy(new ComponentInstance(*instance));

                    // Determine a unique name for the instance->
                    QString instanceName = instance->getInstanceName();
                    int runningNumber = 1;

                    while (getInstanceIndex(elements, instanceName) != -1)
                    {
                        instanceName = instance->getInstanceName() + "_" + QString::number(runningNumber);
                    }

                    copy->setInstanceName(instanceName);
                    elements.append(copy);
                }
                else
                {
                    QString view = "";

                    if (designConf != 0)
                    {
                        view = designConf->getActiveView(instance->getInstanceName());
                    }

                    // Otherwise parse the hierarchical components recursively.
                    parseProgrammableElementsV2(lh, childComp->getHierRef(view), elements);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addNewInstances()
//-----------------------------------------------------------------------------
void addNewInstancesV2(LibraryInterface* lh, QList<QSharedPointer<ComponentInstance> > elements,
                       QSharedPointer<QList<QSharedPointer<ComponentInstance> > > hwInstances,
                       QList<QSharedPointer<SWInstance> > swInstances,
                       QList<QSharedPointer<ApiInterconnection> > apiDependencies)
{
    foreach (QSharedPointer<ComponentInstance> element, elements)
    {
        // Duplicate the component instance and set its import reference.
        QSharedPointer<ComponentInstance> instance(new ComponentInstance(element->getInstanceName(), 
            element->getComponentRef()));

        instance->setDisplayName(element->getDisplayName());
        instance->setDescription(element->getDescription()),
        instance->setUuid(element->getUuid());
        instance->setImported(true);

        // Add the newly created HW component to the list of HW instances.
        hwInstances->append(instance);

        // Import SW components from SW design if found.
        QSharedPointer<Document const> libComp = lh->getModelReadOnly(*instance->getComponentRef());
        QSharedPointer<Component const> component = libComp.staticCast<Component const>();

        if (component != 0 && component->hasSWViews())
        {
            QSharedPointer<Design const> swDesign;
            QSharedPointer<DesignConfiguration const> swDesignConf;

            VLNV designVLNV = component->getHierSWRef();
            if (!getDesign(lh, designVLNV, swDesign, swDesignConf))
            {
                continue;
            }

            foreach (QSharedPointer<SWInstance> instance, swDesign->getSWInstances())
            {
                instance->setPosition(QPointF());
                instance->setImported(true);
                instance->setMapping(element->getUuid());
                swInstances.append(instance);
            }

            foreach (QSharedPointer<ApiInterconnection> dependency, swDesign->getApiConnections())
            {
                apiDependencies.append(dependency);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: generateSystemDesign()
//-----------------------------------------------------------------------------
void generateSystemDesignV2(LibraryInterface* lh, VLNV const& designVLNV, Design& sysDesign)
{
    // Parse all programmable elements from the HW component.
    QList<QSharedPointer<ComponentInstance> > elements;
    parseProgrammableElementsV2(lh, designVLNV, elements);

    // Add them as instances to the system design.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > 
        hwInstances(new QList<QSharedPointer<ComponentInstance> >());
    QList<QSharedPointer<SWInstance> > swInstances;
    QList<QSharedPointer<ApiInterconnection> > apiDependencies;
    addNewInstancesV2(lh, elements, hwInstances, swInstances, apiDependencies);

    sysDesign.setComponentInstances(hwInstances);
    sysDesign.setSWInstances(swInstances);
    sysDesign.setApiConnections(apiDependencies);
}

//-----------------------------------------------------------------------------
// Function: getMatchingApiDependency()
//-----------------------------------------------------------------------------
int getMatchingApiDependency(QList<QSharedPointer<ApiInterconnection> > apiDependencies,
                             QList<QSharedPointer<SWInstance> > swInstances,
                             QSharedPointer<ApiInterconnection> dependency, QString const& mapping) 
{
    int index = -1;

    // Retrieve the full names for the connected components in the system design.
    QString startComponentReference = dependency->getStartInterface()->getComponentReference();
    QString startApiReference = dependency->getStartInterface()->getBusReference();
    int instanceIndex1 = getInstanceIndex(swInstances, startComponentReference, mapping);

    if (instanceIndex1 == -1)
    {
        return -1;
    }
    
    int instanceIndex2 = -1;

    QString endComponentReference = "";
    QString endApiReference = dependency->getEndInterface()->getBusReference();
    QSharedPointer<ActiveInterface> activeEndInterface =
        dependency->getEndInterface().dynamicCast<ActiveInterface>();
    if (activeEndInterface)
    {
        instanceIndex2 = getInstanceIndex(swInstances, endComponentReference, mapping);
    }

    if (instanceIndex2 == -1)
    {
        return -1;
    }

    // Search for a match in the list.
    for (int i = 0; i < apiDependencies.size(); ++i)
    {
        QSharedPointer<ActiveInterface> listStartInterface = apiDependencies.at(i)->getStartInterface();
        QSharedPointer<ActiveInterface> listEndInterface =
            apiDependencies.at(i)->getEndInterface().dynamicCast<ActiveInterface>();
        if (listEndInterface)
        {
            if (listStartInterface->getComponentReference() == swInstances[instanceIndex1]->getInstanceName() &&
                listEndInterface->getComponentReference() == swInstances[instanceIndex2]->getInstanceName() &&
                listStartInterface->getBusReference() == startApiReference &&
                listEndInterface->getBusReference() == endApiReference)
            {
                index = i;
                break;
            }
        }
        else
        {
            break;
        }

    }

    return index;
}

//-----------------------------------------------------------------------------
// Function: updateSystemDesign()
//-----------------------------------------------------------------------------
void updateSystemDesignV2(LibraryInterface* lh, VLNV const& hwDesignVLNV, Design& sysDesign,
                          QSharedPointer<DesignConfiguration> designConf)
{
    // Parse all programmable elements from the HW design.
    QList<QSharedPointer<ComponentInstance> >  elements;
    parseProgrammableElementsV2(lh, hwDesignVLNV, elements);

    // Reflect changes in the programmable elements to the system design.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > 
        hwInstances(new QList<QSharedPointer<ComponentInstance> >());
    QList<QSharedPointer<SWInstance> > swInstances;
    QList<QSharedPointer<ApiInterconnection> > apiDependencies;

    // 1. PHASE: Check already existing elements against the new list and remove those that
    // are no longer part of the new element list.
    foreach (QSharedPointer<ComponentInstance> hwInstance, *sysDesign.getComponentInstances())
    {
        // Imported ones should be checked.
        if (hwInstance->isImported())
        {
            // Search for the corresponding element in the new list based on the UUID.
            int index = getInstanceIndexByUUID(elements, hwInstance->getUuid());

            // If the element was removed, it is not added to the updated design.
            if (index == -1)
            {
                continue;
            }

            QSharedPointer<ComponentInstance> instanceCopy(new ComponentInstance(*hwInstance));
            instanceCopy->setComponentRef(elements[index]->getComponentRef());
            hwInstances->append(instanceCopy);

            // Remove the element from the element list since it has been processed.
            elements.removeAt(index);
        }
        else
        {
            // Non-imported instances are always kept.
            hwInstances->append(hwInstance);
        }
    }

    // 2. PHASE: Add fresh new elements.
    foreach (QSharedPointer<ComponentInstance> element, elements)
    {
        // Duplicate the component instance and set its kts_hw_ref.
        QSharedPointer<ComponentInstance> instance(new ComponentInstance(element->getInstanceName(),
            element->getComponentRef()));
        instance->setDisplayName(element->getDisplayName());
        instance->setDescription(element->getDescription()); 
        instance->setUuid(element->getUuid());

        instance->setImported(true);

        hwInstances->append(instance);
    }

    QList<QSharedPointer<SWInstance> > oldSWInstances = sysDesign.getSWInstances();
    QList<QSharedPointer<ApiInterconnection> > oldApiDependencies = sysDesign.getApiConnections();

    // 3. PHASE: Copy non-imported instances from the old list to the new list.
    foreach (QSharedPointer<SWInstance> swInstance, oldSWInstances)
    {
        if (!swInstance->isImported())
        {
            swInstances.append(swInstance);
        }
    }

    // 4. PHASE: Parse SW designs from active SW views to retrieve the imported SW instances.
    foreach (QSharedPointer<ComponentInstance> hwInstance, *hwInstances)
    {
        QSharedPointer<Document const> libComp = lh->getModelReadOnly(*hwInstance->getComponentRef());
        QSharedPointer<Component const> component = libComp.staticCast<Component const>();

        QString viewName = "";

        if (designConf != 0)
        {
            viewName = designConf->getActiveView(hwInstance->getInstanceName());
        }

        if (component != 0 && component->hasSWViews())
        {
            QSharedPointer<Design const> swDesign;
            QSharedPointer<DesignConfiguration const> swDesignConf;

            VLNV designVLNV = component->getHierSWRef(viewName);

            if (!getDesign(lh, designVLNV, swDesign, swDesignConf))
            {
                continue;
            }

            foreach (QSharedPointer<SWInstance> swInstance, swDesign->getSWInstances())
            {
                // Check if the instance already exists in the old system design.
                int index = getInstanceIndex(oldSWInstances, swInstance->getInstanceName(), hwInstance->getUuid());

                if (index != -1 && oldSWInstances[index]->isImported())
                {
                    swInstance = oldSWInstances[index];
                    swInstance->setComponentRef(swInstance->getComponentRef());

                    swInstances.append(swInstance);
                    oldSWInstances.removeAt(index);
                }
                else
                {
                    // Otherwise add the SW instance as a fresh new one.
                    swInstance->setPosition(QPointF());
                    swInstance->setImportRef(swInstance->getInstanceName());
                    swInstance->setMapping(hwInstance->getUuid());
                    swInstance->setDisplayName(swInstance->getInstanceName());
                    swInstance->setInstanceName(hwInstance->getInstanceName() + "_" + swInstance->getInstanceName());
                    swInstances.append(swInstance);
                }
            }

            foreach (QSharedPointer<ApiInterconnection> dependency, swDesign->getApiConnections())
            {
                int connIndex = getMatchingApiDependency(oldApiDependencies, swInstances,
                                                         dependency, hwInstance->getInstanceName());

                if (connIndex != -1 && oldApiDependencies[connIndex]->isImported())
                {
                    apiDependencies.append(oldApiDependencies[connIndex]);
                    oldApiDependencies.removeAt(connIndex);
                }
                else
                {
                    dependency->setName(hwInstance->getInstanceName() + "_" + dependency->name());
                    QSharedPointer<ActiveInterface> startInterface = dependency->getStartInterface();
                    QString startInterfaceComponentRef = hwInstance->getInstanceName() + "_" +
                        startInterface->getComponentReference();

                    QSharedPointer<ActiveInterface> newStartInterface (
                        new ActiveInterface(startInterfaceComponentRef, startInterface->getBusReference()));
                    dependency->setStartInterface(newStartInterface);

                    QSharedPointer<ActiveInterface> activeEndInterface =
                        dependency->getEndInterface().dynamicCast<ActiveInterface>();
                    if (activeEndInterface)
                    {
                        QString endInterfaceComponentRef =
                            hwInstance->getInstanceName() + "_" + activeEndInterface->getComponentReference();

                        QSharedPointer<ActiveInterface> newEndInterface (
                            new ActiveInterface(endInterfaceComponentRef, activeEndInterface->getBusReference()));
                        dependency->setInterface(newEndInterface);
                    }

                    dependency->setImported(true);
                    apiDependencies.append(dependency);
                }
            }
        }
    }

    // 5. PHASE: Copy non-imported API dependencies from the old list to the new list.
    foreach (QSharedPointer<ApiInterconnection> dependency, oldApiDependencies)
    {
        if (!dependency->isImported())
        {
            apiDependencies.append(dependency);
        }
    }

    sysDesign.setComponentInstances(hwInstances);
    sysDesign.setSWInstances(swInstances);
    sysDesign.setApiConnections(apiDependencies);
}
