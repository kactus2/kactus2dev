//-----------------------------------------------------------------------------
// File: ComDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Writer class for IP-XACT ComDefinition element.
//-----------------------------------------------------------------------------

#include "ComDefinitionWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: ComDefinitionWriter::ComDefinitionWriter()
//-----------------------------------------------------------------------------
ComDefinitionWriter::ComDefinitionWriter(QObject* parent): DocumentWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComDefinitionWriter::~ComDefinitionWriter()
//-----------------------------------------------------------------------------
ComDefinitionWriter::~ComDefinitionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ComDefinitionWriter::writeComDefinitionFrom()
//-----------------------------------------------------------------------------
void ComDefinitionWriter::writeComDefinition(QXmlStreamWriter& writer, QSharedPointer<ComDefinition> comDefinition) const
{
    writer.writeStartDocument();
    
    writeTopComments(writer, comDefinition);
	
    writer.writeStartElement("kactus2:comDefinition");
    writeNamespaceDeclarations(writer);

	// Write basic information.
	writer.writeTextElement("ipxact:vendor", comDefinition->getVlnv().getVendor());
	writer.writeTextElement("ipxact:library", comDefinition->getVlnv().getLibrary());
	writer.writeTextElement("ipxact:name", comDefinition->getVlnv().getName());
	writer.writeTextElement("ipxact:version", comDefinition->getVlnv().getVersion());

	if (!comDefinition->getDescription().isEmpty())
	{
		writer.writeTextElement("ipxact:description", comDefinition->getDescription());
	}

    // Write data types.
    writer.writeStartElement("kactus2:transferTypes");

    foreach (QString const& type, *comDefinition->getTransferTypes())
    {
        writer.writeEmptyElement("kactus2:transferType");
        writer.writeAttribute("name", type);
    }

    writer.writeEndElement(); // kactus2:transferTypes

    // Write properties.
    writer.writeStartElement("kactus2:properties");

    foreach (QSharedPointer<ComProperty> prop, *comDefinition->getProperties())
	{
		prop->write(writer);
    }

	writer.writeEndElement(); // kactus2:properties

	if (comDefinition->getVendorExtensions()->isEmpty())
	{
		writer.writeStartElement("ipxact:vendorExtensions");
		foreach (QSharedPointer<VendorExtension> extension, *comDefinition->getVendorExtensions())
		{
			extension->write(writer);
		}
		writer.writeEndElement(); // ipxact:vendorExtensions
	}

    writer.writeEndElement(); // kactus2:comDefinition
    writer.writeEndDocument();
}