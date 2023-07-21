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
#include "MemoryArray.h"

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::validateMemoryArray()
//-----------------------------------------------------------------------------
bool MemoryArrayValidator::validateMemoryArray(QSharedPointer<MemoryArray> memoryArray)
{
    return Details::hasDimensions(memoryArray);
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::findErrorsInMemoryArray()
//-----------------------------------------------------------------------------
void MemoryArrayValidator::findErrorsInMemoryArray(QStringList& errorList, QSharedPointer<MemoryArray> memoryArray,
    QString const& context)
{
    if (!Details::hasDimensions(memoryArray))
    {
        errorList.append(QObject::tr("No dimensions defined for memory array in %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayValidator::Details::hasDimensions()
//-----------------------------------------------------------------------------
bool MemoryArrayValidator::Details::hasDimensions(QSharedPointer<MemoryArray> memoryArray)
{
    return !memoryArray->getDimensions()->isEmpty();
}
