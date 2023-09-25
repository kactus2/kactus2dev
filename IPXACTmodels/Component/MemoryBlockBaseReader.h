//-----------------------------------------------------------------------------
// File: MemoryBlockBaseReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.01.2022
//
// Description:
// Base reader for memory map sub elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKBASEREADER_H
#define MEMORYBLOCKBASEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QSharedPointer>
#include <QDomNode>

class MemoryBlockBase;

//-----------------------------------------------------------------------------
//! Base reader for memory map sub elements.
//-----------------------------------------------------------------------------
namespace MemoryBlockBaseReader
{
    /*!
     *  Reads the name group.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parseNameGroup(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the presence.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parsePresence(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the base address.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parseBaseAddress(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the parameters.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parseParameters(QDomNode const& addressBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock, Document::Revision docRevision);
}

#endif // MEMORYBLOCKBASEREADER_H