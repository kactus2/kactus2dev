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

class MemoryArray;

//-----------------------------------------------------------------------------
//! Writer for ipxact:array memory array elements.
//-----------------------------------------------------------------------------
namespace MemoryArrayValidator
{
    
    /*!
     *	Validates the given memory array.
     *  
     *      @param [in] memoryArray     The memory array to validate.
     *	    
     * 	    @return True, if memory array is valid, otherwise false.
     */
    IPXACTMODELS_EXPORT bool validateMemoryArray(QSharedPointer<MemoryArray> memoryArray);

    /*!
     *	Finds errors within a memory array.
     *  
     *      @param [in] errorList       A list of found errors.
     *      @param [in] memoryArray     The memory array to check.
     *      @param [in] context         Context to help locate the error.
     */
    IPXACTMODELS_EXPORT void findErrorsInMemoryArray(QStringList& errorList, QSharedPointer<MemoryArray> memoryArray,
        QString const& context);

    namespace Details
    {
        /*!
         *	Checks if a given memory array has dimensions defined.
         *  
         *      @param [in] memoryArray     The memory array to check.
         *	    
         * 	    @return True, if at least one dimension is defined for the array.
         */
        bool hasDimensions(QSharedPointer<MemoryArray> memoryArray);
    }
};

#endif // MEMORYARRAYVALIDATOR_H
