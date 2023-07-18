//-----------------------------------------------------------------------------
// File: AddressSpaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 30.09.2015
//
// Description:
// Reader for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#include "AddressSpaceReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/Component/MemoryMapBaseReader.h>

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> AddressSpaceReader::createAddressSpaceFrom(QDomNode const& addressSpaceNode,
    Document::Revision docRevision)
{
	QSharedPointer<AddressSpace> newAddressSpace (new AddressSpace());

    Details::parseNameGroup(addressSpaceNode, newAddressSpace);

    if (docRevision != Document::Revision::Std22)
    {
	    newAddressSpace->setIsPresent(CommonItemsReader::parseIsPresent(addressSpaceNode.firstChildElement(QStringLiteral("ipxact:isPresent"))));
    }

    Details::readBlockSize(addressSpaceNode, newAddressSpace);

    Details::readSegments(addressSpaceNode, newAddressSpace);

    Details::parseAddressUnitBits(addressSpaceNode, newAddressSpace);

    Details::parseLocalMemoryMap(addressSpaceNode, newAddressSpace);

    Details::readParameters(addressSpaceNode, newAddressSpace);

    CommonItemsReader::parseVendorExtensions(addressSpaceNode, newAddressSpace);

    return newAddressSpace;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::parseNameGroup()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::parseNameGroup(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace)
{
    NameGroupReader::parseNameGroup(addressSpaceNode, newAddressSpace);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::readBlockSize()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::readBlockSize(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace)
{
    QDomElement rangeElement = addressSpaceNode.firstChildElement(QStringLiteral("ipxact:range"));
    QString range = rangeElement.firstChild().nodeValue();
    newAddressSpace->setRange(range);

    QDomElement widthElement = addressSpaceNode.firstChildElement(QStringLiteral("ipxact:width"));
    QString width = widthElement.firstChild().nodeValue();
    newAddressSpace->setWidth(width);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::readSegments()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::readSegments(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace)
{
    QDomElement segmentsElement = addressSpaceNode.firstChildElement(QStringLiteral("ipxact:segments"));

    QDomNodeList segmentNodeList = segmentsElement.elementsByTagName(QStringLiteral("ipxact:segment"));

    if (!segmentNodeList.isEmpty())
    {
        for (int i = 0; i < segmentNodeList.count(); ++i)
        {
            QDomNode segmentNode = segmentNodeList.at(i);
            QSharedPointer<Segment> newSegment(new Segment());

            NameGroupReader::parseNameGroup(segmentNode, newSegment);

            newSegment->setIsPresent(CommonItemsReader::parseIsPresent(segmentNode.firstChildElement(QStringLiteral("ipxact:isPresent"))));

            parseAddressOffset(segmentNode, newSegment);

            parseRange(segmentNode, newSegment);

            CommonItemsReader::parseVendorExtensions(segmentNode, newSegment);

            newAddressSpace->getSegments()->append(newSegment);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::parseAddressOffset()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::parseAddressOffset(QDomNode const& segmentNode, QSharedPointer<Segment> newSegment)
{
    QDomElement addressOffsetElement = segmentNode.firstChildElement(QStringLiteral("ipxact:addressOffset"));
    newSegment->setOffset(addressOffsetElement.firstChild().nodeValue());

    QDomNamedNodeMap offsetAttributes = addressOffsetElement.attributes();
    if (!offsetAttributes.isEmpty())
    {
        QMap<QString, QString> newOffsetAttributes;
        for (int i = 0 ; i < offsetAttributes.count(); ++i)
        {
            QDomNode attributeNode = offsetAttributes.item(i);
            QString key = attributeNode.nodeName();
            QString value = attributeNode.firstChild().nodeValue();

            newOffsetAttributes.insert(key, value);
        }
        newSegment->setOffsetAttributes(newOffsetAttributes);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::parseRange()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::parseRange(QDomNode const& segmentNode, QSharedPointer<Segment> newSegment)
{
    QDomElement rangeElement = segmentNode.firstChildElement(QStringLiteral("ipxact:range"));
    newSegment->setRange(rangeElement.firstChild().nodeValue());

    QDomNamedNodeMap rangeAttributes = rangeElement.attributes();
    if (!rangeAttributes.isEmpty())
    {
        QMap<QString, QString> newRangeAttributes;
        for (int i = 0 ; i < rangeAttributes.count(); ++i)
        {
            QDomNode attributeNode = rangeAttributes.item(i);
            QString key = attributeNode.nodeName();
            QString value = attributeNode.firstChild().nodeValue();

            newRangeAttributes.insert(key, value);
        }
        newSegment->setRangeAttributes(newRangeAttributes);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::parseAddressUnitBits()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::parseAddressUnitBits(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace)
{
    QDomElement addressUnitElement = addressSpaceNode.firstChildElement(QStringLiteral("ipxact:addressUnitBits"));
    if (!addressUnitElement.isNull())
    {
        QString addressUnitBits = addressUnitElement.firstChild().nodeValue();
        newAddressSpace->setAddressUnitBits(addressUnitBits);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::parseLocalMemoryMap()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::parseLocalMemoryMap(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace)
{
    QDomNode localMemoryMapNode = addressSpaceNode.firstChildElement(QStringLiteral("ipxact:localMemoryMap"));
    if (!localMemoryMapNode.isNull())
    {
        QSharedPointer<MemoryMapBase> newLocalMemoryMap(new MemoryMapBase());

        MemoryMapBaseReader localReader;
        localReader.readMemoryMapBase(localMemoryMapNode, newLocalMemoryMap);
        newAddressSpace->setLocalMemoryMap(newLocalMemoryMap);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::Details::readParameters()
//-----------------------------------------------------------------------------
void AddressSpaceReader::Details::readParameters(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = 
        CommonItemsReader::parseAndCreateParameters(addressSpaceNode);

    if (!newParameters->isEmpty())
    {
        newAddressSpace->setParameters(newParameters);
    }
}
