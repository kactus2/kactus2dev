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

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/RegisterReader.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::AddressBlockReader()
//-----------------------------------------------------------------------------
AddressBlockReader::AddressBlockReader():
MemoryBlockBaseReader()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::createAddressBlockFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> AddressBlockReader::createAddressBlockFrom(QDomNode const& addressBlockNode, Document::Revision docRevision) const
{
    QSharedPointer<AddressBlock> newAddressBlock(new AddressBlock());

    parseNameGroup(addressBlockNode, newAddressBlock);

    parsePresence(addressBlockNode, newAddressBlock);

    parseBaseAddress(addressBlockNode, newAddressBlock);

    parseTypeIdentifier(addressBlockNode, newAddressBlock);

    parseRange(addressBlockNode, newAddressBlock);

    parseWidth(addressBlockNode, newAddressBlock);

    parseUsage(addressBlockNode, newAddressBlock);

    parseVolatile(addressBlockNode, newAddressBlock);

    parseAccess(addressBlockNode, newAddressBlock);

    parseParameters(addressBlockNode, newAddressBlock);

    parseRegisterData(addressBlockNode, newAddressBlock, docRevision);

    parseVendorExtensions(addressBlockNode, newAddressBlock);

    return newAddressBlock;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseTypeIdentifier()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseTypeIdentifier(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
{
    QDomElement typeIdentifierElement = addressBlockNode.firstChildElement(QStringLiteral("ipxact:typeIdentifier"));
    if (!typeIdentifierElement.isNull())
    {
        QString typeIdentifier = typeIdentifierElement.firstChild().nodeValue();
        newAddressBlock->setTypeIdentifier(typeIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseRange()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseRange(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
    const
{
    QString range = addressBlockNode.firstChildElement(QStringLiteral("ipxact:range")).firstChild().nodeValue();
    newAddressBlock->setRange(range);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseWidth()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseWidth(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
    const
{
    QString width = addressBlockNode.firstChildElement(QStringLiteral("ipxact:width")).firstChild().nodeValue();
    newAddressBlock->setWidth(width);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseUsage()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseUsage(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
    const
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
// Function: AddressBlockReader::parseVolatile()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseVolatile(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
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
// Function: AddressBlockReader::parseAccess()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseAccess(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
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
// Function: AddressBlockReader::parseRegisterData()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseRegisterData(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock,
    Document::Revision docRevision) const
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