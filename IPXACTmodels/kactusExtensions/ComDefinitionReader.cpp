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
ComDefinitionReader::ComDefinitionReader(QObject *parent):
DocumentReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComDefinitionReader::~ComDefinitionReader()
//-----------------------------------------------------------------------------
ComDefinitionReader::~ComDefinitionReader()
{

}

//-----------------------------------------------------------------------------
// Function: ComDefinitionReader::createComDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComDefinition> ComDefinitionReader::createComDefinitionFrom(QDomNode const& document) const
{
	QSharedPointer<ComDefinition> comDefinition(new ComDefinition());

	QDomElement comNode = document.firstChildElement();

	QString vendor = comNode.firstChildElement(QStringLiteral("ipxact:vendor")).firstChild().nodeValue();
	QString library = comNode.firstChildElement(QStringLiteral("ipxact:library")).firstChild().nodeValue();
	QString name = comNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();
	QString version = comNode.firstChildElement(QStringLiteral("ipxact:version")).firstChild().nodeValue();

	VLNV itemVLNV;
	itemVLNV.setType(VLNV::COMDEFINITION);
	itemVLNV.setVendor(vendor);
	itemVLNV.setLibrary(library);
	itemVLNV.setName(name);
	itemVLNV.setVersion(version);

	comDefinition->setVlnv(itemVLNV);

	QDomNodeList extensionNodes = comNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions")).childNodes();

	int extensionCount = extensionNodes.count();
	for (int i = 0; i < extensionCount; i++)
	{
		QDomNode extensionNode = extensionNodes.at(i);

		if (!extensionNode.nodeName().startsWith(QLatin1String("kactus2:")))
		{
			QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNode));
			comDefinition->getVendorExtensions()->append(extension);
		}
	}

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