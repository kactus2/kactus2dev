//-----------------------------------------------------------------------------
// File: SubspaceMapReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.01.2022
//
// Description:
// Reader class for ipxact:subspaceMap element.
//-----------------------------------------------------------------------------

#include "SubspaceMapReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>

//-----------------------------------------------------------------------------
// Function: SubspaceMapReader::SubspaceMapReader()
//-----------------------------------------------------------------------------
SubspaceMapReader::SubspaceMapReader() :
MemoryBlockBaseReader()
{

}

//-----------------------------------------------------------------------------
// Function: SubspaceMapReader::createMemoryBlockFrom()
//-----------------------------------------------------------------------------
QSharedPointer<SubSpaceMap> SubspaceMapReader::createSubspaceMapFrom(QDomNode const& subspaceMapNode) const
{
    QSharedPointer<SubSpaceMap> newSubspaceMap(new SubSpaceMap());

    parseNameGroup(subspaceMapNode, newSubspaceMap);

    parseAttributes(subspaceMapNode, newSubspaceMap);

    parseNameGroup(subspaceMapNode, newSubspaceMap);

    parsePresence(subspaceMapNode, newSubspaceMap);

    parseBaseAddress(subspaceMapNode, newSubspaceMap);

    parseParameters(subspaceMapNode, newSubspaceMap);

    parseVendorExtensions(subspaceMapNode, newSubspaceMap);

    return newSubspaceMap;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapReader::parseAttributes()
//-----------------------------------------------------------------------------
void SubspaceMapReader::parseAttributes(QDomNode const& subspaceMapNode,
    QSharedPointer<SubSpaceMap> newSubspaceMap) const
{
    QDomNamedNodeMap attributes = subspaceMapNode.attributes();
    for (int j = 0; j < attributes.size(); ++j)
    {
        QString attributeName = attributes.item(j).nodeName();
        QString attributeValue = attributes.item(j).nodeValue();

        if (attributeName == QStringLiteral("masterRef"))
        {
            newSubspaceMap->setMasterReference(attributeValue);
        }
        else if (attributeName == QStringLiteral("segmentRef"))
        {
            newSubspaceMap->setSegmentReference(attributeValue);
        }
    }

}
