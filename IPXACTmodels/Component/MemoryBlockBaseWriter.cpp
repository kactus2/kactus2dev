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
#include <IPXACTmodels/Component/AccessPolicyWriter.h>
#include <IPXACTmodels/Component/AccessHandleWriter.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeNameGroup(QXmlStreamWriter& writer, 
    QSharedPointer<MemoryBlockBase> memoryBlock, Document::Revision docRevision)
{
    NameGroupWriter::writeNameGroup(writer, memoryBlock, docRevision);
}

void MemoryBlockBaseWriter::writeAccessHandles(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock, Document::Revision docRevision)
{
    if (memoryBlock->getAccessHandles()->isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:accessHandles"));

        for (auto const& accessHandle : *memoryBlock->getAccessHandles())
        {
            AccessHandleWriter::writeAccessHandle(writer, accessHandle, docRevision);
        }

        writer.writeEndElement(); // ipxact:accessHandles
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeUsage()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeUsage(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock)
{
    if (memoryBlock->getUsage() != General::USAGE_COUNT)
    {
        QString usageString = General::usage2Str(memoryBlock->getUsage());
        writer.writeTextElement(QStringLiteral("ipxact:usage"), usageString);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeVolatile()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeVolatile(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock)
{
    if (!memoryBlock->getVolatile().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:volatile"), memoryBlock->getVolatile());
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeAccess()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeAccess(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock)
{
    if (memoryBlock->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString accessString = AccessTypes::access2Str(memoryBlock->getAccess());
        writer.writeTextElement(QStringLiteral("ipxact:access"), accessString);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeAccessPolicies()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeAccessPolicies(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock)
{
    if (auto accessPolicies = memoryBlock->getAccessPolicies();
        accessPolicies->isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:accessPolicies"));

        for (auto const& accessPolicy : *memoryBlock->getAccessPolicies())
        {
            AccessPolicyWriter::writeAccessPolicy(writer, accessPolicy);
        }

        writer.writeEndElement(); // ipxact:accessPolicies
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseWriter::writeBaseAddress()
//-----------------------------------------------------------------------------
void MemoryBlockBaseWriter::writeBaseAddress(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:baseAddress"), memoryBlock->getBaseAddress());
}
