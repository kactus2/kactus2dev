//-----------------------------------------------------------------------------
// File: MemoryBlockBaseWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.01.2022
//
// Description:
// Writer for base memory block element.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKBASEWRITER_H
#define MEMORYBLOCKBASEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>
#include <QObject>

class MemoryBlockBase;

//-----------------------------------------------------------------------------
//! Writer for base memory block element.
//-----------------------------------------------------------------------------
namespace  MemoryBlockBaseWriter
{
    /*!
     *  Write the name group.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock, 
        Document::Revision docRevision);

    /*!
     *  Write the base address.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeBaseAddress(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock);
};

#endif // ADDRESSBLOCKWRITER_H