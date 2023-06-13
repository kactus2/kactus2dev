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
// Function: ApiDefinitionWriter::writeApiDefinitionFrom()
//-----------------------------------------------------------------------------
void ApiDefinitionWriter::writeApiDefinition(QXmlStreamWriter& writer, QSharedPointer<ApiDefinition> apiDefinition)
{
    writer.writeStartDocument();
    
    DocumentWriter::writeTopComments(writer, apiDefinition);

    writer.writeStartElement(QStringLiteral("kactus2:apiDefinition"));
	DocumentWriter::writeNamespaceDeclarations(writer, apiDefinition);

	DocumentWriter::writeDocumentNameGroup(writer, apiDefinition);

	// Write COM definition reference.
	VLNV comDefRef = apiDefinition->getComDefinitionRef();

	writer.writeEmptyElement(QStringLiteral("kactus2:comDefinitionRef"));
	CommonItemsWriter::writeVLNVAttributes(writer, comDefRef);

	// Write data types.
	writer.writeStartElement(QStringLiteral("kactus2:dataTypes"));

	for (QString const& type : *apiDefinition->getDataTypes())
	{
		writer.writeEmptyElement(QStringLiteral("kactus2:dataType"));
		writer.writeAttribute(QStringLiteral("name"), type);
	}

	writer.writeEndElement(); // kactus2:dataTypes

	// Write properties.
	writer.writeStartElement(QStringLiteral("kactus2:functions"));

	for (QSharedPointer<ApiFunction> func : *apiDefinition->getFunctions())
	{
		func->write(writer);
	}

	writer.writeEndElement(); // kactus2:functions

    CommonItemsWriter::writeVendorExtensions(writer, apiDefinition);

	writer.writeEndElement(); // kactus2:apiDefinition
	writer.writeEndDocument();
}
