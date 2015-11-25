//-----------------------------------------------------------------------------
// File: MemoryMapBaseValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Validator for the base ipxact:memoryMap.
//-----------------------------------------------------------------------------

#include "MemoryMapBaseValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::MemoryMapBaseValidator()
//-----------------------------------------------------------------------------
MemoryMapBaseValidator::MemoryMapBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices):
expressionParser_(expressionParser),
availableChoices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::~MemoryMapBaseValidator()
//-----------------------------------------------------------------------------
MemoryMapBaseValidator::~MemoryMapBaseValidator()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::validate(QSharedPointer<MemoryMapBase> memoryMapBase) const
{
    return hasValidName(memoryMapBase) && hasValidIsPresent(memoryMapBase) && hasValidMemoryBlocks(memoryMapBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::hasValidName()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::hasValidName(QSharedPointer<MemoryMapBase> memoryMapBase) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
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
bool MemoryMapBaseValidator::hasValidMemoryBlocks(QSharedPointer<MemoryMapBase> memoryMapBase) const
{
    if (!memoryMapBase->getMemoryBlocks()->isEmpty())
    {
        AddressBlockValidator validator(expressionParser_, availableChoices_);
        QStringList addressBlockNames;
        for (int blockIndex = 0; blockIndex < memoryMapBase->getMemoryBlocks()->size(); ++blockIndex)
        {
            QSharedPointer<MemoryBlockBase> blockData = memoryMapBase->getMemoryBlocks()->at(blockIndex);
            QSharedPointer<AddressBlock> addressBlock = blockData.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                if (addressBlockNames.contains(addressBlock->name()) || !validator.validate(addressBlock))
                {
                    return false;
                }
                else
                {
                    addressBlockNames.append(addressBlock->name());

                    if (addressBlockOverlaps(addressBlock, memoryMapBase, blockIndex))
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::addressBlockOverlap()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::addressBlockOverlaps(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<MemoryMapBase> memoryMapBase, int addressBlockIndex) const
{
    for (int blockIndex = addressBlockIndex + 1; blockIndex < memoryMapBase->getMemoryBlocks()->size();
        ++blockIndex)
    {
        QSharedPointer<MemoryBlockBase> blockData = memoryMapBase->getMemoryBlocks()->at(blockIndex);
        QSharedPointer<AddressBlock> comparedBlock = blockData.dynamicCast<AddressBlock>();
        if (comparedBlock)
        {
            if (twoAddressBlocksOverlap(addressBlock, comparedBlock))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::twoAddressBlocksOverlap()
//-----------------------------------------------------------------------------
bool MemoryMapBaseValidator::twoAddressBlocksOverlap(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<AddressBlock> comparedBlock) const
{
    int blockBegin = expressionParser_->parseExpression(addressBlock->getBaseAddress()).toInt();
    int blockEnd = blockBegin + expressionParser_->parseExpression(addressBlock->getRange()).toInt() - 1;

    int compareBegin = expressionParser_->parseExpression(comparedBlock->getBaseAddress()).toInt();
    int compareEnd = compareBegin + expressionParser_->parseExpression(comparedBlock->getRange()).toInt() - 1;

    if (((blockBegin >= compareBegin && blockBegin <= compareEnd) ||
        (blockEnd >= compareBegin && blockEnd <= compareEnd)) ||
        ((compareBegin >= blockBegin && compareBegin <= blockEnd) ||
        (compareEnd >= blockBegin && compareEnd <= blockEnd)))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<MemoryMapBase> memoryMapBase,
    QString const& context) const
{
    QString memoryMapContext = "memory map " + memoryMapBase->name();

    findErrorsInName(errors, memoryMapBase, context);
    findErrorsInIsPresent(errors, memoryMapBase, context);
    findErrorsInAddressBlocks(errors, memoryMapBase, memoryMapContext);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& context) const
{
    if (!hasValidName(memoryMapBase))
    {
        errors.append(QObject::tr("Invalid name specified for memory map %1 within %2").arg(memoryMapBase->name()).
            arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& context) const
{
    if (!hasValidIsPresent(memoryMapBase))
    {
        errors.append(QObject::tr("Invalid isPresent set for memory map %1 within %2").arg(memoryMapBase->name())
            .arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInAddressBlocks(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QString const& context) const
{
    if (!memoryMapBase->getMemoryBlocks()->isEmpty())
    {
        AddressBlockValidator validator(expressionParser_, availableChoices_);
        QStringList addressBlockNames;
        for (int blockIndex = 0; blockIndex < memoryMapBase->getMemoryBlocks()->size(); ++blockIndex)
        {
            QSharedPointer<MemoryBlockBase> memoryBlock = memoryMapBase->getMemoryBlocks()->at(blockIndex);
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                validator.findErrorsIn(errors, addressBlock, context);
            }

            if (addressBlockNames.contains(addressBlock->name()))
            {
                errors.append(QObject::tr("Name %1 of address blocks in %2 is not unique")
                    .arg(addressBlock->name()).arg(context));
            }

            findErrorsInOverlappingBlocks(errors, memoryMapBase, addressBlock, blockIndex, context);

            addressBlockNames.append(addressBlock->name());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseValidator::findErrorsInOverlappingBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseValidator::findErrorsInOverlappingBlocks(QVector<QString>& errors,
    QSharedPointer<MemoryMapBase> memoryMapBase, QSharedPointer<AddressBlock> addressBlock, int blockIndex,
    QString const& context) const
{
    for (int i = blockIndex; i < memoryMapBase->getMemoryBlocks()->size(); ++i)
    {
        QSharedPointer<MemoryBlockBase> blockData = memoryMapBase->getMemoryBlocks()->at(i);
        QSharedPointer<AddressBlock> comparisonBlock = blockData.dynamicCast<AddressBlock>();
        if (comparisonBlock)
        {
            if (twoAddressBlocksOverlap(addressBlock, comparisonBlock))
            {
                errors.append(QObject::tr("Address blocks %1 and %2 overlap in %3")
                    .arg(addressBlock->name()).arg(comparisonBlock->name()).arg(context));
            }
        }
    }
}