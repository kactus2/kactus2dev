//-----------------------------------------------------------------------------
// File: AddressSpaceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Writer class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#include "AddressSpaceWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/Component/MemoryMapBaseWriter.h>
#include "../XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::AddressSpaceWriter()
//-----------------------------------------------------------------------------
AddressSpaceWriter::AddressSpaceWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::~AddressSpaceWriter()
//-----------------------------------------------------------------------------
AddressSpaceWriter::~AddressSpaceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::writeAddressSpace()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::writeAddressSpace(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace) const
{
	// Start the element, write name group, presence and, vendor extensions with pre-existing writers.
	writer.writeStartElement("ipxact:addressSpace");
	NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, addressSpace);
	writeIsPresent(writer, addressSpace->getIsPresent());
	writeVendorExtensions( writer, addressSpace );

	// Write parameters.
	writeParameters(addressSpace, writer);

	// Write block size.
	writeBlockSize(writer, addressSpace);

	// Write address unit bits.
	writer.writeStartElement("ipxact:addressUnitBits");
	writer.writeCharacters(QString::number( addressSpace->getAddressUnitBits() ));
	writer.writeEndElement();

	// Write segments.
	writeSegments(addressSpace, writer);

	// Write local memory map.
	writer.writeStartElement("ipxact:localMemoryMap");

	MemoryMapBaseWriter mmbw;
	mmbw.writeMemoryMapBase( writer, addressSpace->getLocalMemoryMap() );

	writer.writeEndElement();

	writer.writeEndElement();
	return;
}

void AddressSpaceWriter::writeParameters(QSharedPointer<AddressSpace> addressSpace, QXmlStreamWriter &writer) const
{
	// Acquire references for address space and parameter lists.
	QList<QSharedPointer<Parameter> >& parameters = addressSpace->getParameters();

	if (parameters.size() != 0)
	{
		writer.writeStartElement("ipxact:parameters");

		// Use pre-existing writer for parameters.
		ParameterWriter parameterWriter;
		// Write each parameter.
		for (int i = 0; i < parameters.size(); ++i)
		{
			parameterWriter.writeParameter(writer, parameters.at(i));
		}

		writer.writeEndElement(); 
	}
}

void AddressSpaceWriter::writeBlockSize(QXmlStreamWriter &writer, QSharedPointer<AddressSpace> addressSpace) const
{
	writer.writeStartElement("ipxact:blockSize");

	// Write range of the logical port.
	if ( addressSpace->getRange() )
	{
		writer.writeStartElement("ipxact:range");

		writer.writeTextElement("ipxact:left", addressSpace->getRange()->getLeft());
		writer.writeTextElement("ipxact:right", addressSpace->getRange()->getRight());

		writer.writeEndElement();
	}

	// Start the ipxact:width element.
	writer.writeStartElement("ipxact:width");

	// Write the value of the element and close the tag.
	writer.writeCharacters(QString::number( addressSpace->getWidth() ));
	writer.writeEndElement();

	writer.writeEndElement();
}

void AddressSpaceWriter::writeSegments(QSharedPointer<AddressSpace> addressSpace, QXmlStreamWriter &writer) const
{
	if (!addressSpace->getSegments().isEmpty())
	{
		writer.writeStartElement("ipxact:segments");

		// Write each segment.
		foreach (QSharedPointer<Segment> segment, addressSpace->getSegments())
		{
			writer.writeStartElement("ipxact:segment");

			// They have the usual properties, like name group, present, and extenstions.
			NameGroupWriter nameGroupWriter;
			nameGroupWriter.writeNameGroup(writer, segment);
			writeIsPresent(writer, segment->getIsPresent());
			writeVendorExtensions( writer, segment );

			// Write offset along its attributes if exists.
			if (!segment->getAddressOffset().isEmpty())
			{
				writer.writeStartElement("ipxact:addressOffset");

				XmlUtils::writeAttributes(writer, segment->getOffsetAttributes());
				writer.writeCharacters(segment->getAddressOffset());

				writer.writeEndElement();
			}

			// Write range along its attributes if exists.
			if ( !segment->getRange().isEmpty() )
			{
				writer.writeStartElement("ipxact:range");

				XmlUtils::writeAttributes(writer, segment->getRangeAttributes());
				writer.writeCharacters(segment->getRange());

				writer.writeEndElement();
			}

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}
}
