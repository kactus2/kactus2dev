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

//-----------------------------------------------------------------------------
// Function: MemoryMapsValidator::MemoryMapsValidator()
//-----------------------------------------------------------------------------
MemoryMapsValidator::MemoryMapsValidator(QSharedPointer<MemoryMapValidator> mapValidator) : memMapValidator_(mapValidator)
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
