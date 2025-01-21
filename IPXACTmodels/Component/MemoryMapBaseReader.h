//-----------------------------------------------------------------------------
// File: MemoryMapBaseReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Reader for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------

#ifndef MemoryMapBaseREADER_H
#define MemoryMapBaseREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

class MemoryMapBase;
class MemoryRemap;

//-----------------------------------------------------------------------------
//! Reader for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------
namespace MemoryMapBaseReader
{

    /*!
     *  Creates a new memory map from a given memory map node.
     *
     *    @param [in] MemoryMapBaseNode   XML description of the memory map.
     *
     *    @return The created memory map.
     */
    IPXACTMODELS_EXPORT void readMemoryMapBase(QDomNode const& MemoryMapBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Reads the name group.
         *
         *    @param [in] MemoryMapBaseBaseNode   XML description of the memory map base.
         *    @param [in] newMemoryMapBaseBase    The new memory map base item.
         */
        void parseNameGroup(QDomNode const& MemoryMapBaseBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase);

        /*!
         *  Reads the isPresent value.
         *
         *    @param [in] MemoryMapBaseBaseNode   XML description of the memory map base.
         *    @param [in] newMemoryMapBaseBase    The new memory map base item.
         */
        void parsePresence(QDomNode const& MemoryMapBaseBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase);

        /*!
         *  Reads the contained memory blocks.
         *
         *    @param [in] memoryMapBaseBaseNode   XML description of the memory map base.
         *    @param [in] newMemoryMapBaseBase    The new memory map base item.
         */
        void parseMemoryBlocks(QDomNode const& memoryMapBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase, Document::Revision docRevision);
    }
};

#endif // MemoryMapBaseREADER_H