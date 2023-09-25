//-----------------------------------------------------------------------------
// File: AddressBlockWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Writer for ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#include "AddressBlockWriter.h"
#include "AddressBlock.h"

#include "MemoryArrayWriter.h"
#include "AccessPolicyWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/RegisterWriter.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeAddressBlock()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeAddressBlock(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:addressBlock"));
 
    if (docRevision == Document::Revision::Std22)
    {
        CommonItemsWriter::writeNonEmptyAttribute(writer, QStringLiteral("misalignmentAllowed"),
            addressBlock->getMisalignmentAllowed());
    }

    MemoryBlockBaseWriter::writeNameGroup(writer, addressBlock, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, addressBlock->getIsPresent());
    }
    else if (docRevision == Document::Revision::Std22)
    {
        MemoryArrayWriter::writeMemoryArray(writer, addressBlock->getMemoryArray(), false);
    }

    MemoryBlockBaseWriter::writeBaseAddress(writer, addressBlock);

    if (docRevision == Document::Revision::Std22)
    {
        Details::writeDefinitionReference(writer, addressBlock);
    }

    Details::writeTypeIdentifier(writer, addressBlock);

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:range"), addressBlock->getRange());
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:width"), addressBlock->getWidth());

    Details::writeUsage(writer, addressBlock);

    Details::writeVolatile(writer, addressBlock);

    if (docRevision == Document::Revision::Std14)
    {
        Details::writeAccess(writer, addressBlock);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::writeAccessPolicies(writer, addressBlock);
    }

    CommonItemsWriter::writeParameters(writer, addressBlock->getParameters(), docRevision);

    Details::writeRegisterData(writer, addressBlock, docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, addressBlock);

    writer.writeEndElement(); // ipxact:addressBlock
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::Details::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void AddressBlockWriter::Details::writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
{
    if (!addressBlock->getTypeIdentifier().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:typeIdentifier"), addressBlock->getTypeIdentifier());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::Details::writeUsage()
//-----------------------------------------------------------------------------
void AddressBlockWriter::Details::writeUsage(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
{
    if (addressBlock->getUsage() != General::USAGE_COUNT)
    {
        QString usageString = General::usage2Str(addressBlock->getUsage());
        writer.writeTextElement(QStringLiteral("ipxact:usage"), usageString);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::Details::writeVolatile()
//-----------------------------------------------------------------------------
void AddressBlockWriter::Details::writeVolatile(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
{
    if (!addressBlock->getVolatile().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:volatile"), addressBlock->getVolatile());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::Details::writeAccess()
//-----------------------------------------------------------------------------
void AddressBlockWriter::Details::writeAccess(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
{
    if (addressBlock->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString accessString = AccessTypes::access2Str(addressBlock->getAccess());
        writer.writeTextElement(QStringLiteral("ipxact:access"), accessString);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::Details::writeRegisterData()
//-----------------------------------------------------------------------------
void AddressBlockWriter::Details::writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock, Document::Revision docRevision)
{
    if (!addressBlock->getRegisterData()->isEmpty())
    {
        for (auto const& registerBase : *addressBlock->getRegisterData())
        {
            RegisterWriter::writeRegisterData(writer, registerBase, docRevision);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::Details::writeDefinitionReference()
//-----------------------------------------------------------------------------
void AddressBlockWriter::Details::writeDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
{
    if (auto const& definitionReference = addressBlock->getAddressBlockDefinitionRef();
        definitionReference.isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:addressBlockDefinitionRef"));

        if (auto const& typeDefinitionsRef = addressBlock->getTypeDefinitionsRef();
            typeDefinitionsRef.isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("typeDefinitions"), typeDefinitionsRef);
        }

        writer.writeCharacters(definitionReference);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::Details::writeAccessPolicies()
//-----------------------------------------------------------------------------
void AddressBlockWriter::Details::writeAccessPolicies(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
{
    if (auto accessPolicies = addressBlock->getAccessPolicies();
        accessPolicies->isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:accessPolicies"));

        for (auto const& accessPolicy : *addressBlock->getAccessPolicies())
        {
            AccessPolicyWriter::writeAccessPolicy(writer, accessPolicy);
        }

        writer.writeEndElement(); // ipxact:accessPolicies
    }
}
