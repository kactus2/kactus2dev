//-----------------------------------------------------------------------------
// File: ArrayableMemoryInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 19.05.2025
//
// Description:
// Interface for accessing memory array of arrayable memories.
//-----------------------------------------------------------------------------

#ifndef ARRAYABLEMEMORYINTERFACE_H
#define ARRAYABLEMEMORYINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>
#include <KactusAPI/include/NameGroupInterface.h>
#include <KactusAPI/include/ParameterizableInterface.h>

#include <IPXACTmodels/Component/ArrayableMemory.h>

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ExpressionParser.h>

class KACTUS2_API ArrayableMemoryInterface : public NameGroupInterface, public ParameterizableInterface
{
public:

    ArrayableMemoryInterface(QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> formatter);

    virtual ~ArrayableMemoryInterface() = default;

    //! Copy constructor
    ArrayableMemoryInterface(ArrayableMemoryInterface const& other) = delete;

    //! Assignment
    ArrayableMemoryInterface& operator=(ArrayableMemoryInterface const& other) = delete;

    
    /*!
     *  Get the calculated dimension value of the selected memory.
     *
     *    @param [in] memoryName    Name of the selected memory.
     *    @param [in] baseNumber    Base for displaying the value.
     *
     *    @return Calculated dimension value of the selected memory.
     */
    std::string getDimensionValue(std::string const& memoryName, int const& baseNumber = 0) const;

    /*!
     *  Get the formatted dimension expression of the selected memory.
     *
     *    @param [in] memoryName    Name of the selected memory.
     *
     *    @return Formatted dimension expression of the selected memory.
     */
    std::string getDimensionFormattedExpression(std::string const& memoryName) const;

    /*!
     *  Get the dimension expression of the selected memory.
     *
     *    @param [in] memoryName    Name of the selected memory.
     *
     *    @return Dimension expression of the selected memory.
     */
    std::string getDimensionExpression(std::string const& memoryName) const;

    /*!
     *  Set a new dimension value for the selected memory.
     *
     *    @param [in] memoryName    Name of the selected memory.
     *    @param [in] newDimension  New dimension value.
     *
     *    @return True, if successful, false otherwise.
     */
    bool setDimension(std::string const& memoryName, std::string const& newDimension);

};

#endif // ARRAYABLEMEMORYINTERFACE_H
