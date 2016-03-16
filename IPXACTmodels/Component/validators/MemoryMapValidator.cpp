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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/RemapState.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::MemoryMapValidator()
//-----------------------------------------------------------------------------
MemoryMapValidator::MemoryMapValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<AddressBlockValidator> addressBlockValidator,
    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates):
MemoryMapBaseValidator(expressionParser, addressBlockValidator),
availableRemapStates_(remapStates)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::~MemoryMapValidator()
//-----------------------------------------------------------------------------
MemoryMapValidator::~MemoryMapValidator()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::componentChange()
//-----------------------------------------------------------------------------
void MemoryMapValidator::componentChange(QSharedPointer<QList<QSharedPointer<RemapState> > > newRemapStates)
{
    availableRemapStates_ = newRemapStates;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryMapValidator::validate(QSharedPointer<MemoryMap> memoryMap) const
{
    return MemoryMapBaseValidator::validate(memoryMap, memoryMap->getAddressUnitBits()) &&
        hasValidAddressUnitBits(memoryMap) && hasValidMemoryRemaps(memoryMap);
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
        foreach (QSharedPointer<MemoryRemap> memoryRemap, *memoryMap->getMemoryRemaps())
        {
            if (remapNames.contains(memoryRemap->name()) ||
                remapStates.contains(memoryRemap->getRemapState()) ||
                remapStateIsNotValid(memoryRemap) ||
                !MemoryMapBaseValidator::validate(memoryRemap, memoryMap->getAddressUnitBits()))
            {
                return false;
            }
            else
            {
                remapNames.append(memoryRemap->name());
                remapStates.append(memoryRemap->getRemapState());
            }
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
        foreach (QSharedPointer<RemapState> state, *availableRemapStates_)
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
// Function: MemoryMapValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void MemoryMapValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMap> memoryMap,
    QString const& context) const
{
    QString memoryMapContext = memoryMap->elementName() + " " + memoryMap->name(); 

    MemoryMapBaseValidator::findErrorsIn(errors, memoryMap, memoryMap->getAddressUnitBits(), context);
    findErrorsInAddressUnitBits(errors, memoryMap, context);
    findErrorsInMemoryRemaps(errors, memoryMap, memoryMapContext);
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
        foreach (QSharedPointer<MemoryRemap> memoryRemap, *memoryMap->getMemoryRemaps())
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
    }
}