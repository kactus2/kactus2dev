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
#include "../XmlUtils.h"

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
	// Create the new CPU.
	QSharedPointer<AddressSpace> newAddressSpace (new AddressSpace());

	// Parse presence, name group, and vendor extensions with pre-existing parsers.
	// CPU has no vendor extensions supported by Kactus2.
	newAddressSpace->setIsPresent(parseIsPresent(addressSpaceNode.firstChildElement("ipxact:isPresent")));
	NameGroupReader nameReader;
	nameReader.parseNameGroup(addressSpaceNode, newAddressSpace);
	parseVendorExtensions(addressSpaceNode, newAddressSpace);

	QList<QSharedPointer<Parameter> >& parameters = newAddressSpace->getParameters();
	QList<QSharedPointer<Segment> > segments;

	for (int i = 0; i < addressSpaceNode.childNodes().count(); ++i)
	{
		QDomNode tempNode = addressSpaceNode.childNodes().at(i);

		// Do not try to parse comments.
		if (tempNode.isComment())
		{
			continue;
		}

		// Branch based on nodename.
		if (tempNode.nodeName() == QString("ipxact:parameters"))
		{
			readParameters(tempNode, parameters);
		}
		else if (tempNode.nodeName() == QString("ipxact:blockSize"))
		{
			readBlockSize(tempNode, newAddressSpace);
		}
		else if (tempNode.nodeName() == QString("ipxact:addressUnitBits"))
		{
			newAddressSpace->setAddressUnitBits( tempNode.childNodes().at(0).nodeValue().toInt() );
		}
		else if (tempNode.nodeName() == QString("ipxact:segments"))
		{
			readSegments(tempNode, segments);
		}
		else if (tempNode.nodeName() == QString("ipxact:localMemoryMap"))
		{
			MemoryMapBaseReader reader;
			QSharedPointer<MemoryMapBase> mmb( new MemoryMapBase() );
			reader.readMemoryMapBase(tempNode, mmb);
			newAddressSpace->setLocalMemoryMap( mmb );
		}
	}

	newAddressSpace->setSegments( segments );

    return newAddressSpace;
}

void AddressSpaceReader::readParameters(QDomNode &tempNode, QList<QSharedPointer<Parameter> > &parameters) const
{
	// Use pre-existing parameter reader for reading parameters.
	ParameterReader reader;

	// Go through all parameters.
	for (int j = 0; j < tempNode.childNodes().count(); ++j)
	{
		QDomNode parameterNode = tempNode.childNodes().at(j);

		// Do not try to parse comments.
		if (!parameterNode.isComment())
		{
			parameters.append(QSharedPointer<Parameter>(reader.createParameterFrom(parameterNode)));
		}
	}
}

void AddressSpaceReader::readBlockSize(QDomNode &tempNode, QSharedPointer<AddressSpace> newAddressSpace) const
{
	for (int j = 0; j < tempNode.childNodes().count(); ++j)
	{
		// A block has a range and width.
		if (tempNode.childNodes().at(j).nodeName() == QString("ipxact:range"))
		{
			QString leftRange = tempNode.childNodes().at(j).firstChildElement("ipxact:left").firstChild().nodeValue();
			QString rightRange = tempNode.childNodes().at(j).firstChildElement("ipxact:right").firstChild().nodeValue();

			newAddressSpace->setRange( QSharedPointer<Range>(new Range(leftRange, rightRange) ) );
		}
		else if (tempNode.childNodes().at(j).nodeName() == QString("ipxact:width"))
		{
			newAddressSpace->setWidth( tempNode.childNodes().at(j).firstChild().nodeValue().toInt() );
		}
	}
}

void AddressSpaceReader::readSegments(QDomNode &tempNode, QList<QSharedPointer<Segment> > &segments) const
{
	for (int j = 0; j < tempNode.childNodes().count(); ++j)
	{
		QDomNode segmentNode = tempNode.childNodes().at(j);
		QSharedPointer<Segment> newSegment(new Segment());

		parseVendorExtensions(segmentNode, newSegment);
		NameGroupReader nameReader;
		nameReader.parseNameGroup(segmentNode, newSegment);
		newSegment->setIsPresent( parseIsPresent(segmentNode.firstChildElement("ipxact:isPresent")) );

		for (int i = 0; i < segmentNode.childNodes().count(); ++i)
		{
			QDomNode tempNode = segmentNode.childNodes().at(i);

			// Read address off set along its paramaters.
			if (tempNode.nodeName() == QString("ipxact:addressOffset"))
			{
				newSegment->setOffset( tempNode.childNodes().at(0).nodeValue() );

				newSegment->setOffsetAttributes( XmlUtils::parseAttributes(tempNode) );
			}

			// Read range off set along its paramaters.
			else if (tempNode.nodeName() == QString("ipxact:range"))
			{
				newSegment->setRange( tempNode.childNodes().at(0).nodeValue() );

				newSegment->setRangeAttributes( XmlUtils::parseAttributes(tempNode) );
			}
		}

		segments.append(newSegment);
	}
}
