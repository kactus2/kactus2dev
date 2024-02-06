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
// Function: ComDefinitionReader::createComDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComDefinition> ComDefinitionReader::createComDefinitionFrom(QDomNode const& document)
{
    QDomElement comNode = document.firstChildElement();
	auto vlnv = DocumentReader::createVLNVFrom(comNode, VLNV::COMDEFINITION);
	auto revision = DocumentReader::getXMLDocumentRevision(comNode);
	if (revision == Document::Revision::Unknown)
	{
		revision = Document::Revision::Std22;
	}

    QSharedPointer<ComDefinition> comDefinition(new ComDefinition(vlnv, revision));

    DocumentReader::parseTopComments(document, comDefinition);

	DocumentReader::parseXMLProcessingInstructions(document, comDefinition);

	DocumentReader::parseNamespaceDeclarations(comNode, comDefinition);

	DocumentReader::parseDocumentNameGroup(document, comDefinition);

	// Parse child nodes.
	for (int i = 0; i < comNode.childNodes().count(); ++i)
	{
		QDomNode childNode = comNode.childNodes().at(i);

		if (childNode.isComment())
		{
			continue;
		}

		if (childNode.nodeName() == QLatin1String("kactus2:transferTypes"))
		{
			Details::parseTransferTypes(childNode, comDefinition);
		}
		else if (childNode.nodeName() == QLatin1String("kactus2:properties"))
		{
			Details::parseProperties(childNode, comDefinition);
		}
    }

	DocumentReader::parseKactusAndVendorExtensions(comNode, comDefinition);

	return comDefinition;
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionReader::parseTransferTypes()
//-----------------------------------------------------------------------------
void ComDefinitionReader::Details::parseTransferTypes(QDomNode const& node, QSharedPointer<ComDefinition> comDefinition)
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
void ComDefinitionReader::Details::parseProperties(QDomNode const& node, QSharedPointer<ComDefinition> comDefinition)
{
	QList<QSharedPointer<ComProperty> > properties;
	for (int i = 0; i < node.childNodes().count(); ++i)
	{
		QDomNode propNode = node.childNodes().at(i);

		if (propNode.nodeName() == QLatin1String("kactus2:property"))
		{
			QSharedPointer<ComProperty> prop(new ComProperty(propNode));
			properties.append(prop);
		}
	}

	comDefinition->setProperties(properties);
}
