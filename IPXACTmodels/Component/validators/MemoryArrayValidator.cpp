//-----------------------------------------------------------------------------
// File: MemoryArrayValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Validator for ipxact:array memory array elements.
//-----------------------------------------------------------------------------

#include "MemoryArrayValidator.h"
#include <IPXACTmodels/Component/MemoryArray.h>

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::MemoryArrayValidator()
//-----------------------------------------------------------------------------
MemoryArrayValidator::MemoryArrayValidator(QSharedPointer<ExpressionParser> expressionParser):
    expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryArrayValidator::validate(QSharedPointer<MemoryArray> memoryArray) const
{
    return hasValidDimensions(memoryArray) && hasValidStride(memoryArray);
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void MemoryArrayValidator::findErrorsIn(QStringList& errorList, QSharedPointer<MemoryArray> memoryArray,
    QString const& context) const
{
    if (memoryArray->getDimensions()->isEmpty())
    {
        errorList.append(QObject::tr("No dimensions defined for memory array in %1").arg(context));
    }
    
    bool invalidDim = false;
    for (auto const& dim : *memoryArray->getDimensions())
    {
        if (!hasValidElementValue(dim->value_, false))
        {
            invalidDim = true;
            break;
        }
    }

    if (invalidDim && !memoryArray->getDimensions()->isEmpty())
    {
        errorList.append(QObject::tr("One or more dimensions of the memory array in %1 have an invalid value.")
            .arg(context));
    }

    if (!hasValidStride(memoryArray))
    {
        errorList.append(QObject::tr("Memory array in %1 has an invalid stride value.").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::hasDimensions()
//-----------------------------------------------------------------------------
bool MemoryArrayValidator::hasValidDimensions(QSharedPointer<MemoryArray> memoryArray) const
{
    if (!memoryArray)
    {
        return true;
    }

    if (memoryArray->getDimensions()->isEmpty())
    {
        return false;
    }

    for (auto const& dim : *memoryArray->getDimensions())
    {
        if (!hasValidElementValue(dim->value_, false))
        {
            return false;
        }
    }
        
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::hasValidStride()
//-----------------------------------------------------------------------------
bool MemoryArrayValidator::hasValidStride(QSharedPointer<MemoryArray> memoryArray) const
{
    return hasValidElementValue(memoryArray->getStride(), true);
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::hasValidElementValue()
//-----------------------------------------------------------------------------
bool MemoryArrayValidator::hasValidElementValue(QString const& value, bool canBeEmpty) const
{
    if (!value.isEmpty())
    {
        bool validValue = false;
        auto solvedValue = expressionParser_->parseExpression(value, &validValue);

        if (!validValue)
        {
            return false;
        }

        auto castedValue = solvedValue.toULong(&validValue);
        if (!validValue || castedValue == 0)
        {
            return false;
        }

        return true;
    }

    return canBeEmpty;
}
