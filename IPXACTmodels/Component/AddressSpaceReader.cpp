//-----------------------------------------------------------------------------
// File: AddressSpaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 30.09.2015
//
// Description:
// Reader class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#include "AddressSpaceReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/Component/MemoryMapBaseReader.h>

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::AddressSpaceReader()
//-----------------------------------------------------------------------------
AddressSpaceReader::AddressSpaceReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::~AddressSpaceReader()
//-----------------------------------------------------------------------------
AddressSpaceReader::~AddressSpaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> AddressSpaceReader::createAddressSpaceFrom(QDomNode const& addressSpaceNode) const
{
	QSharedPointer<AddressSpace> newAddressSpace (new AddressSpace());

    parseNameGroup(addressSpaceNode, newAddressSpace);

	newAddressSpace->setIsPresent(parseIsPresent(addressSpaceNode.firstChildElement("ipxact:isPresent")));

    readBlockSize(addressSpaceNode, newAddressSpace);

    readSegments(addressSpaceNode, newAddressSpace);

    parseAddressUnitBits(addressSpaceNode, newAddressSpace);

    parseLocalMemoryMap(addressSpaceNode, newAddressSpace);

    readParameters(addressSpaceNode, newAddressSpace);

    parseVendorExtensions(addressSpaceNode, newAddressSpace);

    return newAddressSpace;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::parseNameGroup()
//-----------------------------------------------------------------------------
void AddressSpaceReader::parseNameGroup(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace) const
{
    NameGroupReader nameReader;
    nameReader.parseNameGroup(addressSpaceNode, newAddressSpace);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::readBlockSize()
//-----------------------------------------------------------------------------
void AddressSpaceReader::readBlockSize(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace) const
{
    QDomElement rangeElement = addressSpaceNode.firstChildElement("ipxact:range");
    QString range = rangeElement.firstChild().nodeValue();
    newAddressSpace->setRange(range);

    QDomElement widthElement = addressSpaceNode.firstChildElement("ipxact:width");
    QString width = widthElement.firstChild().nodeValue();
    newAddressSpace->setWidth(width);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::readSegments()
//-----------------------------------------------------------------------------
void AddressSpaceReader::readSegments(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace) const
{
    QDomElement segmentsElement = addressSpaceNode.firstChildElement("ipxact:segments");

    QDomNodeList segmentNodeList = segmentsElement.elementsByTagName("ipxact:segment");

    if (!segmentNodeList.isEmpty())
    {
        NameGroupReader nameReader;
        
        for (int i = 0; i < segmentNodeList.count(); ++i)
        {
            QDomNode segmentNode = segmentNodeList.at(i);
            QSharedPointer<Segment> newSegment(new Segment());

            nameReader.parseNameGroup(segmentNode, newSegment);

            QDomElement isPresentElement = segmentNode.firstChildElement("ipxact:isPresent");
            if (!isPresentElement.isNull())
            {
                newSegment->setIsPresent(isPresentElement.firstChild().nodeValue());
            }

            QDomElement addressOffsetElement = segmentNode.firstChildElement("ipxact:addressOffset");
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

            QDomElement rangeElement = segmentNode.firstChildElement("ipxact:range");
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

            parseVendorExtensions(segmentNode, newSegment);

            newAddressSpace->getSegments()->append(newSegment);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::parseAddressUnitBits()
//-----------------------------------------------------------------------------
void AddressSpaceReader::parseAddressUnitBits(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace) const
{
    QDomElement addressUnitElement = addressSpaceNode.firstChildElement("ipxact:addressUnitBits");
    if (!addressUnitElement.isNull())
    {
        QString addressUnitBits = addressUnitElement.firstChild().nodeValue();
        newAddressSpace->setAddressUnitBits(addressUnitBits);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::parseLocalMemoryMap()
//-----------------------------------------------------------------------------
void AddressSpaceReader::parseLocalMemoryMap(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace) const
{
    QDomNode localMemoryMapNode = addressSpaceNode.firstChildElement("ipxact:localMemoryMap");
    if (!localMemoryMapNode.isNull())
    {
        QSharedPointer<MemoryMapBase> newLocalMemoryMap(new MemoryMapBase());

        MemoryMapBaseReader localReader;
        localReader.readMemoryMapBase(localMemoryMapNode, newLocalMemoryMap);
        newAddressSpace->setLocalMemoryMap(newLocalMemoryMap);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceReader::readParameters()
//-----------------------------------------------------------------------------
void AddressSpaceReader::readParameters(QDomNode const& addressSpaceNode,
    QSharedPointer<AddressSpace> newAddressSpace) const
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = parseAndCreateParameters(addressSpaceNode);

    if (!newParameters->isEmpty())
    {
        newAddressSpace->setParameters(newParameters);
    }
}
