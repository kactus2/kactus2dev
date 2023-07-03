//-----------------------------------------------------------------------------
// File: AddressSpaceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Writer for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#include "AddressSpaceWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include <IPXACTmodels/Component/MemoryMapBaseWriter.h>

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::writeAddressSpace()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::writeAddressSpace(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace,
    Document::Revision docRevision)
{
	writer.writeStartElement(QStringLiteral("ipxact:addressSpace"));

    NameGroupWriter::writeNameGroup(writer, addressSpace, docRevision);

    if (docRevision != Document::Revision::Std22)
    {
	    CommonItemsWriter::writeIsPresent(writer, addressSpace->getIsPresent());
    }
	
    Details::writeBlockSize(writer, addressSpace);

    Details::writeSegments(addressSpace, writer);

    Details::writeAddressUnitBits(writer, addressSpace);

    Details::writeLocalMemoryMap(writer, addressSpace);

    CommonItemsWriter::writeParameters(writer, addressSpace->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions( writer, addressSpace );

	writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::Details::writeBlockSize()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::Details::writeBlockSize(QXmlStreamWriter &writer, QSharedPointer<AddressSpace> addressSpace)
{
    writer.writeTextElement(QStringLiteral("ipxact:range"), addressSpace->getRange());
    writer.writeTextElement(QStringLiteral("ipxact:width"), addressSpace->getWidth());
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::Details::writeSegments()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::Details::writeSegments(QSharedPointer<AddressSpace> addressSpace, QXmlStreamWriter &writer)
{
	if (!addressSpace->getSegments()->isEmpty())
	{
		writer.writeStartElement(QStringLiteral("ipxact:segments"));

		for (auto const& segment : *addressSpace->getSegments())
		{
            writeSingleSegment(writer, segment);
		}

		writer.writeEndElement(); // ipxact:segments
	}
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::Details::writeSingleSegment()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::Details::writeSingleSegment(QXmlStreamWriter& writer, QSharedPointer<Segment> segment)
{
    writer.writeStartElement(QStringLiteral("ipxact:segment"));

    NameGroupWriter::writeNameGroup(writer, segment);

    CommonItemsWriter::writeIsPresent(writer, segment->getIsPresent());

    if (!segment->getAddressOffset().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:addressOffset"));

        writeAttributeMap(writer, segment->getOffsetAttributes());
        writer.writeCharacters(segment->getAddressOffset());

        writer.writeEndElement();
    }

    if ( !segment->getRange().isEmpty() )
    {
        writer.writeStartElement(QStringLiteral("ipxact:range"));

        writeAttributeMap(writer, segment->getRangeAttributes());
        writer.writeCharacters(segment->getRange());

        writer.writeEndElement();
    }

    CommonItemsWriter::writeVendorExtensions( writer, segment );

    writer.writeEndElement(); // ipxact:segment
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::Details::writeAttributeMap()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::Details::writeAttributeMap(QXmlStreamWriter& writer, QMap<QString, QString> attributes)
{
    for (QMap<QString, QString>::const_iterator i = attributes.begin(); i != attributes.end(); ++i)
    {        
        writer.writeAttribute(i.key(), i.value());
    }
    return;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::Details::writeAddressUnitBits()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::Details::writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace)
   
{
    if (!addressSpace->getAddressUnitBits().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:addressUnitBits"), addressSpace->getAddressUnitBits());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceWriter::Details::writeLocalMemoryMap()
//-----------------------------------------------------------------------------
void AddressSpaceWriter::Details::writeLocalMemoryMap(QXmlStreamWriter& writer, QSharedPointer<AddressSpace> addressSpace)
   
{
    if (addressSpace->getLocalMemoryMap())
    {
        writer.writeStartElement(QStringLiteral("ipxact:localMemoryMap"));
        
        MemoryMapBaseWriter mmbw;
        mmbw.writeMemoryMapBase( writer, addressSpace->getLocalMemoryMap() );
        
        writer.writeEndElement(); // ipxact:localMemoryMap
    }
}
