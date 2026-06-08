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
#include <IPXACTmodels/Component/AccessPolicyReader.h>
#include <IPXACTmodels/Component/AccessHandleReader.h>

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
// Function: MemoryBlockBaseReader::parseAccessHandles()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseAccessHandles(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock, AccessHandle::ElementType elementType, Document::Revision docRevision)
{
    if (elementType != AccessHandle::ElementType::AddressBlock && elementType != AccessHandle::ElementType::Bank)
    {
        return;
    }

    QDomElement accessHandlesElement = memoryBlockNode.firstChildElement(QStringLiteral("ipxact:accessHandles"));
    if (!accessHandlesElement.isNull())
    {
        QSharedPointer<QList<QSharedPointer<AccessHandle> > > accessHandles(new QList<QSharedPointer<AccessHandle> >());

        auto accessHandleNodes = accessHandlesElement.elementsByTagName(QStringLiteral("ipxact:accessHandle"));
        for (int i = 0; i < accessHandleNodes.size(); ++i)
        {
            auto accessHandleNode = accessHandleNodes.at(i);
            auto newAccessHandle = AccessHandleReader::createAccessHandleFrom(accessHandleNode, elementType, docRevision);
            accessHandles->append(newAccessHandle);
        }

        newMemoryBlock->setAccessHandles(accessHandles);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parseUsage()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseUsage(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock)
{
    QDomElement usageElement = memoryBlockNode.firstChildElement(QStringLiteral("ipxact:usage"));
    if (!usageElement.isNull())
    {
        QString usageString = usageElement.firstChild().nodeValue();
        General::Usage usage = General::str2Usage(usageString, General::USAGE_COUNT);
        newMemoryBlock->setUsage(usage);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parseVolatile()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseVolatile(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock)
{
    QDomElement volatileElement = memoryBlockNode.firstChildElement(QStringLiteral("ipxact:volatile"));
    if (!volatileElement.isNull())
    {
        QString volatileString = volatileElement.firstChild().nodeValue();
        if (volatileString == QLatin1String("true"))
        {
            newMemoryBlock->setVolatile(true);
        }
        else if (volatileString == QLatin1String("false"))
        {
            newMemoryBlock->setVolatile(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parseAccess()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseAccess(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock)
{
    QDomElement accessElement = memoryBlockNode.firstChildElement(QStringLiteral("ipxact:access"));
    if (!accessElement.isNull())
    {
        QString accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        newMemoryBlock->setAccess(access);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBaseReader::parseAccessPolicies()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseAccessPolicies(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock)
{
    auto accessPoliciesElement = memoryBlockNode.firstChildElement(QStringLiteral("ipxact:accessPolicies"));

    if (accessPoliciesElement.isNull())
    {
        return;
    }

    auto accessPolicies = accessPoliciesElement.childNodes();

    for (int i = 0; i < accessPolicies.size(); ++i)
    {
        if (accessPolicies.at(i).nodeName() == QStringLiteral("ipxact:accessPolicy"))
        {
            auto newAccessPolicy = AccessPolicyReader::createAccessPolicyFrom(accessPolicies.at(i));

            newMemoryBlock->getAccessPolicies()->append(newAccessPolicy);
        }
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
// Function: MemoryBlockBaseReader::parseParameters()
//-----------------------------------------------------------------------------
void MemoryBlockBaseReader::parseParameters(QDomNode const& memoryBlockNode,
    QSharedPointer<MemoryBlockBase> newMemoryBlock,
    Document::Revision docRevision)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = 
        CommonItemsReader::parseAndCreateParameters(memoryBlockNode, docRevision);

    if (!newParameters->isEmpty())
    {
        newMemoryBlock->setParameters(newParameters);
    }
}
