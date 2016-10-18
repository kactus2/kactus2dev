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

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::ComponentGeneratorWriter()
//-----------------------------------------------------------------------------
ComponentGeneratorWriter::ComponentGeneratorWriter(QObject* parent) :
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
	writer.writeStartElement(QStringLiteral("ipxact:componentGenerator"));

	writeAttributes(writer, componentGenerator);

	// Write name group.
	NameGroupWriter nameGroupWriter;
	nameGroupWriter.writeNameGroup(writer, componentGenerator);

    if (!componentGenerator->getPhase().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:phase"), componentGenerator->getPhase());
    }

    writeParameters(writer, componentGenerator->getParameters());

    writeApiType(writer, componentGenerator);

    writeTransportMethods(writer, componentGenerator);

	if (!componentGenerator->getGeneratorExe().isEmpty())
	{
		writer.writeTextElement(QStringLiteral("ipxact:generatorExe"), componentGenerator->getGeneratorExe());
	}

    writeVendorExtensions(writer, componentGenerator);

	foreach (QString const& group, componentGenerator->getGroups())
	{
		writer.writeTextElement(QStringLiteral("ipxact:group"), group);
	}

	writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::writeAttributes()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::writeAttributes(QXmlStreamWriter& writer,
	QSharedPointer<ComponentGenerator> componentGenerator) const
{
    BooleanValue hiddenValue = componentGenerator->getHidden();
    if (!hiddenValue.toString().isEmpty())
    {
        writer.writeAttribute(QStringLiteral("hidden"), componentGenerator->getHidden().toString());
    }

	if (componentGenerator->getScope() == ComponentGenerator::ENTITY)
	{
		writer.writeAttribute(QStringLiteral("scope"), QStringLiteral("entity"));
	}
	else if (componentGenerator->getScope() == ComponentGenerator::INSTANCE)
	{
		writer.writeAttribute(QStringLiteral("scope"), QStringLiteral("instance"));
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::writeApiType()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::writeApiType(QXmlStreamWriter& writer,
	QSharedPointer<ComponentGenerator> componentGenerator) const
{
    if (componentGenerator->getApiType() == ComponentGenerator::NONE)
    {
        writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("none"));
    }
    else if (componentGenerator->getApiType() == ComponentGenerator::TGI_2014_EXTENDED)
    {
        writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("TGI_2014_EXTENDED"));
    }
    else if (componentGenerator->getApiType() == ComponentGenerator::TGI_2009)
    {
        writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("TGI_2009"));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::writeTransportMethods()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::writeTransportMethods(QXmlStreamWriter& writer,
    QSharedPointer<ComponentGenerator> componentGenerator) const
{
    QStringList transportMethods = componentGenerator->getTransportMethods();
    
    if (!transportMethods.isEmpty())
    {
       writer.writeStartElement(QStringLiteral("ipxact:transportMethods"));
       foreach (QString const& method, transportMethods)
       {
           writer.writeTextElement(QStringLiteral("ipxact:transportMethod"), method);
       }
       writer.writeEndElement();
    }
}
