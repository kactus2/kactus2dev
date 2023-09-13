//-----------------------------------------------------------------------------
// File: MemoryMapBaseWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Writer class for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------

#include "MemoryMapBaseWriter.h"
#include "MemoryMapBase.h"
#include "AddressBlock.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>
#include <IPXACTmodels/Component/AddressBlockWriter.h>
#include <IPXACTmodels/Component/SubSpaceMapWriter.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::MemoryMapBaseWriter()
//-----------------------------------------------------------------------------
MemoryMapBaseWriter::MemoryMapBaseWriter(): CommonItemsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::~MemoryMapBaseWriter()
//-----------------------------------------------------------------------------
MemoryMapBaseWriter::~MemoryMapBaseWriter()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeMemoryMapBase()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::writeMemoryMapBase(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase,
    Document::Revision docRevision) const
{
    NameGroupWriter::writeNameGroup(writer, MemoryMapBase, docRevision);

    writeIsPresent(writer, MemoryMapBase->getIsPresent());

    writeMemoryBlocks(writer, MemoryMapBase, docRevision);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase,
    Document::Revision docRevision) const
{
    NameGroupWriter::writeNameGroup(writer, MemoryMapBase, docRevision);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::writeMemoryBlocks(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase, Document::Revision docRevision) const
{
    if (!MemoryMapBase->getMemoryBlocks()->isEmpty())
    {
        for (auto const& memoryBlock : *MemoryMapBase->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                AddressBlockWriter::writeAddressBlock(writer, addressBlock, docRevision);
            }

            QSharedPointer<SubSpaceMap> subspaceMap = memoryBlock.dynamicCast<SubSpaceMap>();
            if (subspaceMap)
            {
                SubSpaceMapWriter::writeSubSpaceMap(writer, subspaceMap, docRevision);
            }
        }
    }
}
