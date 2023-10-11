//-----------------------------------------------------------------------------
// File: MemoryMapBaseWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Writer for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------

#include "MemoryMapBaseWriter.h"
#include "MemoryMapBase.h"
#include "AddressBlock.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>
#include <IPXACTmodels/Component/AddressBlockWriter.h>
#include <IPXACTmodels/Component/SubSpaceMapWriter.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeMemoryMapBase()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::writeMemoryMapBase(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase,
    Document::Revision docRevision)
{
    NameGroupWriter::writeNameGroup(writer, MemoryMapBase, docRevision);

    CommonItemsWriter::writeIsPresent(writer, MemoryMapBase->getIsPresent());

    Details::writeMemoryBlocks(writer, MemoryMapBase, docRevision);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::Details::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase,
    Document::Revision docRevision)
{
    NameGroupWriter::writeNameGroup(writer, MemoryMapBase, docRevision);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::Details::writeMemoryBlocks(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase, Document::Revision docRevision)
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

            else if (QSharedPointer<SubSpaceMap> subspaceMap = memoryBlock.dynamicCast<SubSpaceMap>())
            {
                SubSpaceMapWriter::writeSubSpaceMap(writer, subspaceMap, docRevision);
            }
        }
    }
}
