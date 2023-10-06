//-----------------------------------------------------------------------------
// File: MemoryMapReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Reader for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#include "MemoryMapReader.h"
#include "MemoryMapBaseReader.h"
#include "MemoryMap.h"
#include "MemoryRemap.h"

#include "AddressBlock.h"
#include "MemoryBlockBase.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/AddressBlockReader.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::createMemoryMapFrom()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> MemoryMapReader::createMemoryMapFrom(QDomNode const& memoryMapNode, Document::Revision docRevision)
{
    QSharedPointer<MemoryMap> newMemoryMap(new MemoryMap());

	MemoryMapBaseReader::readMemoryMapBase(memoryMapNode, newMemoryMap, docRevision);

    Details::parseDefinitionReference(memoryMapNode, newMemoryMap, docRevision);

    Details::parseMemoryRemaps(memoryMapNode, newMemoryMap, docRevision);

    Details::parseAddressUnitBits(memoryMapNode, newMemoryMap);

    Details::parseShared(memoryMapNode, newMemoryMap);

    CommonItemsReader::parseVendorExtensions(memoryMapNode, newMemoryMap);

    return newMemoryMap;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::Details::parseMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMapReader::Details::parseMemoryRemaps(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap, Document::Revision docRevision)
{
    QDomNodeList memoryRemapNodeList = memoryMapNode.toElement().elementsByTagName(QStringLiteral("ipxact:memoryRemap"));

    for (int memoryRemapIndex = 0; memoryRemapIndex < memoryRemapNodeList.count(); ++memoryRemapIndex)
    {
        QDomElement remapElement = memoryRemapNodeList.at(memoryRemapIndex).toElement();

        QSharedPointer<MemoryRemap> newMemoryRemap = createSingleMemoryRemap(remapElement, docRevision);

        newMemoryMap->getMemoryRemaps()->append(newMemoryRemap);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::Details::parseSingleMemoryRemap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryRemap> MemoryMapReader::Details::createSingleMemoryRemap(QDomElement const& memoryRemapElement, Document::Revision docRevision)
{
    QSharedPointer<MemoryRemap> newMemoryRemap (new MemoryRemap());

    if (docRevision == Document::Revision::Std14)
    {
        QString remapState = memoryRemapElement.attribute(QStringLiteral("state"));
        newMemoryRemap->setRemapState(remapState);
        MemoryMapBaseReader::Details::parsePresence(memoryRemapElement, newMemoryRemap);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        newMemoryRemap->setModeReferences(CommonItemsReader::parseModeReferences(memoryRemapElement));

        auto definitionRefElement = memoryRemapElement.firstChildElement(
            QStringLiteral("ipxact:remapDefinitionRef"));

        newMemoryRemap->setMemoryRemapDefinitionReference(definitionRefElement.firstChild().nodeValue());

        newMemoryRemap->setTypeDefinitionsReference(definitionRefElement.attribute(
            QStringLiteral("typeDefinitions")));
    }

    NameGroupReader::parseNameGroup(memoryRemapElement, newMemoryRemap);

    MemoryMapBaseReader::Details::parseMemoryBlocks(memoryRemapElement, newMemoryRemap, docRevision);

    return newMemoryRemap;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::Details::parseAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryMapReader::Details::parseAddressUnitBits(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap)
   
{
    QDomNode addressUnitBitsNode = memoryMapNode.firstChildElement(QStringLiteral("ipxact:addressUnitBits"));
    if (!addressUnitBitsNode.isNull())
    {
        QString addressUnitBits = addressUnitBitsNode.firstChild().nodeValue();
        newMemoryMap->setAddressUnitBits(addressUnitBits);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::Details::parseShared()
//-----------------------------------------------------------------------------
void MemoryMapReader::Details::parseShared(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap)
{
    QDomNode sharedNode = memoryMapNode.firstChildElement(QStringLiteral("ipxact:shared"));
    if (!sharedNode.isNull())
    {
        QString sharedValue = sharedNode.firstChild().nodeValue();
        if (sharedValue == QLatin1String("yes"))
        {
            newMemoryMap->setShared(true);
        }
        else if (sharedValue == QLatin1String("no"))
        {
            newMemoryMap->setShared(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapReader::Details::parseDefinitionReference()
//-----------------------------------------------------------------------------
void MemoryMapReader::Details::parseDefinitionReference(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap, Document::Revision docRevision)
{
    if (docRevision == Document::Revision::Std22)
    {
        auto definitionRefElement = memoryMapNode.firstChildElement(QStringLiteral("ipxact:memoryMapDefinitionRef"));

        newMemoryMap->setMemoryMapDefinitionReference(definitionRefElement.firstChild().nodeValue());
        newMemoryMap->setTypeDefinitionsReference(definitionRefElement.attribute(QStringLiteral("typeDefinitions")));
    }
}
