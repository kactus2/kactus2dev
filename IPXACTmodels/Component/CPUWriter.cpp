//-----------------------------------------------------------------------------
// File: CPUWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Writer class for IP-XACT CPU element.
//-----------------------------------------------------------------------------

#include "CPUWriter.h"
#include "ExecutableImagesWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/FileBuilderWriter.h>

//-----------------------------------------------------------------------------
// Function: CPUWriter::writeCPU()
//-----------------------------------------------------------------------------
void CPUWriter::writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu, Document::Revision docRevision)
{
	// Start the element, write name group, presence and vendor extensions with pre-existing writers.
	writer.writeStartElement(QStringLiteral("ipxact:cpu"));

    NameGroupWriter::writeNameGroup(writer, cpu, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, cpu->getIsPresent());

        Details::writeAddressSpaceRefs(writer, cpu);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::writeBlockSize(writer, cpu);

        Details::writeRegions(writer, cpu);

        Details::writeAddressUnitBits(writer, cpu);

        ExecutableImagesWriter::writeExecutableImages(writer, cpu->getExecutableImages(), docRevision);

        Details::writeMemoryMapRef(writer, cpu);
    }

    CommonItemsWriter::writeParameters(writer, cpu->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions( writer, cpu );

	writer.writeEndElement(); // ipxact:cpu
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::writeAddressSpaceRefs()
//-----------------------------------------------------------------------------
void CPUWriter::Details::writeAddressSpaceRefs(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu)
{
    for (QSharedPointer<Cpu::AddressSpaceRef> reference : *cpu->getAddressSpaceReferences())
    {
        writer.writeStartElement(QStringLiteral("ipxact:addressSpaceRef"));

        writer.writeAttribute(QStringLiteral("addressSpaceRef"), reference->getAddressSpaceRef());

        CommonItemsWriter::writeIsPresent(writer, reference->getIsPresent());

        writer.writeEndElement(); // ipxact:addressSpaceRef
    }
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::Details::writeBlockSize()
//-----------------------------------------------------------------------------
void CPUWriter::Details::writeBlockSize(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:range"), cpu->getRange());
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:width"), cpu->getWidth());
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::Details::writeRegions()
//-----------------------------------------------------------------------------
void CPUWriter::Details::writeRegions(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu)
{
    if (cpu->getRegions()->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:regions"));
    for (auto const& region : *cpu->getRegions())
    {
        writer.writeStartElement(QStringLiteral("ipxact:region"));

        NameGroupWriter::writeNameGroup(writer, region, Document::Revision::Std22);

        writer.writeTextElement(QStringLiteral("ipxact:addressOffset"), region->getAddressOffset());

        writer.writeTextElement(QStringLiteral("ipxact:range"), region->getRange());

        CommonItemsWriter::writeVendorExtensions(writer, region);

        writer.writeEndElement(); // ipxact:region
    }

    writer.writeEndElement(); // ipxact:regions
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::Details::writeExecutableImages()
//-----------------------------------------------------------------------------
void CPUWriter::Details::writeExecutableImages(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu)
{
    ExecutableImagesWriter::writeExecutableImages(writer, cpu->getExecutableImages(), Document::Revision::Std22);
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::Details::writeAddressUnitBits()
//-----------------------------------------------------------------------------
void CPUWriter::Details::writeAddressUnitBits(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:addressUnitBits"), 
        cpu->getAddressUnitBits(false));
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::Details::writeMemoryMapRef()
//-----------------------------------------------------------------------------
void CPUWriter::Details::writeMemoryMapRef(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:memoryMapRef"),
        cpu->getMemoryMapReference());
}
