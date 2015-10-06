//-----------------------------------------------------------------------------
// File: ComponentGeneratorWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Writer class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#include "ComponentGeneratorWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include "../XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::ComponentGeneratorWriter()
//-----------------------------------------------------------------------------
ComponentGeneratorWriter::ComponentGeneratorWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::~ComponentGeneratorWriter()
//-----------------------------------------------------------------------------
ComponentGeneratorWriter::~ComponentGeneratorWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::writeComponentGenerator()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::writeComponentGenerator(QXmlStreamWriter& writer,
	QSharedPointer<ComponentGenerator> componentGenerator) const
{
	writer.writeStartElement("ipxact:componentGenerator");

	writeAttributes(writer, componentGenerator);

	// Write name group.
	NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, componentGenerator);

	writeVendorExtensions( writer, componentGenerator );

	// Write phase, if exists.
	if (componentGenerator->getPhase() >= 0)
	{
		writer.writeTextElement("ipxact:phase", QString::number(componentGenerator->getPhase()));
	}
	
	writeApiType( writer, componentGenerator );

	// Write generator exe.
	if ( !componentGenerator->getGeneratorExe().isEmpty() )
	{
		writer.writeTextElement("ipxact:generatorExe", componentGenerator->getGeneratorExe());
	}

	// Write groups.
	for (int i = 0; i < componentGenerator->getGroups().size(); ++i)
	{
		writer.writeTextElement("ipxact:group", componentGenerator->getGroups().at(i));
	}

	writeParameters (writer, componentGenerator );

	writer.writeEndElement();
}

void ComponentGeneratorWriter::writeAttributes(QXmlStreamWriter &writer,
	QSharedPointer<ComponentGenerator> componentGenerator) const
{
	writer.writeAttribute("hidden", General::bool2Str(componentGenerator->getHidden()));

	if ( componentGenerator->getScope() == ComponentGenerator::ENTITY )
	{
		writer.writeAttribute("scope", "entity");
	}
	else
	{
		writer.writeAttribute("scope", "instance");
	}
}

void ComponentGeneratorWriter::writeApiType(QXmlStreamWriter &writer,
	QSharedPointer<ComponentGenerator> componentGenerator) const
{
	// Branch based on the API type, TGI_2014_BASE is defined as default in the standard.
	switch (componentGenerator->getApiType())
	{
	case ComponentGenerator::NONE:
		{
			writer.writeTextElement("ipxact:apiType", "none");
			break;
		}
	case ComponentGenerator::TGI_2014_EXTENDED:
		{
			writer.writeTextElement("ipxact:apiType", "TGI_2014_EXTENDED");
			break;
		}
	case ComponentGenerator::TGI_2009:
		{
			writer.writeTextElement("ipxact:apiType", "TGI_2009");
			break;
		}
	default:
		{
			writer.writeTextElement("ipxact:apiType", "TGI_2014_BASE");
		}
	}
}

void ComponentGeneratorWriter::writeParameters(QXmlStreamWriter &writer,
	QSharedPointer<ComponentGenerator> componentGenerator) const
{
	if (componentGenerator->getParameters().size() != 0)
	{
		writer.writeStartElement("ipxact:parameters");

		ParameterWriter parameterWriter;
		// write each parameter
		for (int i = 0; i < componentGenerator->getParameters().size(); ++i)
		{
			parameterWriter.writeParameter(writer, componentGenerator->getParameters().at(i));
		}

		writer.writeEndElement();
	}
}
