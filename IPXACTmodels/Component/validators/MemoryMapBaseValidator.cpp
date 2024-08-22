//-----------------------------------------------------------------------------
// File: MemoryMapBaseValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Validator for the memoryMap base class.
//-----------------------------------------------------------------------------

#include "MemoryMapBaseValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>

#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::MemoryMapBaseValidator()
//-----------------------------------------------------------------------------
MemoryMapBaseValidator::MemoryMapBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<AddressBlockValidator> addressBlockValidator,
    QSharedPointer<SubspaceMapValidator> subspaceValidator,
    Document::Revision docRevision):
HierarchicalValidator(),
docRevision_(docRevision),
expressionParser_(expressionParser),
addressBlockValidator_(addressBlockValidator),
subspaceValidator_(subspaceValidator),
availableBusInterfaces_(),
availableSpaces_()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::componentChange()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::componentChange(QSharedPointer<Component> newComponent)
{
    addressBlockValidator_->componentChange(newComponent);
    subspaceValidator_->componentChange(newComponent);

    availableBusInterfaces_.clear();
    availableSpaces_.clear();

    if (newComponent)
    {
        availableBusInterfaces_ = newComponent->getBusInterfaces();
        availableSpaces_ = newComponent->getAddressSpaces();
        docRevision_ = newComponent->getRevision();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::getAddressBlockValidator()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlockValidator> MemoryMapBaseValidator::getAddressBlockValidator() const
{
    return addressBlockValidator_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::getSubspaceValidator()
//-----------------------------------------------------------------------------
QSharedPointer<SubspaceMapValidator> MemoryMapBaseValidator::getSubspaceValidator() const
{
    return subspaceValidator_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::validate(QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits)
{
    return hasValidName(memoryMapBase) && hasValidIsPresent(memoryMapBase) && 
        hasValidMemoryBlocks(memoryMapBase, addressUnitBits) && validComparedToSiblings(memoryMapBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::markBlocksWithDuplicateNames()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::markBlocksWithDuplicateNames(QMultiHash<QString, QSharedPointer<NameGroup>> const& foundNames)
{
    bool errorFound = false;
    for (auto const& name : foundNames.keys())
    {
        if (auto const& duplicateNames = foundNames.values(name);
            duplicateNames.count() > 1)
        {
            std::for_each(duplicateNames.begin(), duplicateNames.end(),
                [this](QSharedPointer<NameGroup> memoryBlock)
                {
                    if (auto asAddressBlock = memoryBlock.dynamicCast<AddressBlock>())
                    {
                        addressBlockValidator_->setChildItemValidity(asAddressBlock, false);
                    }
                    else if (auto asSubspace = memoryBlock.dynamicCast<SubSpaceMap>())
                    {
                        subspaceValidator_->setChildItemValidity(asSubspace, false);
                    }
                });

            errorFound = true;
        }
    }

    return errorFound;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::hasValidName()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::hasValidName(QSharedPointer<MemoryMapBase> memoryMapBase) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(memoryMapBase->name());

    if (memoryMapBase->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::hasValidIsPresent(QSharedPointer<MemoryMapBase> memoryMapBase) const
{
    if (!memoryMapBase->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(memoryMapBase->getIsPresent());

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::hasValidMemoryBlocks()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::hasValidMemoryBlocks(QSharedPointer<MemoryMapBase> memoryMapBase,
    QString const& addressUnitBits)
{
    if (memoryMapBase->getMemoryBlocks()->isEmpty())
    {
        return true;
    }
    
    bool errorFound = false;
    
    // Address block and subspace map names should be unique, subspace can't have the same name as an address block.
    QMultiHash<QString, QSharedPointer<NameGroup> > foundMemoryBlockNames;

    auto childBlocksCopy = QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > >(new QList(*memoryMapBase->getMemoryBlocks()));

    // Sort blocks by base address to make checking for overlaps easier.
    auto sortByBaseAddr = [this](QSharedPointer<MemoryBlockBase> memoryBlockA, QSharedPointer<MemoryBlockBase> memoryBlockB)
        {
            return expressionParser_->parseExpression(memoryBlockA->getBaseAddress()).toLongLong() <
                expressionParser_->parseExpression(memoryBlockB->getBaseAddress()).toLongLong();
        };

    std::sort(childBlocksCopy->begin(), childBlocksCopy->end(), sortByBaseAddr);

    quint64 lastEnd = 0;
    bool lastBlockWasAddressBlock = false;

    // Validate blocks together
    for (int blockIndex = 0; blockIndex < childBlocksCopy->size(); ++blockIndex)
    {
        auto child = childBlocksCopy->at(blockIndex).staticCast<MemoryBlockBase>();
        bool currentIsAddressBlock = false;

        // First check names, but don't set invalidity yet.
        if (auto addressBlock = child.dynamicCast<AddressBlock>())
        {
            if (!foundMemoryBlockNames.contains(addressBlock->name()))
            {
                addressBlockValidator_->setChildItemValidity(addressBlock, true);
            }

            foundMemoryBlockNames.insert(addressBlock->name(), addressBlock);
            currentIsAddressBlock = true;
        }
        else if (auto subspaceMap = child.dynamicCast<SubSpaceMap>())
        {
            if (!foundMemoryBlockNames.contains(subspaceMap->name()))
            {
                subspaceValidator_->setChildItemValidity(subspaceMap, true);
            }

            foundMemoryBlockNames.insert(subspaceMap->name(), subspaceMap);
        }

        // Check overlaps of the sorted blocks comparing the start of the current block to the end of 
        // the current rightmost point, last block or not.
        bool childPresent = child->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(child->getIsPresent()).toInt();

        if (blockIndex == 0 && childPresent)
        {
            quint64 blockBegin = expressionParser_->parseExpression(child->getBaseAddress()).toULongLong();
            lastEnd = blockBegin + getBlockRange(child) - 1;
        }
        else if (childPresent)
        {
            quint64 blockBegin = expressionParser_->parseExpression(child->getBaseAddress()).toULongLong();
            quint64 blockEnd = blockBegin + getBlockRange(child) - 1;

            // Mark this and the last block as invalid, if overlap.
            if (blockBegin <= lastEnd)
            {
                lastBlockWasAddressBlock
                    ? addressBlockValidator_->setChildItemValidity(childBlocksCopy->at(blockIndex - 1), false)
                    : subspaceValidator_->setChildItemValidity(childBlocksCopy->at(blockIndex - 1), false);

                currentIsAddressBlock
                    ? addressBlockValidator_->setChildItemValidity(child, false)
                    : subspaceValidator_->setChildItemValidity(child, false);
                errorFound = true;
            }

            if (blockEnd > lastEnd)
            {
                lastEnd = blockEnd;
            }
        }

        lastBlockWasAddressBlock = currentIsAddressBlock;
    }

    // Mark all blocks with duplicate names invalid.
    if (markBlocksWithDuplicateNames(foundMemoryBlockNames))
    {
        errorFound = true;
    }

    // No need to check child blocks if they are erroneous together.
    if (errorFound)
    {
        return false;
    }

    // Validate blocks separately
    for (auto const& child : *childBlocksCopy)
    {
        if (QSharedPointer<AddressBlock> addressBlock = child.dynamicCast<AddressBlock>();
            addressBlock && (!addressBlockValidator_->validate(addressBlock, addressUnitBits)
            || !addressBlockWidthIsMultipleOfAUB(addressUnitBits, addressBlock)))
        {
            return false;
        }
        else if (QSharedPointer<SubSpaceMap> subspace = child.dynamicCast<SubSpaceMap>();
            subspace && !subspaceValidator_->validate(subspace))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::addressBlockWidthIsMultipleOfAUB()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::addressBlockWidthIsMultipleOfAUB(QString const& addressUnitBits,
    QSharedPointer<AddressBlock> addressBlock) const
{
    bool aubToIntOk = true;
    bool widthToIntOk = true;

    int addressUnitBitsInt = expressionParser_->parseExpression(addressUnitBits).toInt(&aubToIntOk);
    int addressBlockWidth = expressionParser_->parseExpression(addressBlock->getWidth()).toInt(&widthToIntOk);

    bool isValid = aubToIntOk && widthToIntOk && addressUnitBitsInt != 0 && addressBlockWidth % addressUnitBitsInt == 0;
    if (!isValid)
    {
        addressBlockValidator_->setChildItemValidity(addressBlock, false);
    }

    return isValid;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::twoAddressBlocksOverlap()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::twoMemoryBlocksOverlap(QSharedPointer<MemoryBlockBase> memoryBlock,
    QSharedPointer<MemoryBlockBase> comparedBlock) const
{
    bool blockPresent = memoryBlock->getIsPresent().isEmpty() ||
        expressionParser_->parseExpression(memoryBlock->getIsPresent()).toInt();
    bool comparedPresent = comparedBlock->getIsPresent().isEmpty() ||
        expressionParser_->parseExpression(comparedBlock->getIsPresent()).toInt();

    if (blockPresent && comparedPresent)
    {
        quint64 blockBegin = expressionParser_->parseExpression(memoryBlock->getBaseAddress()).toULongLong();
        quint64 blockEnd = blockBegin + getBlockRange(memoryBlock) - 1;

        quint64 compareBegin = expressionParser_->parseExpression(comparedBlock->getBaseAddress()).toULongLong();
        quint64 compareEnd = compareBegin + getBlockRange(comparedBlock) - 1;

        if (((blockBegin >= compareBegin && blockBegin <= compareEnd) ||
            (blockEnd >= compareBegin && blockEnd <= compareEnd)) ||
            ((compareBegin >= blockBegin && compareBegin <= blockEnd) ||
            (compareEnd >= blockBegin && compareEnd <= blockEnd)))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::getBlockEnd()
//-----------------------------------------------------------------------------
quint64 MemoryMapBaseValidator::getBlockRange(QSharedPointer<MemoryBlockBase> block) const
{
    quint64 memoryRange = 0;
    QSharedPointer<AddressBlock> addressBlock = block.dynamicCast<AddressBlock>();
    if (addressBlock)
    {
        memoryRange = expressionParser_->parseExpression(addressBlock->getRange()).toULongLong();
    }

    QSharedPointer<SubSpaceMap> subspace = block.dynamicCast<SubSpaceMap>();
    if (subspace)
    {
        memoryRange = 1;

        QSharedPointer<AddressSpace> referencedSpace = getReferencedAddressSpace(subspace);
        if (referencedSpace)
        {
            QSharedPointer<Segment> referencedSegment = getReferencedSegment(subspace, referencedSpace);

            if (referencedSegment)
            {
                memoryRange = expressionParser_->parseExpression(referencedSegment->getRange()).toULongLong();
            }
            else
            {
                memoryRange = expressionParser_->parseExpression(referencedSpace->getRange()).toULongLong();
            }
        }
    }

    return memoryRange;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::getReferencedAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> MemoryMapBaseValidator::getReferencedAddressSpace(
    QSharedPointer<SubSpaceMap> subspace) const
{
    for (auto bus : *availableBusInterfaces_)
    {
        if (subspace->getInitiatorReference() == bus->name())
        {
            for (auto space : *availableSpaces_)
            {
                if (bus->getAddressSpaceRef() == space->name())
                {
                    return space;
                }
            }
        }
    }

    return QSharedPointer<AddressSpace>();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::getReferencedSegment()
//-----------------------------------------------------------------------------
QSharedPointer<Segment> MemoryMapBaseValidator::getReferencedSegment(QSharedPointer<SubSpaceMap> subspace,
    QSharedPointer<AddressSpace> containingSpace) const
{
    for (auto segment : *containingSpace->getSegments())
    {
        if (segment->name() == subspace->getSegmentReference())
        {
            return segment;
        }
    }

    return QSharedPointer<Segment>();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
    QString const& addressUnitBits, QString const& context)
{
    QString memoryMapContext = memoryMapBase->elementName();
    memoryMapContext.append(QLatin1Char(' '));
    memoryMapContext.append(memoryMapBase->name());

    findErrorsInName(errors, memoryMapBase, context);
    findErrorsInIsPresent(errors, memoryMapBase, context);
    findErrorsInAddressBlocks(errors, memoryMapBase, addressUnitBits, memoryMapContext);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& context) const
{
    if (!hasValidName(memoryMapBase))
    {
        errors.append(QObject::tr("Invalid name specified for %1 %2 within %3").arg(memoryMapBase->elementName())
            .arg(memoryMapBase->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& context) const
{
    if (docRevision_ == Document::Revision::Std14 && !hasValidIsPresent(memoryMapBase))
    {
        errors.append(QObject::tr("Invalid isPresent set for %1 %2 within %3").arg(memoryMapBase->elementName())
            .arg(memoryMapBase->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInAddressBlocks(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits, QString const& context)
{
    if (memoryMapBase->getMemoryBlocks()->isEmpty())
    {
        return;
    }
    
    // Keep track of found names, and if an error has been issued for duplicate names.
    QHash<QString, bool> memoryBlockNames; 

    for (int blockIndex = 0; blockIndex < memoryMapBase->getMemoryBlocks()->size(); ++blockIndex)
    {
        QSharedPointer<MemoryBlockBase> memoryBlock = memoryMapBase->getMemoryBlocks()->at(blockIndex);
        if (memoryBlockNames.contains(memoryBlock->name()) && memoryBlockNames[memoryBlock->name()] == false)
        {
            errors.append(QObject::tr("Name %1 of memory blocks in %2 is not unique")
                .arg(memoryBlock->name()).arg(context));
            memoryBlockNames[memoryBlock->name()] = true;
        }
        else
        {
            memoryBlockNames.insert(memoryBlock->name(), false);
        }

        if (QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>())
        {
            addressBlockValidator_->findErrorsIn(errors, addressBlock, addressUnitBits, context);
            if (!addressBlockWidthIsMultipleOfAUB(addressUnitBits, addressBlock))
            {
                errors.append(QObject::tr(
                    "Width of address block %1 is not a multiple of the address unit bits of %2 %3")
                    .arg(addressBlock->name())
                    .arg(memoryMapBase->elementName())
                    .arg(memoryMapBase->name()));
            }
        }
        else if (QSharedPointer<SubSpaceMap> subspace = memoryBlock.dynamicCast<SubSpaceMap>())
        {
            subspaceValidator_->findErrorsIn(errors, subspace, context);
        }

        findErrorsInOverlappingBlocks(errors, memoryMapBase, memoryBlock, blockIndex, context);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInOverlappingBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInOverlappingBlocks(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QSharedPointer<MemoryBlockBase> memoryBlock, int blockIndex,
    QString const& context) const
{
    for (int comparisonIndex = blockIndex + 1; comparisonIndex < memoryMapBase->getMemoryBlocks()->size();
        ++comparisonIndex)
    {
        QSharedPointer<MemoryBlockBase> comparisonBlock = memoryMapBase->getMemoryBlocks()->at(comparisonIndex);
        if (comparisonBlock)
        {
            if (twoMemoryBlocksOverlap(memoryBlock, comparisonBlock))
            {
                errors.append(QObject::tr("Memory blocks %1 and %2 overlap in %3")
                    .arg(memoryBlock->name()).arg(comparisonBlock->name()).arg(context));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::getExpressionParser()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> MemoryMapBaseValidator::getExpressionParser() const
{
    return expressionParser_;
}
