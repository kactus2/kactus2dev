//-----------------------------------------------------------------------------
// File: ComponentInstanceInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.09.2021
//
// Description:
// Interface for accessing component instances.
//-----------------------------------------------------------------------------

#include <ComponentInstanceInterface.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <InterconnectionInterface.h>
#include <AdHocConnectionInterface.h>

namespace
{
    QString const DEFAULT_NAME = QLatin1String("component_instance");
};

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::ComponentInstanceInterface()
//-----------------------------------------------------------------------------
ComponentInstanceInterface::ComponentInstanceInterface(InterconnectionInterface* connectionInterface,
    AdHocConnectionInterface* adHocInterface):
NameGroupInterface(),
componentInstances_(),
connectionInterface_(connectionInterface),
adHocConnectionInterface_(adHocInterface)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::setComponentInstances()
//-----------------------------------------------------------------------------
void ComponentInstanceInterface::setComponentInstances(QSharedPointer<Design> newDesign)
{
    componentInstances_ = newDesign->getComponentInstances();

    connectionInterface_->setInterconnections(newDesign);
    adHocConnectionInterface_->setConnections(newDesign);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::getComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> ComponentInstanceInterface::getComponentInstance(std::string const& instanceName)
const
{
    for (auto instance : *componentInstances_)
    {
        if (instance->getInstanceName() == instanceName)
        {
            return instance;
        }
    }

    return QSharedPointer<ComponentInstance>();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::getItemIndex()
//-----------------------------------------------------------------------------
int ComponentInstanceInterface::getItemIndex(std::string const& itemName) const
{
    for (int i = 0; i < componentInstances_->size(); ++i)
    {
        if (componentInstances_->at(i)->getInstanceName() == itemName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string ComponentInstanceInterface::getIndexedItemName(int const& itemIndex) const
{
    if (itemIndex >= 0 && itemIndex < itemCount())
    {
        return componentInstances_->at(itemIndex)->getInstanceName();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::itemCount()
//-----------------------------------------------------------------------------
int ComponentInstanceInterface::itemCount() const
{
    return componentInstances_->count();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> ComponentInstanceInterface::getItemNames() const
{
    std::vector<std::string> instanceNames;
    for (auto const& instance : *componentInstances_)
    {
        instanceNames.push_back(instance->getInstanceName());
    }

    return instanceNames;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::setName()
//-----------------------------------------------------------------------------
bool ComponentInstanceInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<ComponentInstance> editedInstance = getComponentInstance(currentName);
    if (editedInstance && nameHasChanged(newName, currentName))
    {
        auto uniqueNewName = getUniqueName(newName, DEFAULT_NAME.toStdString());

        editedInstance->setInstanceName(uniqueNewName);

        connectionInterface_->renameComponentReferences(currentName, newName);
        adHocConnectionInterface_->renameComponentReferences(currentName, newName);

        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::getDescription()
//-----------------------------------------------------------------------------
std::string ComponentInstanceInterface::getDescription(std::string const& itemName) const
{
    QSharedPointer<ComponentInstance> editedInstance = getComponentInstance(itemName);
    if (editedInstance)
    {
        return editedInstance->description().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::setDescription()
//-----------------------------------------------------------------------------
bool ComponentInstanceInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<ComponentInstance> editedInstance = getComponentInstance(itemName);
    if (!editedInstance)
    {
        return false;
    }

    editedInstance->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::validateItems()
//-----------------------------------------------------------------------------
bool ComponentInstanceInterface::validateItems() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool ComponentInstanceInterface::itemHasValidName(std::string const& /*itemName*/) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::getComponentReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> ComponentInstanceInterface::getComponentReference(
    std::string const& instanceName) const
{
    QSharedPointer<ComponentInstance> selectedInstance = getComponentInstance(instanceName);
    if (selectedInstance)
    {
        return selectedInstance->getComponentRef();
    }

    return QSharedPointer<ConfigurableVLNVReference>();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::setComponentReference()
//-----------------------------------------------------------------------------
bool ComponentInstanceInterface::setComponentReference(std::string const& instanceName,
    std::string const& newVendor, std::string const& newLibrary, std::string const& newName,
    std::string const& newVersion)
{
    QSharedPointer<ComponentInstance> editedInstance = getComponentInstance(instanceName);
    if (!editedInstance)
    {
        return false;
    }

    QSharedPointer<ConfigurableVLNVReference> newComponentVLNV(new ConfigurableVLNVReference());
    newComponentVLNV->setVendor(QString::fromStdString(newVendor));
    newComponentVLNV->setLibrary(QString::fromStdString(newLibrary));
    newComponentVLNV->setName(QString::fromStdString(newName));
    newComponentVLNV->setVersion(QString::fromStdString(newVersion));

    editedInstance->setComponentRef(newComponentVLNV);
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::addComponentInstance()
//-----------------------------------------------------------------------------
void ComponentInstanceInterface::addComponentInstance(std::string const& instanceName)
{
    QSharedPointer<ComponentInstance> newInstance(new ComponentInstance());
    newInstance->setInstanceName(getUniqueName(instanceName, DEFAULT_NAME.toStdString()));

    componentInstances_->append(newInstance);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::removeComponentInstance()
//-----------------------------------------------------------------------------
bool ComponentInstanceInterface::removeComponentInstance(std::string const& instanceName)
{
    QSharedPointer<ComponentInstance> removedItem = getComponentInstance(instanceName);
    if (!removedItem)
    {
        return false;
    }

    connectionInterface_->removeInstanceInterconnections(instanceName);
    adHocConnectionInterface_->removeInstanceAdHocConnections(instanceName);

    return componentInstances_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceInterface::instanceExists()
//-----------------------------------------------------------------------------
bool ComponentInstanceInterface::instanceExists(std::string const& instanceName) const
{
    for (auto name : getItemNames())
    {
        if (name == instanceName)
        {
            return true;
        }
    }

    return false;
}
