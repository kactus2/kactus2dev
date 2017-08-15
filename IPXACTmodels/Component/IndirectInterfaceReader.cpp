//-----------------------------------------------------------------------------
// File: IndirectInterfaceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.08.2017
//
// Description:
// XML reader class for IP-XACT Indirect Interface element.
//-----------------------------------------------------------------------------

#include "IndirectInterfaceReader.h"
#include "IndirectInterface.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::IndirectInterfaceReader()
//-----------------------------------------------------------------------------
IndirectInterfaceReader::IndirectInterfaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::~IndirectInterfaceReader()
//-----------------------------------------------------------------------------
IndirectInterfaceReader::~IndirectInterfaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::createIndirectInterfaceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<IndirectInterface> IndirectInterfaceReader::createIndirectInterfaceFrom(QDomNode const& interfaceNode) const
{
    QSharedPointer<IndirectInterface> newIndirectInterface(new IndirectInterface());

    parseAttributes(interfaceNode, newIndirectInterface);

    parseNameGroup(interfaceNode, newIndirectInterface);

    parseIndirectAddressAndData(interfaceNode, newIndirectInterface);

    parseMemoryMapRef(interfaceNode, newIndirectInterface);

    parseTransparentBridges(interfaceNode, newIndirectInterface);

    parseBitsInLau(interfaceNode, newIndirectInterface);

    parseEndianness(interfaceNode, newIndirectInterface);

    parseParameters(interfaceNode, newIndirectInterface);

    parseVendorExtensions(interfaceNode, newIndirectInterface);

    return newIndirectInterface;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseAttributes()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseAttributes(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    QMap<QString, QString> attributes;

    QDomNamedNodeMap attributeMap = interfaceNode.attributes();
    for (int i = 0; i < attributeMap.size(); ++i)
    {
        QString name = attributeMap.item(i).nodeName();
        QString value = attributeMap.item(i).nodeValue();
        attributes.insert(name, value);
    }

    indirectInterface->setAttributes(attributes);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseNameGroup()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseNameGroup(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    NameGroupReader nameReader;
    nameReader.parseNameGroup(interfaceNode, indirectInterface);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseIndirectAddressAndData()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseIndirectAddressAndData(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    QString indirectAddress = 
        interfaceNode.firstChildElement(QStringLiteral("ipxact:indirectAddressRef")).firstChild().nodeValue();

    QString indirectData = 
        interfaceNode.firstChildElement(QStringLiteral("ipxact:indirectDataRef")).firstChild().nodeValue();

    indirectInterface->setIndirectAddressRef(indirectAddress);
    indirectInterface->setIndirectDataRef(indirectData);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseMemoryMapRef()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseMemoryMapRef(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    QString memoryMapRef = 
        interfaceNode.firstChildElement(QStringLiteral("ipxact:memoryMapRef")).firstChild().nodeValue();

    indirectInterface->setMemoryMapRef(memoryMapRef);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseTransparentBridges()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseTransparentBridges(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    QDomNodeList bridgeNodes = 
        interfaceNode.toElement().elementsByTagName(QStringLiteral("ipxact:transparentBridge"));

    int bridgeCount = bridgeNodes.count();
    for (int i = 0; i < bridgeCount; i++)
    {
        QDomElement bridgeNode = bridgeNodes.at(i).toElement();

        QString masterReference = bridgeNode.attributes().namedItem(QStringLiteral("masterRef")).firstChild().nodeValue();
        QString isPresent = bridgeNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();

        QSharedPointer<TransparentBridge> newBridge(new TransparentBridge(masterReference));
        newBridge->setIsPresent(isPresent);
        
        indirectInterface->getTransparentBridges()->append(newBridge);
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseEndianness()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseEndianness(QDomNode const& interfaceNode, 
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    QString endianness = 
        interfaceNode.firstChildElement(QStringLiteral("ipxact:endianness")).firstChild().nodeValue();

    indirectInterface->setEndianness(endianness);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseBitsInLau()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseBitsInLau(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    QDomElement lauElement = interfaceNode.firstChildElement(QStringLiteral("ipxact:bitsInLau"));

    QString bitsInLau = lauElement.firstChild().nodeValue();
    indirectInterface->setBitsInLau(bitsInLau);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseParameters()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::parseParameters(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    indirectInterface->getParameters()->append(*CommonItemsReader::parseAndCreateParameters(interfaceNode));
}
