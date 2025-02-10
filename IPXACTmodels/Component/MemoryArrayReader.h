//-----------------------------------------------------------------------------
// File: MemoryArrayReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Reader for ixpact:array memory array elements and 2014 standard dim elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYARRAYREADER_H
#define MEMORYARRAYREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

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
     *      @param [in] docRevision   The IP-XACT standard revision to comply to.
     *      @param [in] isField       Flag indicating if array is within a field element.
     *	    
     * 	    @return The created memory array.
     */
    IPXACTMODELS_EXPORT QSharedPointer<MemoryArray> createMemoryArrayFrom(QDomNode const& arrayNode,
        Document::Revision docRevision, bool isField);

    namespace Details
    {
        /*!
         *	Parse the array dimensions.
         *  
         *      @param [in] arrayNode     XML description of the memory array.
         *      @param [in] newArray      The new memory array.
         *      @param [in] docRevision   The IP-XACT standard revision to comply to.
         */
        void parseDimensions(QDomNode const& arrayNode, QSharedPointer<MemoryArray> newArray, Document::Revision docRevision);

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
