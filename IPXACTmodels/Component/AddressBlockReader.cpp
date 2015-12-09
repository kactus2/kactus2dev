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
AddressBlockReader::AddressBlockReader(QObject* parent /* = 0 */):
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::~AddressBlockReader()
//-----------------------------------------------------------------------------
AddressBlockReader::~AddressBlockReader()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::createAddressBlockFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> AddressBlockReader::createAddressBlockFrom(QDomNode const& addressBlockNode) const
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

    parseRegisterData(addressBlockNode, newAddressBlock);

    parseVendorExtensions(addressBlockNode, newAddressBlock);

    return newAddressBlock;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseNameGroup()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseNameGroup(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
{
    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(addressBlockNode, newAddressBlock);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parsePresence()
//-----------------------------------------------------------------------------
void AddressBlockReader::parsePresence(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
{
    QDomElement isPresentElement = addressBlockNode.firstChildElement("ipxact:isPresent");
    if (!isPresentElement.isNull())
    {
        QString isPresent = isPresentElement.firstChild().nodeValue();
        newAddressBlock->setIsPresent(isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseBaseAddress()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseBaseAddress(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
{
    QString baseAddress = addressBlockNode.firstChildElement("ipxact:baseAddress").firstChild().nodeValue();
    newAddressBlock->setBaseAddress(baseAddress);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseTypeIdentifier()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseTypeIdentifier(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
{
    QDomElement typeIdentifierElement = addressBlockNode.firstChildElement("ipxact:typeIdentifier");
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
    QString range = addressBlockNode.firstChildElement("ipxact:range").firstChild().nodeValue();
    newAddressBlock->setRange(range);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseWidth()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseWidth(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
    const
{
    QString width = addressBlockNode.firstChildElement("ipxact:width").firstChild().nodeValue();
    newAddressBlock->setWidth(width);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseUsage()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseUsage(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock)
    const
{
    QDomElement usageElement = addressBlockNode.firstChildElement("ipxact:usage");
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
    QDomElement volatileElement = addressBlockNode.firstChildElement("ipxact:volatile");
    if (!volatileElement.isNull())
    {
        QString volatileString = volatileElement.firstChild().nodeValue();
        if (volatileString == "true")
        {
            newAddressBlock->setVolatile(true);
        }
        else if (volatileString == "false")
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
    QDomElement accessElement = addressBlockNode.firstChildElement("ipxact:access");
    if (!accessElement.isNull())
    {
        QString accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        newAddressBlock->setAccess(access);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseParameters()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseParameters(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = parseAndCreateParameters(addressBlockNode);
    if (!newParameters->isEmpty())
    {
        newAddressBlock->setParameters(newParameters);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockReader::parseRegisterData()
//-----------------------------------------------------------------------------
void AddressBlockReader::parseRegisterData(QDomNode const& addressBlockNode,
    QSharedPointer<AddressBlock> newAddressBlock) const
{
    QDomNodeList childNodeList = addressBlockNode.childNodes();
    RegisterReader registerReader;

    for (int i = 0; i < childNodeList.count(); ++i)
    {
        QDomNode currentNode = childNodeList.at(i);
        if (currentNode.nodeName() == "ipxact:register")
        {
            QSharedPointer<Register> newRegister = registerReader.createRegisterfrom(currentNode);
            newAddressBlock->getRegisterData()->append(newRegister);
        }
        else if (childNodeList.at(i).nodeName() == "ipxact:registerFile")
        {
            QSharedPointer<RegisterFile> newFile = registerReader.createRegisterFileFrom(currentNode);
            newAddressBlock->getRegisterData()->append(newFile);
        }
    }
}