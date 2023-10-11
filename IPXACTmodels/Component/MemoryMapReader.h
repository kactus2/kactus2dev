//-----------------------------------------------------------------------------
// File: MemoryMapReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Reader for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPREADER_H
#define MEMORYMAPREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QSharedPointer>
#include <QDomNode>

class MemoryMap;
class MemoryMapBase;
class MemoryRemap;

//-----------------------------------------------------------------------------
//! Reader for ipxact:memoryMap element.
//-----------------------------------------------------------------------------
namespace MemoryMapReader
{
    /*!
     *  Creates a new memory map from a given memory map node.
     *
     *      @param [in] memoryMapNode   XML description of the memory map.
     *
     *      @return The created memory map.
     */
    IPXACTMODELS_EXPORT QSharedPointer<MemoryMap> createMemoryMapFrom(QDomNode const& memoryMapNode, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Reads the memory remaps.
         *
         *      @param [in] memoryMapNode   XML description of the memory map.
         *      @param [in] newMemoryMap    The new memory map item.
         */
        void parseMemoryRemaps(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap, Document::Revision docRevision);

        /*!
         *  Creates a new memory remap from a given memory remap element.
         *
         *      @param [in] memoryRemapElement  XML description of the memory remap.
         *
         *      @return Pointer to the created memory remap.
         */
        QSharedPointer<MemoryRemap> createSingleMemoryRemap(QDomElement const& memoryRemapElement, Document::Revision docRevision);

        /*!
         *  Reads the address unit bits.
         *
         *      @param [in] memoryMapNode   XML description of the memory map.
         *      @param [in] newMemoryMap    The new memory map item.
         */
        void parseAddressUnitBits(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap);

        /*!
         *  Reads the shared value.
         *
         *      @param [in] memoryMapNode   XML description of the memory map.
         *      @param [in] newMemoryMap    The new memory map item.
         */
        void parseShared(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap);

        void parseDefinitionReference(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap, Document::Revision docRevision);
    }
}

#endif // MEMORYMAPREADER_H