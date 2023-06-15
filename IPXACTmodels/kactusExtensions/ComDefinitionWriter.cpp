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
// Function: ComDefinitionWriter::writeComDefinitionFrom()
//-----------------------------------------------------------------------------
void ComDefinitionWriter::writeComDefinition(QXmlStreamWriter& writer, QSharedPointer<ComDefinition> comDefinition)
{
    writer.writeStartDocument();
    
    DocumentWriter::writeTopComments(writer, comDefinition);
	
    writer.writeStartElement(QStringLiteral("kactus2:comDefinition"));
    DocumentWriter::writeNamespaceDeclarations(writer, comDefinition);

	// Write basic information.
    DocumentWriter::writeDocumentNameGroup(writer, comDefinition);

    // Write data types.
    writer.writeStartElement(QStringLiteral("kactus2:transferTypes"));

    for (QString const& type : *comDefinition->getTransferTypes())
    {
        writer.writeEmptyElement(QStringLiteral("kactus2:transferType"));
        writer.writeAttribute(QStringLiteral("name"), type);
    }

    writer.writeEndElement(); // kactus2:transferTypes

    // Write properties.
    writer.writeStartElement(QStringLiteral("kactus2:properties"));

    for (QSharedPointer<ComProperty> prop : *comDefinition->getProperties())
	{
		prop->write(writer);
    }

	writer.writeEndElement(); // kactus2:properties

    DocumentWriter::writeVendorExtensions(writer, comDefinition);

    writer.writeEndElement(); // kactus2:comDefinition
    writer.writeEndDocument();
}
