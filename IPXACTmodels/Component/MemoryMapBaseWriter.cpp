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
#include <IPXACTmodels/Component/AddressBlockWriter.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::MemoryMapBaseWriter()
//-----------------------------------------------------------------------------
MemoryMapBaseWriter::MemoryMapBaseWriter(QObject* parent /* = 0 */):
CommonItemsWriter(parent)
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
void MemoryMapBaseWriter::writeMemoryMapBase(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase) const
{
    writeNameGroup(writer, MemoryMapBase);

    writeIsPresent(writer, MemoryMapBase->getIsPresent());

    writeMemoryBlocks(writer, MemoryMapBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase) const
{
    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, MemoryMapBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseWriter::writeMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseWriter::writeMemoryBlocks(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase) const
{
    if (!MemoryMapBase->getMemoryBlocks()->isEmpty())
    {
        AddressBlockWriter addressBlockWriter;

        foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *MemoryMapBase->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                addressBlockWriter.writeAddressBlock(writer, addressBlock);
            }
        }
    }
}