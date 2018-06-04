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

//-----------------------------------------------------------------------------
// Function: ApiDefinitionReader::ApiDefinitionReader()
//-----------------------------------------------------------------------------
ApiDefinitionReader::ApiDefinitionReader(): DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionReader::~ApiDefinitionReader()
//-----------------------------------------------------------------------------
ApiDefinitionReader::~ApiDefinitionReader()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionReader::createApiDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ApiDefinition> ApiDefinitionReader::createApiDefinitionFrom(QDomNode const& document) const
{
    QSharedPointer<ApiDefinition> apiDefinition(new ApiDefinition());

    parseTopComments(document, apiDefinition);

    parseXMLProcessingInstructions(document, apiDefinition);

    QDomElement apiNode = document.firstChildElement();
    parseNamespaceDeclarations(apiNode, apiDefinition);

    apiDefinition->setVlnv(createVLNVFrom(apiNode, VLNV::APIDEFINITION));

	// Parse child nodes.
	for (int i = 0; i < apiNode.childNodes().count(); ++i)
	{
		QDomNode childNode = apiNode.childNodes().at(i);

		if (childNode.isComment())
		{
			continue;
		}

		if (childNode.nodeName() == QLatin1String("ipxact:description"))
		{
			apiDefinition->setDescription( childNode.childNodes().at(0).nodeValue() );
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:language"))
		{
			apiDefinition->setLanguage( childNode.childNodes().at(0).nodeValue() );
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:comDefinitionRef"))
		{
			apiDefinition->setComDefinitionRef(parseVLNVAttributes(childNode, VLNV::COMDEFINITION));
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:dataTypes"))
		{
			parseDataTypes(childNode, apiDefinition);
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:functions"))
		{
			parseFunctions(childNode, apiDefinition);
		}
    }

    parseKactusAndVendorExtensions(apiNode, apiDefinition);

    return apiDefinition;
}


//-----------------------------------------------------------------------------
// Function: ApiDefinitionReaderReader::parseDataTypes()
//-----------------------------------------------------------------------------
void ApiDefinitionReader::parseDataTypes(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition) const
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
void ApiDefinitionReader::parseFunctions(QDomNode& node, QSharedPointer<ApiDefinition> apiDefinition) const
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