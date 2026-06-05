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
#include <IPXACTmodels/common/Document.h>

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
     *    @param [in] writer          Used XML writer.
     *    @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock, 
        Document::Revision docRevision);

    IPXACTMODELS_EXPORT void writeAccessHandles(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock, Document::Revision docRevision);

    /*!
     *  Write the usage.
     *
     *    @param [in] writer          Used XML writer.
     *    @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeUsage(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock);

    /*!
     *  Write the volatile status.
     *
     *    @param [in] writer          Used XML writer.
     *    @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeVolatile(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock);

    /*!
     *  Write the access value.
     *
     *    @param [in] writer          Used XML writer.
     *    @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeAccess(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock);

    /*!
     *	Write the access policies of the memory block.
     *  
     *    @param [in] writer          Used XML writer.
     *    @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeAccessPolicies(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock);

    /*!
     *  Write the base address.
     *
     *    @param [in] writer          Used XML writer.
     *    @param [in] memoryBlock     Selected memory block.
     */
    IPXACTMODELS_EXPORT void writeBaseAddress(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock);
};

#endif // MEMORYBLOCKBASEWRITER_H