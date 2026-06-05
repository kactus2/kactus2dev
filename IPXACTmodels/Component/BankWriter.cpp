//-----------------------------------------------------------------------------
// File: BankWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.06.2026
//
// Description:
// Writer for ipxact:bank elements.
//-----------------------------------------------------------------------------

#include "BankWriter.h"

#include <IPXACTmodels/Component/MemoryBlockBaseWriter.h>
#include <IPXACTmodels/Component/AddressBlockWriter.h>
#include <IPXACTmodels/Component/SubSpaceMapWriter.h>

#include <IPXACTmodels/Component/Bank.h>

void BankWriter::writeBank(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision)
{
    if (bank == nullptr) return;

    writer.writeStartElement(QStringLiteral("ipxact:bank"));

    Details::writeAlignment(writer, bank);

    MemoryBlockBaseWriter::writeNameGroup(writer, bank, docRevision);

    MemoryBlockBaseWriter::writeAccessHandles(writer, bank, docRevision);

    MemoryBlockBaseWriter::writeBaseAddress(writer, bank);

    if (docRevision == Document::Revision::Std22)
    {
        Details::writeBankDefinitionRef(writer, bank);
    }
    else
    {
        CommonItemsWriter::writeIsPresent(writer, bank->getIsPresent());
    }

    Details::writeAddressBlocks(writer, bank, docRevision);
    Details::writeBanks(writer, bank, docRevision);
    Details::writeSubspaces(writer, bank, docRevision);

    MemoryBlockBaseWriter::writeUsage(writer, bank);

    MemoryBlockBaseWriter::writeVolatile(writer, bank);

    if (docRevision == Document::Revision::Std14)
    {
        MemoryBlockBaseWriter::writeAccess(writer, bank);
    }
    else
    {
        MemoryBlockBaseWriter::writeAccessPolicies(writer, bank);
    }

    CommonItemsWriter::writeParameters(writer, bank->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, bank);

    writer.writeEndElement(); // ipxact:bank
}

void BankWriter::Details::writeAlignment(QXmlStreamWriter& writer, QSharedPointer<Bank> bank)
{
    CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("bankAlignment"), bank->alignmentToString(bank->getAlignment()));
}

void BankWriter::Details::writeBankDefinitionRef(QXmlStreamWriter& writer, QSharedPointer<Bank> bank)
{
    if (bank->getBankDefinitionRef().isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:bankDefinitionRef"));

        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("typeDefinitions"), bank->getTypeDefinitionsRef());

        writer.writeCharacters(bank->getBankDefinitionRef());
        writer.writeEndElement();
    }
}

void BankWriter::Details::writeAddressBlocks(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision)
{
    for (auto const& addressBlock : *bank->getAddressBlocks())
    {
        AddressBlockWriter::writeAddressBlock(writer, addressBlock, docRevision);
    }
}

void BankWriter::Details::writeBanks(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision)
{
    for (auto const& bank : *bank->getBanks())
    {
        writeBank(writer, bank, docRevision);
    }
}

void BankWriter::Details::writeSubspaces(QXmlStreamWriter& writer, QSharedPointer<Bank> bank, Document::Revision docRevision)
{
    for (auto const& subspace : *bank->getSubSpaceMaps())
    {
        SubSpaceMapWriter::writeSubSpaceMap(writer, subspace, docRevision);
    }
}
