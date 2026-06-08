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

    MemoryBlockBaseReader::parseAccessHandles(addressBlockNode, newAddressBlock, AccessHandle::ElementType::AddressBlock, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        MemoryBlockBaseReader::parsePresence(addressBlockNode, newAddressBlock);
        
        MemoryBlockBaseReader::parseAccess(addressBlockNode, newAddressBlock);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::parseMisalignmentAllowed(addressBlockNode, newAddressBlock);

        Details::parseMemoryArray(addressBlockNode, newAddressBlock);

        Details::parseAddressBlockDefinitionRef(addressBlockNode, newAddressBlock);

        MemoryBlockBaseReader::parseAccessPolicies(addressBlockNode, newAddressBlock);
    }

    MemoryBlockBaseReader::parseBaseAddress(addressBlockNode, newAddressBlock);

    Details::parseTypeIdentifier(addressBlockNode, newAddressBlock);

    Details::parseRange(addressBlockNode, newAddressBlock);

    Details::parseWidth(addressBlockNode, newAddressBlock);

    MemoryBlockBaseReader::parseUsage(addressBlockNode, newAddressBlock);

    MemoryBlockBaseReader::parseVolatile(addressBlockNode, newAddressBlock);

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
        auto newMemoryArray = MemoryArrayReader::createMemoryArrayFrom(memArrayElement, Document::Revision::Std22, false);

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
