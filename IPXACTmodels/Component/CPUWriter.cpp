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
CPUWriter::CPUWriter(QObject* parent /* = 0 */) :
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
// Function: CPUWriter::createViewFrom()
//-----------------------------------------------------------------------------
void CPUWriter::writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> cpu) const
{
	// Start the element, write name group, presence and, vendor extensions with pre-existing writers.
	writer.writeStartElement("ipxact:cpu");
	writeNameGroup(writer, cpu);
	writeIsPresent(writer, cpu);
	writeVendorExtensions( writer, cpu );

	// Acquire references for address space and parameter lists.
	const QStringList& addressSpaceRefs = cpu->getAddressSpaceRefs();
	QList<QSharedPointer<Parameter> >& parameters = cpu->getParameters();

	// Write all address spaces.
	for (int i = 0; i < addressSpaceRefs.size(); ++i)
	{
		// It is simply a name.
		writer.writeEmptyElement("ipxact:addressSpaceRef");
		writer.writeAttribute("addressSpaceRef", addressSpaceRefs.at(i));
	}

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


	writer.writeEndElement();
	return;
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