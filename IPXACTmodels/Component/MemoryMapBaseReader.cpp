//-----------------------------------------------------------------------------
// File: MemoryMapBaseReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Reader class for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------

#include "MemoryMapBaseReader.h"
#include "MemoryMapBase.h"

#include "AddressBlock.h"
#include "MemoryBlockBase.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/AddressBlockReader.h>
#include <IPXACTmodels/Component/SubspaceMapReader.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::MemoryMapBaseReader()
//-----------------------------------------------------------------------------
MemoryMapBaseReader::MemoryMapBaseReader(): CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::~MemoryMapBaseReader()
//-----------------------------------------------------------------------------
MemoryMapBaseReader::~MemoryMapBaseReader()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::createMemoryMapBaseFrom()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::readMemoryMapBase(QDomNode const& MemoryMapBaseNode,
	QSharedPointer<MemoryMapBase> newMemoryMapBase) const
{
    parseNameGroup(MemoryMapBaseNode, newMemoryMapBase);

    parsePresence(MemoryMapBaseNode, newMemoryMapBase);

    parseMemoryBlocks(MemoryMapBaseNode, newMemoryMapBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::parseNameGroup()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::parseNameGroup(QDomNode const& MemoryMapBaseBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBaseBase) const
{
    NameGroupReader::parseNameGroup(MemoryMapBaseBaseNode, newMemoryMapBaseBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::parsePresence()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::parsePresence(QDomNode const& MemoryMapBaseBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBaseBase) const
{
    QDomNode isPresentNode = MemoryMapBaseBaseNode.firstChildElement(QStringLiteral("ipxact:isPresent"));
    if (!isPresentNode.isNull())
    {
        QString isPresent = isPresentNode.firstChild().nodeValue();
        newMemoryMapBaseBase->setIsPresent(isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::parseMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::parseMemoryBlocks(QDomNode const& memoryMapBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBase) const
{
    QDomNodeList childNodes = memoryMapBaseNode.childNodes();

    AddressBlockReader addressBlockReader;
    SubspaceMapReader subspaceMapReader;

    for (int childIndex = 0; childIndex < childNodes.count(); ++childIndex)
    {
        QDomNode blockBaseNode = childNodes.at(childIndex);
        if (blockBaseNode.nodeName() == QLatin1String("ipxact:addressBlock"))
        {
            QSharedPointer<AddressBlock> newAddressBlock =
                addressBlockReader.createAddressBlockFrom(blockBaseNode);

            newMemoryMapBase->getMemoryBlocks()->append(newAddressBlock);
        }
        else if (blockBaseNode.nodeName() == QLatin1String("ipxact:subspaceMap"))
        {
            QSharedPointer<SubSpaceMap> newSubSpaceMap = subspaceMapReader.createSubspaceMapFrom(blockBaseNode);

            newMemoryMapBase->getMemoryBlocks()->append(newSubSpaceMap);
        }
    }
}
