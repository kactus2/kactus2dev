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
	writer.writeStartElement("ipxact:addressSpace");

    NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, addressSpace);

	writeIsPresent(writer, addressSpace->getIsPresent());
	
    writeBlockSize(writer, addressSpace);

    writeSegments(addressSpace, writer);

    writeAddressUnitBits(writer, addressSpace);

    writeLocalMemoryMap(writer, addressSpace);

    writeParameters(writer, addressSpace->getParameters());

    writeVendorExtensions( writer, addressSpace );

	writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::writeBlockSize()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::writeBlockSize(QXmlStreamWriter &writer, QSharedPointer<AddressSpace> addressSpace) const
{
    writer.writeTextElement("ipxact:range", addressSpace->getRange());
    writer.writeTextElement("ipxact:width", addressSpace->getWidth());
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::writeSegments()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::writeSegments(QSharedPointer<AddressSpace> addressSpace, QXmlStreamWriter &writer) const
{
	if (!addressSpace->getSegments()->isEmpty())
	{
		writer.writeStartElement("ipxact:segments");

		foreach (QSharedPointer<Segment> segment, *addressSpace->getSegments())
		{
			writer.writeStartElement("ipxact:segment");

            NameGroupWriter nameGroupWriter;
			nameGroupWriter.writeNameGroup(writer, segment);

			writeIsPresent(writer, segment->getIsPresent());

			if (!segment->getAddressOffset().isEmpty())
			{
				writer.writeStartElement("ipxact:addressOffset");

				XmlUtils::writeAttributes(writer, segment->getOffsetAttributes());
				writer.writeCharacters(segment->getAddressOffset());

				writer.writeEndElement();
			}

			if ( !segment->getRange().isEmpty() )
			{
				writer.writeStartElement("ipxact:range");

				XmlUtils::writeAttributes(writer, segment->getRangeAttributes());
				writer.writeCharacters(segment->getRange());

				writer.writeEndElement();
			}

            writeVendorExtensions( writer, segment );

			writer.writeEndElement();
		}

		writer.writeEndElement();
	}
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::writeAddressUnitBits()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace)
    const
{
    if (!addressSpace->getAddressUnitBits().isEmpty())
    {
        writer.writeTextElement("ipxact:addressUnitBits", addressSpace->getAddressUnitBits());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::writeLocalMemoryMap()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::writeLocalMemoryMap(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace)
    const
{
    if (addressSpace->getLocalMemoryMap())
    {
        writer.writeStartElement("ipxact:localMemoryMap");
        
        MemoryMapBaseWriter mmbw;
        mmbw.writeMemoryMapBase( writer, addressSpace->getLocalMemoryMap() );
        
        writer.writeEndElement(); // ipxact:localMemoryMap
    }
}