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
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/AccessHandle.h>

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
     *    @param [in] memoryBlockNode     XML description of the memory block.
     *    @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parseNameGroup(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the presence.
     *
     *    @param [in] memoryBlockNode     XML description of the memory block.
     *    @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parsePresence(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the access handles.
     *
     *    @param [in] memoryBlockNode     XML description of the memory block.
     *    @param [in] newMemoryBlock      The new memory block.
     *    @param [in] elementType         The type of element containing the accesshandle (should be bank or address block)
     *    @param [in] docRevision         The document revision.
     */
    IPXACTMODELS_EXPORT void parseAccessHandles(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock, AccessHandle::ElementType elementType, Document::Revision docRevision);

    /*!
     *  Reads the usage.
     *
     *    @param [in] memoryBlockNode    XML description of the memory block.
     *    @param [in] newMemoryBlock     The new memory block.
     */
    IPXACTMODELS_EXPORT void parseUsage(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the volatile value.
     *
     *    @param [in] memoryBlockNode    XML description of the memory block.
     *    @param [in] newMemoryBlock     The new memory block.
     */
    IPXACTMODELS_EXPORT void parseVolatile(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the access value.
     *
     *    @param [in] memoryBlockNode    XML description of the memory block.
     *    @param [in] newMemoryBlock     The new memory block.
     */
    IPXACTMODELS_EXPORT void parseAccess(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *	Reads the access policies of the memory block.
     *  
     *    @param [in] memoryBlockNode    XML description of the memory block.
     *    @param [in] newMemoryBlock     The new memory block.
     */
    IPXACTMODELS_EXPORT void parseAccessPolicies(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the base address.
     *
     *    @param [in] memoryBlockNode     XML description of the memory block.
     *    @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parseBaseAddress(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock);

    /*!
     *  Reads the parameters.
     *
     *    @param [in] memoryBlockNode     XML description of the memory block.
     *    @param [in] newMemoryBlock      The new memory block.
     */
    IPXACTMODELS_EXPORT void parseParameters(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock, Document::Revision docRevision);
}

#endif // MEMORYBLOCKBASEREADER_H