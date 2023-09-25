//-----------------------------------------------------------------------------
// File: MemoryBlockBaseReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.01.2022
//
// Description:
// Base reader for memory map sub elements.
//-----------------------------------------------------------------------------

#include "MemoryBlockBaseReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterReader.h>

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parseNameGroup()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseNameGroup(QDomNode const& memoryBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock)
{
    NameGroupReader::parseNameGroup(memoryBlockNode, newMemoryBlock);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parsePresence()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parsePresence(QDomNode const& memoryBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock)
{
    QDomElement isPresentElement = memoryBlockNode.firstChildElement(QStringLiteral("ipxact:isPresent"));
    if (!isPresentElement.isNull())
    {
        QString isPresent = isPresentElement.firstChild().nodeValue();
        newMemoryBlock->setIsPresent(isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parseBaseAddress()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseBaseAddress(QDomNode const& memoryBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock)
{
    QDomElement baseAddressElement = memoryBlockNode.firstChildElement(QStringLiteral("ipxact:baseAddress"));
    if (!baseAddressElement.isNull())
    {
        QString baseAddress = baseAddressElement.firstChild().nodeValue();
        newMemoryBlock->setBaseAddress(baseAddress);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseParameters()
//-----------------------------------------------------------------------------
IPXACTMODELS_EXPORT void MemoryBlockBaseReader::parseParameters(QDomNode const& addressBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock,
    Document::Revision docRevision)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = 
        CommonItemsReader::parseAndCreateParameters(addressBlockNode);
    if (!newParameters->isEmpty())
    {
        newMemoryBlock->setParameters(newParameters);
    }
}
