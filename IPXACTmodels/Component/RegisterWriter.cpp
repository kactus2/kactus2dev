//-----------------------------------------------------------------------------
// File: RegisterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Writer class for IP-XACT register element.
//-----------------------------------------------------------------------------

#include "RegisterWriter.h"
#include "Register.h"
#include "RegisterFile.h"
#include "RegisterBase.h"
#include "AlternateRegister.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/FieldWriter.h>

//-----------------------------------------------------------------------------
// Function: RegisterWriter::RegisterWriter()
//-----------------------------------------------------------------------------
RegisterWriter::RegisterWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::~RegisterWriter()
//-----------------------------------------------------------------------------
RegisterWriter::~RegisterWriter()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeRegisterData()
//-----------------------------------------------------------------------------
void RegisterWriter::writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData) const
{
    QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
    QSharedPointer<RegisterFile> registerFile = registerData.dynamicCast<RegisterFile>();

    if (targetRegister)
    {
        writeRegister(writer, targetRegister);
    }
    else if (registerFile)
    {
        writeRegisterFile(writer, registerFile);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeRegister()
//-----------------------------------------------------------------------------
void RegisterWriter::writeRegister(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister) const
{
    writer.writeStartElement("ipxact:register");

    writeNameGroup(writer, targetRegister);

    writeIsPresent(writer, targetRegister->getIsPresent());

    writeDimension(writer, targetRegister->getDimension());

    writeAddressOffset(writer, targetRegister->getAddressOffset());

    writeTypeIdentifier(writer, targetRegister);

    writer.writeTextElement("ipxact:size", targetRegister->getSize());

    writeVolatile(writer, targetRegister);

    writeAccess(writer, targetRegister);

    writeFields(writer, targetRegister);

    writeAlternateRegisters(writer, targetRegister);

    writeParameters(writer, targetRegister->getParameters());

    writeVendorExtensions(writer, targetRegister);

    writer.writeEndElement(); // ipxact:register
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void RegisterWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData) const
{
    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, registerData);
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeDim()
//-----------------------------------------------------------------------------
void RegisterWriter::writeDimension(QXmlStreamWriter& writer, QString const& dimension) const
{
    if (!dimension.isEmpty())
    {
        writer.writeTextElement("ipxact:dim", dimension);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeAddressOffset()
//-----------------------------------------------------------------------------
void RegisterWriter::writeAddressOffset(QXmlStreamWriter& writer, QString const& addressOffset) const
{
    writer.writeTextElement("ipxact:addressOffset", addressOffset);
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void RegisterWriter::writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData) const
{
    if (!registerData->getTypeIdentifier().isEmpty())
    {
        writer.writeTextElement("ipxact:typeIdentifier", registerData->getTypeIdentifier());
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeVolatile()
//-----------------------------------------------------------------------------
void RegisterWriter::writeVolatile(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition)
    const
{
    if (!registerDefinition->getVolatile().isEmpty())
    {
        writer.writeTextElement("ipxact:volatile", registerDefinition->getVolatile());
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeAccess()
//-----------------------------------------------------------------------------
void RegisterWriter::writeAccess(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition)
    const
{
    if (registerDefinition->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString accessString = AccessTypes::access2Str(registerDefinition->getAccess());
        writer.writeTextElement("ipxact:access", accessString);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeFields()
//-----------------------------------------------------------------------------
void RegisterWriter::writeFields(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition)
    const
{
    FieldWriter fieldWriter;
    foreach (QSharedPointer<Field> field, *registerDefinition->getFields())
    {
        fieldWriter.writeField(writer, field);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeAlternateRegisters()
//-----------------------------------------------------------------------------
void RegisterWriter::writeAlternateRegisters(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister)
    const
{
    if (!targetRegister->getAlternateRegisters()->isEmpty())
    {
        writer.writeStartElement("ipxact:alternateRegisters");

        foreach (QSharedPointer<AlternateRegister> alternateRegister, *targetRegister->getAlternateRegisters())
        {
            writeSingleAlternateRegister(writer, alternateRegister);
        }

        writer.writeEndElement(); // ipxact:alternateRegisters
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeSingleAlternateRegister()
//-----------------------------------------------------------------------------
void RegisterWriter::writeSingleAlternateRegister(QXmlStreamWriter& writer,
    QSharedPointer<AlternateRegister> alternateRegister) const
{
    writer.writeStartElement("ipxact:alternateRegister");

    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, alternateRegister);

    writeIsPresent(writer, alternateRegister->getIsPresent());

    writeAlternateGroups(writer, alternateRegister);

    writeTypeIdentifier(writer, alternateRegister);

    writeVolatile(writer, alternateRegister);

    writeAccess(writer, alternateRegister);

    writeFields(writer, alternateRegister);

    writeParameters(writer, alternateRegister->getParameters());

    writeVendorExtensions(writer, alternateRegister);

    writer.writeEndElement(); // ipxact:alternateRegister

}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeAlternateGroups()
//-----------------------------------------------------------------------------
void RegisterWriter::writeAlternateGroups(QXmlStreamWriter& writer,
    QSharedPointer<AlternateRegister> alternateRegister) const
{
    writer.writeStartElement("ipxact:alternateGroups");

    foreach (QString groupName, *alternateRegister->getAlternateGroups())
    {
        writer.writeTextElement("ipxact:alternateGroup", groupName);
    }

    writer.writeEndElement(); // ipxact:alternateGroups
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeRegisterFile()
//-----------------------------------------------------------------------------
void RegisterWriter::writeRegisterFile(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile) const
{
    writer.writeStartElement("ipxact:registerFile");

    writeNameGroup(writer, registerFile);

    writeIsPresent(writer, registerFile->getIsPresent());

    writeDimension(writer, registerFile->getDimension());

    writeAddressOffset(writer, registerFile->getAddressOffset());

    writeTypeIdentifier(writer, registerFile);

    writer.writeTextElement("ipxact:range", registerFile->getRange());

    writeRegisterFileRegisterData(writer, registerFile);

    writeParameters(writer, registerFile->getParameters());

    writeVendorExtensions(writer, registerFile);

    writer.writeEndElement(); // ipxact:registerFile
}

//-----------------------------------------------------------------------------
// Function: RegisterWriter::writeRegisterFileRegisterData()
//-----------------------------------------------------------------------------
void RegisterWriter::writeRegisterFileRegisterData(QXmlStreamWriter& writer,
    QSharedPointer<RegisterFile> registerFile) const
{
    foreach (QSharedPointer<RegisterBase> registerData, *registerFile->getRegisterData())
    {
        writeRegisterData(writer, registerData);
    }
}
