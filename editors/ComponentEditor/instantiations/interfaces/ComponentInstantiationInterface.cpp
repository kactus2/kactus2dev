//-----------------------------------------------------------------------------
// File: ComponentInstantiationInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.09.2020
//
// Description:
// Interface for editing component instantiations.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationInterface.h"

#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

#include <editors/ComponentEditor/parameters/ParametersInterface.h>
#include <editors/ComponentEditor/instantiations/interfaces/ModuleParameterInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileBuilderInterface.h>

namespace
{
    std::string const COMPONENT_INSTANTIATION_TYPE = "instantiation";
    QString const USERFILETYPE = "user";
};

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::ComponentInstantiationInterface()
//-----------------------------------------------------------------------------
ComponentInstantiationInterface::ComponentInstantiationInterface(QSharedPointer<InstantiationsValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    ParametersInterface* parameterInterface, ModuleParameterInterface* moduleParameterInterface,
    FileBuilderInterface* fileBuilderInterface, FileSetInterface* fileSetInterface):
ParameterizableInterface(expressionParser, expressionFormatter),
instantiations_(),
validator_(validator),
parameterInterface_(parameterInterface),
moduleParameterInterface_(moduleParameterInterface),
fileBuilderInterface_(fileBuilderInterface),
fileSetInterface_(fileSetInterface)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setAddressBlocks()
//-----------------------------------------------------------------------------
void ComponentInstantiationInterface::setComponentInstantiations(
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation>>> newInstantiations)
{
    instantiations_ = newInstantiations;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getItemIndex()
//-----------------------------------------------------------------------------
int ComponentInstantiationInterface::getItemIndex(std::string const& itemName) const
{
    for (int i = 0; i < instantiations_->size(); ++i)
    {
        if (instantiations_->at(i)->name().toStdString() == itemName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string instantiationName = "";
    if (itemIndex >= 0 && itemIndex < instantiations_->size())
    {
        instantiationName = instantiations_->at(itemIndex)->name().toStdString();
    }

    return instantiationName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::itemCount()
//-----------------------------------------------------------------------------
int ComponentInstantiationInterface::itemCount() const
{
    return instantiations_->count();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> ComponentInstantiationInterface::getItemNames() const
{
    std::vector<std::string> names;
    for (auto instantiation : *instantiations_)
    {
        names.push_back(instantiation->name().toStdString());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(currentName);
    if (!editedItem)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newName, COMPONENT_INSTANTIATION_TYPE);

    editedItem->setName(uniqueNewName);
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getDisplayName()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getDisplayName(std::string const& itemName) const
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(itemName);
    if (editedItem)
    {
        return editedItem->displayName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setDisplayName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setDisplayName(std::string const& itemName,
    std::string const& newDisplayName)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(itemName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setDisplayName(QString::fromStdString(newDisplayName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getDescription()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getDescription(std::string const& itemName) const
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(itemName);
    if (editedItem)
    {
        return editedItem->description().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setDescription()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setDescription(std::string const& itemName,
    std::string const& newDescription)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(itemName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int ComponentInstantiationInterface::getAllReferencesToIdInItem(
    const std::string& itemName, std::string const&  valueID) const
{
    int totalReferencesToParameter = 0;

    QSharedPointer<ComponentInstantiation> selectedItem = getComponentInstantiation(itemName);
    if (selectedItem)
    {
        QString idString = QString::fromStdString(valueID);

        int referencesInParameters = 0;
        parameterInterface_->setParameters(selectedItem->getParameters());
        for (auto parameter : *selectedItem->getParameters())
        {
            referencesInParameters +=
                parameterInterface_->getAllReferencesToIdInItem(parameter->name().toStdString(), valueID);
        }

        int referencesInModuleParameters = 0;
        moduleParameterInterface_->setModuleParameters(selectedItem);
        for (auto parameter : *selectedItem->getModuleParameters())
        {
            referencesInModuleParameters +=
                parameterInterface_->getAllReferencesToIdInItem(parameter->name().toStdString(), valueID);
        }

        int referencesInFileBuilders = 0;
        fileBuilderInterface_->setFileBuilders(selectedItem->getDefaultFileBuilders());
        for (auto builder : *selectedItem->getDefaultFileBuilders())
        {
            QString builderID = builder->getFileType();
            if (builderID == USERFILETYPE)
            {
                builderID = builder->getUserFileType();
            }

            referencesInFileBuilders +=
                fileBuilderInterface_->getAllReferencesToIdInItem(builderID.toStdString(), valueID);
        }


        totalReferencesToParameter =
            referencesInParameters + referencesInModuleParameters + referencesInFileBuilders;
    }

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getExpressionsInSelectedItems()
//-----------------------------------------------------------------------------
std::vector<std::string> ComponentInstantiationInterface::getExpressionsInSelectedItems(
    std::vector<std::string> instantiationNames) const
{
    std::vector<std::string> expressionList;

    for (auto name : instantiationNames)
    {
        QSharedPointer<ComponentInstantiation> selectedItem = getComponentInstantiation(name);
        if (selectedItem)
        {
            parameterInterface_->setParameters(selectedItem->getParameters());
            for (auto expression :
                parameterInterface_->getExpressionsInSelectedItems(parameterInterface_->getItemNames()))
            {
                expressionList.push_back(expression);
            }

            moduleParameterInterface_->setModuleParameters(selectedItem);
            for (auto expression : moduleParameterInterface_->getExpressionsInSelectedItems(
                moduleParameterInterface_->getItemNames()))
            {
                expressionList.push_back(expression);
            }

            fileBuilderInterface_->setFileBuilders(selectedItem->getDefaultFileBuilders());
            for (auto expression :
                fileBuilderInterface_->getExpressionsInSelectedFileBuilders(fileBuilderInterface_->getItemNames()))
            {
                expressionList.push_back(expression);
            }
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::validateItems()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::validateItems() const
{
    for (auto currentItem : *instantiations_)
    {
        if (!validator_->validateComponentInstantiation(currentItem))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::itemHasValidName(std::string const& itemName) const
{
    return validator_->hasValidName(QString::fromStdString(itemName));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> ComponentInstantiationInterface::getComponentInstantiation(
    std::string const& itemName) const
{
    for (auto instantiation : *instantiations_)
    {
        if (instantiation->name().toStdString() == itemName)
        {
            return instantiation;
        }
    }

    return QSharedPointer<ComponentInstantiation>();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::addComponentInstantiation()
//-----------------------------------------------------------------------------
void ComponentInstantiationInterface::addComponentInstantiation(int const& row,
    std::string const& newInstantiationName)
{
    QSharedPointer<ComponentInstantiation> newInstantiation(new ComponentInstantiation());
    newInstantiation->setName(getUniqueName(newInstantiationName, COMPONENT_INSTANTIATION_TYPE));

    instantiations_->insert(row, newInstantiation);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::removeComponentInstantiation()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::removeComponentInstantiation(std::string const& instantiationName)
{
    QSharedPointer<ComponentInstantiation> removedItem = getComponentInstantiation(instantiationName);
    if (!removedItem)
    {
        return false;
    }

    return instantiations_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getParameterInterface()
//-----------------------------------------------------------------------------
ParametersInterface* ComponentInstantiationInterface::getParameterInterface() const
{
    return parameterInterface_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getModuleParameterInterface()
//-----------------------------------------------------------------------------
ModuleParameterInterface* ComponentInstantiationInterface::getModuleParameterInterface() const
{
    return moduleParameterInterface_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getFileBuilderInterface()
//-----------------------------------------------------------------------------
FileBuilderInterface* ComponentInstantiationInterface::getFileBuilderInterface() const
{
    return fileBuilderInterface_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getFileSetInterface()
//-----------------------------------------------------------------------------
FileSetInterface* ComponentInstantiationInterface::getFileSetInterface() const
{
    return fileSetInterface_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getLanguage()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getLanguage(std::string const& instantiationName) const
{
    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        return editedInstantiation->getLanguage().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setLanguage()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setLanguage(std::string const& instantiationName,
    std::string const& newLanguage)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setLanguage(QString::fromStdString(newLanguage));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::isLanguageStrict()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::isLanguageStrict(std::string const& instantiationName) const
{
    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        return editedInstantiation->isLanguageStrict();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setLanguageStrictness()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setLanguageStrictness(std::string const& instantiationName,
    bool newStrictness)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setLanguageStrictness(newStrictness);
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getLibraryName()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getLibraryName(std::string const& instantiationName) const
{
    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        return editedInstantiation->getLibraryName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setLibraryName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setLibraryName(std::string const& instantiationName,
    std::string const& newLibraryName)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setLibraryName(QString::fromStdString(newLibraryName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getPackageName()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getPackageName(std::string const& instantiationName) const
{
    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        return editedInstantiation->getPackageName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setPackageName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setPackageName(std::string const& instantiationName,
    std::string const& newPackageName)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setPackageName(QString::fromStdString(newPackageName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getModuleName()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getModuleName(std::string const& instantiationName) const
{
    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        return editedInstantiation->getModuleName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setModuleName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setModuleName(std::string const& instantiationName,
    std::string const& newModuleName)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setModuleName(QString::fromStdString(newModuleName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getArchitectureName()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getArchitectureName(std::string const& instantiationName) const
{
    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        return editedInstantiation->getArchitectureName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setArchitectureName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setArchitectureName(std::string const& instantiationName,
    std::string const& newArchitectureName)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setArchitectureName(QString::fromStdString(newArchitectureName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getConfigurationName()
//-----------------------------------------------------------------------------
std::string ComponentInstantiationInterface::getConfigurationName(std::string const& instantiationName) const
{
    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        return editedInstantiation->getConfigurationName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setConfigurationName()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setConfigurationName(std::string const& instantiationName,
    std::string const& newConfigurationName)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setConfigurationName(QString::fromStdString(newConfigurationName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::getFileSetReferences()
//-----------------------------------------------------------------------------
std::vector<std::string> ComponentInstantiationInterface::getFileSetReferences(
    std::string const& instantiationName) const
{
    std::vector<std::string> fileSetReferences;

    QSharedPointer<ComponentInstantiation> editedInstantiation = getComponentInstantiation(instantiationName);
    if (editedInstantiation)
    {
        for (auto fileReference : *editedInstantiation->getFileSetReferences())
        {
            fileSetReferences.push_back(fileReference.toStdString());
        }
    }

    return fileSetReferences;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationInterface::setFileSetReferences()
//-----------------------------------------------------------------------------
bool ComponentInstantiationInterface::setFileSetReferences(std::string const& instantiationName,
    std::vector<std::string> const& newReferences)
{
    QSharedPointer<ComponentInstantiation> editedItem = getComponentInstantiation(instantiationName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->getFileSetReferences()->clear();
    for (auto setReference : newReferences)
    {
        editedItem->getFileSetReferences()->append(QString::fromStdString(setReference));
    }

    return true;
}
