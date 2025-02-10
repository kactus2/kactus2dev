//-----------------------------------------------------------------------------
// File: MemoryBlockBaseWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.01.2022
//
// Description:
// Writer for base memory block element.
//-----------------------------------------------------------------------------

#include "MemoryBlockBaseWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeNameGroup(QXmlStreamWriter& writer, 
    QSharedPointer<MemoryBlockBase> memoryBlock, Document::Revision docRevision)
{
    NameGroupWriter::writeNameGroup(writer, memoryBlock, docRevision);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeBaseAddress()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeBaseAddress(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock)
{
    writer.writeTextElement(QStringLiteral("ipxact:baseAddress"), memoryBlock->getBaseAddress());
}
