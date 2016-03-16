//-----------------------------------------------------------------------------
// File: MemoryMapReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Reader class for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#include "MemoryMapReader.h"
#include "MemoryMap.h"
#include "MemoryRemap.h"

#include "AddressBlock.h"
#include "MemoryBlockBase.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/AddressBlockReader.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::MemoryMapReader()
//-----------------------------------------------------------------------------
MemoryMapReader::MemoryMapReader(QObject* parent /* = 0 */):
MemoryMapBaseReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::~MemoryMapReader()
//-----------------------------------------------------------------------------
MemoryMapReader::~MemoryMapReader()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::createMemoryMapFrom()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> MemoryMapReader::createMemoryMapFrom(QDomNode const& memoryMapNode) const
{
    QSharedPointer<MemoryMap> newMemoryMap(new MemoryMap());

	readMemoryMapBase(memoryMapNode, newMemoryMap);

    parseMemoryRemaps(memoryMapNode, newMemoryMap);

    parseAddressUnitBits(memoryMapNode, newMemoryMap);

    parseShared(memoryMapNode, newMemoryMap);

    parseVendorExtensions(memoryMapNode, newMemoryMap);

    return newMemoryMap;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::parseMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapReader::parseMemoryBlocks(QDomNode const& memoryMapBaseNode,
    QSharedPointer<MemoryMapBase> newMemoryMapBase) const
{
    QDomNodeList childNodes = memoryMapBaseNode.childNodes();

    AddressBlockReader addressBlockReader;

    for (int childIndex = 0; childIndex < childNodes.count(); ++childIndex)
    {
        QDomNode addressBlockNode = childNodes.at(childIndex);
        if (addressBlockNode.nodeName() == "ipxact:addressBlock")
        {
            QSharedPointer<AddressBlock> newAddressBlock =
                addressBlockReader.createAddressBlockFrom(addressBlockNode);

            newMemoryMapBase->getMemoryBlocks()->append(newAddressBlock);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::parseMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMapReader::parseMemoryRemaps(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap)
    const
{
    QDomNodeList memoryRemapNodeList = memoryMapNode.toElement().elementsByTagName("ipxact:memoryRemap");

    for (int memoryRemapIndex = 0; memoryRemapIndex < memoryRemapNodeList.count(); ++memoryRemapIndex)
    {
        QDomElement remapElement = memoryRemapNodeList.at(memoryRemapIndex).toElement();

        QSharedPointer<MemoryRemap> newMemoryRemap = createSingleMemoryRemap(remapElement);

        newMemoryMap->getMemoryRemaps()->append(newMemoryRemap);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::parseSingleMemoryRemap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryRemap> MemoryMapReader::createSingleMemoryRemap(QDomElement const& memoryRemapElement) const
{
    QSharedPointer<MemoryRemap> newMemoryRemap (new MemoryRemap());

    QString remapState = memoryRemapElement.attribute("state");
    newMemoryRemap->setRemapState(remapState);

    parseNameGroup(memoryRemapElement, newMemoryRemap);

    parsePresence(memoryRemapElement, newMemoryRemap);

    parseMemoryBlocks(memoryRemapElement, newMemoryRemap);

    return newMemoryRemap;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::parseAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryMapReader::parseAddressUnitBits(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap)
    const
{
    QDomNode addressUnitBitsNode = memoryMapNode.firstChildElement("ipxact:addressUnitBits");
    if (!addressUnitBitsNode.isNull())
    {
        QString addressUnitBits = addressUnitBitsNode.firstChild().nodeValue();
        newMemoryMap->setAddressUnitBits(addressUnitBits);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::parseShared()
//-----------------------------------------------------------------------------
void MemoryMapReader::parseShared(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap) const
{
    QDomNode sharedNode = memoryMapNode.firstChildElement("ipxact:shared");
    if (!sharedNode.isNull())
    {
        QString sharedValue = sharedNode.firstChild().nodeValue();
        if (sharedValue == "yes")
        {
            newMemoryMap->setShared(true);
        }
        else if (sharedValue == "no")
        {
            newMemoryMap->setShared(false);
        }
    }
}