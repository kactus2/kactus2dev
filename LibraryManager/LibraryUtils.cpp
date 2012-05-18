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

#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/component.h>
#include <models/model.h>

#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>

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
int getInstanceIndex(QList<Design::ComponentInstance> const& instances, QString const& instanceName)
{
    int index = -1;

    // Search for a match in the list.
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances[i].instanceName == instanceName)
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
void parseProgrammableElements(LibraryInterface* lh, VLNV designVLNV,
                               QList<Design::ComponentInstance>& elements)
{
    // The received type is always VLNV::DESIGN so it must be asked from the
    // library handler to make sure the type is correct.
    designVLNV.setType(lh->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        return;
    }

    QSharedPointer<Design> compDesign;
    QSharedPointer<DesignConfiguration> designConf;

    // Check if the component contains a direct reference to a design.
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
        compDesign = libComp.staticCast<Design>();
    }
    // Otherwise check if the component had reference to a design configuration.
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
        designConf = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
            compDesign = libComp.staticCast<Design>();
        }

        // If design configuration did not contain a reference to a design.
        if (!compDesign)
        {
            return;
        }
    }
    // If referenced view was not found.
    else
    {
        return;
    }

    // Go through all component instances and search for programmable elements.
    foreach (Design::ComponentInstance instance, compDesign->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(instance.componentRef);
        QSharedPointer<Component> childComp = libComp.staticCast<Component>();

		if (childComp)
		{
			// Add the component to the system design only if it is a leaf component and has a CPU
			// or an attached SW design.
			if ((!childComp->isHierarchical() && childComp->isCpu()) || childComp->findView("kts_sw_ref") != 0)
			{
                // Make sure the name is unique by prefixing it with the design's name.
                instance.instanceName = designVLNV.getName().remove(".design") + "_" + instance.instanceName;
				elements.append(instance);
			}
			else
			{
                QString view = "";

                if (designConf != 0)
                {
                    view = designConf->getActiveView(instance.instanceName);
                }

                // Otherwise parse the hierarchical components recursively.
				parseProgrammableElements(lh, childComp->getHierRef(view), elements);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: addNewInstances()
//-----------------------------------------------------------------------------
void addNewInstances(QList<Design::ComponentInstance> elements, Design& sysDesign, LibraryInterface* lh,
                     QString const& directory, QList<Design::ComponentInstance>& sysInstances)
{
    foreach (Design::ComponentInstance const& element, elements)
    {
        // Generate a VLNV.
        VLNV swVLNV(VLNV::COMPONENT, sysDesign.getVlnv()->getVendor(),
                    sysDesign.getVlnv()->getLibrary(),
                    sysDesign.getVlnv()->getName().remove(".design") + "_swmapping_" + element.instanceName + ".comp",
                    sysDesign.getVlnv()->getVersion());

        // Create the SW mapping component.
        QSharedPointer<Component> swComp(new Component(swVLNV));
        swComp->setComponentHierarchy(KactusAttribute::KTS_PRODUCT);
        swComp->setComponentFirmness(KactusAttribute::KTS_FIXED);
        swComp->setComponentImplementation(KactusAttribute::KTS_SW);
        swComp->setComponentSWType(KactusAttribute::KTS_SW_MAPPING);

        // Create the model parameter for mapping HW to SW.
        QSharedPointer<ModelParameter> modelParam(new ModelParameter());
        modelParam->setName("kts_hw_ref");
        modelParam->setValue(element.instanceName);

        QMap<QString, QSharedPointer<ModelParameter> > modelParameters;
        modelParameters.insert("kts_hw_ref", modelParam);

        // Create a design for the SW mapping component. Use the software mappings as a template
        // if found.
        VLNV designVLNV(VLNV::DESIGN, swVLNV.getVendor(), swVLNV.getLibrary(),
            swVLNV.getName().remove(".comp") + ".design", swVLNV.getVersion());

        QSharedPointer<Design> swDesign(new Design(designVLNV));
        
        // Parse software mappings if found.
        QSharedPointer<Component> comp = lh->getModel(element.componentRef).staticCast<Component>();
        View* swView = comp->findView("kts_sw_ref");

        if (swView != 0)
        {
            QSharedPointer<LibraryComponent> libComp = lh->getModel(swView->getHierarchyRef());
            QSharedPointer<Design> swMapping = libComp.staticCast<Design>();

            libComp = QSharedPointer<LibraryComponent>(lh->getModel(swMapping->getComponentInstances().at(0).componentRef));
            QSharedPointer<Component> swMappingComp = libComp.staticCast<Component>();
            View* swDesignView = swMappingComp->findView("kts_sw_ref");

            if (swDesignView != 0)
            {
                libComp = QSharedPointer<LibraryComponent>(lh->getModel(swDesignView->getHierarchyRef()));
                QSharedPointer<Design> softwareMap = libComp.staticCast<Design>();
                QList<Design::ComponentInstance> swInstances;

                foreach (Design::ComponentInstance instance, softwareMap->getComponentInstances())
                {
                    instance.imported = true;
                    swInstances.append(instance);
                }

                swDesign->setComponentInstances(swInstances);
                swDesign->setInterconnections(softwareMap->getInterconnections());
            }
        }

        // Parse CPUs if found.
        swComp->setAddressSpaces(comp->getAddressSpaces());
        swComp->setCpus(comp->getCpus());

        // Create the view for the design.
        View* hierView = new View("kts_sys_ref");
        hierView->setHierarchyRef(designVLNV);
        hierView->addEnvIdentifier("");

        // Initialize the model.
        Model* model = new Model();
        model->setModelParameters(modelParameters);
        model->addView(hierView);
        swComp->setModel(model);

        // Write object files.
        lh->writeModelToFile(directory, swDesign);
        lh->writeModelToFile(directory, swComp);

        //  Create the corresponding component instance and set its kts_hw_ref.
        Design::ComponentInstance swInstance(element.instanceName, element.displayName,
            QString(), swVLNV, QPointF(0, 0));
        swInstance.imported = true;
        swInstance.configurableElementValues.insert("kts_hw_ref", element.instanceName);


        // Add the newly created SW component to the list of SW instances.
        sysInstances.append(swInstance);
    }
}

//-----------------------------------------------------------------------------
// Function: generateSystemDesign()
//-----------------------------------------------------------------------------
void generateSystemDesign(LibraryInterface* lh, QString const& directory,
                          VLNV const& designVLNV, Design& sysDesign)
{
    // Parse all programmable elements from the HW component.
    QList<Design::ComponentInstance> elements;
    parseProgrammableElements(lh, designVLNV, elements);

    // Create SW mapping components for each programmable element.
    QList<Design::ComponentInstance> sysInstances;
    addNewInstances(elements, sysDesign, lh, directory, sysInstances);

    sysDesign.setComponentInstances(sysInstances);
}

//-----------------------------------------------------------------------------
// Function: updateSystemDesign()
//-----------------------------------------------------------------------------
void updateSystemDesign(LibraryInterface* lh, QString const& directory,
                        VLNV const& designVLNV, Design& sysDesign)
{
    // Parse all programmable elements from the HW design.
    QList<Design::ComponentInstance> elements;
    parseProgrammableElements(lh, designVLNV, elements);

    // Reflect changes in the programmable elements to the system design.
    QList<Design::ComponentInstance> sysInstances;

    // 1. PHASE: Check already existing elements against the new list and remove those that
    // are no longer part of the new element list.
    foreach (Design::ComponentInstance const& instance, sysDesign.getComponentInstances())
    {
        // Imported ones should be checked.
        if (instance.imported)
        {
            // Search for the corresponding element in the new list.
            int index = getInstanceIndex(elements, instance.configurableElementValues.value("kts_hw_ref", ""));

            // If the element was removed, it is not added to the updated design.
            if (index == -1)
            {
                continue;
            }

            sysInstances.append(instance);

            // Check for changes in the software mappings.
            Design::ComponentInstance const& element = elements[index];
            QSharedPointer<Component> comp = lh->getModel(element.componentRef).staticCast<Component>();
            View* swView = comp->findView("kts_sw_ref");

            if (swView != 0)
            {
                // Retrieve the software mappings.
                QSharedPointer<LibraryComponent> libComp = lh->getModel(swView->getHierarchyRef());
                QSharedPointer<Design> swMapping = libComp.staticCast<Design>();

                libComp = QSharedPointer<LibraryComponent>(lh->getModel(swMapping->getComponentInstances().at(0).componentRef));
                QSharedPointer<Component> swMappingComp = libComp.staticCast<Component>();

                if (swMappingComp != 0)
                {
                    View* swDesignView = swMappingComp->findView("kts_sw_ref");

                    if (swDesignView != 0)
                    {
                        libComp = QSharedPointer<LibraryComponent>(lh->getModel(swDesignView->getHierarchyRef()));
                        QSharedPointer<Design> softwareMap = libComp.staticCast<Design>();

                        QList<Design::ComponentInstance> softwareMapInstances = softwareMap->getComponentInstances();
                        QList<Design::Interconnection> softwareMapConnections = softwareMap->getInterconnections();

                        // Retrieve the old design for comparison.
                        libComp = lh->getModel(instance.componentRef);
                        swMappingComp = libComp.staticCast<Component>();
                        libComp = lh->getModel(swMappingComp->getHierRef("kts_sys_ref"));
                        QSharedPointer<Design> oldDesign = libComp.staticCast<Design>();

                        QList<Design::ComponentInstance> swInstances;
                        QList<Design::Interconnection> swConnections;

                        // Update the instance list by first checking the current design against the new software mapping.
                        foreach (Design::ComponentInstance swInstance, oldDesign->getComponentInstances())
                        {
                            // Imported ones should be checked.
                            if (swInstance.imported)
                            {
                                int swIndex = getInstanceIndex(softwareMapInstances, swInstance.instanceName);

                                // If the element was found, it is kept in the updated design.
                                if (swIndex != -1)
                                {
                                    swInstances.append(swInstance);

                                    // Remove the connection from the software map instances since it has been processed.
                                    softwareMapInstances.removeAt(swIndex);
                                }
                            }
                            else
                            {
                                // Non-imported instances are always kept.
                                swInstances.append(swInstance);
                            }
                        }

                        // Add the brand new instances at the end.
                        foreach (Design::ComponentInstance instance, softwareMapInstances)
                        {
                            instance.imported = true;
                            swInstances.append(instance);
                        }

                        // Same for the interconnections.
                        foreach (Design::Interconnection const& conn, oldDesign->getInterconnections())
                        {
                            // The interconnection is an imported one if both of its interfaces are
                            // found in the new software mapping design.
                            int index1 = getInstanceIndex(softwareMap->getComponentInstances(),
                                                          conn.interface1.componentRef);
                            int index2 = getInstanceIndex(softwareMap->getComponentInstances(),
                                                          conn.interface2.componentRef);
                            bool imported = (index1 != -1) && (index2 != -1);

                            if (imported)
                            {
                                // Keep only those imported connections that are found in the new software mapping design.
                                int connIndex = getConnectionIndex(softwareMapConnections, conn.name);

                                if (connIndex != -1)
                                {
                                    swConnections.append(conn);
                                    softwareMapConnections.removeAt(connIndex);
                                }
                            }
                            // Non-imported interconnections are kept if the interface1
                            // will still be part of the new design.
                            else if (getInstanceIndex(swInstances, conn.interface1.componentRef) != -1)
                            {
                                swConnections.append(conn);
                            }
                        }

                        // Add the brand new connections at the end.
                        foreach (Design::Interconnection const& conn, softwareMapConnections)
                        {
                            swConnections.append(conn);
                        }

                        oldDesign->setComponentInstances(swInstances);
                        oldDesign->setInterconnections(swConnections);
                        lh->writeModelToFile(oldDesign);
                    }
                }
            }

            // Update CPUs to the mapping component.
            QSharedPointer<LibraryComponent> libComp = lh->getModel(instance.componentRef);
            QSharedPointer<Component> swComp = libComp.dynamicCast<Component>();

            if (swComp != 0)
            {
                swComp->setAddressSpaces(comp->getAddressSpaces());
                swComp->setCpus(comp->getCpus());
                lh->writeModelToFile(swComp);
            }

            // Remove the element from the element list since it has been processed.
            elements.removeAt(index);
        }
        else
        {
            // Non-imported instances are always kept.
            sysInstances.append(instance);
        }
    }

    // 2. PHASE: Add rest of the elements as brand new instances.
    addNewInstances(elements, sysDesign, lh, directory, sysInstances);

    sysDesign.setComponentInstances(sysInstances);
}

//-----------------------------------------------------------------------------
// Function: parseProgrammableElements()
//-----------------------------------------------------------------------------
void parseProgrammableElementsV2(LibraryInterface* lh, VLNV designVLNV,
                                 QList<Design::ComponentInstance>& elements)
{
    // The received type is always VLNV::DESIGN so it must be asked from the
    // library handler to make sure the type is correct.
    designVLNV.setType(lh->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        return;
    }

    QSharedPointer<Design> compDesign;
    QSharedPointer<DesignConfiguration> designConf;

    // Check if the component contains a direct reference to a design.
    if (designVLNV.getType() == VLNV::DESIGN)
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
        compDesign = libComp.staticCast<Design>();
    }
    // Otherwise check if the component had reference to a design configuration.
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
        designConf = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf->getDesignRef();

        if (designVLNV.isValid())
        {
            QSharedPointer<LibraryComponent> libComp = lh->getModel(designVLNV);
            compDesign = libComp.staticCast<Design>();
        }

        // If design configuration did not contain a reference to a design.
        if (!compDesign)
        {
            return;
        }
    }
    // If referenced view was not found.
    else
    {
        return;
    }

    // Go through all component instances and search for programmable elements.
    foreach (Design::ComponentInstance instance, compDesign->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComp = lh->getModel(instance.componentRef);
        QSharedPointer<Component> childComp = libComp.staticCast<Component>();

        if (childComp)
        {
            // Add the component to the system design only if it is a leaf component and has a CPU
            // or COM interfaces.
            if ((!childComp->isHierarchical() && childComp->isCpu()) || !childComp->getComInterfaces().isEmpty())
            {
                // Make sure the name is unique by prefixing it with the design's name.
                instance.instanceName = designVLNV.getName().remove(".design") + "_" + instance.instanceName;
                elements.append(instance);
            }
            else
            {
                QString view = "";

                if (designConf != 0)
                {
                    view = designConf->getActiveView(instance.instanceName);
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
void addNewInstancesV2(QList<Design::ComponentInstance> elements, Design& sysDesign, LibraryInterface* lh,
                       QString const& directory, QList<Design::ComponentInstance>& sysInstances)
{
    foreach (Design::ComponentInstance const& element, elements)
    {
        // Duplicate the component instance and set its kts_hw_ref.
        Design::ComponentInstance instance(element.instanceName, element.displayName,
                                           QString(), element.componentRef, QPointF(0, 0));
        instance.imported = true;
        instance.configurableElementValues.insert("kts_hw_ref", element.instanceName);

        // Add the newly created SW component to the list of SW instances.
        sysInstances.append(instance);
    }
}

//-----------------------------------------------------------------------------
// Function: generateSystemDesign()
//-----------------------------------------------------------------------------
void generateSystemDesignV2(LibraryInterface* lh, QString const& directory,
                            VLNV const& designVLNV, Design& sysDesign)
{
    // Parse all programmable elements from the HW component.
    QList<Design::ComponentInstance> elements;
    parseProgrammableElements(lh, designVLNV, elements);

    // Add them as instances to the system design.
    QList<Design::ComponentInstance> sysInstances;
    addNewInstances(elements, sysDesign, lh, directory, sysInstances);

    sysDesign.setComponentInstances(sysInstances);
}

//-----------------------------------------------------------------------------
// Function: updateSystemDesign()
//-----------------------------------------------------------------------------
void updateSystemDesignV2(LibraryInterface* lh, QString const& directory,
                          VLNV const& designVLNV, Design& sysDesign)
{
    // Parse all programmable elements from the HW design.
    QList<Design::ComponentInstance> elements;
    parseProgrammableElements(lh, designVLNV, elements);

    // Reflect changes in the programmable elements to the system design.
    QList<Design::ComponentInstance> sysInstances;

    // 1. PHASE: Check already existing elements against the new list and remove those that
    // are no longer part of the new element list.
    foreach (Design::ComponentInstance const& instance, sysDesign.getComponentInstances())
    {
        // Imported ones should be checked.
        if (instance.imported)
        {
            // Search for the corresponding element in the new list.
            int index = getInstanceIndex(elements, instance.configurableElementValues.value("kts_hw_ref", ""));

            // If the element was removed, it is not added to the updated design.
            if (index == -1)
            {
                continue;
            }

            Design::ComponentInstance instanceCopy = instance;
            instanceCopy.componentRef = elements[index].componentRef;
            sysInstances.append(instanceCopy);

            // Remove the element from the element list since it has been processed.
            elements.removeAt(index);
        }
        else
        {
            // Non-imported instances are always kept.
            sysInstances.append(instance);
        }
    }

    // 2. PHASE: Add rest of the elements as brand new instances.
    addNewInstancesV2(elements, sysDesign, lh, directory, sysInstances);

    sysDesign.setComponentInstances(sysInstances);
}
