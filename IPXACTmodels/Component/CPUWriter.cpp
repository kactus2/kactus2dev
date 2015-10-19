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

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>

//-----------------------------------------------------------------------------
// Function: CPUWriter::CPUWriter()
//-----------------------------------------------------------------------------
CPUWriter::CPUWriter(QObject* parent) :
CommonItemsWriter(parent)
{
    
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::~CPUWriter()
//-----------------------------------------------------------------------------
CPUWriter::~CPUWriter()
{

}

//-----------------------------------------------------------------------------
// Function: CPUWriter::writeCPU()
//-----------------------------------------------------------------------------
void CPUWriter::writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const
{
	// Start the element, write name group, presence and vendor extensions with pre-existing writers.
	writer.writeStartElement("ipxact:cpu");
	
    writeNameGroup(writer, cpu);

    writeIsPresent(writer, cpu);

    writeAddressSpaceRefs(writer, cpu);

    writeParameters(writer, cpu->getParameters());

    writeVendorExtensions( writer, cpu );

	writer.writeEndElement(); // ipxact:cpu
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::writeNameGroup()
//-----------------------------------------------------------------------------
void CPUWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const
{
	NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, cpu);
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void CPUWriter::writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const
{
	if (!cpu->getIsPresent().isEmpty())
	{
		writer.writeTextElement("ipxact:isPresent", cpu->getIsPresent());
	}
}

//-----------------------------------------------------------------------------
// Function: CPUWriter::writeAddressSpaceRefs()
//-----------------------------------------------------------------------------
void CPUWriter::writeAddressSpaceRefs(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const
{
    foreach (QSharedPointer<Cpu::AddressSpaceRef> reference, *cpu->getAddressSpaceReferences())
    {
        writer.writeStartElement("ipxact:addressSpaceRef");

        writer.writeAttribute("addressSpaceRef", reference->getAddressSpaceRef());

        if (!reference->getIsPresent().isEmpty())
        {
            writer.writeTextElement("ipxact:isPresent", reference->getIsPresent());
        }

        writer.writeEndElement(); // ipxact:addressSpaceRef
    }
}