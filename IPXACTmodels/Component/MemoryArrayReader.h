//-----------------------------------------------------------------------------
// File: MemoryArrayReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Reader for ixpact:array memory array elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYARRAYREADER_H
#define MEMORYARRAYREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>

class MemoryArray;

//-----------------------------------------------------------------------------
//! Reader for ixpact:array memory array elements.
//-----------------------------------------------------------------------------
namespace MemoryArrayReader
{
    
    /*!
     *	Creates a new memory array from a given memory array node.
     *  
     *      @param [in] arrayNode     XML description of the memory array.
     *      @param [in] isField       Flag indicating if array is within a field element.
     *	    
     * 	    @return The created memory array.
     */
    IPXACTMODELS_EXPORT QSharedPointer<MemoryArray> createMemoryArrayFrom(QDomNode const& arrayNode, bool isField);

    namespace Details
    {
        /*!
         *	Parse the array dimensions.
         *  
         *      @param [in] arrayNode     XML description of the memory array.
         *      @param [in] newArray      The new memory array.
         */
        void parseDimensions(QDomNode const& arrayNode, QSharedPointer<MemoryArray> newArray);

        /*!
         *	Parse the array stride.
         *  
         *      @param [in] arrayNode     XML description of the memory array.
         *      @param [in] newArray      The new memory array.
         *      @param [in] isField       Flag indicating if array is within a field element.
         */
        void parseStride(QDomNode const& arrayNode, QSharedPointer<MemoryArray> newArray, bool isField);
    }
};

#endif // MEMORYARRAYREADER_H
