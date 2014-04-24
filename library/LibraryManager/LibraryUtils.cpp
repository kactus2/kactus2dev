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

#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>

#include <library/LibraryManager/vlnv.h>
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
        QSharedPointer<LibraryComponent const> libComp = lh->getModelReadOnly(designVLNV);
        design = libComp.staticCast<Design const>();
    }
    // Otherwise check if the component had reference to a design configuration.
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent const> libComp = lh->getModelReadOnly(designVLNV);
        designConf = libComp.staticCast<DesignConfiguration const>();

        designVLNV = designConf->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<LibraryComponent const> libComp = lh->getModelReadOnly(designVLNV);
            design = libComp.staticCast<Design const>();
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
int getConnectionIndex(QList<Design::Interconnection> const& connections, QString const& name)
{
    int index = -1;

    // Search for a match in the list.
    for (int i = 0; i < connections.size(); ++i)
    {
        if (connections[i].name == name)
        {
            index = i;
            break;
        }
    }

    return index;
}

//-----------------------------------------------------------------------------
// Function: getInstanceIndex()
//-----------------------------------------------------------------------------
int getInstanceIndex(QList<ComponentInstance> const& instances, QString const& instanceName)
{
    int index = -1;

    // Search for a match in the list.
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances[i].getInstanceName() == instanceName)
        {
            index = i;
            break;
        }
    }

    return index;
}

//-----------------------------------------------------------------------------
// Function: getInstanceIndexByUUID()
//-----------------------------------------------------------------------------
int getInstanceIndexByUUID(QList<ComponentInstance> const& instances, QString const& uuid)
{
    int index = -1;

    // Search for a match in the list.
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances[i].getUuid() == uuid)
        {
            index = i;
            break;
        }
    }

    return index;
}


//-----------------------------------------------------------------------------
// Function: getInstanceIndex()
//-----------------------------------------------------------------------------
int getInstanceIndex(QList<SWInstance> const& instances, QString const& importRef,
                     QString const& mapping)
{
    int index = -1;

    // Search for a match in the list.
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances[i].getImportRef() == importRef && instances[i].getMapping() == mapping)
        {
            index = i;
            break;
        }
    }

    return index;
}

