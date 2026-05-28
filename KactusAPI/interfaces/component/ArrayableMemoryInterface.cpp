//-----------------------------------------------------------------------------
// File: ArrayableMemoryInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 19.05.2025
//
// Description:
// Interface for accessing memory array of arrayable memories.
//-----------------------------------------------------------------------------

#include "ArrayableMemoryInterface.h"

#include <string>

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::ArrayableMemoryInterface()
//-----------------------------------------------------------------------------
ArrayableMemoryInterface::ArrayableMemoryInterface(QSharedPointer<ExpressionParser> expressionParser, 
    QSharedPointer<ExpressionFormatter> formatter) : 
NameGroupInterface(),
ParameterizableInterface(expressionParser, formatter)
{

}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::getDimensionValue()
//-----------------------------------------------------------------------------
std::string ArrayableMemoryInterface::getDimensionValue(std::string const& memoryName, int const& baseNumber /*= 0*/) const
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        return parseExpressionToBaseNumber(selectedMemory->getDimension(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::getDimensionFormattedExpression()
//-----------------------------------------------------------------------------
std::string ArrayableMemoryInterface::getDimensionFormattedExpression(std::string const& memoryName) const
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        return formattedValueFor(selectedMemory->getDimension()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::getDimensionExpression()
//-----------------------------------------------------------------------------
std::string ArrayableMemoryInterface::getDimensionExpression(std::string const& memoryName) const
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        return selectedMemory->getDimension().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::setDimension()
//-----------------------------------------------------------------------------
bool ArrayableMemoryInterface::setDimension(std::string const& memoryName, std::string const& newDimension)
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        selectedMemory->setDimension(QString::fromStdString(newDimension));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::getStrideValue()
//-----------------------------------------------------------------------------
std::string ArrayableMemoryInterface::getStrideValue(std::string const& memoryName, int const& baseNumber /*= 0*/) const
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        return parseExpressionToBaseNumber(selectedMemory->getStride(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::getStrideFormattedExpression()
//-----------------------------------------------------------------------------
std::string ArrayableMemoryInterface::getStrideFormattedExpression(std::string const& memoryName) const
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        return formattedValueFor(selectedMemory->getStride()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::getStrideExpression()
//-----------------------------------------------------------------------------
std::string ArrayableMemoryInterface::getStrideExpression(std::string const& memoryName) const
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        return selectedMemory->getStride().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryInterface::setStride()
//-----------------------------------------------------------------------------
bool ArrayableMemoryInterface::setStride(std::string const& memoryName, std::string const& newStride)
{
    if (auto selectedMemory = getItem(memoryName).dynamicCast<ArrayableMemory>())
    {
        selectedMemory->setStride(QString::fromStdString(newStride));
        return true;
    }

    return false;
}
