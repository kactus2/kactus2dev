//-----------------------------------------------------------------------------
// File: ApiDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Writer class for IP-XACT ApiDefinition element.
//-----------------------------------------------------------------------------

#include "ApiDefinitionWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: ApiDefinitionWriter::ApiDefinitionWriter()
//-----------------------------------------------------------------------------
ApiDefinitionWriter::ApiDefinitionWriter(QObject* parent): DocumentWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionWriter::~ApiDefinitionWriter()
//-----------------------------------------------------------------------------
ApiDefinitionWriter::~ApiDefinitionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionWriter::writeApiDefinitionFrom()
//-----------------------------------------------------------------------------
void ApiDefinitionWriter::writeApiDefinition(QXmlStreamWriter& writer, QSharedPointer<ApiDefinition> apiDefinition) const
{
    writer.writeStartDocument();
    
    writeTopComments(writer, apiDefinition);
	
    writer.writeStartElement("kactus2:apiDefinition");
    writeNamespaceDeclarations(writer);

	// Write basic information.
	writer.writeTextElement("ipxact:vendor", apiDefinition->getVlnv().getVendor());
	writer.writeTextElement("ipxact:library", apiDefinition->getVlnv().getLibrary());
	writer.writeTextElement("ipxact:name", apiDefinition->getVlnv().getName());
	writer.writeTextElement("ipxact:version", apiDefinition->getVlnv().getVersion());

	if (!apiDefinition->getDescription().isEmpty())
	{
		writer.writeTextElement("ipxact:description", apiDefinition->getDescription());
	}

	// Write COM definition reference.
	VLNV comDefRef = apiDefinition->getComDefinitionRef();

	writer.writeEmptyElement("kactus2:comDefinitionRef");
	writer.writeAttribute("vendor", comDefRef.getVendor());
	writer.writeAttribute("library", comDefRef.getLibrary());
	writer.writeAttribute("name", comDefRef.getName());
	writer.writeAttribute("version", comDefRef.getVersion());

	// Write data types.
	writer.writeStartElement("kactus2:dataTypes");

	foreach (QString const& type, *apiDefinition->getDataTypes())
	{
		writer.writeEmptyElement("kactus2:dataType");
		writer.writeAttribute("name", type);
	}

	writer.writeEndElement(); // kactus2:dataTypes

	// Write properties.
	writer.writeStartElement("kactus2:functions");

	foreach (QSharedPointer<ApiFunction> func, *apiDefinition->getFunctions())
	{
		func->write(writer);
	}

	writer.writeEndElement(); // kactus2:functions

	if (apiDefinition->getVendorExtensions()->isEmpty())
	{
		writer.writeStartElement("ipxact:vendorExtensions");
		foreach (QSharedPointer<VendorExtension> extension, *apiDefinition->getVendorExtensions())
		{
			extension->write(writer);
		}
		writer.writeEndElement(); // ipxact:vendorExtensions
	}

	writer.writeEndElement(); // kactus2:apiDefinition
	writer.writeEndDocument();
}