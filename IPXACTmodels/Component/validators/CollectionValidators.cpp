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
    //checkChildren(CollectionValidators::itemListToNameGroupList(memoryMaps));

    auto memoryMapsAsNameGroup = CollectionValidators::itemListToNameGroupList(memoryMaps);
    QSharedPointer<ValidationData> memoryMapsValidationData(new ValidationData());
    memoryMapsValidationData->children_ = memoryMapsAsNameGroup;
    checkChildren(memoryMapsValidationData);

    for (auto const& child : *memoryMaps)
    {
        if (!memMapValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}
