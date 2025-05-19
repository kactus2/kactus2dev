//-----------------------------------------------------------------------------
// File: ArrayableMemory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 19.05.2025
//
// Description:
// Base class for IP-XACT memory elements containing the memory array element (ipxact:array).
//-----------------------------------------------------------------------------

#include "ArrayableMemory.h"

#include <utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: ArrayableMemory::ArrayableMemory()
//-----------------------------------------------------------------------------
ArrayableMemory::ArrayableMemory(ArrayableMemory const& other)
{
    if (other.memoryArray_)
    {
        memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray(*other.memoryArray_));
    }
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemory::operator=()
//-----------------------------------------------------------------------------
ArrayableMemory& ArrayableMemory::operator=(ArrayableMemory const& other)
{
    if (this != &other && other.memoryArray_)
    {
        memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray(*other.memoryArray_));
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemory::getDimension()
//-----------------------------------------------------------------------------
QString ArrayableMemory::getDimension() const
{
    if (memoryArray_)
    {
        return memoryArray_->getDimensions()->first()->value_;
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemory::setDimension()
//-----------------------------------------------------------------------------
void ArrayableMemory::setDimension(QString const& newDimension)
{
    if (!memoryArray_)
    {
        memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray());
    }

    if (newDimension.isEmpty())
    {
        memoryArray_.clear();
        return;
    }

    // Create dimension if missing, otherwise set the new value to the first dim.
    if (memoryArray_->getDimensions()->isEmpty())
    {
        QSharedPointer<MemoryArray::Dimension> newDim(new MemoryArray::Dimension());
        newDim->value_ = newDimension;
        memoryArray_->getDimensions()->append(newDim);
    }
    else
    {
        memoryArray_->getDimensions()->first()->value_ = newDimension;
    }
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemory::getMemoryArray()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryArray> ArrayableMemory::getMemoryArray() const
{
    return memoryArray_;
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemory::setMemoryArray()
//-----------------------------------------------------------------------------
void ArrayableMemory::setMemoryArray(QSharedPointer<MemoryArray> newMemArray)
{
    memoryArray_ = newMemArray;
}
