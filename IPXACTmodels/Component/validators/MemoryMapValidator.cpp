//-----------------------------------------------------------------------------
// File: MemoryMapValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Validator for the ipxact:memoryMap.
//-----------------------------------------------------------------------------

#include "MemoryMapValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ModeReference.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::MemoryMapValidator()
//-----------------------------------------------------------------------------
MemoryMapValidator::MemoryMapValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<AddressBlockValidator> addressBlockValidator,
    QSharedPointer<SubspaceMapValidator> subspaceValidator, QSharedPointer<Component> component):
MemoryMapBaseValidator(expressionParser, addressBlockValidator, subspaceValidator),
availableRemapStates_()
{
    componentChange(component);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::componentChange()
//-----------------------------------------------------------------------------
void MemoryMapValidator::componentChange(QSharedPointer<Component> newComponent)
{
    if (availableRemapStates_)
    {
        availableRemapStates_.clear();
    }

    if (newComponent)
    {
        availableRemapStates_ = newComponent->getRemapStates();
    }

    MemoryMapBaseValidator::componentChange(newComponent);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::validate(QSharedPointer<MemoryMap> memoryMap) const
{
    if (docRevision_ == Document::Revision::Std14)
    {
        return MemoryMapBaseValidator::validate(memoryMap, memoryMap->getAddressUnitBits()) &&
            hasValidAddressUnitBits(memoryMap) && hasValidMemoryRemaps(memoryMap);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        return MemoryMapBaseValidator::validate(memoryMap, memoryMap->getAddressUnitBits()) &&
            hasValidAddressUnitBits(memoryMap) && hasValidMemoryRemaps(memoryMap) && hasValidStructure(memoryMap);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::hasValidAddressUnitBits()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::hasValidAddressUnitBits(QSharedPointer<MemoryMap> memoryMap) const
{
    if (!memoryMap->getAddressUnitBits().isEmpty())
    {
        QString solvedValue = getExpressionParser()->parseExpression(memoryMap->getAddressUnitBits());
        bool changeOk = true;
        quint64 addressUnitBits = solvedValue.toULongLong(&changeOk);

        if (!changeOk || addressUnitBits <= 0)
        {
            return false;
        }
    }
    else if (!memoryMap->getMemoryBlocks()->isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::hasValidMemoryRemaps()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::hasValidMemoryRemaps(QSharedPointer<MemoryMap> memoryMap) const
{
    if (!memoryMap->getMemoryRemaps()->isEmpty())
    {
        QStringList remapNames;
        QStringList remapStates;
        for (QSharedPointer<MemoryRemap> memoryRemap : *memoryMap->getMemoryRemaps())
        {
            if (docRevision_ == Document::Revision::Std14)
            {
                if (remapNames.contains(memoryRemap->name()) ||
                    remapStates.contains(memoryRemap->getRemapState()) ||
                    remapStateIsNotValid(memoryRemap) ||
                    !MemoryMapBaseValidator::validate(memoryRemap, memoryMap->getAddressUnitBits()))
                {
                    return false;
                }
            }
            else if (docRevision_ == Document::Revision::Std22)
            {
                if (remapNames.contains(memoryRemap->name()) ||
                    !remapsHaveValidModeRefs(memoryMap) ||
                    !remapHasValidStructure(memoryRemap) ||
                    !MemoryMapBaseValidator::validate(memoryRemap, memoryMap->getAddressUnitBits()))
                {
                    return false;
                }
            }

            remapNames.append(memoryRemap->name());
            remapStates.append(memoryRemap->getRemapState());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::remapStateIsNotValid()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::remapStateIsNotValid(QSharedPointer<MemoryRemap> memoryRemap) const
{
    if (!memoryRemap->getRemapState().isEmpty())
    {
        for (QSharedPointer<RemapState> state : *availableRemapStates_)
        {
            if (state->name() == memoryRemap->getRemapState())
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::remapHasValidStructure()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::remapHasValidStructure(QSharedPointer<MemoryRemap> memoryRemap) const
{
    if (!memoryRemap->getMemoryRemapDefinitionReference().isEmpty() && !memoryRemap->getMemoryBlocks()->isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::hasValidStructure()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::hasValidStructure(QSharedPointer<MemoryMap> memoryMap) const
{
    if (!memoryMap->getMemoryMapDefinitionReference().isEmpty() && (
        !memoryMap->getMemoryBlocks()->isEmpty() ||
        !memoryMap->getMemoryRemaps()->isEmpty() ||
        !memoryMap->getAddressUnitBits().isEmpty() ||
        !memoryMap->getShared().isEmpty()))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void MemoryMapValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
    QString const& context) const
{
    QString memoryMapContext = memoryMap->elementName();
    memoryMapContext.append(QLatin1Char(' '));
    memoryMapContext.append(memoryMap->name());

    MemoryMapBaseValidator::findErrorsIn(errors, memoryMap, memoryMap->getAddressUnitBits(), context);
    findErrorsInAddressUnitBits(errors, memoryMap, context);
    findErrorsInMemoryRemaps(errors, memoryMap, memoryMapContext);
    if (docRevision_ == Document::Revision::Std22)
    {
        findErrorsInStrucutre(errors, memoryMap, memoryMapContext);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::remapsHaveValidModeRefs()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::remapsHaveValidModeRefs(QSharedPointer<MemoryMap> memoryMap) const
{
    if (docRevision_ != Document::Revision::Std22)
    {
        return true;
    }

    QStringList references; // References must be unique within the containing memoryMap

    for (auto const& remap : *memoryMap->getMemoryRemaps())
    {
        QStringList priorities; // Must be unique within remap.

        for (auto const& ref : *remap->getModeReferences())
        {
            if (references.contains(ref->getReference()) ||
                priorities.contains(ref->getPriority()) ||
                ref->getReference().isEmpty() ||
                ref->getPriority().isEmpty())
            {
                return false;
            }

            references.append(ref->getReference());
            priorities.append(ref->getPriority());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::findErrorsInRemapModeRefs()
//-----------------------------------------------------------------------------
void MemoryMapValidator::findErrorsInRemapModeRefs(QStringList& errors, QSharedPointer<MemoryMap> memoryMap, 
    QString const& context) const
{
    if (docRevision_ != Document::Revision::Std22)
    {
        return;
    }

    QStringList references; // References must be unique within the containing memoryMap

    for (auto const& remap : *memoryMap->getMemoryRemaps())
    {
        QStringList priorities; // Must be unique within remap.

        for (auto const& ref : *remap->getModeReferences())
        {
            QString const& referenceValue = ref->getReference();
            QString const& referencePriority = ref->getPriority();

            if (referenceValue.isEmpty())
            {
                errors.append(QObject::tr("Empty mode reference value set for memory remap %1 in %2").arg(remap->name()).arg(context));
            }

            if (referencePriority.isEmpty())
            {
                errors.append(QObject::tr("Empty mode reference priority set for memory remap %1 in %2").arg(remap->name()).arg(context));
            }

            if (references.contains(referenceValue))
            {
                errors.append(QObject::tr("Duplicate mode reference value %1 set for memory remap %2 in %3")
                    .arg(referenceValue).arg(remap->name()).arg(context));
            }
            else
            {
                references.append(referenceValue);
            }

            if (priorities.contains(referencePriority))
            {
                errors.append(QObject::tr("Duplicate mode reference priority set for memory remap %1 in %2")
                    .arg(remap->name()).arg(context));
            }
            else
            {
                priorities.append(referencePriority);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::findErrorsInStrucutre()
//-----------------------------------------------------------------------------
void MemoryMapValidator::findErrorsInStrucutre(QStringList& errors, QSharedPointer<MemoryMap> memoryMap, QString const& context) const
{
    if (!hasValidStructure(memoryMap))
    {
        errors.append(QObject::tr("Memory map %1 in %2 cannot contain both a definition reference"
            " and memory blocks, remaps or definitions for address unit bits or shared values.").arg(memoryMap->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::findErrorsInAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryMapValidator::findErrorsInAddressUnitBits(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
    QString const& context) const
{
    if (!hasValidAddressUnitBits(memoryMap))
    {
        errors.append(QObject::tr("Invalid address unit bits specified for %1 %2 within %3")
            .arg(memoryMap->elementName()).arg(memoryMap->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::findErrorsInMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMapValidator::findErrorsInMemoryRemaps(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
    QString const& context) const
{
    if (!memoryMap->getMemoryRemaps()->isEmpty())
    {
        QStringList memoryRemapNames;
        QStringList nonUniqueNames;
        QStringList remapStates;
        bool remapStatesAreUnique = true;
        for (QSharedPointer<MemoryRemap> memoryRemap : *memoryMap->getMemoryRemaps())
        {
            MemoryMapBaseValidator::findErrorsIn(errors, memoryRemap, memoryMap->getAddressUnitBits(), context);

            if (memoryRemapNames.contains(memoryRemap->name()) && !nonUniqueNames.contains(memoryRemap->name()))
            {
                nonUniqueNames.append(memoryRemap->name());
                errors.append(QObject::tr("Name %1 of memory remaps in memory map %2 is not unique.")
                    .arg(memoryRemap->name()).arg(memoryMap->name()));
            }
            else
            {
                memoryRemapNames.append(memoryRemap->name());
            }

            if (docRevision_ == Document::Revision::Std14)
            {
                if (remapStateIsNotValid(memoryRemap))
                {
                    errors.append(QObject::tr("Invalid remap state %1 set for %2 %3 within %4")
                        .arg(memoryRemap->getRemapState()).arg(memoryRemap->elementName()).arg(memoryRemap->name())
                        .arg(context));
                }
                if (remapStatesAreUnique && remapStates.contains(memoryRemap->getRemapState()))
                {
                    remapStatesAreUnique = false;
                    errors.append(QObject::tr("Remap states are not unique for each memory remap within memory map %1")
                        .arg(memoryMap->name()));
                }
                else if (remapStatesAreUnique)
                {
                    remapStates.append(memoryRemap->getRemapState());
                }
            }
            else if (docRevision_ == Document::Revision::Std22)
            {
                findErrorsInRemapStructure(errors, memoryRemap, context);
            }
        }

        findErrorsInRemapModeRefs(errors, memoryMap, context);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::findErrorsInRemapStructure()
//-----------------------------------------------------------------------------
void MemoryMapValidator::findErrorsInRemapStructure(QStringList& errors, QSharedPointer<MemoryRemap> memoryRemap, QString const& context) const
{
    if (!remapHasValidStructure(memoryRemap))
    {
        errors.append(QObject::tr("Memory remap %1 in %2 cannot contain both a definition reference" 
            " and address blocks / subspace maps.").arg(memoryRemap->name()).arg(context));
    }
}
