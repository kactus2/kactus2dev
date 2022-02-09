//-----------------------------------------------------------------------------
// File: MemoryBlockBaseWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.01.2022
//
// Description:
// Writer class for base memory block element.
//-----------------------------------------------------------------------------

#include "MemoryBlockBaseWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::MemoryBlockBaseWriter()
//-----------------------------------------------------------------------------
MemoryBlockBaseWriter::MemoryBlockBaseWriter():
CommonItemsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeNameGroup(QXmlStreamWriter& writer,
    QSharedPointer<MemoryBlockBase> memoryBlock) const
{
    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, memoryBlock);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeBaseAddress()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeBaseAddress(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock)
const
{
    writer.writeTextElement(QStringLiteral("ipxact:baseAddress"), memoryBlock->getBaseAddress());
}
