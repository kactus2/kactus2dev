//-----------------------------------------------------------------------------
// File: MemoryBlockBaseReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.01.2022
//
// Description:
// Base reader class for memory map sub elements.
//-----------------------------------------------------------------------------

#include "MemoryBlockBaseReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterReader.h>

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::MemoryBlockBaseReader()
//-----------------------------------------------------------------------------
MemoryBlockBaseReader::MemoryBlockBaseReader()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parseNameGroup()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseNameGroup(QDomNode const& memoryBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock) const
{
    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(memoryBlockNode, newMemoryBlock);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parsePresence()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parsePresence(QDomNode const& memoryBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock) const
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
    QSharedPointer<MemoryBlockBase> newMemoryBlock) const
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
void MemoryBlockBaseReader::parseParameters(QDomNode const& addressBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock) const
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = parseAndCreateParameters(addressBlockNode);
    if (!newParameters->isEmpty())
    {
        newMemoryBlock->setParameters(newParameters);
    }
}
