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
#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/Component/AddressBlockReader.h>
#include <IPXACTmodels/Component/SubspaceMapReader.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::createMemoryMapBaseFrom()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::readMemoryMapBase(QDomNode const& MemoryMapBaseNode,
	QSharedPointer<MemoryMapBase> newMemoryMapBase,
	Document::Revision docRevision)
{
    NameGroupReader::parseNameGroup(MemoryMapBaseNode, newMemoryMapBase);

    if (docRevision == Document::Revision::Std14)
    {
        Details::parsePresence(MemoryMapBaseNode, newMemoryMapBase);
    }

    Details::parseMemoryBlocks(MemoryMapBaseNode, newMemoryMapBase, docRevision);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::Details::parseNameGroup()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::Details::parseNameGroup(QDomNode const& MemoryMapBaseBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBaseBase)
{
    NameGroupReader::parseNameGroup(MemoryMapBaseBaseNode, newMemoryMapBaseBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::Details::parsePresence()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::Details::parsePresence(QDomNode const& MemoryMapBaseBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBaseBase)
{
    QDomNode isPresentNode = MemoryMapBaseBaseNode.firstChildElement(QStringLiteral("ipxact:isPresent"));
    if (!isPresentNode.isNull())
    {
        QString isPresent = isPresentNode.firstChild().nodeValue();
        newMemoryMapBaseBase->setIsPresent(isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::Details::parseMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::Details::parseMemoryBlocks(QDomNode const& memoryMapBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBase,
    Document::Revision docRevision)
{
    QDomNodeList childNodes = memoryMapBaseNode.childNodes();

    for (int childIndex = 0; childIndex < childNodes.count(); ++childIndex)
    {
        QDomNode blockBaseNode = childNodes.at(childIndex);
        if (blockBaseNode.nodeName() == QLatin1String("ipxact:addressBlock"))
        {
            QSharedPointer<AddressBlock> newAddressBlock =
                AddressBlockReader::createAddressBlockFrom(blockBaseNode, docRevision);

            newMemoryMapBase->getMemoryBlocks()->append(newAddressBlock);
        }
        else if (blockBaseNode.nodeName() == QLatin1String("ipxact:subspaceMap"))
        {
            QSharedPointer<SubSpaceMap> newSubSpaceMap = SubspaceMapReader::createSubspaceMapFrom(blockBaseNode, docRevision);

            newMemoryMapBase->getMemoryBlocks()->append(newSubSpaceMap);
        }
    }
}
