//-----------------------------------------------------------------------------
// File: ComDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// XML reader class for Kactus2 Com definition.
//-----------------------------------------------------------------------------

#include "ComDefinitionReader.h"

#include "ComDefinition.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: ComDefinitionReader::ComDefinitionReader()
//-----------------------------------------------------------------------------
ComDefinitionReader::ComDefinitionReader(): DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: ComDefinitionReader::createComDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComDefinition> ComDefinitionReader::createComDefinitionFrom(QDomNode const& document) const
{
    QSharedPointer<ComDefinition> comDefinition(new ComDefinition());

    parseTopComments(document, comDefinition);

    parseXMLProcessingInstructions(document, comDefinition);

    QDomElement comNode = document.firstChildElement();
    parseNamespaceDeclarations(comNode, comDefinition);

	comDefinition->setVlnv(createVLNVFrom(comNode, VLNV::COMDEFINITION));

	// Parse child nodes.
	for (int i = 0; i < comNode.childNodes().count(); ++i)
	{
		QDomNode childNode = comNode.childNodes().at(i);

		if (childNode.isComment())
		{
			continue;
		}

		if (childNode.nodeName() == QLatin1String("ipxact:description"))
		{
			comDefinition->setDescription(childNode.childNodes().at(0).nodeValue());
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:transferTypes"))
		{
			parseTransferTypes(childNode, comDefinition);
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:properties"))
		{
			parseProperties(childNode, comDefinition);
		}
    }

    parseKactusAndVendorExtensions(comNode, comDefinition);

	return comDefinition;
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionReader::parseTransferTypes()
//-----------------------------------------------------------------------------
void ComDefinitionReader::parseTransferTypes(QDomNode& node, QSharedPointer<ComDefinition> comDefinition) const
{
	for (int i = 0; i < node.childNodes().count(); ++i)
	{
		QDomNode typeNode = node.childNodes().at(i);

		if (typeNode.nodeName() == QLatin1String("kactus2:transferType"))
		{
			QString name = typeNode.attributes().namedItem(QStringLiteral("name")).nodeValue();
			comDefinition->getTransferTypes()->append(name);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionReader::parseProperties()
//-----------------------------------------------------------------------------
void ComDefinitionReader::parseProperties(QDomNode& node, QSharedPointer<ComDefinition> comDefinition) const
{
	for (int i = 0; i < node.childNodes().count(); ++i)
	{
		QDomNode propNode = node.childNodes().at(i);

		if (propNode.nodeName() == QLatin1String("kactus2:property"))
		{
			QSharedPointer<ComProperty> prop(new ComProperty(propNode));
			comDefinition->getProperties()->append(prop);
		}
	}
}