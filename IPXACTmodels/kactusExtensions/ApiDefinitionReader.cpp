//-----------------------------------------------------------------------------
// File: ApiDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// XML reader class for Kactus2 API definition.
//-----------------------------------------------------------------------------

#include "ApiDefinitionReader.h"

#include "ApiDefinition.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: ApiDefinitionReader::createApiDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ApiDefinition> ApiDefinitionReader::createApiDefinitionFrom(QDomNode const& document)
{
    QDomElement apiNode = document.firstChildElement();
	auto vlnv = DocumentReader::createVLNVFrom(apiNode, VLNV::APIDEFINITION);

	auto revision = DocumentReader::getXMLDocumentRevision(document);
	if (revision == Document::Revision::Unknown)
	{
		revision = Document::Revision::Std22;
	}

    QSharedPointer<ApiDefinition> apiDefinition(new ApiDefinition(vlnv, revision));

    DocumentReader::parseTopComments(document, apiDefinition);

	DocumentReader::parseXMLProcessingInstructions(document, apiDefinition);

	DocumentReader::parseNamespaceDeclarations(apiNode, apiDefinition);

    DocumentReader::parseDocumentNameGroup(apiNode, apiDefinition);

	// Parse child nodes.
	for (int i = 0; i < apiNode.childNodes().count(); ++i)
	{
		QDomNode childNode = apiNode.childNodes().at(i);

		if (childNode.isComment())
		{
			continue;
		}

		if (childNode.nodeName() == QLatin1String("kactus2:language"))
		{
			apiDefinition->setLanguage( childNode.childNodes().at(0).nodeValue() );
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:comDefinitionRef"))
		{
			apiDefinition->setComDefinitionRef(
                CommonItemsReader::parseVLNVAttributes(childNode, VLNV::COMDEFINITION));
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:dataTypes"))
		{
			Details::parseDataTypes(childNode, apiDefinition);
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:functions"))
		{
			Details::parseFunctions(childNode, apiDefinition);
		}
    }

	DocumentReader::parseKactusAndVendorExtensions(apiNode, apiDefinition);

    return apiDefinition;
}


//-----------------------------------------------------------------------------
// Function: ApiDefinitionReaderReader::parseDataTypes()
//-----------------------------------------------------------------------------
void ApiDefinitionReader::Details::parseDataTypes(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition)
{
	for (int i = 0; i < node.childNodes().count(); ++i)
	{
		QDomNode typeNode = node.childNodes().at(i);

		if (typeNode.nodeName() == QLatin1String("kactus2:dataType"))
		{
			QString name = typeNode.attributes().namedItem(QStringLiteral("name")).nodeValue();
			apiDefinition->getDataTypes()->append(name);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionReader::parseFunctions()
//-----------------------------------------------------------------------------
void ApiDefinitionReader::Details::parseFunctions(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition)
{
	for (int i = 0; i < node.childNodes().count(); ++i)
	{
		QDomNode propNode = node.childNodes().at(i);

		if (propNode.nodeName() == QLatin1String("kactus2:function"))
		{
			QSharedPointer<ApiFunction> func(new ApiFunction(propNode));
			apiDefinition->getFunctions()->append(func);
		}
	}
}