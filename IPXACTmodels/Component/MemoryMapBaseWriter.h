//-----------------------------------------------------------------------------
// File: MemoryMapBaseWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Writer for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------

#ifndef MemoryMapBaseWRITER_H
#define MemoryMapBaseWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class MemoryMapBase;
class MemoryMapBaseBase;

//-----------------------------------------------------------------------------
//! Writer for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------
namespace MemoryMapBaseWriter
{
    /*!
     *  Write a memory map to an XML file.
     *
     *    @param [in] writer          The used XML writer.
     *    @param [in] MemoryMapBase   The memory map to be written.
     *    @param [in] docRevision     The standard revision in use.
     */
    IPXACTMODELS_EXPORT void writeMemoryMapBase(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase,
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Write the name group.
         *
         *    @param [in] writer              Used XML writer.
         *    @param [in] MemoryMapBaseBase   The selected memory map base.
         *    @param [in] docRevision         The standard revision in use.
         */
        void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase,
            Document::Revision docRevision);

        /*!
         *  Write the memory blocks.
         *
         *    @param [in] writer              Used XML writer.
         *    @param [in] MemoryMapBaseBase   The selected memory map base.
         */
        void writeMemoryBlocks(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase, Document::Revision docRevision);
    }
}

#endif // MemoryMapBaseWRITER_H