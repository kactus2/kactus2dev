//-----------------------------------------------------------------------------
// File: MemoryMapWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Writer for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPWRITER_H
#define MEMORYMAPWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class MemoryMap;
class MemoryRemap;
class MemoryMapBase;

//-----------------------------------------------------------------------------
//! Writer for ipxact:memoryMap element.
//-----------------------------------------------------------------------------
namespace MemoryMapWriter
{

    /*!
     *  Write a memory map to an XML file.
     *
     *    @param [in] writer        The used XML writer.
     *    @param [in] memoryMap     The memory map to be written.
     *    @param [in] docRevision   The standard revision in use.
     */
    IPXACTMODELS_EXPORT void writeMemoryMap(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Write the memory remaps.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] memoryMap   The selected memory map.
         *    @param [in] docRevision     The standard revision in use.
         */
        void writeMemoryRemaps(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap, Document::Revision docRevision);

        /*!
         *  Write the address unit bits.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] memoryMap   The selected memory map.


         */
        void writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap);

        /*!
         *  Write the shared value.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] memoryMap   The selected memory map.
         */
        void writeShared(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap);

        /*!
         *	Write the definition reference of the memory map.
         *  
         *    @param [in] writer      Used XML writer.
         *    @param [in] memoryMap   The selected memory map.
         */
        void writeMemoryMapDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<MemoryMap> memoryMap);

        /*!
         *	Write the definition reference of the memory remap.
         *
         *    @param [in] writer        Used XML writer.
         *    @param [in] memoryRemap   The selected memory remap.
         */
        void writeRemapDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<MemoryRemap> memoryRemap);
    }
}

#endif // MEMORYMAPWRITER_H