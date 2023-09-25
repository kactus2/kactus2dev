//-----------------------------------------------------------------------------
// File: AddressBlockReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Reader class for ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#include "AddressBlockReader.h"
#include "AddressBlock.h"

#include "Register.h"
#include "RegisterFile.h"

#include "MemoryArrayReader.h"
#include "AccessPolicyReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/RegisterReader.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::createAddressBlockFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> AddressBlockReader::createAddressBlockFrom(QDomNode const& addressBlockNode, Document::Revision docRevision)
{
    QSharedPointer<AddressBlock> newAddressBlock(new AddressBlock());

    MemoryBlockBaseReader::parseNameGroup(addressBlockNode, newAddressBlock);
    
    if (docRevision == Document::Revision::Std14)
    {
        MemoryBlockBaseReader::parsePresence(addressBlockNode, newAddressBlock);
        
        Details::parseAccess(addressBlockNode, newAddressBlock);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::parseMisalignmentAllowed(addressBlockNode, newAddressBlock);

        Details::parseMemoryArray(addressBlockNode, newAddressBlock);

        Details::parseAddressBlockDefinitionRef(addressBlockNode, newAddressBlock);

        Details::parseAccessPolicies(addressBlockNode, newAddressBlock);
    }

    MemoryBlockBaseReader::parseBaseAddress(addressBlockNode, newAddressBlock);

    Details::parseTypeIdentifier(addressBlockNode, newAddressBlock);

    Details::parseRange(addressBlockNode, newAddressBlock);

    Details::parseWidth(addressBlockNode, newAddressBlock);

    Details::parseUsage(addressBlockNode, newAddressBlock);

    Details::parseVolatile(addressBlockNode, newAddressBlock);

    MemoryBlockBaseReader::parseParameters(addressBlockNode, newAddressBlock, docRevision);

    Details::parseRegisterData(addressBlockNode, newAddressBlock, docRevision);

    CommonItemsReader::parseVendorExtensions(addressBlockNode, newAddressBlock);

    return newAddressBlock;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseTypeIdentifier()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseTypeIdentifier(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock)
{
    QDomElement typeIdentifierElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:typeIdentifier"));
    if (!typeIdentifierElement.isNull())
    {
        QString typeIdentifier = typeIdentifierElement.firstChild().nodeValue();
        newAddressBlock->setTypeIdentifier(typeIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseRange()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseRange(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
   
{
    QString range = addressBlockNode.firstChildElement(QStringLiteral("ipxact:range")).firstChild().nodeValue();
    newAddressBlock->setRange(range);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseWidth()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseWidth(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
   
{
    QString width = addressBlockNode.firstChildElement(QStringLiteral("ipxact:width")).firstChild().nodeValue();
    newAddressBlock->setWidth(width);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseUsage()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseUsage(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
   
{
    QDomElement usageElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:usage"));
    if (!usageElement.isNull())
    {
        QString usageString = usageElement.firstChild().nodeValue();
        General::Usage usage = General::str2Usage(usageString, General::USAGE_COUNT);
        newAddressBlock->setUsage(usage);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseVolatile()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseVolatile(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock)
{
    QDomElement volatileElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:volatile"));
    if (!volatileElement.isNull())
    {
        QString volatileString = volatileElement.firstChild().nodeValue();
        if (volatileString == QLatin1String("true"))
        {
            newAddressBlock->setVolatile(true);
        }
        else if (volatileString == QLatin1String("false"))
        {
            newAddressBlock->setVolatile(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseAccess()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseAccess(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock)
{
    QDomElement accessElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:access"));
    if (!accessElement.isNull())
    {
        QString accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        newAddressBlock->setAccess(access);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseRegisterData()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseRegisterData(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock,
    Document::Revision docRevision)
{
    QDomNodeList childNodeList = addressBlockNode.childNodes();

    for (int i = 0; i < childNodeList.count(); ++i)
    {
        QDomNode currentNode = childNodeList.at(i);
        if (currentNode.nodeName() == QStringLiteral("ipxact:register"))
        {
            QSharedPointer<Register> newRegister = RegisterReader::createRegisterfrom(currentNode, docRevision);
            newAddressBlock->getRegisterData()->append(newRegister);
        }
        else if (childNodeList.at(i).nodeName() == QStringLiteral("ipxact:registerFile"))
        {
            QSharedPointer<RegisterFile> newFile = RegisterReader::createRegisterFileFrom(currentNode, docRevision);
            newAddressBlock->getRegisterData()->append(newFile);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseMemoryArray()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseMemoryArray(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
{
    auto memArrayElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:array"));

    if (!memArrayElement.isNull())
    {
        auto newMemoryArray = MemoryArrayReader::createMemoryArrayFrom(memArrayElement, false);

        newAddressBlock->setMemoryArray(newMemoryArray);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseMisalignmentAllowed()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseMisalignmentAllowed(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
{
    auto attributes = CommonItemsReader::parseAttributes(addressBlockNode);

    auto misalignmentAllowedStr = attributes.value(QStringLiteral("misalignmentAllowed"), QString());

    newAddressBlock->setMisalignmentAllowed(General::str2Bool(misalignmentAllowedStr, true));
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseAddressBlockDefinitionRef()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseAddressBlockDefinitionRef(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
{
    auto definitionRefElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:addressBlockDefinitionRef"));

    if (!definitionRefElement.isNull())
    {
        QString definitionRef = definitionRefElement.firstChild().nodeValue();
        QString typeDefinitionsRef = definitionRefElement.attribute(QStringLiteral("typeDefinitions"), QString());

        newAddressBlock->setAddressBlockDefinitionRef(definitionRef);
        newAddressBlock->setTypeDefinitionsRef(typeDefinitionsRef);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::Details::parseAccessPolicies()
//-----------------------------------------------------------------------------
void AddressBlockReader::Details::parseAccessPolicies(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
{
    auto accessPoliciesElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:accessPolicies"));

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

            newAddressBlock->getAccessPolicies()->append(newAccessPolicy);
        }
    }
}
