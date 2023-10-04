//-----------------------------------------------------------------------------
// File: RegisterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Writer for IP-XACT register element.
//-----------------------------------------------------------------------------

#include "RegisterWriter.h"
#include "Register.h"
#include "RegisterFile.h"
#include "RegisterBase.h"
#include "AlternateRegister.h"
#include "FieldWriter.h"
#include "AccessPolicyWriter.h"
#include "MemoryArrayWriter.h"
#include "MemoryArray.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeRegisterData()
//-----------------------------------------------------------------------------
void RegisterWriter::writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData, Document::Revision docRevision)
{
    QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
    QSharedPointer<RegisterFile> registerFile = registerData.dynamicCast<RegisterFile>();

    if (targetRegister)
    {
        Details::writeRegister(writer, targetRegister, docRevision);
    }
    else if (registerFile)
    {
        Details::writeRegisterFile(writer, registerFile, docRevision);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeRegister()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeRegister(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:register"));

    NameGroupWriter::writeNameGroup(writer, targetRegister, docRevision);
    
    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, targetRegister->getIsPresent());

        // Write dimensions
        MemoryArrayWriter::writeMemoryArray(writer, targetRegister->getMemoryArray(), docRevision, false);

        writeAddressOffset(writer, targetRegister->getAddressOffset());

        writeTypeIdentifier(writer, targetRegister);

        writer.writeTextElement(QStringLiteral("ipxact:size"), targetRegister->getSize());
        
        writeVolatile(writer, targetRegister);

        writeAccess(writer, targetRegister);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        MemoryArrayWriter::writeMemoryArray(writer, targetRegister->getMemoryArray(), docRevision, false);

        writeAddressOffset(writer, targetRegister->getAddressOffset());

        writeRegisterDefinitionReference(writer, targetRegister);

        writeTypeIdentifier(writer, targetRegister);


        if (auto const& regSize = targetRegister->getSize(); regSize.isEmpty() == false)
        {
            writer.writeTextElement(QStringLiteral("ipxact:size"), targetRegister->getSize());
        }

        writeVolatile(writer, targetRegister);

        writeAccessPolicies(writer, targetRegister);
    }

    writeFields(writer, targetRegister, docRevision);

    writeAlternateRegisters(writer, targetRegister, docRevision);

    CommonItemsWriter::writeParameters(writer, targetRegister->getParameters());

    CommonItemsWriter::writeVendorExtensions(writer, targetRegister);

    writer.writeEndElement(); // ipxact:register
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeAddressOffset()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeAddressOffset(QXmlStreamWriter& writer, QString const& addressOffset)
{
    writer.writeTextElement(QStringLiteral("ipxact:addressOffset"), addressOffset);
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData)
{
    if (!registerData->getTypeIdentifier().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:typeIdentifier"), registerData->getTypeIdentifier());
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeVolatile()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeVolatile(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition)
   
{
    if (!registerDefinition->getVolatile().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:volatile"), registerDefinition->getVolatile());
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeAccess()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeAccess(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition)
   
{
    if (registerDefinition->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString accessString = AccessTypes::access2Str(registerDefinition->getAccess());
        writer.writeTextElement(QStringLiteral("ipxact:access"), accessString);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeFields()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeFields(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition, Document::Revision docRevision)
{
    for (auto const& field : *registerDefinition->getFields())
    {
        FieldWriter::writeField(writer, field, docRevision);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeAlternateRegisters()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeAlternateRegisters(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister, Document::Revision docRevision)
{
    if (!targetRegister->getAlternateRegisters()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:alternateRegisters"));

        for (auto const& alternateRegister : *targetRegister->getAlternateRegisters())
        {
            writeSingleAlternateRegister(writer, alternateRegister, docRevision);
        }

        writer.writeEndElement(); // ipxact:alternateRegisters
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeSingleAlternateRegister()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeSingleAlternateRegister(QXmlStreamWriter& writer,
    QSharedPointer<AlternateRegister> alternateRegister,
    Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:alternateRegister"));

    NameGroupWriter::writeNameGroup(writer, alternateRegister, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, alternateRegister->getIsPresent());

        writeAlternateGroups(writer, alternateRegister);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        CommonItemsWriter::writeModeReferences(writer, alternateRegister->getModeReferences());
    }

    writeTypeIdentifier(writer, alternateRegister);

    writeVolatile(writer, alternateRegister);

    if (docRevision == Document::Revision::Std14)
    {
        writeAccess(writer, alternateRegister);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        writeAccessPolicies(writer, alternateRegister);
    }

    writeFields(writer, alternateRegister, docRevision);

    CommonItemsWriter::writeParameters(writer, alternateRegister->getParameters());

    CommonItemsWriter::writeVendorExtensions(writer, alternateRegister);

    writer.writeEndElement(); // ipxact:alternateRegister

}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeAlternateGroups()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeAlternateGroups(QXmlStreamWriter& writer,
    QSharedPointer<AlternateRegister> alternateRegister)
{
    writer.writeStartElement(QStringLiteral("ipxact:alternateGroups"));

    for (auto const& groupName : *alternateRegister->getAlternateGroups())
    {
        writer.writeTextElement(QStringLiteral("ipxact:alternateGroup"), groupName);
    }

    writer.writeEndElement(); // ipxact:alternateGroups
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeRegisterFile()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeRegisterFile(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:registerFile"));

    NameGroupWriter::writeNameGroup(writer, registerFile, docRevision);
    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, registerFile->getIsPresent());

        MemoryArrayWriter::writeMemoryArray(writer, registerFile->getMemoryArray(), docRevision, false);

        writeAddressOffset(writer, registerFile->getAddressOffset());

        writeTypeIdentifier(writer, registerFile);

        writer.writeTextElement(QStringLiteral("ipxact:range"), registerFile->getRange());
    }
    else if (docRevision == Document::Revision::Std22)
    {
        MemoryArrayWriter::writeMemoryArray(writer, registerFile->getMemoryArray(), docRevision, false);

        writeAddressOffset(writer, registerFile->getAddressOffset());

        writeRegisterFileDefinitionReference(writer, registerFile);

        writeTypeIdentifier(writer, registerFile);

        if (auto const& range = registerFile->getRange(); range.isEmpty() == false)
        {
            writer.writeTextElement(QStringLiteral("ipxact:range"), registerFile->getRange());
        }

        writeAccessPolicies(writer, registerFile);
    }

    writeRegisterFileRegisterData(writer, registerFile, docRevision);

    CommonItemsWriter::writeParameters(writer, registerFile->getParameters());

    CommonItemsWriter::writeVendorExtensions(writer, registerFile);

    writer.writeEndElement(); // ipxact:registerFile
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeRegisterFileRegisterData()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeRegisterFileRegisterData(QXmlStreamWriter& writer,
    QSharedPointer<RegisterFile> registerFile,
    Document::Revision docRevision)
{
    for (auto const& registerData : *registerFile->getRegisterData())
    {
        writeRegisterData(writer, registerData, docRevision);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeAccessPolicies()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeAccessPolicies(QXmlStreamWriter& writer, 
    QSharedPointer<RegisterBase> targetRegisterBase)
{
    if (auto accessPolicies = targetRegisterBase->getAccessPolicies();
        accessPolicies->isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:accessPolicies"));

        for (auto const& accessPolicy : *targetRegisterBase->getAccessPolicies())
        {
            AccessPolicyWriter::writeAccessPolicy(writer, accessPolicy);
        }

        writer.writeEndElement(); // ipxact:accessPolicies
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeRegisterDefinitionReference()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeRegisterDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister)
{
    if (auto const& definitionReference = targetRegister->getRegisterDefinitionReference();
        definitionReference.isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:registerDefinitionRef"));

        if (auto const& typeDefinitionsRef = targetRegister->getTypeDefinitionsReference();
            typeDefinitionsRef.isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("typeDefinitions"), typeDefinitionsRef);
        }

        writer.writeCharacters(definitionReference);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::Details::writeRegisterFileDefinitionReference()
//-----------------------------------------------------------------------------
void RegisterWriter::Details::writeRegisterFileDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile)
{
    if (auto const& definitionReference = registerFile->getRegisterFileDefinitionReference();
        definitionReference.isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:registerFileDefinitionRef"));

        if (auto const& typeDefinitionsRef = registerFile->getTypeDefinitionsReference();
            typeDefinitionsRef.isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("typeDefinitions"), typeDefinitionsRef);
        }

        writer.writeCharacters(definitionReference);
        writer.writeEndElement();
    }
}
