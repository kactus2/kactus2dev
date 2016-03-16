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

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::ComponentGeneratorReader()
//-----------------------------------------------------------------------------
ComponentGeneratorReader::ComponentGeneratorReader(QObject* parent) :
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
QSharedPointer<ComponentGenerator> ComponentGeneratorReader::createComponentGeneratorFrom(
    QDomNode const& componentGeneratorNode) const
{
	QSharedPointer<ComponentGenerator> newComponentGenerator (new ComponentGenerator());

	NameGroupReader nameReader;
	nameReader.parseNameGroup(componentGeneratorNode, newComponentGenerator);

	readAttributes(componentGeneratorNode, newComponentGenerator);

    QDomElement phaseElement = componentGeneratorNode.firstChildElement("ipxact:phase");
    if (!phaseElement.isNull())
    {
        newComponentGenerator->setPhase(phaseElement.childNodes().at(0).nodeValue());
    }

    readParameters(componentGeneratorNode.firstChildElement("ipxact:parameters"), newComponentGenerator);
        
    readApiType(componentGeneratorNode.firstChildElement("ipxact:apiType"), newComponentGenerator);
   
    readTransportMethods(componentGeneratorNode.firstChildElement("ipxact:transportMethods"), newComponentGenerator);

    QDomElement exeElement = componentGeneratorNode.firstChildElement("ipxact:generatorExe");
    if (!exeElement.isNull())
    {
        newComponentGenerator->setGeneratorExe(exeElement.childNodes().at(0).nodeValue());
    }

    parseVendorExtensions(componentGeneratorNode, newComponentGenerator);

    parseGroups(componentGeneratorNode, newComponentGenerator);

    return newComponentGenerator;
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::readAttributes()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::readAttributes(QDomNode const& componentGeneratorNode,
	QSharedPointer<ComponentGenerator> newComponentGenerator) const
{
	QDomNamedNodeMap attributeMap = componentGeneratorNode.attributes();

	QString scope = attributeMap.namedItem("scope").nodeValue();
	if (scope == QLatin1String("entity"))
	{
		newComponentGenerator->setScope(ComponentGenerator::ENTITY);
	}
	else if (scope == QLatin1String("instance"))
	{
		newComponentGenerator->setScope(ComponentGenerator::INSTANCE);
	}

    QString hidden = attributeMap.namedItem("hidden").childNodes().at(0).nodeValue();
    if (hidden == QLatin1String("true"))
    {
        newComponentGenerator->setHidden(true);
    }
    else if (hidden == QLatin1String("false"))
    {
        newComponentGenerator->setHidden(false);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::readParameters()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::readParameters(QDomNode const& parametersNode, 
    QSharedPointer<ComponentGenerator> componentGenerator) const
{
	ParameterReader reader;

    int parameterCount = parametersNode.childNodes().count();
	for (int i = 0; i < parameterCount; i++)
	{
		QDomNode parameterNode = parametersNode.childNodes().at(i);
		componentGenerator->getParameters()->append(reader.createParameterFrom(parameterNode));
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::readApiType()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::readApiType(QDomNode const& apiTypeNode,
	QSharedPointer<ComponentGenerator> newComponentGenerator) const
{
	QString apiType = apiTypeNode.childNodes().at(0).nodeValue();
	if (apiType == QLatin1String("TGI_2014_BASE"))
	{
		newComponentGenerator->setApiType(ComponentGenerator::TGI_2014_BASE);
	}
	else if (apiType == QLatin1String("TGI_2014_EXTENDED"))
	{
		newComponentGenerator->setApiType(ComponentGenerator::TGI_2014_EXTENDED);
	}
	else if (apiType == QLatin1String("TGI_2009"))
	{
		newComponentGenerator->setApiType(ComponentGenerator::TGI_2009);
	}
	else if (apiType == QLatin1String("none"))
	{
		newComponentGenerator->setApiType(ComponentGenerator::NONE);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::readTransportMethods()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::readTransportMethods(QDomElement const& methodsNode, 
    QSharedPointer<ComponentGenerator> newComponentGenerator) const
{
    if (methodsNode.isNull())
    {
        return;
    }

    QStringList transportMethods;
    int methodsCount = methodsNode.childNodes().count();
    for (int i = 0; i < methodsCount; i++)
    {
        QDomNode methodNode = methodsNode.childNodes().at(i);
        transportMethods.append(methodNode.firstChild().nodeValue());
    }

    newComponentGenerator->setTransportMethods(transportMethods);
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorReader::parseGroups()
//-----------------------------------------------------------------------------
void ComponentGeneratorReader::parseGroups(QDomNode const& componentGeneratorNode, 
    QSharedPointer<ComponentGenerator> newComponentGenerator) const
{
    QDomNodeList groupNodes = componentGeneratorNode.toElement().elementsByTagName("ipxact:group");

    QStringList groupNames;
    int groupCount = groupNodes.count();
    for (int i = 0; i < groupCount; i++)
    {
        groupNames.append(groupNodes.at(i).firstChild().nodeValue());
    }

    newComponentGenerator->setGroups(groupNames);
}
