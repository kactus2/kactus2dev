//-----------------------------------------------------------------------------
// File: AccessPolicyReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Reader for ipxact:accessPolicy.
//-----------------------------------------------------------------------------

#include "AccessPolicyReader.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: AccessPolicyReader::createAccessPolicyFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AccessPolicy> AccessPolicyReader::createAccessPolicyFrom(QDomNode const& accessPolicyNode)
{
    QSharedPointer<AccessPolicy> accessPolicy(new AccessPolicy());

    Details::parseModeRefs(accessPolicyNode, accessPolicy->getModeReferences());

    Details::parseAccess(accessPolicyNode, accessPolicy);

    CommonItemsReader::parseVendorExtensions(accessPolicyNode, accessPolicy);

    return accessPolicy;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyReader::Details::parseModeRefs()
//-----------------------------------------------------------------------------
void AccessPolicyReader::Details::parseModeRefs(QDomNode const& rootNode, 
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefList)
{
    auto modeRefNodes = rootNode.childNodes();

    for (int i = 0; i < modeRefNodes.size(); ++i)
    {
        if (auto const& modeRefNode = modeRefNodes.at(i);
            modeRefNode.nodeName() == QStringLiteral("ipxact:modeRef"))
        {
            QSharedPointer<ModeReference> newModeRef(new ModeReference());

            newModeRef->setReference(modeRefNode.firstChild().nodeValue());
            
            if (auto const& priority = modeRefNode.attributes().namedItem(QStringLiteral("priority")).nodeValue();
                !priority.isEmpty())
            {
                newModeRef->setPriority(priority);
            }

            modeRefList->append(newModeRef);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AccessPolicyReader::Details::parseAccess()
//-----------------------------------------------------------------------------
void AccessPolicyReader::Details::parseAccess(QDomNode const& accessPolicyNode,
    QSharedPointer<AccessPolicy> accessPolicy)
{
    auto accessElement = accessPolicyNode.firstChildElement(QStringLiteral("ipxact:access"));
    if (!accessElement.isNull())
    {
        auto accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        accessPolicy->setAccess(access);
    }
}
