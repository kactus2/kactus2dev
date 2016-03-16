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

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::MemoryMapBaseReader()
//-----------------------------------------------------------------------------
MemoryMapBaseReader::MemoryMapBaseReader(QObject* parent /* = 0 */):
CommonItemsReader(parent)
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
    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(MemoryMapBaseBaseNode, newMemoryMapBaseBase);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::parsePresence()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::parsePresence(QDomNode const& MemoryMapBaseBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBaseBase) const
{
    QDomNode isPresentNode = MemoryMapBaseBaseNode.firstChildElement("ipxact:isPresent");
    if (!isPresentNode.isNull())
    {
        QString isPresent = isPresentNode.firstChild().nodeValue();
        newMemoryMapBaseBase->setIsPresent(isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBaseReader::parseMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBaseReader::parseMemoryBlocks(QDomNode const& MemoryMapBaseBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBaseBase) const
{
    QDomNodeList childNodes = MemoryMapBaseBaseNode.childNodes();

    AddressBlockReader addressBlockReader;

    for (int childIndex = 0; childIndex < childNodes.count(); ++childIndex)
    {
        QDomNode addressBlockNode = childNodes.at(childIndex);
        if (addressBlockNode.nodeName() == "ipxact:addressBlock")
        {
            QSharedPointer<AddressBlock> newAddressBlock =
                addressBlockReader.createAddressBlockFrom(addressBlockNode);

            newMemoryMapBaseBase->getMemoryBlocks()->append(newAddressBlock);
        }
    }
}