//-----------------------------------------------------------------------------
// Function: parseProgrammableElements()
//-----------------------------------------------------------------------------
void parseProgrammableElementsV2(LibraryInterface* lh, VLNV designVLNV,
                                 QList<ComponentInstance>& elements)
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
    foreach (ComponentInstance const& instance, compDesign->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(instance.getComponentRef());
        QSharedPointer<Component> childComp = libComp.staticCast<Component>();

        if (childComp)
        {
            // Add the component to the system design only if it is a leaf component and has a CPU
            // or COM interfaces.
            if ((!childComp->isHierarchical() && childComp->isCpu()) || !childComp->getComInterfaces().isEmpty())
            {
                ComponentInstance copy = instance;

                // Determine a unique name for the instance.
                QString instanceName = instance.getInstanceName();
                int runningNumber = 1;

                while (getInstanceIndex(elements, instanceName) != -1)
                {
                    instanceName = instance.getInstanceName() + "_" + QString::number(runningNumber);
                }

                copy.setInstanceName(instanceName);
                elements.append(copy);
            }
            else
            {
                QString view = "";

                if (designConf != 0)
                {
                    view = designConf->getActiveView(instance.getInstanceName());
                }

                // Otherwise parse the hierarchical components recursively.
                parseProgrammableElementsV2(lh, childComp->getHierRef(view), elements);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addNewInstances()
//-----------------------------------------------------------------------------
void addNewInstancesV2(QList<ComponentInstance> const& elements,
                       LibraryInterface* lh,
                       QList<ComponentInstance>& hwInstances,
                       QList<SWInstance>& swInstances,
                       QList<ApiDependency>& apiDependencies)
{
    foreach (ComponentInstance const& element, elements)
    {
        // Duplicate the component instance and set its import reference.
        ComponentInstance instance(element.getInstanceName(), element.getDisplayName(),
                                   element.getDescription(), element.getComponentRef(),
                                   QPointF(0, 0), element.getUuid());
        instance.setImported(true);

        // Add the newly created HW component to the list of HW instances.
        hwInstances.append(instance);

        // Import SW components from SW design if found.
        QSharedPointer<LibraryComponent const> libComp = lh->getModelReadOnly(instance.getComponentRef());
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

            foreach (SWInstance instance, swDesign->getSWInstances())
            {
                instance.setPosition(QPointF());
                instance.setImported(true);
                instance.setMapping(element.getUuid());
                swInstances.append(instance);
            }

            foreach (ApiDependency const& dependency, swDesign->getApiDependencies())
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
    QList<ComponentInstance> elements;
    parseProgrammableElementsV2(lh, designVLNV, elements);

    // Add them as instances to the system design.
    QList<ComponentInstance> hwInstances;
    QList<SWInstance> swInstances;
    QList<ApiDependency> apiDependencies;
    addNewInstancesV2(elements, lh, hwInstances, swInstances, apiDependencies);

    sysDesign.setComponentInstances(hwInstances);
    sysDesign.setSWInstances(swInstances);
    sysDesign.setApiDependencies(apiDependencies);
}

//-----------------------------------------------------------------------------
// Function: getMatchingApiDependency()
//-----------------------------------------------------------------------------
int getMatchingApiDependency(QList<ApiDependency> const& apiDependencies,
                             QList<SWInstance> const& swInstances,
                             ApiDependency const& dependency, QString const& mapping) 
{
    int index = -1;

    // Retrieve the full names for the connected components in the system design.
    int instanceIndex1 = getInstanceIndex(swInstances, dependency.getInterface1().componentRef, mapping);

    if (instanceIndex1 == -1)
    {
        return -1;
    }
    
    int instanceIndex2 = getInstanceIndex(swInstances, dependency.getInterface2().componentRef, mapping);
    
    if (instanceIndex2 == -1)
    {
        return -1;
    }

    // Search for a match in the list.
    for (int i = 0; i < apiDependencies.size(); ++i)
    {
        if (apiDependencies[i].getInterface1().componentRef == swInstances[instanceIndex1].getInstanceName() &&
            apiDependencies[i].getInterface2().componentRef == swInstances[instanceIndex2].getInstanceName() &&
            apiDependencies[i].getInterface1().apiRef == dependency.getInterface1().apiRef &&
            apiDependencies[i].getInterface2().apiRef == dependency.getInterface2().apiRef)
        {
            index = i;
            break;
        }
    }

    return index;
}

//-----------------------------------------------------------------------------
// Function: updateSystemDesign()
//-----------------------------------------------------------------------------
void updateSystemDesignV2(LibraryInterface* lh,
                          VLNV const& hwDesignVLNV, Design& sysDesign,
                          QSharedPointer<DesignConfiguration> designConf)
{
    // Parse all programmable elements from the HW design.
    QList<ComponentInstance> elements;
    parseProgrammableElementsV2(lh, hwDesignVLNV, elements);

    // Reflect changes in the programmable elements to the system design.
    QList<ComponentInstance> hwInstances;
    QList<SWInstance> swInstances;
    QList<ApiDependency> apiDependencies;

    // 1. PHASE: Check already existing elements against the new list and remove those that
    // are no longer part of the new element list.
    foreach (ComponentInstance const& hwInstance, sysDesign.getComponentInstances())
    {
        // Imported ones should be checked.
        if (hwInstance.isImported())
        {
            // Search for the corresponding element in the new list based on the UUID.
            int index = getInstanceIndexByUUID(elements, hwInstance.getUuid());

            // If the element was removed, it is not added to the updated design.
            if (index == -1)
            {
                continue;
            }

            ComponentInstance instanceCopy = hwInstance;
            instanceCopy.setComponentRef(elements[index].getComponentRef());
            hwInstances.append(instanceCopy);

            // Remove the element from the element list since it has been processed.
            elements.removeAt(index);
        }
        else
        {
            // Non-imported instances are always kept.
            hwInstances.append(hwInstance);
        }
    }

    // 2. PHASE: Add fresh new elements.
    foreach (ComponentInstance const& element, elements)
    {
        // Duplicate the component instance and set its kts_hw_ref.
        ComponentInstance instance(element.getInstanceName(), element.getDisplayName(),
                                   element.getDescription(), element.getComponentRef(), QPointF(0, 0), element.getUuid());
        instance.setImported(true);

        hwInstances.append(instance);
    }

    QList<SWInstance> oldSWInstances = sysDesign.getSWInstances();
    QList<ApiDependency> oldApiDependencies = sysDesign.getApiDependencies();

    // 3. PHASE: Copy non-imported instances from the old list to the new list.
    foreach (SWInstance const& swInstance, oldSWInstances)
    {
        if (!swInstance.isImported())
        {
            swInstances.append(swInstance);
        }
    }

    // 4. PHASE: Parse SW designs from active SW views to retrieve the imported SW instances.
    foreach (ComponentInstance const& hwInstance, hwInstances)
    {
        QSharedPointer<LibraryComponent const> libComp = lh->getModelReadOnly(hwInstance.getComponentRef());
        QSharedPointer<Component const> component = libComp.staticCast<Component const>();

        QString viewName = "";

        if (designConf != 0)
        {
            viewName = designConf->getActiveView(hwInstance.getInstanceName());
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

            foreach (SWInstance swInstance, swDesign->getSWInstances())
            {
                // Check if the instance already exists in the old system design.
                int index = getInstanceIndex(oldSWInstances, swInstance.getInstanceName(), hwInstance.getUuid());

                if (index != -1 && oldSWInstances[index].isImported())
                {
                    VLNV componentRef = swInstance.getComponentRef();
                    swInstance = oldSWInstances[index];
                    swInstance.setComponentRef(componentRef);

                    swInstances.append(swInstance);
                    oldSWInstances.removeAt(index);
                }
                else
                {
                    // Otherwise add the SW instance as a fresh new one.
                    swInstance.setImported(true);
                    swInstance.setPosition(QPointF());
                    swInstance.setImportRef(swInstance.getInstanceName());
                    swInstance.setMapping(hwInstance.getUuid());
                    swInstance.setDisplayName(swInstance.getInstanceName());
                    swInstance.setInstanceName(hwInstance.getInstanceName() + "_" + swInstance.getInstanceName());
                    swInstances.append(swInstance);
                }
            }

            foreach (ApiDependency dependency, swDesign->getApiDependencies())
            {
                int connIndex = getMatchingApiDependency(oldApiDependencies, swInstances,
                                                         dependency, hwInstance.getInstanceName());

                if (connIndex != -1 && oldApiDependencies[connIndex].isImported())
                {
                    apiDependencies.append(oldApiDependencies[connIndex]);
                    oldApiDependencies.removeAt(connIndex);
                }
                else
                {
                    dependency.setName(hwInstance.getInstanceName() + "_" + dependency.getName());
                    dependency.setInterface1(ApiInterfaceRef(hwInstance.getInstanceName() + "_" + dependency.getInterface1().componentRef,
                                                             dependency.getInterface1().apiRef));
                    dependency.setInterface2(ApiInterfaceRef(hwInstance.getInstanceName() + "_" + dependency.getInterface2().componentRef,
                                                             dependency.getInterface2().apiRef));
                    dependency.setRoute(QList<QPointF>());
                    dependency.setImported(true);
                    apiDependencies.append(dependency);
                }
            }
        }
    }

    // 5. PHASE: Copy non-imported API dependencies from the old list to the new list.
    foreach (ApiDependency const& dependency, oldApiDependencies)
    {
        if (!dependency.isImported())
        {
            apiDependencies.append(dependency);
        }
    }

    sysDesign.setComponentInstances(hwInstances);
    sysDesign.setSWInstances(swInstances);
    sysDesign.setApiDependencies(apiDependencies);
}
