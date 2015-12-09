//-----------------------------------------------------------------------------
// File: AddressBlockWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Writer class for ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#include "AddressBlockWriter.h"
#include "AddressBlock.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/RegisterWriter.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::AddressBlockWriter()
//-----------------------------------------------------------------------------
AddressBlockWriter::AddressBlockWriter(QObject* parent /* = 0 */):
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::~AddressBlockWriter()
//-----------------------------------------------------------------------------
AddressBlockWriter::~AddressBlockWriter()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeAddressBlock()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeAddressBlock(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
    const
{
    writer.writeStartElement("ipxact:addressBlock");

    writeNameGroup(writer, addressBlock);

    writeIsPresent(writer, addressBlock->getIsPresent());

    writer.writeTextElement("ipxact:baseAddress", addressBlock->getBaseAddress());

    writeTypeIdentifier(writer, addressBlock);

    writer.writeTextElement("ipxact:range", addressBlock->getRange());

    writer.writeTextElement("ipxact:width", addressBlock->getWidth());

    writeUsage(writer, addressBlock);

    writeVolatile(writer, addressBlock);

    writeAccess(writer, addressBlock);

    writeParameters(writer, addressBlock->getParameters());

    writeRegisterData(writer, addressBlock);

    writeVendorExtensions(writer, addressBlock);

    writer.writeEndElement(); // ipxact:addressBlock
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const
{
    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, addressBlock);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
    const
{
    if (!addressBlock->getTypeIdentifier().isEmpty())
    {
        writer.writeTextElement("ipxact:typeIdentifier", addressBlock->getTypeIdentifier());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeUsage()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeUsage(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const
{
    if (addressBlock->getUsage() != General::USAGE_COUNT)
    {
        QString usageString = General::usage2Str(addressBlock->getUsage());
        writer.writeTextElement("ipxact:usage", usageString);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeVolatile()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeVolatile(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const
{
    if (!addressBlock->getVolatile().isEmpty())
    {
        writer.writeTextElement("ipxact:volatile", addressBlock->getVolatile());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeAccess()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeAccess(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const
{
    if (addressBlock->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString accessString = AccessTypes::access2Str(addressBlock->getAccess());
        writer.writeTextElement("ipxact:access", accessString);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockWriter::writeRegisterData()
//-----------------------------------------------------------------------------
void AddressBlockWriter::writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock)
    const
{
    if (!addressBlock->getRegisterData()->isEmpty())
    {
        RegisterWriter registerWriter;

        foreach (QSharedPointer<RegisterBase> registerBase, *addressBlock->getRegisterData())
        {
            registerWriter.writeRegisterData(writer, registerBase);
        }
    }
}