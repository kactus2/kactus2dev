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
// Function: IndirectInterfaceReader::createIndirectInterfaceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<IndirectInterface> IndirectInterfaceReader::createIndirectInterfaceFrom(QDomNode const& interfaceNode,
    Document::Revision docRevision)
{
    QSharedPointer<IndirectInterface> newIndirectInterface(new IndirectInterface());

    Details::parseAttributes(interfaceNode, newIndirectInterface);

    NameGroupReader::parseNameGroup(interfaceNode, newIndirectInterface);

    Details::parseIndirectAddressAndData(interfaceNode, newIndirectInterface);

    Details::parseMemoryMapRef(interfaceNode, newIndirectInterface);

    Details::parseTransparentBridges(interfaceNode, newIndirectInterface, docRevision);

    Details::parseBitsInLau(interfaceNode, newIndirectInterface);

    Details::parseEndianness(interfaceNode, newIndirectInterface);

    Details::parseParameters(interfaceNode, newIndirectInterface, docRevision);

    CommonItemsReader::parseVendorExtensions(interfaceNode, newIndirectInterface);

    return newIndirectInterface;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseAttributes()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::Details::parseAttributes(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface)
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
// Function: IndirectInterfaceReader::parseIndirectAddressAndData()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::Details::parseIndirectAddressAndData(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface)
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
void IndirectInterfaceReader::Details::parseMemoryMapRef(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface)
{
    QString memoryMapRef = 
        interfaceNode.firstChildElement(QStringLiteral("ipxact:memoryMapRef")).firstChild().nodeValue();

    indirectInterface->setMemoryMapRef(memoryMapRef);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseTransparentBridges()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::Details::parseTransparentBridges(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface, Document::Revision docRevision)
{
    QDomNodeList bridgeNodes = 
        interfaceNode.toElement().elementsByTagName(QStringLiteral("ipxact:transparentBridge"));

    int bridgeCount = bridgeNodes.count();
    for (int i = 0; i < bridgeCount; i++)
    {
        QDomElement bridgeNode = bridgeNodes.at(i).toElement();

        QString initiatorReference;
        if (docRevision == Document::Revision::Std14)
        {
            initiatorReference = bridgeNode.attributes().namedItem(QStringLiteral("masterRef")).firstChild().nodeValue();
        }
        else if (docRevision == Document::Revision::Std22)
        {
            initiatorReference = bridgeNode.attributes().namedItem(QStringLiteral("initiatorRef")).firstChild().nodeValue();
        }

        QSharedPointer<TransparentBridge> newBridge(new TransparentBridge(initiatorReference));

        if (docRevision == Document::Revision::Std14)
        {
            QString isPresent = bridgeNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
            newBridge->setIsPresent(isPresent);
        }
        
        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsReader::parseVendorExtensions(bridgeNode, newBridge);
        }

        indirectInterface->getTransparentBridges()->append(newBridge);
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseEndianness()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::Details::parseEndianness(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface)
{
    QString endianness = 
        interfaceNode.firstChildElement(QStringLiteral("ipxact:endianness")).firstChild().nodeValue();

    indirectInterface->setEndianness(endianness);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseBitsInLau()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::Details::parseBitsInLau(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface)
{
    QDomElement lauElement = interfaceNode.firstChildElement(QStringLiteral("ipxact:bitsInLau"));

    QString bitsInLau = lauElement.firstChild().nodeValue();
    indirectInterface->setBitsInLau(bitsInLau);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceReader::parseParameters()
//-----------------------------------------------------------------------------
void IndirectInterfaceReader::Details::parseParameters(QDomNode const& interfaceNode,
    QSharedPointer<IndirectInterface> indirectInterface, Document::Revision docRevision) 
{
    indirectInterface->getParameters()->append(
        *CommonItemsReader::parseAndCreateParameters(interfaceNode, docRevision));
}
