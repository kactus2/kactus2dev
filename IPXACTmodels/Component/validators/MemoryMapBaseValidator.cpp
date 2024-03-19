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

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::MemoryMapBaseValidator()
//-----------------------------------------------------------------------------
MemoryMapBaseValidator::MemoryMapBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<AddressBlockValidator> addressBlockValidator,
    QSharedPointer<SubspaceMapValidator> subspaceValidator,
    Document::Revision docRevision):
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
    const
{
    return hasValidName(memoryMapBase) && hasValidIsPresent(memoryMapBase) &&
        hasValidMemoryBlocks(memoryMapBase, addressUnitBits);
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
    QString const& addressUnitBits) const
{
    if (!memoryMapBase->getMemoryBlocks()->isEmpty())
    {
        QStringList memoryBlockNames;
        for (int blockIndex = 0; blockIndex < memoryMapBase->getMemoryBlocks()->size(); ++blockIndex)
        {
            QSharedPointer<MemoryBlockBase> blockData = memoryMapBase->getMemoryBlocks()->at(blockIndex);
            if (memoryBlockNames.contains(blockData->name()))
            {
                return false;
            }

            memoryBlockNames.append(blockData->name());

            QSharedPointer<AddressBlock> addressBlock = blockData.dynamicCast<AddressBlock>();
            if (addressBlock &&
                (!addressBlockValidator_->validate(addressBlock, addressUnitBits) ||
                    !addressBlockWidthIsMultiplicationOfAUB(addressUnitBits, addressBlock)))
            {
                return false;
            }
            else
            {
                QSharedPointer<SubSpaceMap> subspace = blockData.dynamicCast<SubSpaceMap>();
                if (subspace && !subspaceValidator_->validate(subspace))
                {
                    return false;
                }
            }

            if (memoryBlockOverlaps(blockData, memoryMapBase, blockIndex))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::addressUnitBitsIsValidForMemoryBlocks()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::addressBlockWidthIsMultiplicationOfAUB(QString const& addressUnitBits,
    QSharedPointer<AddressBlock> addressBlock) const
{
    bool aubToIntOk = true;
    bool widthToIntOk = true;

    int addressUnitBitsInt = expressionParser_->parseExpression(addressUnitBits).toInt(&aubToIntOk);

    int addressBlockWidth = expressionParser_->parseExpression(addressBlock->getWidth()).toInt(&widthToIntOk);

    return aubToIntOk && widthToIntOk && addressUnitBitsInt != 0 && addressBlockWidth % addressUnitBitsInt == 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::addressBlockOverlap()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::memoryBlockOverlaps(QSharedPointer<MemoryBlockBase> memoryBlock,
    QSharedPointer<MemoryMapBase> memoryMapBase, int memoryBlockIndex) const
{
    for (int blockIndex = memoryBlockIndex + 1; blockIndex < memoryMapBase->getMemoryBlocks()->size();
        ++blockIndex)
    {
        QSharedPointer<MemoryBlockBase> comparisonBlock = memoryMapBase->getMemoryBlocks()->at(blockIndex);
        if (twoMemoryBlocksOverlap(memoryBlock, comparisonBlock))
        {
            return true;
        }
    }

    return false;
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
    QString const& addressUnitBits, QString const& context) const
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
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits, QString const& context) const
{
    if (!memoryMapBase->getMemoryBlocks()->isEmpty())
    {
        QStringList memoryBlockNames;
        for (int blockIndex = 0; blockIndex < memoryMapBase->getMemoryBlocks()->size(); ++blockIndex)
        {
            QSharedPointer<MemoryBlockBase> memoryBlock = memoryMapBase->getMemoryBlocks()->at(blockIndex);
            if (memoryBlockNames.contains(memoryBlock->name()))
            {
                errors.append(QObject::tr("Name %1 of memory blocks in %2 is not unique")
                    .arg(memoryBlock->name()).arg(context));
            }

            memoryBlockNames.append(memoryBlock->name());

            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            QSharedPointer<SubSpaceMap> subspace = memoryBlock.dynamicCast<SubSpaceMap>();
            if (addressBlock || subspace)
            {
                if (addressBlock)
                {
                    addressBlockValidator_->findErrorsIn(errors, addressBlock, addressUnitBits, context);
                    if (!addressBlockWidthIsMultiplicationOfAUB(addressUnitBits, addressBlock))
                    {
                        errors.append(QObject::tr(
                            "Width of address block %1 is not a multiple of the address unit bits of %2 %3")
                            .arg(addressBlock->name())
                            .arg(memoryMapBase->elementName())
                            .arg(memoryMapBase->name()));
                    }
                }
                else if (subspace)
                {
                    subspaceValidator_->findErrorsIn(errors, subspace, context);
                }

                findErrorsInOverlappingBlocks(errors, memoryMapBase, memoryBlock, blockIndex, context);
            }
        }
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
