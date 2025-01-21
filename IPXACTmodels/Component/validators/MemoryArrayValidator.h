//-----------------------------------------------------------------------------
// File: MemoryArrayValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Validator for ipxact:array memory array elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYARRAYVALIDATOR_H
#define MEMORYARRAYVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <QObject>

class MemoryArray;

class IPXACTMODELS_EXPORT MemoryArrayValidator
{
public:

    /*!
     *	The constructor.
     *  
     *    @param [in] expressionParser     The expression parser to use.
     */
    MemoryArrayValidator(QSharedPointer<ExpressionParser> expressionParser);

    virtual ~MemoryArrayValidator() = default;

    MemoryArrayValidator(MemoryArrayValidator const& other) = delete;
    MemoryArrayValidator& operator=(MemoryArrayValidator const& other) = delete;
    
    /*!
     *	Validates the given memory array.
     *
     *    @param [in] memoryArray     The memory array to validate.
     *
     * 	    @return True, if memory array is valid, otherwise false.
     */
    bool validate(QSharedPointer<MemoryArray> memoryArray) const;
    
    /*!
     *	Checks if a given memory array has dimensions defined.
     *
     *    @param [in] memoryArray     The memory array to check.
     *
     * 	    @return True, if at least one dimension is defined for the array.
     */
    bool hasValidDimensions(QSharedPointer<MemoryArray> memoryArray) const;
    
    /*!
     *	Finds errors within a memory array.
     *
     *    @param [in] errorList       A list of found errors.
     *    @param [in] memoryArray     The memory array to check.
     *    @param [in] context         Context to help locate the error.
     */
    void findErrorsIn(QStringList& errorList, QSharedPointer<MemoryArray> memoryArray,
        QString const& context) const;

private:


    /*!
     *	Validate the (bit)stride of the array.
     *  
     *    @param [in] memoryArray     The memory array to check.
     *	    
     * 	    @return True, if the stride can be evaluated or is empty, false otherwise.
     */
    bool hasValidStride(QSharedPointer<MemoryArray> memoryArray) const;

    /*!
     *	Validates a given value as an unsigned, nonzero long int.
     *  
     *    @param [in] value       The value to check.
     *    @param [in] canBeEmpty  Flag that indicates if the value given can be empty or not.
     *	    
     * 	    @return True, if the given value is valid, false otherwise.
     */
    bool hasValidElementValue(QString const& value, bool canBeEmpty) const;

    //! The expression parser to use.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // MEMORYARRAYVALIDATOR_H
