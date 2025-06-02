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
#include <IPXACTmodels/Component/validators/MemoryReserve.h>

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
    QHash<QString, QSharedPointer<MemoryBlockBase> > foundMemoryBlockNames;

    MemoryReserve memReserve;
    setupMemoryAreas(memoryMapBase, memReserve);

    QSet<QString> erroneousAreas;
    memReserve.checkOverlapAndContainment(erroneousAreas, 0, false);

    auto markItemInvalid = [this](QSharedPointer<MemoryBlockBase> block)
        {
            if (auto asAddressBlock = block.dynamicCast<AddressBlock>())
            {
                addressBlockValidator_->setChildItemValidity(asAddressBlock, false);
            }
            else if (auto asSubspaceMap = block.dynamicCast<SubSpaceMap>())
            {
                subspaceValidator_->setChildItemValidity(asSubspaceMap, false);
            }
        };

    // Validate blocks together
    for (auto const& memoryBlock : *memoryMapBase->getMemoryBlocks())
    {
        // If duplicate name, mark current and first found reg with this name as invalid
        if (auto found = foundMemoryBlockNames[memoryBlock->name()])
        {
            markItemInvalid(memoryBlock);
            markItemInvalid(found);

            errorFound = true;
        }
        else
        {
            foundMemoryBlockNames.insert(memoryBlock->name(), memoryBlock);
        }

        // mark invalid, if block overlaps
        if (erroneousAreas.contains(memoryBlock->name()))
        {
            markItemInvalid(memoryBlock);
            errorFound = true;
        }
    }

    // No need to check child blocks if they are erroneous together.
    if (errorFound)
    {
        return false;
    }

    // Validate blocks separately
    for (auto const& child : *memoryMapBase->getMemoryBlocks())
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
// Function: MemoryMapBaseValidator::getBlockEnd()
//-----------------------------------------------------------------------------
quint64 MemoryMapBaseValidator::getBlockRange(QSharedPointer<MemoryBlockBase> block) const
{
    quint64 memoryRange = 0;
    if (auto addressBlock = block.dynamicCast<AddressBlock>())
    {
        memoryRange = expressionParser_->parseExpression(addressBlock->getRange()).toULongLong();
    }
    else if (auto subspace = block.dynamicCast<SubSpaceMap>())
    {
        memoryRange = 1;

        if (auto referencedSpace = getReferencedAddressSpace(subspace))
        {
            if (auto referencedSegment = getReferencedSegment(subspace, referencedSpace))
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
    QString memoryMapContext = QObject::tr("%1 %2 in %3").arg(memoryMapBase->elementName())
        .arg(memoryMapBase->name()).arg(context);

    findErrorsInName(errors, memoryMapBase, context);
    findErrorsInIsPresent(errors, memoryMapBase, context);
    findErrorsInMemoryBlocks(errors, memoryMapBase, addressUnitBits, memoryMapContext);
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
// Function: MemoryMapBaseValidator::findErrorsInMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInMemoryBlocks(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& addressUnitBits, QString const& context)
{
    if (memoryMapBase->getMemoryBlocks()->isEmpty())
    {
        return;
    }
    
    // Keep track of found names, and if an error has been issued for duplicate names.
    QHash<QString, bool> memoryBlockNames;

    MemoryReserve reservedArea;
    setupMemoryAreas(memoryMapBase, reservedArea);

    for (int blockIndex = 0; blockIndex < memoryMapBase->getMemoryBlocks()->size(); ++blockIndex)
    {
        QSharedPointer<MemoryBlockBase> memoryBlock = memoryMapBase->getMemoryBlocks()->at(blockIndex);
        if (memoryBlockNames.contains(memoryBlock->name()) && memoryBlockNames[memoryBlock->name()] == false)
        {
            errors.append(QObject::tr("Name %1 of memory blocks within %2 is not unique")
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
    }

    reservedArea.findErrorsInOverlap(errors, QLatin1String("Memory blocks"), context);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::setupMemoryAreas()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::setupMemoryAreas(QSharedPointer<MemoryMapBase> memMap, MemoryReserve& memReserve)
{
    if (memMap->getIsPresent().isEmpty() == false && getExpressionParser()->parseExpression(memMap->getIsPresent()).toInt() != 1)
    {
        return;
    }

    for (auto const& memoryBlock : *memMap->getMemoryBlocks())
    {
        if (memoryBlock->getIsPresent().isEmpty() == false &&
            getExpressionParser()->parseExpression(memoryBlock->getIsPresent()).toInt() != 1)
        {
            continue;
        }

        auto blockRange = getBlockRange(memoryBlock);
        auto baseAddr = expressionParser_->parseExpression(memoryBlock->getBaseAddress()).toULongLong();

        // Account for stride if address block
        if (auto asAddressBlock = memoryBlock.dynamicCast<AddressBlock>())
        {
            addressBlockValidator_->setChildItemValidity(asAddressBlock, true);

            quint64 blockStride = 0;
            auto dimension = asAddressBlock->getDimension();

            if (auto blockStrideStr = asAddressBlock->getStride(); blockStrideStr.isEmpty())
            {
                blockStride = blockRange;
            }
            else
            {
                blockStride = expressionParser_->parseExpression(blockStrideStr).toULongLong();
            }

            // Single dimension
            if (dimension.isEmpty() || expressionParser_->parseExpression(dimension).toULongLong() == 1)
            {
                memReserve.addArea(asAddressBlock->name(), baseAddr, baseAddr + blockRange - 1);
            }
            // Multiple dimensions, can have stride
            else
            {
                auto allDimensions = asAddressBlock->getMemoryArray()->getDimensions();

                quint64 replicas = 1;
                std::for_each(allDimensions->cbegin(), allDimensions->cend(),
                    [&replicas, this](QSharedPointer<MemoryArray::Dimension> dim)
                    {
                        replicas *= getExpressionParser()->parseExpression(dim->value_).toULongLong();
                    });

                for (int i = 0; i < replicas; i++)
                {
                    // start from offset, then add i multiples of stride
                    quint64 replicaStart = baseAddr + i * blockStride;
                    quint64 replicaEnd = replicaStart + blockRange - 1;

                    memReserve.addArea(asAddressBlock->name(), replicaStart, replicaEnd);
                }
            }
        }
        // If subspace map, add calculated range
        else
        {
            subspaceValidator_->setChildItemValidity(memoryBlock, true);

            memReserve.addArea(memoryBlock->name(), baseAddr, baseAddr + blockRange - 1);
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
