//-----------------------------------------------------------------------------
// File: BankReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.06.2026
//
// Description:
// Reader for ipxact:bank elements.
//-----------------------------------------------------------------------------

#include "BankReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/AccessHandleReader.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

#include <IPXACTmodels/Component/MemoryBlockBaseReader.h>
#include <IPXACTmodels/Component/AddressBlockReader.h>
#include <IPXACTmodels/Component/SubspaceMapReader.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/MemoryArray.h>

QSharedPointer<Bank> BankReader::createBankFrom(QDomNode const& bankNode, Document::Revision docRevision)
{
    QSharedPointer<Bank> newBank(new Bank());

    MemoryBlockBaseReader::parseNameGroup(bankNode, newBank);

    Details::parseBankAlignment(bankNode, newBank);

    MemoryBlockBaseReader::parseAccessHandles(bankNode, newBank, AccessHandle::ElementType::Bank, docRevision);

    MemoryBlockBaseReader::parseBaseAddress(bankNode, newBank);

    if (docRevision == Document::Revision::Std22)
    {
        Details::parseBankDefinitionRef(bankNode, newBank);
    }
    else
    {
        MemoryBlockBaseReader::parsePresence(bankNode, newBank);
    }

    Details::parseAddressBlocks(bankNode, newBank, docRevision);
    Details::parseBanks(bankNode, newBank, docRevision);
    Details::parseSubspaces(bankNode, newBank, docRevision);

    if (docRevision == Document::Revision::Std22)
    {
        MemoryBlockBaseReader::parseAccessPolicies(bankNode, newBank);
    }
    else
    {
        MemoryBlockBaseReader::parseAccess(bankNode, newBank);
    }

    MemoryBlockBaseReader::parseUsage(bankNode, newBank);
    MemoryBlockBaseReader::parseVolatile(bankNode, newBank);

    return newBank;
}

void BankReader::Details::parseBankAlignment(QDomNode const& bankNode, QSharedPointer<Bank> newBank)
{
    auto attributes = CommonItemsReader::parseAttributes(bankNode);
    auto alignmentValue = attributes.value(QStringLiteral("bankAlignment"));
    newBank->setAlignment(Bank::stringToAlignment(alignmentValue));
}

void BankReader::Details::parseBankDefinitionRef(QDomNode const& bankNode, QSharedPointer<Bank> newBank)
{
    auto definitionRefElement = bankNode.firstChildElement(QStringLiteral("ipxact:bankDefinitionRef"));

    if (!definitionRefElement.isNull())
    {
        QString definitionRef = definitionRefElement.firstChild().nodeValue();
        QString typeDefinitionsRef = definitionRefElement.attribute(QStringLiteral("typeDefinitions"), QString());

        newBank->setBankDefinitionRef(definitionRef);
        newBank->setTypeDefinitionsRef(typeDefinitionsRef);
    }
}

void BankReader::Details::parseAddressBlocks(QDomNode const& bankNode, QSharedPointer<Bank> newBank, Document::Revision docRevision)
{
    // Read address blocks with AddressBlockReader. Then clear any values of memory array and base address, as those don't exist for banked address block.
    auto addressBlockNodes = bankNode.toElement().elementsByTagName(QStringLiteral("ipxact:addressBlock"));
    for (int i = 0; i < addressBlockNodes.count(); ++i)
    {
        auto addressBlockNode = addressBlockNodes.at(i);
        auto newAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, docRevision);
        newAddressBlock->setBaseAddress(QString());
        newAddressBlock->getMemoryArray().clear();
        newAddressBlock->setTypeIdentifier(QString());
        newBank->getAddressBlocks()->append(newAddressBlock);
    }
}

void BankReader::Details::parseBanks(QDomNode const& bankNode, QSharedPointer<Bank> newBank, Document::Revision docRevision)
{
    // Read banks recursively. Clear any values of base address (nonexistent in banked bank).
    auto bankNodes = bankNode.toElement().elementsByTagName(QStringLiteral("ipxact:bank"));
    for (int i = 0; i < bankNodes.count(); ++i)
    {
        auto bankNode = bankNodes.at(i);
        auto bankedBank = createBankFrom(bankNode, docRevision);
        bankedBank->setBaseAddress(QString());
        newBank->getBanks()->append(bankedBank);
    }
}

void BankReader::Details::parseSubspaces(QDomNode const& bankNode, QSharedPointer<Bank> newBank, Document::Revision docRevision)
{
    // Read subspace maps with SubSpaceMapReader. Clear any values of base address and segment reference (nonexistent in banked subspace map).
    auto subspaceMapNodes = bankNode.toElement().elementsByTagName(QStringLiteral("ipxact:subspaceMap"));
    for (int i = 0; i < subspaceMapNodes.count(); ++i)
    {
        auto subspaceMapNode = subspaceMapNodes.at(i);
        auto newSubSpaceMap = SubspaceMapReader::createSubspaceMapFrom(subspaceMapNode, docRevision);
        newSubSpaceMap->setBaseAddress(QString());
        newSubSpaceMap->setSegmentReference(QString());
        newBank->getSubSpaceMaps()->append(newSubSpaceMap);
    }
}
