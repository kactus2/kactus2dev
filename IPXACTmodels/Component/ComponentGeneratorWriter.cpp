//-----------------------------------------------------------------------------
// File: ComponentGeneratorWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Writer for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#include "ComponentGeneratorWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::writeComponentGenerator()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::writeComponentGenerator(QXmlStreamWriter& writer,
	QSharedPointer<ComponentGenerator> componentGenerator, Document::Revision docRevision)
{
	writer.writeStartElement(QStringLiteral("ipxact:componentGenerator"));

	Details::writeAttributes(writer, componentGenerator);

	// Write name group.
    NameGroupWriter::writeNameGroup(writer, componentGenerator, docRevision);

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:phase"), componentGenerator->getPhase());

    CommonItemsWriter::writeParameters(writer, componentGenerator->getParameters(), docRevision);

    Details::writeApiType(writer, componentGenerator, docRevision);

    Details::writeApiService(writer, componentGenerator, docRevision);

    Details::writeTransportMethods(writer, componentGenerator);

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:generatorExe"), 
        componentGenerator->getGeneratorExe());

    CommonItemsWriter::writeVendorExtensions(writer, componentGenerator);

	for (auto const& group : componentGenerator->getGroups())
	{
		writer.writeTextElement(QStringLiteral("ipxact:group"), group);
	}

	writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::Details::writeAttributes()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::Details::writeAttributes(QXmlStreamWriter& writer,
	QSharedPointer<ComponentGenerator> componentGenerator)
{
    if (auto hiddenValue = componentGenerator->getHidden().toString(); 
        !hiddenValue.isEmpty())
    {
        writer.writeAttribute(QStringLiteral("hidden"), hiddenValue);
    }

	if (componentGenerator->getScope() == ComponentGenerator::Scope::ENTITY)
	{
		writer.writeAttribute(QStringLiteral("scope"), QStringLiteral("entity"));
	}
	else if (componentGenerator->getScope() == ComponentGenerator::Scope::INSTANCE)
	{
		writer.writeAttribute(QStringLiteral("scope"), QStringLiteral("instance"));
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::Details::writeApiType()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::Details::writeApiType(QXmlStreamWriter& writer,
	QSharedPointer<ComponentGenerator> componentGenerator, Document::Revision docRevision)
{
    if (componentGenerator->getApiType() == ComponentGenerator::ApiType::NONE)
    {
        writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("none"));
    }
    else if (componentGenerator->getApiType() == ComponentGenerator::ApiType::TGI_2014_EXTENDED)
    {
        writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("TGI_2014_EXTENDED"));
    }
    else if (componentGenerator->getApiType() == ComponentGenerator::ApiType::TGI_2009)
    {
        writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("TGI_2009"));
    }
    
    if (docRevision == Document::Revision::Std22)
    {
        if (componentGenerator->getApiType() == ComponentGenerator::ApiType::TBGI_2022_EXTENDED)
        {
            writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("TBGI_2022_EXTENDED"));
        }
        else if (componentGenerator->getApiType() == ComponentGenerator::ApiType::TGI_2022_BASE)
        {
            writer.writeTextElement(QStringLiteral("ipxact:apiType"), QStringLiteral("TGI_2022_BASE"));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::Details::writeApiService()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::Details::writeApiService(QXmlStreamWriter& writer, 
    QSharedPointer<ComponentGenerator> componentGenerator, Document::Revision docRevision)
{
    if (docRevision == Document::Revision::Std22)
    {
        CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:apiService"),
            componentGenerator->getApiService());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorWriter::Details::writeTransportMethods()
//-----------------------------------------------------------------------------
void ComponentGeneratorWriter::Details::writeTransportMethods(QXmlStreamWriter& writer,
    QSharedPointer<ComponentGenerator> componentGenerator)
{
    QStringList transportMethods = componentGenerator->getTransportMethods();
    
    if (!transportMethods.isEmpty())
    {
       writer.writeStartElement(QStringLiteral("ipxact:transportMethods"));
       for (auto const& method : transportMethods)
       {
           writer.writeTextElement(QStringLiteral("ipxact:transportMethod"), method);
       }
       writer.writeEndElement();
    }
}
