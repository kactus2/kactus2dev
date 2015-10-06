//-----------------------------------------------------------------------------
// File: ComponentGeneratorReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Reader class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#include "ComponentGeneratorReader.h"
#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include "../XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::ComponentGeneratorReader()
//-----------------------------------------------------------------------------
ComponentGeneratorReader::ComponentGeneratorReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::~ComponentGeneratorReader()
//-----------------------------------------------------------------------------
ComponentGeneratorReader::~ComponentGeneratorReader()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::createComponentGeneratorFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentGenerator> ComponentGeneratorReader::createComponentGeneratorFrom
	(QDomNode const& componentGeneratorNode) const
{
	// Create the new ComponentGenerator.
	QSharedPointer<ComponentGenerator> newComponentGenerator (new ComponentGenerator());

	// Read name group, attributes and vendor extensions.
	NameGroupReader nameReader;
	nameReader.parseNameGroup(componentGeneratorNode, newComponentGenerator);
	readAttributes(componentGeneratorNode, newComponentGenerator);
	parseVendorExtensions(componentGeneratorNode,newComponentGenerator);

	// These may occur multiple times.
	QStringList groups;
	QStringList transportMethods;
	QList<QSharedPointer<Parameter > > parameters;
.
	// Go through all the child nodes of the component generator
	for (int i = 0; i < componentGeneratorNode.childNodes().count(); ++i)
	{
		QDomNode tempNode = componentGeneratorNode.childNodes().at(i);

		// Add another group.
		if (tempNode.nodeName() == QString("ipxact:group"))
		{
			QString groupName = tempNode.childNodes().at(0).nodeValue();
			groupName = XmlUtils::removeWhiteSpace(groupName);
			groups.append(groupName);
		}

		// Parameters encountered: read.
		else if (tempNode.nodeName() == QString("ipxact:parameters"))
		{
			readParameters(tempNode, parameters);
		}

		// Read phase.
		else if (tempNode.nodeName() == QString("ipxact:phase"))
		{
			newComponentGenerator->setPhase( tempNode.childNodes().at(0).nodeValue().toDouble() );
		}

		// Add another API type.
		else if (tempNode.nodeName() == QString("ipxact:apiType"))
		{
			readApiType(tempNode, newComponentGenerator);
		}

		// Read generator exeutavble location.
		else if (tempNode.nodeName() == QString("ipxact:generatorExe"))
		{
			newComponentGenerator->setGeneratorExe( tempNode.childNodes().at(0).nodeValue() );
		}

		// Add transport methods.
		else if (tempNode.nodeName() == QString("ipxact:transportMethods"))
		{
			for (int j = 0; j < tempNode.childNodes().count(); ++j)
			{
				if (tempNode.childNodes().at(j).nodeName() == QString("ipxact:transportMethod"))
				{
					transportMethods.append( tempNode.childNodes().at(j).firstChild().nodeValue() );
				}
			}
		}
	}

	// Set the lists for the created object.
	newComponentGenerator->setGroups(groups);
	newComponentGenerator->setParameters(parameters);
	newComponentGenerator->setTransportMethods(transportMethods);

    return newComponentGenerator;
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::readAttributes()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::readAttributes(QDomNode const &componentGeneratorNode,
	QSharedPointer<ComponentGenerator> newComponentGenerator) const
{
	QDomNamedNodeMap attributeMap = componentGeneratorNode.attributes();

	// The scope is instance by default.
	QString scope = attributeMap.namedItem(QString("scope")).nodeValue();
	if (scope == QString("entity"))
	{
		newComponentGenerator->setScope( ComponentGenerator::ENTITY );
	}
	else
	{
		newComponentGenerator->setScope( ComponentGenerator::INSTANCE );
	}

	// The "hidden" is false by default.
	QString hidden = attributeMap.namedItem(QString("hidden")).childNodes().at(0).nodeValue();
	newComponentGenerator->setHidden( General::str2Bool(hidden, false) );
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::readParameters()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::readParameters(QDomNode &tempNode,
	QList<QSharedPointer<Parameter > > &parameters) const
{
	ParameterReader reader;

	for (int j = 0; j < tempNode.childNodes().count(); ++j)
	{
		QDomNode parameterNode = tempNode.childNodes().at(j);
		parameters.append(reader.createParameterFrom(parameterNode));
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::readApiType()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::readApiType(QDomNode &tempNode,
	QSharedPointer<ComponentGenerator> newComponentGenerator) const
{
	// Select correct api type.
	if (tempNode.childNodes().at(0).nodeValue() == QString("TGI_2014_BASE"))
	{
		newComponentGenerator->setApiType( ComponentGenerator::TGI_2014_BASE );
	}
	else if (tempNode.childNodes().at(0).nodeValue() == QString("TGI_2014_EXTENDED"))
	{
		newComponentGenerator->setApiType( ComponentGenerator::TGI_2014_EXTENDED );
	}
	else if (tempNode.childNodes().at(0).nodeValue() == QString("TGI_2009"))
	{
		newComponentGenerator->setApiType( ComponentGenerator::TGI_2009 );
	}
	else
	{
		newComponentGenerator->setApiType( ComponentGenerator::NONE );
	}
}
