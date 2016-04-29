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
ApiDefinitionReader::ApiDefinitionReader(QObject *parent):
DocumentReader(parent)
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

	QDomElement apiNode = document.firstChildElement();

	QString vendor = apiNode.firstChildElement("ipxact:vendor").firstChild().nodeValue();
	QString library = apiNode.firstChildElement("ipxact:library").firstChild().nodeValue();
	QString name = apiNode.firstChildElement("ipxact:name").firstChild().nodeValue();
	QString version = apiNode.firstChildElement("ipxact:version").firstChild().nodeValue();

	VLNV itemVLNV;
	itemVLNV.setType(VLNV::APIDEFINITION);
	itemVLNV.setVendor(vendor);
	itemVLNV.setLibrary(library);
	itemVLNV.setName(name);
	itemVLNV.setVersion(version);

	apiDefinition->setVlnv(itemVLNV);

	QDomNodeList extensionNodes = apiNode.firstChildElement("ipxact:vendorExtensions").childNodes();

	int extensionCount = extensionNodes.count();
	for (int i = 0; i < extensionCount; i++)
	{
		QDomNode extensionNode = extensionNodes.at(i);

		if (!extensionNode.nodeName().startsWith("kactus2:"))
		{
			QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNode));
			apiDefinition->getVendorExtensions()->append(extension);
		}
	}

	// Parse child nodes.
	for (int i = 0; i < apiNode.childNodes().count(); ++i)
	{
		QDomNode childNode = apiNode.childNodes().at(i);

		if (childNode.isComment())
		{
			continue;
		}

		if (childNode.nodeName() ==QString("ipxact:description"))
		{
			apiDefinition->setDescription( childNode.childNodes().at(0).nodeValue() );
		}
		else if (childNode.nodeName() == "kactus2:language")
		{
			apiDefinition->setLanguage( childNode.childNodes().at(0).nodeValue() );
		}
		else if (childNode.nodeName() == "kactus2:comDefinitionRef")
		{
			apiDefinition->setComDefinitionRef( VLNV::createVLNV(childNode, VLNV::COMDEFINITION) );
		}
		else if (childNode.nodeName() == "kactus2:dataTypes")
		{
			parseDataTypes(childNode, apiDefinition);
		}
		else if (childNode.nodeName() == "kactus2:functions")
		{
			parseFunctions(childNode, apiDefinition);
		}
	}

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

		if (typeNode.nodeName() == "kactus2:dataType")
		{
			QString name = typeNode.attributes().namedItem("name").nodeValue();
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

		if (propNode.nodeName() == "kactus2:function")
		{
			QSharedPointer<ApiFunction> func(new ApiFunction(propNode));
			apiDefinition->getFunctions()->append(func);
		}
	}
}