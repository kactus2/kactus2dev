//-----------------------------------------------------------------------------
// File: AddressSpaceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.11.2015
//
// Description:
// Validator for the ipxact:addressSpace.
//-----------------------------------------------------------------------------

#include "AddressSpaceValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/MemoryReserve.h>
#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::AddressSpaceValidator()
//-----------------------------------------------------------------------------
AddressSpaceValidator::AddressSpaceValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<MemoryMapBaseValidator> memoryMapBaseValidator,
    QSharedPointer<ParameterValidator> parameterValidator) :
HierarchicalValidator(),
expressionParser_(expressionParser),
localMemoryMapValidator_(memoryMapBaseValidator),
parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::getLocalMemoryMapValidator()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMapBaseValidator> AddressSpaceValidator::getLocalMemoryMapValidator()
{
    return localMemoryMapValidator_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::validate()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::validate(QSharedPointer<AddressSpace> addressSpace) const
{
    return hasValidName(addressSpace->name()) && 
        hasValidIsPresent(addressSpace->getIsPresent()) &&
        hasValidRange(addressSpace) && hasValidWidth(addressSpace) && hasValidSegments(addressSpace) &&
        hasValidAddressUnitBits(addressSpace) && hasValidLocalMemoryMap(addressSpace) &&
        hasValidParameters(addressSpace) && validComparedToSiblings(addressSpace);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::changeComponent()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::componentChange(QSharedPointer<Component> newComponent)
{
    localMemoryMapValidator_->componentChange(newComponent);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidName(QString const& name) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

    if (name.isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidIsPresent(QString const& isPresent) const
{
    if (!isPresent.isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(isPresent);

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
// Function: AddressSpaceValidator::hasValidRange()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidRange(QSharedPointer<AddressSpace> addressSpace) const
{
    if (!addressSpace->getRange().isEmpty())
    {
        bool rangeOk = true;
        bool expressionValid = false;
        quint64 range =
            expressionParser_->parseExpression(addressSpace->getRange(), &expressionValid).toULongLong(&rangeOk);

        return rangeOk && expressionValid && range != 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::hasValidWidth()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidWidth(QSharedPointer<AddressSpace> addressSpace) const
{
    if (!addressSpace->getWidth().isEmpty())
    {
        bool widthOk = true;
        bool expressionValid = false;
        auto width =
            expressionParser_->parseExpression(addressSpace->getWidth(), &expressionValid).toLongLong(&widthOk);

        return widthOk && width >= 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::hasValidSegments()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidSegments(QSharedPointer<AddressSpace> addressSpace) const
{
    if (!addressSpace->getSegments()->isEmpty())
    {
        MemoryReserve reservedArea;

        QStringList segmentNames;

        qint64 addressSpaceRange = expressionParser_->parseExpression(addressSpace->getRange()).toLongLong();

        for (QSharedPointer<Segment> segment : *addressSpace->getSegments())
        {
            qint64 segmentBegin = expressionParser_->parseExpression(segment->getAddressOffset()).toLongLong();
            qint64 segmentRange = expressionParser_->parseExpression(segment->getRange()).toLongLong();
            qint64 segmentEnd = segmentBegin + segmentRange - 1;

            if (segmentNames.contains(segment->name()) ||
                !hasValidName(segment->name()) ||
                !hasValidIsPresent(segment->getIsPresent()) ||
                !segmentHasValidAddressOffset(segment) ||
                !segmentHasValidRange(segment) ||
                segmentEnd > addressSpaceRange)
            {
                return false;
            }
            else
            {
                segmentNames.append(segment->name());

                reservedArea.addArea(segment->name(), segmentBegin, segmentEnd);
            }
        }

        return !reservedArea.hasOverlap();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::segmentHasValidAddressOffset()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::segmentHasValidAddressOffset(QSharedPointer<Segment> segment) const
{
    if (!segment->getAddressOffset().isEmpty())
    {
        bool offsetOk = true;
        expressionParser_->parseExpression(segment->getAddressOffset()).toULongLong(&offsetOk);

        return offsetOk;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::segmentHasValidRange()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::segmentHasValidRange(QSharedPointer<Segment> segment) const
{
    if (!segment->getRange().isEmpty())
    {
        bool rangeOk = true;
        quint64 range = expressionParser_->parseExpression(segment->getRange()).toULongLong(&rangeOk);

        return rangeOk && range > 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::localMemMapaddressBlocksOverlap()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::addressBlocksAreContainedWithinAddressSpace(QSharedPointer<AddressSpace> addressSpace) const
{
    auto localMemMap = addressSpace->getLocalMemoryMap();

    if (!localMemMap)
    {
        return true;
    }

    for (auto const& memoryBlock : *localMemMap->getMemoryBlocks())
    {
        auto addressBlock = memoryBlock.dynamicCast<AddressBlock>();

        if (addressBlock && !addressBlockIsContainedWithinAddressSpace(addressSpace, addressBlock))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::addressBlockIsContainedWithinLocalMemMap()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::addressBlockIsContainedWithinAddressSpace(QSharedPointer<AddressSpace> addressSpace, QSharedPointer<AddressBlock> addressBlock) const
{
    if (!addressBlock)
    {
        return false;
    }
    
    bool rangeOk = false;
    bool baseAddrOk = false;

    quint64 addressBlockRange = expressionParser_->parseExpression(addressBlock->getRange()).toULongLong(&rangeOk);
    quint64 startPos = expressionParser_->parseExpression(addressBlock->getBaseAddress()).toULongLong(&baseAddrOk);

    if (!rangeOk || !baseAddrOk)
    {
        return false;
    }

    quint64 endPos = startPos + addressBlockRange;

    return endPos <= expressionParser_->parseExpression(addressSpace->getRange()).toULongLong();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInAddressBlockRanges()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInAddressBlockRanges(QStringList& errors, QSharedPointer<AddressSpace> addressSpace, QString const& context) const
{
    auto localMemMap = addressSpace->getLocalMemoryMap();

    if (!localMemMap)
    {
        return;
    }

    for (auto const& memoryBlock : *localMemMap->getMemoryBlocks())
    {
        auto addressBlock = memoryBlock.dynamicCast<AddressBlock>();

        if (addressBlock && !addressBlockIsContainedWithinAddressSpace(addressSpace, addressBlock))
        {
            errors.append(QObject::tr("Address block %1 is not contained within local memory map of %2.")
                .arg(addressBlock->name()).arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::hasValidAddressUnitBits()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidAddressUnitBits(QSharedPointer<AddressSpace> addressSpace) const
{
    if (!addressSpace->getAddressUnitBits().isEmpty())
    {
        bool bitsOk = true;

        quint64 unitBits =
            expressionParser_->parseExpression(addressSpace->getAddressUnitBits()).toULongLong(&bitsOk);

        return bitsOk && unitBits > 0;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::hasValidLocalMemoryMap()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidLocalMemoryMap(QSharedPointer<AddressSpace> addressSpace) const
{
    if (auto localMemoryMap = addressSpace->getLocalMemoryMap())
    {
        QString addressUnitBits = addressSpace->getAddressUnitBits();
        if (addressUnitBits.isEmpty())
        {
            addressUnitBits = QLatin1String("8");
        }

        if (!localMemoryMapValidator_->validate(localMemoryMap, addressUnitBits) ||
            !addressBlocksAreContainedWithinAddressSpace(addressSpace))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool AddressSpaceValidator::hasValidParameters(QSharedPointer<AddressSpace> addressSpace) const
{
    if (!addressSpace->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *addressSpace->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
            {
                return false;
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
    QString const& context) const
{
    QString addressSpaceContext = QStringLiteral("address space ") + addressSpace->name();

    findErrorsInName(errors, addressSpace, context);
    findErrorsInIsPresent(errors, addressSpace, context);
    findErrorsInRange(errors, addressSpace, context);
    findErrorsInWidth(errors, addressSpace, context);
    findErrorsInSegments(errors, addressSpace, addressSpaceContext);
    findErrorsInAddressUnitBits(errors, addressSpace, context);
    findErrorsInLocalMemoryMap(errors, addressSpace, addressSpaceContext);
    findErrorsInParameters(errors, addressSpace, addressSpaceContext);
    findErrorsInAddressBlockRanges(errors, addressSpace, addressSpaceContext);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
    QString const& context) const
{
    if (!hasValidName(addressSpace->name()))
    {
        errors.append(QObject::tr("Invalid name specified for address space %1 within %2")
            .arg(addressSpace->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<AddressSpace> addressSpace, QString const& context) const
{
    if (!hasValidIsPresent(addressSpace->getIsPresent()))
    {
        errors.append(QObject::tr("Invalid isPresent set for address space %1 within %2")
            .arg(addressSpace->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInRange()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInRange(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
    QString const& context) const
{
    if (!hasValidRange(addressSpace))
    {
        errors.append(QObject::tr("Invalid range set for address space %1 within %2")
            .arg(addressSpace->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInWidth()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInWidth(QVector<QString>& errors, QSharedPointer<AddressSpace> addressSpace,
    QString const& context) const
{
    if (!hasValidWidth(addressSpace))
    {
        errors.append(QObject::tr("Invalid width set for address space %1 within %2")
            .arg(addressSpace->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInSegments()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInSegments(QVector<QString>& errors,
    QSharedPointer<AddressSpace> addressSpace, QString const& context) const
{
    if (!addressSpace->getSegments()->isEmpty())
    {
        qint64 addressSpaceRange = expressionParser_->parseExpression(addressSpace->getRange()).toLongLong();

        MemoryReserve reservedArea;
        QStringList segmentNames;
        QStringList foundNames;
        for (QSharedPointer<Segment> const& segment : *addressSpace->getSegments())
        {
            if (segmentNames.contains(segment->name()) && !foundNames.contains(segment->name()))
            {
                errors.append(QObject::tr("Name %1 of segments in %2 is not unique")
                    .arg(segment->name()).arg(context));
                foundNames.append(segment->name());
            }
            else
            {
                segmentNames.append(segment->name());
            }
            if (!hasValidName(segment->name()))
            {
                errors.append(QObject::tr("Invalid name specified for segment %1 within %2")
                    .arg(segment->name()).arg(context));
            }
            if (!hasValidIsPresent(segment->getIsPresent()))
            {
                errors.append(QObject::tr("Invalid isPresent set for segment %1 within %2")
                    .arg(segment->name()).arg(context));
            }
            if (!segmentHasValidAddressOffset(segment))
            {
                errors.append(QObject::tr("Invalid address offset set for segment %1 within %2")
                    .arg(segment->name()).arg(context));
            }
            if (!segmentHasValidRange(segment))
            {
                errors.append(QObject::tr("Invalid range set for segment %1 within %2")
                    .arg(segment->name()).arg(context));
            }

            qint64 segmentBegin = expressionParser_->parseExpression(segment->getAddressOffset()).toLongLong();
            qint64 segmentRange = expressionParser_->parseExpression(segment->getRange()).toLongLong();
            qint64 segmentEnd = segmentBegin + segmentRange - 1;

            reservedArea.addArea(segment->name(), segmentBegin, segmentEnd);

            if (segmentEnd > addressSpaceRange)
            {
                errors.append(QObject::tr("Segment %1 is not contained within %2")
                    .arg(segment->name()).arg(context));
            }
        }

        reservedArea.findErrorsInOverlap(errors, QLatin1String("Segments"), context);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInAddressUnitBits()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInAddressUnitBits(QVector<QString>& errors,
    QSharedPointer<AddressSpace> addressSpace, QString const& context) const
{
    if (!hasValidAddressUnitBits(addressSpace))
    {
        errors.append(QObject::tr("Invalid address unit bits set for address space %1 within %2")
            .arg(addressSpace->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInLocalMemoryMap()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInLocalMemoryMap(QVector<QString>& errors,
    QSharedPointer<AddressSpace> addressSpace, QString const& context) const
{
    if (addressSpace->getLocalMemoryMap())
    {
        QString addressUnitBits= addressSpace->getAddressUnitBits();
        if (addressUnitBits.isEmpty())
        {
            addressUnitBits = QLatin1String("8");
        }

        localMemoryMapValidator_->findErrorsIn(
            errors, addressSpace->getLocalMemoryMap(), addressUnitBits, context);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void AddressSpaceValidator::findErrorsInParameters(QVector<QString>& errors,
    QSharedPointer<AddressSpace> addressSpace, QString const& context) const
{
    if (!addressSpace->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *addressSpace->getParameters())
        {
            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Name %1 of parameters in %2 is not unique.").arg(parameter->name()).
                    arg(context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }

            parameterValidator_->findErrorsIn(errors, parameter, context);
        }
    }
}
