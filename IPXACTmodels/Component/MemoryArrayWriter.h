//-----------------------------------------------------------------------------
// File: MemoryArrayWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Writer for ipxact:array memory array elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYARRAYWRITER_H
#define MEMORYARRAYWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>

class MemoryArray;

//-----------------------------------------------------------------------------
//! Writer for ipxact:array memory array elements.
//-----------------------------------------------------------------------------
namespace MemoryArrayWriter
{
    
    /*!
     *	Write a memory array to XML.
     *  
     *      @param [in] writer          The XML writer to use.
     *      @param [in] memoryArray     The memory array to write.
     *      @param [in] isField         Flag indicating if the array is within a field.
     */
    IPXACTMODELS_EXPORT void writeMemoryArray(QXmlStreamWriter& writer, QSharedPointer<MemoryArray> memoryArray, bool isField);

    namespace Details
    {
        /*!
         *	Write the array dimensions.
         *  
         *      @param [in] writer          The XML writer to use.
         *      @param [in] memoryArray     The memory array whose dimensions are written.
         */
        void writeDimensions(QXmlStreamWriter& writer, QSharedPointer<MemoryArray> memoryArray);

        /*!
         *	Write the stride or bitStride of the memory array.
         *  
         *      @param [in] writer          The XML writer to use.
         *      @param [in] memoryArray     The memory array to write.
         *      @param [in] isField         Indicates whether the memory array is within a field or not. 
         *                                  Write bitStride if true, otherwise write stride.
         */
        void writeStride(QXmlStreamWriter& writer, QSharedPointer<MemoryArray> memoryArray, bool isField);
    }
};

#endif // MEMORYARRAYWRITER_H
