//-----------------------------------------------------------------------------
// File: CollectionValidators.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 02.08.2024
//
// Description:
// Validators for validating top-level items inside a component.
//-----------------------------------------------------------------------------

#include "CollectionValidators.h"

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>

#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapsValidator::MemoryMapsValidator()
//-----------------------------------------------------------------------------
MemoryMapsValidator::MemoryMapsValidator(QSharedPointer<MemoryMapValidator> mapValidator) : 
    HierarchicalValidator(),
    memMapValidator_(mapValidator)
{
    setChildValidator(mapValidator);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryMapsValidator::validate(MapList memoryMaps)
{
    if (auto memoryMapsAsNameGroup = CollectionValidators::itemListToNameGroupList(memoryMaps); 
        !childrenHaveUniqueNames(memoryMapsAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *memoryMaps)
    {
        if (!memMapValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetsValidator::FileSetsValidator()
//-----------------------------------------------------------------------------
FileSetsValidator::FileSetsValidator(QSharedPointer<FileSetValidator> fileSetValidator):
    HierarchicalValidator(),
    fileSetValidator_(fileSetValidator)
{
    setChildValidator(fileSetValidator);
}

//-----------------------------------------------------------------------------
// Function: FileSetsValidator::validate()
//-----------------------------------------------------------------------------
bool FileSetsValidator::validate(FileSetList fileSets)
{
    if (auto fileSetsAsNameGroup = CollectionValidators::itemListToNameGroupList(fileSets);
        !childrenHaveUniqueNames(fileSetsAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *fileSets)
    {
        if (!fileSetValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesValidator::AddressSpacesValidator()
//-----------------------------------------------------------------------------
AddressSpacesValidator::AddressSpacesValidator(QSharedPointer<AddressSpaceValidator> addressSpaceValidator):
    HierarchicalValidator(),
    addressSpaceValidator_(addressSpaceValidator)
{
    setChildValidator(addressSpaceValidator);
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesValidator::validate()
//-----------------------------------------------------------------------------
bool AddressSpacesValidator::validate(AddressSpaceList addressSpaces)
{
    if (auto addressSpacesAsNameGroup = CollectionValidators::itemListToNameGroupList(addressSpaces);
        !childrenHaveUniqueNames(addressSpacesAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *addressSpaces)
    {
        if (!addressSpaceValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::AllInstantiationsValidator()
//-----------------------------------------------------------------------------
AllInstantiationsValidator::AllInstantiationsValidator(QSharedPointer<InstantiationsValidator> singleInstantiationValidator):
    HierarchicalValidator(),
    singleInstantiationValidator_(singleInstantiationValidator)
{
    setChildValidator(singleInstantiationValidator);
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::validate()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::validate(QSharedPointer<Component> component)
{
    auto instantiationsAsNameGroups = CollectionValidators::itemListToNameGroupList(component->getComponentInstantiations());
    instantiationsAsNameGroups->append(*CollectionValidators::itemListToNameGroupList(component->getDesignInstantiations()));
    instantiationsAsNameGroups->append(*CollectionValidators::itemListToNameGroupList(component->getDesignConfigurationInstantiations()));
    if (!childrenHaveUniqueNames(instantiationsAsNameGroups))
    {
        return false;
    }
    
    if (!hasValidComponentInstantiations(component) || !hasValidDesignConfigurationInstantiations(component) ||
        !hasValidDesignInstantiations(component))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::hasValidComponentInstantiations()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::hasValidComponentInstantiations(QSharedPointer<Component> component) const
{
    for (auto const& componentInstantiation : *component->getComponentInstantiations())
    {
        if (!singleInstantiationValidator_->validateComponentInstantiation(componentInstantiation))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::hasValidDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::hasValidDesignConfigurationInstantiations(QSharedPointer<Component> component) const
{
    for (auto const& designConfigInstantiation : *component->getDesignConfigurationInstantiations())
    {
        if (!singleInstantiationValidator_->validateDesignConfigurationInstantiation(designConfigInstantiation))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::hasValidDesignInstantiations()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::hasValidDesignInstantiations(QSharedPointer<Component> component) const
{
    for (auto const& designInstantiation : *component->getDesignInstantiations())
    {
        if (!singleInstantiationValidator_->validateDesignInstantiation(designInstantiation))
        {
            return false;
        }
    }

    return true;
}
